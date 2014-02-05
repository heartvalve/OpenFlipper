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
*   $Revision: 10413 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2010-12-09 15:40:04 +0100 (Thu, 09 Dec 2010) $                     *
*                                                                            *
\*===========================================================================*/

#include <iostream>
#include "OpenFlipperQSettings.hh"

QVariant OpenFlipperQSettings::value(const QString& key, const QVariant& defaultValue) const
{
  QVariant var = QSettings::value(key, defaultValue);
  if (!var.isValid())
    std::cerr << "Tried loading OpenFlipper settings value for " << key.toStdString() << ", but it was not found and no default value given!" << std::endl;
    

  #ifdef OPENFLIPPER_SETTINGS_DEBUG
    std::cerr << "Settings: Read key " << key.toStdString() << " with value " << var.toString().toStdString() << std::endl;
  #endif

  return var;
}


void OpenFlipperQSettings::setValue ( const QString & key, const QVariant & value ) {

  #ifdef OPENFLIPPER_SETTINGS_DEBUG
    std::cerr << "Settings: Set key " << key.toStdString() << " to value " << value.toString().toStdString() << std::endl;
  #endif

  QSettings::setValue(key,value);
}
