#include "ui_shaderWidget.hh"
#include <QtGui>

class ShaderWidget : public QWidget, public Ui::ShaderSettings
{
  Q_OBJECT

  public:
    ShaderWidget(QWidget *parent = 0);
};

