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

	  HelpWidget(QWidget* parent = 0, const QString& _homeSite = "");

	  virtual ~HelpWidget();

	  void setHomeSite(const QString& _homeSite);

private slots:

	void goBack();

	void goForward();

	void goHome();

	void searchItems();

	void update(const QUrl& url);

	void openFoundSite(QListWidgetItem* _item);

  private:

	void updateButtons();

	// For the search popup
	QDockWidget* searchWidget_;
	QListWidget* results_;

	HelpBrowser* textWindow_;

	QHelpEngine* helpEngine_;

	QString homeSite_;

};


#endif /* HELPWIDGET_HH_ */
