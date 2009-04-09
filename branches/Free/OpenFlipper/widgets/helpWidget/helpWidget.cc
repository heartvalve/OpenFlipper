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
	results_(0),
	textWindow_(0),
	helpEngine_(0) {

	setupUi(this);

	homeSite_ = _homeSite;

	QString filename = QString(OpenFlipper::Options::applicationDirStr());
	filename += "/Help/Help.qhc";

	QString stylesheet = QString(OpenFlipper::Options::applicationDirStr());
	stylesheet += "/Help/acg_style.css";

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

	helpEngine_ = new QHelpEngine(filename, this);

	helpEngine_->setupData();

    textWindow_ = new HelpBrowser(helpEngine_, this);

    gridLayout_->addWidget(helpEngine_->contentWidget(), 0, 0);
    gridLayout_->addWidget(textWindow_, 0, 1);

    gridLayout_->setColumnStretch(0, 1);
    gridLayout_->setColumnStretch(1, 3);

    // Search popup at bottom of window
    searchWidget_ = new QDockWidget(tr("Search results"), this);
	searchWidget_->setFeatures( QDockWidget::DockWidgetClosable );

	searchWidget_->resize(this->size().width(), floor( double(this->size().height() / 3)) );

	results_ = new QListWidget();

	searchWidget_->setWidget(helpEngine_->indexWidget());
	//searchWidget_->setWidget(results_);

	searchWidget_->hide();
	addDockWidget(Qt::BottomDockWidgetArea, searchWidget_);


    // Entry in tree view has been clicked
    connect(helpEngine_->contentWidget(), SIGNAL(linkActivated(const QUrl&)),
            textWindow_, SLOT(load(const QUrl&)));

    connect(helpEngine_->indexWidget(), SIGNAL(linkActivated(const QUrl&, const QString&)),
            textWindow_, SLOT(load(const QUrl&, const QString&)));

    // Entry in results view (search) has been clicked
    //connect(results_, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
    //		textWindow_, SLOT(openFoundSite(QListWidgetItem*)));

    // Search button
    connect(searchButton_, SIGNAL(clicked()), this, SLOT(searchItems()));

    // Back button
    connect(backButton_, SIGNAL(clicked()), this, SLOT(goBack()));

    // Forward button
    connect(forwardButton_, SIGNAL(clicked()), this, SLOT(goForward()));

    // Forward button
    connect(homeButton_, SIGNAL(clicked()), this, SLOT(goHome()));

    // Source has been reloaded
    connect(textWindow_, SIGNAL(urlChanged(const QUrl&)), this, SLOT(update(const QUrl&)));

    // Register documentation (if not registered yet)
    helpEngine_->registerDocumentation(filename);
    helpEngine_->setCurrentFilter("");

    // Load main page
    textWindow_->load(QUrl(homeSite_));
}

void HelpWidget::setHomeSite(const QString& _homeSite) {

	homeSite_ = _homeSite;
}

void HelpWidget::update(const QUrl& /* url */) {

	updateButtons();
}

void HelpWidget::goForward() {

	textWindow_->forward();

	updateButtons();
}

void HelpWidget::goBack() {

	textWindow_->backward();

	updateButtons();
}

void HelpWidget::goHome() {

	textWindow_->load(homeSite_);

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

void HelpWidget::searchItems() {

	QMap<QString, QUrl> results = helpEngine_->linksForIdentifier(searchButton_->text());

	std::cerr << "Found: " << results.size() << std::endl;

	searchWidget_->show();
}

void HelpWidget::openFoundSite(QListWidgetItem* /*_item*/) {

}

HelpWidget::~HelpWidget() {

	delete searchWidget_;
	delete results_;
	delete textWindow_;
}
