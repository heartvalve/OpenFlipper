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
	helpEngine_(_helpEngine),
	doc_(0) {

	currentPage_ = 0;

	doc_ = new QTextDocument();

	this->setDocument(doc_);
}

HelpBrowser::~HelpBrowser() {
	delete helpEngine_;
}

QVariant HelpBrowser::loadResource(int type, const QUrl &url) {
	if (url.scheme() == "qthelp") {

		QByteArray bytes = helpEngine_->fileData(url);

		return QVariant(bytes);
	}
	else {
		return QTextBrowser::loadResource(type, url);
	}
}

void HelpBrowser::setSource(const QUrl& url) {

	if(url.scheme() != "qthelp") {
		// Convert link to be within qthelp domain namespace
		QString new_file = visitedPages_[currentPage_];
		QStringList comp = new_file.split("/");
		comp[comp.size()-1] = url.toString();

		QUrl newUrl = QUrl(comp.join("/"));

		if(helpEngine_->findFile(newUrl).isValid())
			this->setSource(newUrl, newUrl.toString());
		else
			QMessageBox::warning( this, "OpenFlipper Help",
				"Unable to find specified file within documentation.");


	} else {

		this->setSource(url, url.toString());
	}
}


void HelpBrowser::setSource(const QUrl& url, const QString& /* str */, bool _skipSave) {

	QVariant data = this->loadResource(QTextDocument::HtmlResource, url);

	QString txt;

	if (data.type() == QVariant::String) {
		txt = data.toString();
	} else if (data.type() == QVariant::ByteArray) {

		QByteArray ba = data.toByteArray();
		QTextCodec *codec = Qt::codecForHtml(ba);
		txt = codec->toUnicode(ba);

		txt = data.toString();
	}

	doc_->setHtml(txt);
	refreshCSS();

	if(!_skipSave) {
		visitedPages_.push_back(url.toString());
		currentPage_ = visitedPages_.size()-1;
	}

	emit sourceChanged ( url.toString() );
}

bool HelpBrowser::setCSSData(QString& _filename) {

	// Load style sheet
	QFile file(_filename);
	if(!file.open(QIODevice::ReadOnly))
		return false;

	QTextStream css_stream(&file);

	// Read css data into member variable
	css_str_ = css_stream.readAll();
	// Cut off the path to css file
#ifdef WIN32
	css_file_ = _filename.split("\\").last();
#else
	css_file_ = _filename.split("/").last();
#endif

	doc_->addResource( QTextDocument::StyleSheetResource, QUrl( css_file_ ), css_str_ );
	doc_->setDefaultStyleSheet(css_file_);

	return true;
}

void HelpBrowser::refreshCSS() {

	if(css_str_.length() > 0)
		doc_->addResource( QTextDocument::StyleSheetResource, QUrl( css_file_ ), css_str_ );
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
		setSource(QUrl(visitedPages_[currentPage_]), visitedPages_[currentPage_], true);
	}
}

void HelpBrowser::forward() {

	if(isForwardAvailable()) {
		currentPage_++;
		setSource(QUrl(visitedPages_[currentPage_]), visitedPages_[currentPage_], true);
	}
}

