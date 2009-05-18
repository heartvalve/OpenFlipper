#include "ui_toolbox.hh"
#include <QtGui>

class DatacontrolToolboxWidget : public QWidget, public Ui::dctoolbox
{
  Q_OBJECT

  signals:
    void keyEvent( QKeyEvent* _event );

  public:
    DatacontrolToolboxWidget(QWidget* _parent = 0);

  protected:
    void keyPressEvent ( QKeyEvent* _event );
};

