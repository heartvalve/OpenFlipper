#include "ui_subdividerControls.hh"
#include <QtGui>

class subdividerToolbarWidget : public QWidget, public Ui::SubdividerControls
{
  Q_OBJECT

  public:
    subdividerToolbarWidget(QWidget *parent = 0);
};

