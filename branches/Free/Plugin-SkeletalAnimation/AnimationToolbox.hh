#include <ui_animationControls.hh>

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif


class AnimationToolboxWidget : public QWidget, public Ui::AnimationControls
{
  Q_OBJECT

  public:
    AnimationToolboxWidget(QWidget *parent = 0);
};

