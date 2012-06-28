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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




//
// C++ Interface: prototypeVec3d
//
// Description: 
//
//
// Author: Jan Möbius <moebius@cs.rwth-aachen.de>, (C) 2007
//

#ifndef PROTOTYPEVEC3D_HH 
#define PROTOTYPEVEC3D_HH 


#include <QtCore/QObject>
#include <QtScript/QtScript>
 
class prototypeVec3d : public QObject , public QScriptable
{
     Q_OBJECT
 
 public:
     prototypeVec3d(QObject *parent = 0);

 public slots:
     // Multiplies the given vector with the scalar
     void multiply(QScriptValue _scalar);

     // Adds another vector to this vector
     void add(QScriptValue _vector);

     // Subtracts another vector from this vector
     void sub(QScriptValue _vector);

 public Q_SLOTS:
     QString toString() const;

};
 
      
#endif // PROTOTYPEVEC3D_HH

