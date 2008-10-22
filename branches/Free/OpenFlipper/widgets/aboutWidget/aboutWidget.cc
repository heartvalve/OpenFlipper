#include "aboutWidget.hh"
#include <QtGui>

#include <OpenFlipper/common/GlobalOptions.hh>

AboutWidget::AboutWidget(QWidget *parent )
    : QMainWindow(parent)
{
  setupUi(this);

  QIcon icon;
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"window-close.png");
  actionClose->setIcon(icon);

  connect( actionClose , SIGNAL(triggered() ) , this, SLOT(hide()) );
  connect( closeButton , SIGNAL(clicked()   ) , this, SLOT(hide()) );
}
