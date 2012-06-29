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

#define PROTOTYPEMATRIX4X4_C 

#include "prototypeMatrix4x4.hh"
#include <OpenFlipper/Scripting/scriptWrappers/vec3dWrapper.hh>
#include <iostream>

prototypeMatrix4x4::prototypeMatrix4x4(QObject *parent ) : 
    QObject(parent) 
{
  
}

QString prototypeMatrix4x4::toString() const { 
  
  QString result = "Matrix4x4 : \n";
  
  for ( uint i = 0 ; i < 4 ; ++i ) {
    for ( uint j = 0 ; j < 4 ; ++j ) {
      result = result + thisObject().property(QString::number(i) + QString::number(j)).toString() + "  ";
    }
    
    // Don't add a newline at the end of the Matrix
    if ( i != 3 )
      result += "\n";
  }
    
  return result;
}

QScriptValue prototypeMatrix4x4::transform_vector(QScriptValue _vector )
{

  // Convert from this objects properties to the matrix representation
  Matrix4x4 matrix;
  for ( uint i = 0 ; i < 4 ; ++i )
    for ( uint j = 0 ; j < 4 ; ++j )
      matrix(i,j) = thisObject().property(QString::number(i) + QString::number(j)).toNumber();

  // Convert the vector from scripting representation to C++
  ACG::Vec3d vector(_vector.property("x").toNumber(),_vector.property("y").toNumber(),_vector.property("z").toNumber());

  // Calculate result vector
  ACG::Vec3d result = matrix.transform_vector(vector);

  // Transform back to scriptable vector
  return QScriptValue( engine()->toScriptValue(result) );

}

QScriptValue prototypeMatrix4x4::transform_point(QScriptValue _vector )
{

  // Convert from this objects properties to the matrix representation
  Matrix4x4 matrix;
  for ( uint i = 0 ; i < 4 ; ++i )
    for ( uint j = 0 ; j < 4 ; ++j )
      matrix(i,j) = thisObject().property(QString::number(i) + QString::number(j)).toNumber();

  // Convert the vector from scripting representation to C++
  ACG::Vec3d vector(_vector.property("x").toNumber(),_vector.property("y").toNumber(),_vector.property("z").toNumber());

  // Calculate result vector
  const ACG::Vec3d result = matrix.transform_point(vector);

  // Transform back to scriptable vector
  return QScriptValue( engine()->toScriptValue(result)  );

}
      
