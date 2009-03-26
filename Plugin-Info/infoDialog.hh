#include "ui_infoWidget.hh"
#include <QtGui>

class InfoDialog : public QDialog, public Ui::InfoWidget
{
  Q_OBJECT

  public:
    InfoDialog(QWidget *parent = 0);
};

