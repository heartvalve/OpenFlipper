#include <QMenu>
#include <QMouseEvent>

class Menu : public QMenu
{
    bool pressed_;
  public:
    Menu(QWidget* _parent = 0);

  protected:
    void mousePressEvent(QMouseEvent* _event);
    void mouseReleaseEvent(QMouseEvent* _event);
};