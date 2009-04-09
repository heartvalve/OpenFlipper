/*
 * helpBrowser.hh
 *
 *  Created on: Apr 8, 2009
 *      Author: kremer
 */

#ifndef HELPBROWSER_HH_
#define HELPBROWSER_HH_

#include <QtGui>
#include <QWebView>
#include <QtHelp>

class HelpBrowser : public QWebView {
	Q_OBJECT

public:
	HelpBrowser(QHelpEngine* _helpEngine, QWidget* parent = 0);

	~HelpBrowser();

	QVariant loadBytes(const QUrl& url);

	bool isBackwardAvailable();

	bool isForwardAvailable();

	void setUrl(const QUrl& _url);

signals:
	void urlChanged ( const QUrl& src );

public slots:

	void load(const QUrl& _url);

	void load(const QUrl& url, const QString& _s_url, bool _skipSave = false);

	void backward();

	void forward();

private:

	QUrl getCurrentDir(const QUrl& _url);

	QHelpEngine* helpEngine_;

	QString css_str_;
	QString css_file_;

	QStringList visitedPages_;
	int currentPage_;

};


#endif /* HELPBROWSER_HH_ */
