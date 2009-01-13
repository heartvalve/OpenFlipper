#include "ShortcutButton.hh"

#include <iostream>

///Constructor
ShortcutButton::ShortcutButton(QWidget* _parent) : QPushButton(_parent)
{
  key_ = -1;
  setCheckable(true);
}

///set the current shortcut
void ShortcutButton::setCurrentShortcut(int _key, Qt::KeyboardModifiers _modifiers)
{
  key_ = _key;
  modifiers_ = _modifiers;

  showCurrentShortcut();
}

///get current key
int ShortcutButton::key()
{
  return key_;
}

///get current modifiers
Qt::KeyboardModifiers ShortcutButton::modifiers()
{
  return modifiers_;
}

///key press
void ShortcutButton::keyPressEvent(QKeyEvent* event)
{
  if (firstPress_){
    oldKey_ = key_;
    oldModifiers_ = modifiers_;
  }

  firstPress_ = false;

  key_ = event->key();
  modifiers_ = event->modifiers();
}

///key release
void ShortcutButton::keyReleaseEvent(QKeyEvent* /*event*/)
{
  showCurrentShortcut();
  emit keyChanged();
}

///show the current shortcut
void ShortcutButton::showCurrentShortcut()
{

  firstPress_ = true;

  QString keyString;

  if (key_ == Qt::Key_AltGr || key_ == Qt::Key_Alt || key_ == Qt::Key_Control || key_ == Qt::Key_Shift || key_ == Qt::Key_Meta){
    keyString = QKeySequence( modifiers_ ).toString();
    keyString = keyString.left(keyString.size()-1);
  }else
    keyString = QKeySequence( key_ + modifiers_ ).toString();

  setText(keyString);
}

void ShortcutButton::focusInEvent(QFocusEvent* event){
  setChecked(true);
  setText("");
  QPushButton::focusInEvent(event);
}

void ShortcutButton::focusOutEvent(QFocusEvent* event){
  setChecked(false);

  key_ = oldKey_;
  modifiers_ = oldModifiers_;

  showCurrentShortcut();
  QPushButton::focusOutEvent(event);
}

void ShortcutButton::mousePressEvent(QMouseEvent* event){
  checked_ = isChecked();
  QPushButton::mousePressEvent(event);
}

void ShortcutButton::mouseReleaseEvent(QMouseEvent* event){
  QPushButton::mouseReleaseEvent(event);
  setChecked( checked_ );
}

