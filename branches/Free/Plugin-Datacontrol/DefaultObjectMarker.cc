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

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/common/BaseObjectData.hh>
#include "DefaultObjectMarker.hh"

//******************************************************************************

const ACG::Vec4f base_color (0.6f,0.6f,0.6f,1.0f);
const ACG::Vec4f source_color (0.6f,0.6f,1.0f,1.0f);
const ACG::Vec4f target_color (1.0f,1.0f,1.0f,1.0f);
const ACG::Vec4f source_target_color (0.6f,1.0f,1.0f,1.0f);

//******************************************************************************

bool DefaultObjectMarker::stencilRefForObject(BaseObjectData * _obj, GLuint & _reference)
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
  else if (_obj->target() && !_obj->source())
  {
    _reference = 3;
    return true;
  }
  else if (_obj->target() && _obj->source())
  {
    _reference = 4;
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
      _color = target_color;
      return true;
    case 4:
      _color = source_target_color;
      return true;
    default:
      return false;
  }
  return false;
}

