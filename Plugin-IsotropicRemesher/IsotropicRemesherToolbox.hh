#include "ui_toolbox.hh"
#include <QtGui>
#include <iostream>

class IsotropicRemesherToolBox : public QWidget, public Ui::IsotropicRemesherTool
{
  Q_OBJECT

  signals:
    // emitted when the widget will get visible
    void showing();

  public:
    IsotropicRemesherToolBox(QWidget *parent = 0);

  protected:
    void showEvent ( QShowEvent * _event );
};

