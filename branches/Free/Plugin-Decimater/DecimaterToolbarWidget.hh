#ifndef DECIMATERTOOLBARWIDGET_HH
#define DECIMATERTOOLBARWIDGET_HH

#include "ui_decimaterToolbar.hh"
#include <QtGui>

class DecimaterToolbarWidget : public QWidget, public Ui::DecimaterToolbar
{
  Q_OBJECT

  public:
    DecimaterToolbarWidget(QWidget *parent = 0);
};

#endif //DECIMATERTOOLBARWIDGET_HH

