/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include "PlaneType.hh"


//== IMPLEMENTATION ==========================================================


void Plane::setPlane(const ACG::Vec3f& _position, const ACG::Vec3f& _xDirection, const ACG::Vec3f& _yDirection)
{
  position   = _position;
  xDirection = _xDirection;
  yDirection = _yDirection;
  normal     = (_xDirection % _yDirection).normalize();
}

//----------------------------------------------------------------

void Plane::setPlane(const ACG::Vec3f& _position, const ACG::Vec3f& _normal)
{

  //find a non zero component
  int comp = -1;
  for (int i=0; i < 3; i++)
    if ( _normal[i] != 0.0 ){
      comp = i;
      break;
    }

  if (comp == -1){
    std::cerr << "PlaneNode: normal is invalid!" << std::endl;
    return;
  }

  //compute orthogonal vectors in the plane
  xDirection[comp] = (-_normal[ (comp + 1) % 3 ] - _normal[(comp + 2) % 3]) / _normal[comp];
  xDirection[ (comp + 1) % 3 ] = 1;
  xDirection[ (comp + 2) % 3 ] = 1;
  xDirection = xDirection.normalize();

  yDirection = _normal % xDirection;
  yDirection = yDirection.normalize();

  position = _position;
  normal   = _normal;
}

//----------------------------------------------------------------

void Plane::transform(const ACG::Matrix4x4d& _mat)
{
  position    = _mat.transform_point(position);
  xDirection  = _mat.transform_vector(xDirection);
  yDirection  = _mat.transform_vector(yDirection);

  normal      = (xDirection % yDirection).normalize();
}

//----------------------------------------------------------------

void Plane::setSize(double _xDirection, double _yDirection)
{
  xDirection = xDirection.normalize() * _xDirection;
  yDirection = yDirection.normalize() * _yDirection;
}


//=============================================================================
