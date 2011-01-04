#include <ui_animationControls.hh>
#include <QtGui>

class AnimationToolboxWidget : public QWidget, public Ui::AnimationControls
{
  Q_OBJECT

  public:
    AnimationToolboxWidget(QWidget *parent = 0);
};

