//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 1909 $
//   $Author: wilden $
//   $Date: 2008-06-03 18:45:21 +0200 (Tue, 03 Jun 2008) $
//
//=============================================================================




#include "loggerWidget.hh"

LoggerWidget::LoggerWidget( QWidget *parent)
  : QTextEdit(parent)
{
}

void LoggerWidget::keyPressEvent (QKeyEvent * _event ) {
  // Return key event to parent if not one of the standard key combinations ( ... Core )
  if ( ( (_event->modifiers() & Qt::ControlModifier ) && ( _event->key() == Qt::Key_C ) ) ||
       ( (_event->modifiers() & Qt::ControlModifier ) && ( _event->key() == Qt::Key_A ) ) ) {
      QTextEdit::keyPressEvent(_event);
  } else
    _event->ignore();
}

