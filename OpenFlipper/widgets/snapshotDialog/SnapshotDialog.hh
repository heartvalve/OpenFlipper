#include "ui_SnapshotDialog.hh"
#include <QtGui>

class SnapshotDialog : public QDialog, public Ui::SnapshotDialog
{
  Q_OBJECT

  public:
    SnapshotDialog(QString _suggest, bool _captureViewers, QWidget *parent = 0);

  private:
    bool captureViewers_;

  private slots:
    void slotOk();
    void findFile();

  signals:
    void resizeViewers(int _width, int _height);
    void resizeApplication(int _width, int _height);
};

