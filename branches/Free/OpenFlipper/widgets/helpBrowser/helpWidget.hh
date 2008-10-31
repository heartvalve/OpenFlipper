#include "ui_helpWidget.hh"
#include <QtGui>

#include "webView.hh"
#include "findEdit.hh"

class HelpWidget : public QMainWindow, public Ui::HelpWidget
{
  Q_OBJECT

  public:
    HelpWidget(QWidget *parent = 0, bool _user = true);

  private:
    QString baseDir_;
    QString startFile_;

    void generateTree();

    void setConnections();

    QString getTitle(QString htmlFile);

    QAction* back_;
    QAction* forward_;

    webView* currentView_;

  private slots:
    void loadFinished( bool ok );
    void goHome();
    void treeClicked(QTreeWidgetItem* item, int column );
    void urlChanged(const QUrl& url);
    void newTab(QWebView* _webView);
    void titleChanged (const QString & title );
    void tabChanged (int _index);
    void closeCurrentTab();
    void duplicateCurrentTab();

  //finding
  private:
    findEdit* findText_;

  private slots:
    void showFind();
    void findNext1();
    void findPrev1();
    void initFindFrame();

  //context menu
  private:
    QMenu* contextMenu_;
    QPoint lastContextMenu_;
  private slots:
    void showContextMenu(const QPoint& _pos);
    void contextOpenTab();
    void contextOpenNewTab();
    void contextCloseTab();
};

