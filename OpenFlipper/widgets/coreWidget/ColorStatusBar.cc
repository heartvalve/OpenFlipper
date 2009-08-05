/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
\*===========================================================================*/


#include "ColorStatusBar.hh"

ColorStatusBar::ColorStatusBar(QWidget* parent) : QStatusBar(parent)
{
  connect (this, SIGNAL(messageChanged(const QString&)), this, SLOT(slotMessageChanged(const QString &)) );
  colored_ = false;
}

void ColorStatusBar::showMessage(const QString & message, int timeout)
{
  QStatusBar::showMessage(message, timeout);
}

void ColorStatusBar::showMessage(const QString & message, QColor _color, int timeout)
{
  QPalette pal = palette();
  pal.setColor(QPalette::Active, QPalette::WindowText, _color );
  setPalette(pal);

  colored_ = true;

  QStatusBar::showMessage(message, timeout);
}

void ColorStatusBar::slotMessageChanged(const QString & message)
{
  if ( !colored_ ){

    QPalette pal = palette();
    pal.setColor(QPalette::Active, QPalette::WindowText, QColor(Qt::black) );
    setPalette(pal);

  } else 
    colored_ = false;
}

