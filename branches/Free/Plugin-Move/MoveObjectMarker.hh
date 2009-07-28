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


#ifndef MOVEOBJECTMARKER_HH
#define MOVEOBJECTMARKER_HH

#include <OpenFlipper/common/ViewObjectMarker.hh>

/** Object marker to dim all Objects during Maipulator transformation
*/
class MoveObjectMarker : public ViewObjectMarker
{
  public:

    bool stencilRefForObject (BaseObjectData *_obj, GLuint &_reference);

    bool blendForStencilRefNumber (GLuint _reference, GLenum &_src, GLenum &_dst, ACG::Vec4f &_color);
};

#endif
