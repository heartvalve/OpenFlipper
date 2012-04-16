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
 * helpWidget.hh
 *
 *  Created on: Apr 7, 2009
 *      Author: kremer
 */

#ifndef HELPWIDGET_HH_
#define HELPWIDGET_HH_

#include "ui_helpWidget.hh"
#include <QtGui>

#include <QtHelp>
#include <QTextDocument>

#include <QHelpSearchEngine>
#include <QTabWidget>

#include <iostream>

#include "helpBrowser.hh"

class HelpWidget : public QMainWindow, public Ui::HelpWidget
{
  Q_OBJECT

  public:

	  HelpWidget(QWidget* parent = 0, const QString& _homeSite = "", const bool _loadHomeSite = true);

	  virtual ~HelpWidget();

	  void setHomeSite(const QString& _homeSite);

private slots:

	void startSearch();

	void goBack();

	void goForward();

	void goHome();

	void showSearchWidget();

	//the entry was already selected by user
	void linkActivated(const QUrl& _url);

	//activate the url and jump to the entry
	void activateLink(const QUrl& _url);

	void showFoundSite(const QUrl& _url);

	void showResults(int _hits);

	void update(const QUrl& url);

	void openFoundSite(QListWidgetItem* _item);

  private:

	void updateButtons();

	// For the search popup
	QDockWidget* searchWidget_;

	QTabWidget* tabWidget_;

	HelpBrowser* textWindow_;

	QHelpEngine* helpEngine_;
	QHelpSearchEngine* searchEngine_;

	QString homeSite_;

	int homeIndex_;

};


#endif /* HELPWIDGET_HH_ */
