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

#include <iostream>

#include "helpBrowser.hh"

class HelpWidget : public QMainWindow, public Ui::HelpWidget
{
  Q_OBJECT

  public:

	  HelpWidget(QWidget* parent = 0);

	  virtual ~HelpWidget();

private slots:

	void goBack();

	void goForward();

	void searchItems();

	void update(const QString& url);

	void openFoundSite(QListWidgetItem* _item);

  private:

	void updateButtons();

	// For the search popup
	QDockWidget* searchWidget_;
	QListWidget* results_;

	HelpBrowser* textWindow_;

	QHelpEngine* helpEngine_;

};


#endif /* HELPWIDGET_HH_ */
