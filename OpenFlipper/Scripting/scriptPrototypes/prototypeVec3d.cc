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




//
// C++ Interface: prototypeVec3d
//
// Description: 
//
//
// Author: Jan Möbius <moebius@cs.rwth-aachen.de>, (C) 2007
//

#define PROTOTYPEVEC3D_C 

#include "prototypeVec3d.hh"

#include <iostream>
 
prototypeVec3d::prototypeVec3d(QObject *parent ) : 
    QObject(parent) 
{
  
}

QString prototypeVec3d::toString() const { 
  return "Vector : ( " + 
          thisObject().property("x").toString() + "," +
          thisObject().property("y").toString() + "," +
          thisObject().property("z").toString() + " )";
}
      
void prototypeVec3d::multiply(QScriptValue _scalar) {
  thisObject().setProperty("x", QScriptValue(thisObject().property("x").toNumber() * _scalar.toNumber()));
  thisObject().setProperty("y", QScriptValue(thisObject().property("y").toNumber() * _scalar.toNumber()));
  thisObject().setProperty("z", QScriptValue(thisObject().property("z").toNumber() * _scalar.toNumber()));
}

void prototypeVec3d::add(QScriptValue _vector) {
  thisObject().setProperty("x", QScriptValue(_vector.property("x").toNumber() + thisObject().property("x").toNumber()));
  thisObject().setProperty("y", QScriptValue(_vector.property("y").toNumber() + thisObject().property("y").toNumber()));
  thisObject().setProperty("z", QScriptValue(_vector.property("z").toNumber() + thisObject().property("z").toNumber()));
}


void prototypeVec3d::sub(QScriptValue _vector) {
  thisObject().setProperty("x", QScriptValue(thisObject().property("x").toNumber() - _vector.property("x").toNumber()));
  thisObject().setProperty("y", QScriptValue(thisObject().property("y").toNumber() - _vector.property("y").toNumber()));
  thisObject().setProperty("z", QScriptValue(thisObject().property("z").toNumber() - _vector.property("z").toNumber()));
}

void prototypeVec3d::zero() {
  thisObject().setProperty("x", QScriptValue(0.0));
  thisObject().setProperty("y", QScriptValue(0.0));
  thisObject().setProperty("z", QScriptValue(0.0));
}

QScriptValue prototypeVec3d::sprod(QScriptValue _vector) {
  return QScriptValue( thisObject().property("x").toNumber() * _vector.property("x").toNumber() +
                       thisObject().property("y").toNumber() * _vector.property("y").toNumber() +
                       thisObject().property("z").toNumber() * _vector.property("z").toNumber() );

}

QScriptValue prototypeVec3d::norm() {
  return QScriptValue( sqrt( thisObject().property("x").toNumber() * thisObject().property("x").toNumber() +
                             thisObject().property("y").toNumber() * thisObject().property("y").toNumber() +
                             thisObject().property("z").toNumber() * thisObject().property("z").toNumber() ) );
}

QScriptValue prototypeVec3d::sqrnorm() {
  return QScriptValue( thisObject().property("x").toNumber() * thisObject().property("x").toNumber() +
                       thisObject().property("y").toNumber() * thisObject().property("y").toNumber() +
                       thisObject().property("z").toNumber() * thisObject().property("z").toNumber() );
}

void prototypeVec3d::normalize() {
  double length = sqrt( thisObject().property("x").toNumber() * thisObject().property("x").toNumber() +
                        thisObject().property("y").toNumber() * thisObject().property("y").toNumber() +
                        thisObject().property("z").toNumber() * thisObject().property("z").toNumber() );

  if ( length != 0.0 ) {
    thisObject().setProperty("x", ( thisObject().property("x").toNumber() / length ) );
    thisObject().setProperty("y", ( thisObject().property("y").toNumber() / length ) );
    thisObject().setProperty("z", ( thisObject().property("z").toNumber() / length ) );
  }


}

