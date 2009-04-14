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
	HelpBrowser(QHelpEngine* _helpEngine, const QUrl& _basePath, QWidget* parent = 0);

	virtual ~HelpBrowser();

	QVariant loadResource ( int _type, const QUrl& _name );

	bool isBackwardAvailable();

	bool isForwardAvailable();

signals:
	void urlChanged ( const QUrl& src );

	void linkClicked(const QString& _link);

public slots:

	void open(const QUrl& _url);

	void open(const QString& _url);

	void open(const QUrl& _url, const QString& _str, bool _skipSave = false);

	void backward();

	void forward();

private:

	QUrl getCurrentDir(const QUrl& _url);

	QHelpEngine* helpEngine_;

	QStringList visitedPages_;
	int currentPage_;

	QUrl basePath_;

};


#endif /* HELPBROWSER_HH_ */
