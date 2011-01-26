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
 * helpWidget.cc
 *
 *  Created on: Apr 7, 2009
 *      Author: kremer
 */

#include "helpWidget.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

#include <iostream>

#include <QTextStream>
#include <QDir>

HelpWidget::HelpWidget(QWidget* parent, const QString& _homeSite)
  : QMainWindow(parent),
  searchWidget_(0),
  tabWidget_(0),
  textWindow_(0),
  helpEngine_(0),
  searchEngine_(0) {

  setupUi(this);

  homeSite_ = _homeSite;

  QString filename = QString( OpenFlipper::Options::configDirStr() );
  filename += OpenFlipper::Options::dirSeparator();
  filename += "Help.qhc";
    
  QDir helpDir = QDir(OpenFlipper::Options::helpDirStr());
  QStringList helpFiles = helpDir.entryList(QStringList("*.qch"),QDir::Files);

  QString iconPath = QString(OpenFlipper::Options::iconDirStr());
#ifdef WIN32
  iconPath += "\\";
#else
  iconPath += "/";
#endif

  // Set Buttons
  backButton_->setIcon(QIcon(iconPath+"arrow-left.png"));
  forwardButton_->setIcon(QIcon(iconPath+"arrow-right.png"));
  homeButton_->setIcon(QIcon(iconPath+"go-home.png"));
  searchButton_->setIcon(QIcon(iconPath+"edit-find.png"));

  tabWidget_ = new QTabWidget(this);

  helpEngine_ = new QHelpEngine(filename, this);
  
  helpEngine_->setupData();
  
  for ( int i = 0 ; i < helpFiles.size() ; ++i ) 
    if ( !helpEngine_->registerDocumentation( helpDir.path()+ QDir::separator() + helpFiles[i]) ) 
      std::cerr << helpEngine_->error().toStdString() << std::endl;

  searchEngine_ = new QHelpSearchEngine(helpEngine_, this);

  textWindow_ = new HelpBrowser(helpEngine_, this);

  homeIndex_ = tabWidget_->addTab(textWindow_, tr("Home"));

  gridLayout_->addWidget(helpEngine_->contentWidget(), 1, 0);
  gridLayout_->addWidget(tabWidget_, 1, 1);

  gridLayout_->setColumnStretch(0, 1);
  gridLayout_->setColumnStretch(1, 3);

  // Search popup at bottom of window
  searchWidget_ = new QDockWidget(tr("Search results"), this);
  searchWidget_->setFeatures( QDockWidget::DockWidgetClosable );

  searchWidget_->resize(this->size().width(), floor( double(this->size().height() / 3)) );

  searchWidget_->setWidget(searchEngine_->queryWidget());
  //searchWidget_->setWidget(results_);

  searchWidget_->hide();
  addDockWidget(Qt::BottomDockWidgetArea, searchWidget_);


  // Entry in tree view has been clicked
  connect(helpEngine_->contentWidget(), SIGNAL(linkActivated(const QUrl&)),
          this, SLOT(linkActivated(const QUrl&)));


  // Search button
  connect(searchButton_, SIGNAL(clicked()), this, SLOT(showSearchWidget()));

  // Search button
  connect(searchEngine_->queryWidget(), SIGNAL(search()), this, SLOT(startSearch()));

  // Show results if search is finished
  connect(searchEngine_, SIGNAL(searchingFinished(int)), this, SLOT(showResults(int)));

  // Show results if search is finished
  connect(searchEngine_->resultWidget(), SIGNAL(requestShowLink(const QUrl&)),
              this, SLOT(showFoundSite(const QUrl&)));

  // Back button
  connect(backButton_, SIGNAL(clicked()), this, SLOT(goBack()));

  // Forward button
  connect(forwardButton_, SIGNAL(clicked()), this, SLOT(goForward()));

  // Forward button
  connect(homeButton_, SIGNAL(clicked()), this, SLOT(goHome()));

  // Source has been reloaded
  connect(textWindow_, SIGNAL(urlChanged(const QUrl&)), this, SLOT(update(const QUrl&)));

  // Register documentation
  helpEngine_->registerDocumentation(filename);

  // Load main page
  textWindow_->open(QUrl(homeSite_));
}

void HelpWidget::linkActivated(const QUrl& _url) {

  textWindow_->open(_url);
  tabWidget_->setCurrentIndex(homeIndex_);
}

void HelpWidget::startSearch() {

  searchEngine_->search(searchEngine_->queryWidget()->query());
}

void HelpWidget::setHomeSite(const QString& _homeSite) {

  homeSite_ = _homeSite;
}

void HelpWidget::showFoundSite(const QUrl& _url) {

  textWindow_->open(_url);
  tabWidget_->setCurrentIndex(homeIndex_);
}

void HelpWidget::update(const QUrl& /* url */) {

  updateButtons();
}

void HelpWidget::goForward() {

  textWindow_->forward();

  tabWidget_->setCurrentIndex(homeIndex_);

  updateButtons();
}

void HelpWidget::goBack() {

  textWindow_->backward();

  tabWidget_->setCurrentIndex(homeIndex_);

  updateButtons();
}

void HelpWidget::goHome() {

  textWindow_->open(homeSite_);

  tabWidget_->setCurrentIndex(homeIndex_);

  updateButtons();
}

void HelpWidget::updateButtons() {

  if(!textWindow_->isBackwardAvailable()) {
          backButton_->setEnabled(false);
  } else {
          backButton_->setEnabled(true);
  }

  if(!textWindow_->isForwardAvailable()) {
          forwardButton_->setEnabled(false);
  } else {
          forwardButton_->setEnabled(true);
  }
}

void HelpWidget::showSearchWidget() {

  searchWidget_->show();
}

void HelpWidget::showResults(int /*_hits*/) {

  searchWidget_->hide();

  int resultsTabIndex_ = tabWidget_->addTab(searchEngine_->resultWidget(), tr("Results") );
  tabWidget_->setCurrentIndex(resultsTabIndex_);
}

void HelpWidget::openFoundSite(QListWidgetItem* /*_item*/) {

}

HelpWidget::~HelpWidget() {

  delete searchWidget_;
  delete textWindow_;
}
