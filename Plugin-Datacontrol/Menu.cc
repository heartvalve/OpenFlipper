#include "Menu.hh"

Menu::Menu(QWidget* _parent) : QMenu(_parent)
{
  pressed_ = false;
}

void Menu::mouseReleaseEvent(QMouseEvent * _event)
{
  if (pressed_)
    QMenu::mouseReleaseEvent(_event);

  pressed_ = false;
}

void Menu::mousePressEvent(QMouseEvent * _event)
{
  pressed_ = true;
  QMenu::mousePressEvent(_event);
}
