#include "ui_scriptingWidget.hh"
#include <QtGui>

class ScriptWidget : public QMainWindow, public Ui::ScriptWidget
{
  Q_OBJECT

  public:
    ScriptWidget(QWidget *parent = 0);
};

