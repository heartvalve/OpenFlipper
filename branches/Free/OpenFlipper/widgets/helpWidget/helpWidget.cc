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

HelpWidget::HelpWidget(QWidget* parent)
	: QMainWindow(parent),
	searchWidget_(0),
	textWindow_(0),
	helpEngine_(0) {

	setupUi(this);

	QString filename = QString(OpenFlipper::Options::applicationDirStr());
	filename += "/Help/Help.qhc";

	QString stylesheet = QString(OpenFlipper::Options::applicationDirStr());
	stylesheet += "/Help/acg_style.css";

	//std::cerr << filename.toStdString() << std::endl;

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

	searchWidget_->resize(this->size().width(), floor(this->size().height() / 3));

	results_ = new QListWidget();

	searchWidget_->setWidget(results_);

	searchWidget_->hide();
	addDockWidget(Qt::BottomDockWidgetArea, searchWidget_);


    // Load css data
    textWindow_->setCSSData(stylesheet);

    // Entry in tree view has been clicked
    connect(helpEngine_->contentWidget(), SIGNAL(linkActivated(const QUrl&)),
            textWindow_, SLOT(setSource(const QUrl&)));

    // Entry in results view (search) has been clicked
    connect(results_, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
    		textWindow_, SLOT(openFoundSite(QListWidgetItem*)));

    // Search button
    connect(searchButton_, SIGNAL(clicked()), this, SLOT(searchItems()));

    // Back button
    connect(backButton_, SIGNAL(clicked()), this, SLOT(goBack()));

    // Forward button
    connect(forwardButton_, SIGNAL(clicked()), this, SLOT(goForward()));

    // Source has been reloaded
    connect(textWindow_, SIGNAL(sourceChanged(const QString&)), this, SLOT(update(const QString&)));

    // Load main page
    textWindow_->setSource(QUrl("qthelp://com.openflipper.dev/openflipper-1.0/main.html"));
}

void HelpWidget::update(const QString& /* url */) {

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

	QMap<QString, QUrl> map = helpEngine_->linksForIdentifier(searchEdit_->text());
	std::cerr << "Searching for " << searchEdit_->text().toStdString() << std::endl;

	QList<QUrl> list = map.values();

	std::cerr << "Found " << list.size() << " items!" << std::endl;

	for(int i = 0; i < list.size(); i++) {
		QListWidgetItem itm(list[i].toString());
		results_->insertItem(i, &itm);

		std::cerr << "Adding item " << list[i].toString().toStdString() << std::endl;
	}

	searchWidget_->show();
}

void HelpWidget::openFoundSite(QListWidgetItem* _item) {

}
