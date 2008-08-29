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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//
// C++ Interface: prototypeVec3d
//
// Description: 
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#define PROTOTYPEMATRIX4X4_C 

#include "prototypeMatrix4x4.hh"
 
prototypeMatrix4x4::prototypeMatrix4x4(QObject *parent ) : 
    QObject(parent) 
{
  
}

QString prototypeMatrix4x4::toString() const { 
  
  QString result = "Matrix4x4 : \n";
  
  for ( uint i = 0 ; i < 4 ; ++i ) {
    for ( uint j = 0 ; j < 4 ; ++j ) {
      result = result + thisObject().property(QString::number(j) + QString::number(i)).toString() + "  ";
    }
    
    //Dont add a newline at the end of the Matrix
    if ( i != 3 )
      result += "\n";
  }
    
  return result;
}
      
