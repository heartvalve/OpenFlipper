#include "ui_polylineToolbar.hh"
#include <QtGui>

class PolyLineToolbarWidget : public QWidget, public Ui::PolyLineToolbar
{
  Q_OBJECT

  public:
  PolyLineToolbarWidget(QWidget *parent = 0);
};

