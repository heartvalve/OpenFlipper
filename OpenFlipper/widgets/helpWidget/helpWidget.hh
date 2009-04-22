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

	  HelpWidget(QWidget* parent = 0, const QString& _homeSite = "");

	  virtual ~HelpWidget();

	  void setHomeSite(const QString& _homeSite);

private slots:

	void startSearch();

	void goBack();

	void goForward();

	void goHome();

	void showSearchWidget();

	void linkActivated(const QUrl& _url);

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
