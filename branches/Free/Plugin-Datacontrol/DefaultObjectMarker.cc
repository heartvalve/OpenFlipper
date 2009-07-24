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
#include <OpenFlipper/common/BaseObject.hh>
#include "DefaultObjectMarker.hh"

//******************************************************************************

const ACG::Vec4f base_color (0.6,0.6,0.6,1.0);
const ACG::Vec4f source_color (0.6,0.6,1.0,1.0);
const ACG::Vec4f source_target_color (0.6,1.0,1.0,1.0);

//******************************************************************************

bool DefaultObjectMarker::stencilRefForObject(BaseObject * _obj, GLuint & _reference)
{
  if (!_obj->target() && !_obj->source())
  {
    _reference = 1;
    return true;
  }
  else if (!_obj->target() && _obj->source())
  {
    _reference = 2;
    return true;
  }
  else if (_obj->target() && _obj->source())
  {
    _reference = 3;
    return true;
  }

  return false;
}

bool DefaultObjectMarker::blendForStencilRefNumber(GLuint _reference, GLenum & _src, GLenum & _dst, ACG::Vec4f & _color)
{
  _src = GL_ZERO;
  _dst = GL_SRC_COLOR;

  switch (_reference)
  {
    case 1:
      _color = base_color;
      return true;
    case 2:
      _color = source_color;
      return true;
    case 3:
      _color = source_target_color;
      return true;
    default:
      return false;
  }
  return false;
}

