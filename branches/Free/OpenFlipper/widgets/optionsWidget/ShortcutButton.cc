/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/


#include "ShortcutButton.hh"

#include <iostream>

///Constructor
ShortcutButton::ShortcutButton(QWidget* _parent) :
QPushButton(_parent),
key_(-1),
oldKey_(-1),
checked_(false),
firstPress_(true)
{
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

  if (key_ == -1){
    keyString = tr("unassigned");
  }else if (key_ == Qt::Key_AltGr || key_ == Qt::Key_Alt || key_ == Qt::Key_Control || key_ == Qt::Key_Shift || key_ == Qt::Key_Meta){
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

