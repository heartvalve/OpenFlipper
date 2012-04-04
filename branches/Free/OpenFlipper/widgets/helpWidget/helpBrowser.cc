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

}

HelpBrowser::~HelpBrowser() {

}


void HelpBrowser::updateNameSpaceAndFolder (const QUrl& _url) {

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
}

void HelpBrowser::rememberHistory (const QUrl& _url) {

  // Delete the visited pages after the current position if they exist
  if ( currentPage_ < visitedPages_.size()-1 )
    visitedPages_.erase((visitedPages_.begin()+currentPage_),visitedPages_.end());

  visitedPages_.push_back(_url);
  currentPage_ = visitedPages_.size()-1;

}

QVariant HelpBrowser::loadResource (int /*_type*/, const QUrl& _url) {

	if (_url.scheme() == "qthelp") {

	  updateNameSpaceAndFolder(_url);
		return QVariant(helpEngine_->fileData(_url));

	}	else {

		QUrl newUrl;

		if ( _url.toString().startsWith("..") ) {

		  // Relative url. We jump to a new context, so we first remove the relative part
		  QUrl tmpURL("qthelp://" + currentNameSpace_ + "/" + currentVirtualFolder_ + "/");
		  newUrl = tmpURL.resolved(_url);

		  // Update context
		  updateNameSpaceAndFolder(newUrl);

		} else {

		  // Normal URL without relative parts so we can safely combine them
		  // and stay in the current context
		  newUrl = "qthelp://" + currentNameSpace_ + "/" + currentVirtualFolder_ + "/" + _url.toString();

		}

		const QUrl newFileUrl = helpEngine_->findFile(newUrl);

		if(newFileUrl.isValid())
			return QVariant(helpEngine_->fileData(newFileUrl));
		else {
			std::cerr << "Unable to find file at url : " << newUrl.toString().toStdString() << std::endl;
			return QVariant();
		}

	}

}

void HelpBrowser::open(const QString& _url) {

	open(QUrl(_url));
}

void HelpBrowser::open(const QUrl& _url, bool _skipSave) {

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

	setHtml(txt);

	if(!_skipSave)
	  rememberHistory(_url);

  //jumps to a reference (Doxygen reference name and not section name)
  //references are at the end of url after last '#'
  QStringList Anchor = _url.toString().split("#");
  if (Anchor.size() > 1)
    this->scrollToAnchor(Anchor[Anchor.size()-1]);

	emit urlChanged ( _url.toString() );
}

QUrl HelpBrowser::getCurrentDir(const QUrl& _url) {

	QStringList str_list = _url.toString().split("/");

	if ( str_list.size() > 0  )
	  str_list[str_list.size() - 1] = "";
	else
	  std::cerr << "Warning, getCurrentDir got invalid input: " << _url.toString().toStdString() << std::endl;

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
		open(visitedPages_[currentPage_], true);
	}

}

void HelpBrowser::forward() {

	if(isForwardAvailable()) {
		currentPage_++;
		open(visitedPages_[currentPage_], true);
	}

}

