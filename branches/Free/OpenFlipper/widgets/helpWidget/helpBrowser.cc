/*
 * helpBrowser.cc
 *
 *  Created on: Apr 8, 2009
 *      Author: kremer
 */

#include "helpBrowser.hh"

#include <iostream>

#include <QWebFrame>

HelpBrowser::HelpBrowser(QHelpEngine* _helpEngine, QWidget* parent) :

	QWebView(parent),
	helpEngine_(_helpEngine) {

	currentPage_ = 0;

	page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	// Link has been clicked
    connect(this->page(), SIGNAL(linkClicked(const QUrl&)),
            this, SLOT(load(const QUrl&)));

}

HelpBrowser::~HelpBrowser() {
	delete helpEngine_;
}

QVariant HelpBrowser::loadBytes(const QUrl &url) {

	QByteArray bytes = helpEngine_->fileData(url);

	return QVariant(bytes);
}

void HelpBrowser::setUrl(const QUrl& _url) {

	load(_url, "");
}

void HelpBrowser::load(const QUrl& _url) {

	load(_url, "");
}

void HelpBrowser::load(const QUrl& _url, const QString& /*_s_url*/, bool _skipSave) {

	QUrl newUrl;

	if(_url.scheme() != "qthelp") {
		// Convert link to be within qthelp domain namespace
		QString new_file = visitedPages_[currentPage_];
		QStringList comp = new_file.split("/");
		comp[comp.size()-1] = _url.toString();

		newUrl = QUrl(comp.join("/"));

		if(!helpEngine_->findFile(newUrl).isValid()) {
			QMessageBox::warning( this, "OpenFlipper Help",
				"Unable to find specified file within documentation.");
			return;
		}

	} else {
		newUrl = _url;
	}

	QVariant data = this->loadBytes(newUrl);

	QString txt;

	if (data.type() == QVariant::String) {
		txt = data.toString();
	} else if (data.type() == QVariant::ByteArray) {

		QByteArray ba = data.toByteArray();
		QTextCodec *codec = Qt::codecForHtml(ba);
		txt = codec->toUnicode(ba);

		txt = data.toString();
	}

	page()->mainFrame()->setHtml(txt, getCurrentDir(newUrl));

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
		load(QUrl(visitedPages_[currentPage_]), visitedPages_[currentPage_], true);
	}
}

void HelpBrowser::forward() {

	if(isForwardAvailable()) {
		currentPage_++;
		load(QUrl(visitedPages_[currentPage_]), visitedPages_[currentPage_], true);
	}
}

