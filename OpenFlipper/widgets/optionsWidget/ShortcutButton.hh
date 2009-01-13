#ifndef SHORTCUTBUTTON_HH
#define SHORTCUTBUTTON_HH

#include <QPushButton>
#include <QKeyEvent>

class ShortcutButton : public QPushButton
{
  Q_OBJECT

  signals:
    void keyChanged();

  private:
    int                    key_;
    Qt::KeyboardModifiers  modifiers_;

    int                    oldKey_;
    Qt::KeyboardModifiers  oldModifiers_;

    void showCurrentShortcut();

    bool checked_;
    bool firstPress_;

  protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
  public:
    ShortcutButton(QWidget* _parent = 0);

    void setCurrentShortcut(int _key, Qt::KeyboardModifiers _modifiers);

    int                    key();
    Qt::KeyboardModifiers  modifiers();
};

#endif //SHORTCUTBUTTON_HH

