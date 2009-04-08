/*
 * helpBrowser.hh
 *
 *  Created on: Apr 8, 2009
 *      Author: kremer
 */

#ifndef HELPBROWSER_HH_
#define HELPBROWSER_HH_

#include <QtGui>
#include <QTextBrowser>
#include <QtHelp>

class HelpBrowser : public QTextBrowser {
	Q_OBJECT

public:
	HelpBrowser(QHelpEngine* _helpEngine, QWidget* parent = 0);

	~HelpBrowser();

	QVariant loadResource(int type, const QUrl& url);

	bool setCSSData(QString& _filename);

	void refreshCSS();

	bool isBackwardAvailable();

	bool isForwardAvailable();

signals:
	void sourceChanged ( const QString& src );

public slots:

	void setSource(const QUrl& url);

	void setSource(const QUrl& url, const QString& /* str */, bool _skipSave = false);

	void backward();

	void forward();

private:

	QHelpEngine* helpEngine_;

	QTextDocument* doc_;

	QString css_str_;
	QString css_file_;

	QStringList visitedPages_;
	int currentPage_;

};


#endif /* HELPBROWSER_HH_ */
