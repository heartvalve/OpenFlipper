#include "ui_smootherToolbar.hh"
#include <QtGui>

class SmootherToolbarWidget : public QWidget, public Ui::SmootherToolbar
{
  Q_OBJECT

  public:
    SmootherToolbarWidget(QWidget *parent = 0);
};

