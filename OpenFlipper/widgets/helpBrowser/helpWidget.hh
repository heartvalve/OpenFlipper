#include "ui_helpWidget.hh"
#include <QtGui>

class HelpWidget : public QMainWindow, public Ui::HelpWidget
{
  Q_OBJECT

  public:
    HelpWidget(QWidget *parent = 0, bool _user = true);

  private:
    QString baseDir_;
    QString startFile_;

    void generateTree();

    QString getTitle(QString htmlFile);


  private slots:
    void loadFinished( bool ok ); 
    void goHome();
    void treeClicked(QTreeWidgetItem* item, int column );
};

