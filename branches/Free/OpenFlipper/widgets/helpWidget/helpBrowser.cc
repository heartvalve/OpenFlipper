/*
 * helpBrowser.cc
 *
 *  Created on: Apr 8, 2009
 *      Author: kremer
 */

#include "helpBrowser.hh"

#include <iostream>


HelpBrowser::HelpBrowser(QHelpEngine* _helpEngine, QWidget* parent) :

	QTextBrowser(parent),
	helpEngine_(_helpEngine) {

	currentPage_ = 0;

	connect(this, SIGNAL(linkClicked(const QString&)),
			this, SLOT(open(const QString&)));

}

HelpBrowser::~HelpBrowser() {

}

QVariant HelpBrowser::loadResource (int /*_type*/, const QUrl& _url) {

	if (_url.scheme() == "qthelp") {

		return QVariant(helpEngine_->fileData(_url));
	}
	else {

		QUrl newUrl;

		QStringList docDomains = helpEngine_->registeredDocumentations();

		// Search in all namespaces for requested file
		for(int i = 0; i < docDomains.size(); i++) {

			QString sNewUrl = "qthelp://" + docDomains[i] + "/" + VIRTUAL_FOLDER +
				"/" + _url.toString();

			newUrl = helpEngine_->findFile(QUrl(sNewUrl));

			if(newUrl.isValid()) return QVariant(helpEngine_->fileData(newUrl));
		}

		// If file has not been found in any of the namespaces
		// return an empty QVariant
		return QVariant();
	}

}

void HelpBrowser::open(const QString& _url) {

	open(QUrl(_url), "");
}

void HelpBrowser::open(const QUrl& _url) {

	open(_url, "");
}

void HelpBrowser::open(const QUrl& _url, const QString& /*_str*/, bool _skipSave) {

	QVariant data = this->loadResource(QTextDocument::HtmlResource, _url);

	QString txt;

	if (data.type() == QVariant::String) {
		txt = data.toString();
	} else if (data.type() == QVariant::ByteArray) {

		QByteArray ba = data.toByteArray();
		QTextCodec *codec = Qt::codecForHtml(ba);
		txt = codec->toUnicode(ba);

		txt = data.toString();
	}

	//std::cerr << txt.toStdString() << std::endl;

	setHtml(txt);

	if(!_skipSave) {
		visitedPages_.push_back(_url.toString());
		currentPage_ = visitedPages_.size()-1;
	}

	emit urlChanged ( _url.toString() );
}

QUrl HelpBrowser::getCurrentDir(const QUrl& _url) {

	QStringList str_list = _url.toString().split("/");
	str_list[str_list.size() - 1] = "";

	QString nstr = str_list.join("/");

	return QUrl(nstr);
}

bool HelpBrowser::isForwardAvailable() {

	return currentPage_ < visitedPages_.size() - 1;
}

bool HelpBrowser::isBackwardAvailable() {

	return currentPage_ > 0;
}

void HelpBrowser::backward() {

	if(isBackwardAvailable()) {
		currentPage_--;
		open(QUrl(visitedPages_[currentPage_]), visitedPages_[currentPage_], true);
	}
}

void HelpBrowser::forward() {

	if(isForwardAvailable()) {
		currentPage_++;
		open(QUrl(visitedPages_[currentPage_]), visitedPages_[currentPage_], true);
	}
}

