#include "ui_polylineToolbar.hh"

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif


class PolyLineToolbarWidget : public QWidget, public Ui::PolyLineToolbar
{
  Q_OBJECT

  public:
  PolyLineToolbarWidget(QWidget *parent = 0);
};

