//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2009 by Computer Graphics Group, RWTH Aachen
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
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/common/BaseObjectData.hh>
#include "MoveObjectMarker.hh"

//******************************************************************************

const ACG::Vec4f base_color (0.4,0.4,0.4,1.0);

//******************************************************************************

bool MoveObjectMarker::stencilRefForObject(BaseObjectData * _obj, GLuint & _reference)
{
  _reference = 1;
  return true;
}

bool MoveObjectMarker::blendForStencilRefNumber(GLuint _reference, GLenum & _src, GLenum & _dst, ACG::Vec4f & _color)
{
  if (_reference != 1)
    return false;

  _src = GL_ZERO;
  _dst = GL_SRC_COLOR;

  _color = base_color;
  return true;
}

