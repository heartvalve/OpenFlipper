/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

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
	currentVirtualFolder_(""),
	currentNameSpace_("")
{

	currentPage_ = 0;

	connect(this, SIGNAL(linkClicked(const QString&)),
			this, SLOT(open(const QString&)));

}

HelpBrowser::~HelpBrowser() {

}

QVariant HelpBrowser::loadResource (int /*_type*/, const QUrl& _url) {

	if (_url.scheme() == "qthelp") {

		// Extract the global virtual folder from this link
		QString link = _url.toString();

		QStringList linkParts = link.split("/");

		if ( linkParts.size() > 3) {
			currentNameSpace_     = linkParts[2];
			currentVirtualFolder_ = linkParts[3];
		} else {
			currentNameSpace_ = "";
			currentVirtualFolder_ = "";
			std::cerr << "Unable to detect virtual folder or namespace of this link" << _url.toString().toStdString() << std::endl;
		}

		return QVariant(helpEngine_->fileData(_url));
	}	else {

		const QString sNewUrl = "qthelp://" + currentNameSpace_ + "/" + currentVirtualFolder_ + "/" + _url.toString();
		const QUrl newUrl = helpEngine_->findFile(QUrl(sNewUrl));

		if(newUrl.isValid())
			return QVariant(helpEngine_->fileData(newUrl));
		else {
			std::cerr << "Unable to find file at url : " << sNewUrl.toStdString() << std::endl;
			return QVariant();
		}

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

  //jumps to a reference (Doxygen reference name and not section name)
  //references are at the end of url after last '#'
  QStringList Anchor = _url.toString().split("#");
  if (Anchor.size() > 1)
    this->scrollToAnchor(Anchor[Anchor.size()-1]);

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

