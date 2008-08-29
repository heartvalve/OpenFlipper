#include "ui_helpWidget.hh"
#include <QtGui>

class HelpWidget : public QMainWindow, public Ui::HelpWidget
{
  Q_OBJECT

  public:
    HelpWidget(QWidget *parent = 0, bool _user = true);
};

