#include "ui_aboutWidget.hh"
#include <QtGui>

class AboutWidget : public QMainWindow, public Ui::AboutWidget
{
  Q_OBJECT

  public:
    AboutWidget(QWidget *parent = 0);
};

