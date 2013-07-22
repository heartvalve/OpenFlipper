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
#include "SelectionObjectMarker.hh"

//******************************************************************************

const ACG::Vec4f disabled_color (0.4f,0.4f,0.4f,1.0f);
const ACG::Vec4f selected_color (0.0f,1.0f,0.0f,0.5f);

//******************************************************************************

bool SelectionObjectMarker::stencilRefForObject(BaseObjectData * _obj, GLuint & _reference)
{
  if (_obj->flag("vsi_objectId_disabled"))
  {
    _reference = 1;
    return true;
  }
  else if (_obj->flag("vsi_objectId_selected"))
  {
    _reference = 2;
    return true;
  }

  return false;
}

bool SelectionObjectMarker::blendForStencilRefNumber(GLuint _reference, GLenum & _src, GLenum & _dst, ACG::Vec4f & _color)
{
  switch (_reference)
  {
    case 1:
      _src = GL_ZERO;
      _dst = GL_SRC_COLOR;
      _color = disabled_color;
      return true;
    case 2:
      _src = GL_SRC_ALPHA;
      _dst = GL_ONE_MINUS_SRC_ALPHA;
      _color = selected_color;
      return true;
    default:
      return false;
  }
  return false;
}

