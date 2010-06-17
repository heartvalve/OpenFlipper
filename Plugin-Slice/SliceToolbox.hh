#include "ui_toolbox.hh"
#include <QtGui>

class SliceToolBox : public QWidget, public Ui::SliceTool
{
  Q_OBJECT

  public:
    SliceToolBox(QWidget *parent = 0);
};

