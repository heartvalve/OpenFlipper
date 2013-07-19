#include <ui_addAnimation.hh>

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif


class AddAnimationDialog : public QDialog, public Ui::AddAnimation
{
  Q_OBJECT

  public:
    AddAnimationDialog(QWidget *parent = 0);
};

