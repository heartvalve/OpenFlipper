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

#define PROTOTYPEVEC3D_C 

#include "prototypeVec3d.hh"
 
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
      
