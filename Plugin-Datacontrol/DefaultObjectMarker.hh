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


#ifndef DEFAULTOBJECTMARKER_HH
#define DEFAULTOBJECTMARKER_HH

#include <OpenFlipper/BasePlugin/ViewObjectMarker.hh>

/** Object marker to visualize "target" and "source" object flags
*/
class DefaultObjectMarker : public ViewObjectMarker
{
  public:

    bool stencilRefForObject (BaseObject *_obj, GLuint &_reference);

    bool blendForStencilRefNumber (GLuint _reference, GLenum &_src, GLenum &_dst, ACG::Vec4f &_color);
};

#endif
