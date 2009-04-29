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

HelpWidget::HelpWidget(QWidget* parent, const QString& _homeSite)
	: QMainWindow(parent),
	searchWidget_(0),
	tabWidget_(0),
	textWindow_(0),
	helpEngine_(0),
	searchEngine_(0) {

	setupUi(this);

	homeSite_ = _homeSite;

	QString filename = QString(OpenFlipper::Options::helpDirStr());
	filename += OpenFlipper::Options::dirSeparator();
	filename += "Help.qhc";

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

	searchEngine_ = new QHelpSearchEngine(helpEngine_, this);

    textWindow_ = new HelpBrowser(helpEngine_, this);

    homeIndex_ = tabWidget_->addTab(textWindow_, "Home");

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

	int resultsTabIndex_ = tabWidget_->addTab(searchEngine_->resultWidget(), "Results");
	tabWidget_->setCurrentIndex(resultsTabIndex_);
}

void HelpWidget::openFoundSite(QListWidgetItem* /*_item*/) {

}

HelpWidget::~HelpWidget() {

	delete searchWidget_;
	delete textWindow_;
}
