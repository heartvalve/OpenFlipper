#ifndef COLORSTATUSBAR_HH
#define COLORSTATUSBAR_HH

#include <QStatusBar>

class ColorStatusBar : public QStatusBar
{
  Q_OBJECT

  public:
    ColorStatusBar(QWidget* parent = 0 );

  public slots:
    void showMessage ( const QString& message, int timeout = 0 );
    void showMessage ( const QString& message, QColor _color, int timeout = 0 );

  private slots:
    void slotMessageChanged ( const QString & message );

  private:
    bool colored_;
};

#endif //COLORSTATUSBAR_HH
