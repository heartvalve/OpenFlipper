#include "ui_PropertyVisToolbarBase.hh"
#include <QtGui>

class PropertyVisToolbar : public QWidget, public Ui::PropertyVisToolbarBase
{
  Q_OBJECT

public:
  PropertyVisToolbar(QWidget * parent = 0)
    : QWidget(parent)
  {
    setupUi(this);
  }
  
  signals:
    void widgetShown();
        
  protected:
    void showEvent ( QShowEvent* /*event*/ ) {
      emit widgetShown();
    }
};
