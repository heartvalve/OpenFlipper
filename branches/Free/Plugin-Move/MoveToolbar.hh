#include "ui_moveControls.hh"
#include <QtGui>

class moveToolbarWidget : public QWidget, public Ui::moveControls
{
  Q_OBJECT

  public:
    moveToolbarWidget(QWidget *parent = 0);
};

