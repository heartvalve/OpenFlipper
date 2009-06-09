#include "ui_VideoCaptureDialog.hh"
#include <QtGui>

class VideoCaptureDialog : public QDialog, public Ui::VideoCaptureDialog
{
  Q_OBJECT

  public:
    VideoCaptureDialog(QWidget *parent = 0);

  private slots:
    void slotStartVideoCapture();
    void findFile();

  signals:
    void startVideoCapture(QString _baseName, int _fps, bool _captureViewers);
};

