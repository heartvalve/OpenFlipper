#ifndef WEBVIEW_HH
#define WEBVIEW_HH

#include "ui_helpWidget.hh"
#include <QtWebKit>

class webView : public QWebView
{
  Q_OBJECT

  signals:
    void windowCreated(QWebView* _webView);
    void find();
    void findNext();

  public:
    webView(QWidget *parent = 0);

  protected:
    QWebView* createWindow( QWebPage::WebWindowType type );
    void keyPressEvent( QKeyEvent* event );
};


#endif //WEBVIEW_HH
