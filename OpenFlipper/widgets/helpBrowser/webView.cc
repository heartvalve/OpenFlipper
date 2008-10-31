#include "webView.hh"

#include <QKeyEvent>

webView::webView(QWidget * parent) : QWebView(parent)
{
}

QWebView* webView::createWindow(QWebPage::WebWindowType /*type*/)
{
  webView* window = new webView();

  emit windowCreated(window);

  return window;
}

void webView::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_F && event->modifiers() == Qt::ControlModifier)
    emit find();
  else if (event->key() == Qt::Key_F3)
    emit findNext();
  else
    QWebView::keyPressEvent(event);
}

