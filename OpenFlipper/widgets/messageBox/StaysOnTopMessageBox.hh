/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision: 17184 $                                                       *
*   $LastChangedBy: lim $                                                *
*   $Date: 2013-08-02 14:59:34 +0200 (Fri, 02 Aug 2013) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef STAYSONTOPMESSAGEBOX_HH
#define STAYSONTOPMESSAGEBOX_HH

#include <QMessageBox>

class StaysOnTopMessageBox : public QMessageBox {
  Q_OBJECT


  public:
    StaysOnTopMessageBox(QWidget* _parent) :
      QMessageBox(_parent)
  {
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
  }
    StaysOnTopMessageBox(Icon _icon,
                         const QString& _title,
                         const QString& _text,
                         StandardButtons _buttons = NoButton,
                         QWidget* _parent = 0,
                         Qt::WindowFlags _f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint) :
      QMessageBox(_icon, _title, _text, _buttons, _parent, _f | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint)
    {}
};


//=============================================================================
#endif // STAYSONTOPMESSAGEBOX_HH defined
//=============================================================================
