#include "ui_CatmullClarkToolbarBase.hh"
#include <QtGui>

class CatmullClarkToolbar : public QWidget, public Ui::CatmullClarkToolbarBase
{
  Q_OBJECT

public:
  CatmullClarkToolbar(QWidget * parent = 0)
    : QWidget(parent)
  {
    setupUi(this);
  }
};
