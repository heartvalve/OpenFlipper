#include <ui_addAnimation.hh>
#include <QtGui>

class AddAnimationDialog : public QDialog, public Ui::AddAnimation
{
  Q_OBJECT

  public:
    AddAnimationDialog(QWidget *parent = 0);
};

