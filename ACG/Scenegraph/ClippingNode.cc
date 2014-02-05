/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS ClippingNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "ClippingNode.hh"

#include <QImage>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {

  
//== IMPLEMENTATION ========================================================== 


void
ClippingNode::set_plane(const Vec3f& _position, 
			const Vec3f& _normal, 
			float _eps)
{
  position_    = _position;
  normal_      = _normal; normal_.normalize();
  slice_width_ = _eps;
  

  // one clipping plane
  if (slice_width_ == 0.0)
  {
    plane0_[0] = normal_[0];
    plane0_[1] = normal_[1];
    plane0_[2] = normal_[2];
    plane0_[3] = -(normal_|position_);
  }


  // two planes -> slice
  else 
  {
    float d = -(normal_|position_);
    if (d > 0) { normal_ = -normal_; d = -d; }
    
    plane0_[0] = normal_[0];
    plane0_[1] = normal_[1];
    plane0_[2] = normal_[2];
    plane0_[3] = d + 0.5f*slice_width_;

    plane1_[0] = -normal_[0];
    plane1_[1] = -normal_[1];
    plane1_[2] = -normal_[2];
    plane1_[3] = -(d - 0.5f*slice_width_);
  }


  set_offset(offset_);
}


//----------------------------------------------------------------------------


void 
ClippingNode::set_offset(float _offset)
{
  offset_ = _offset;

  offset_plane0_[0] = plane0_[0];
  offset_plane0_[1] = plane0_[1];
  offset_plane0_[2] = plane0_[2];
  offset_plane0_[3] = plane0_[3] - offset_;

  offset_plane1_[0] = plane1_[0];
  offset_plane1_[1] = plane1_[1];
  offset_plane1_[2] = plane1_[2];
  offset_plane1_[3] = plane1_[3] + offset_;
}


//----------------------------------------------------------------------------
  

void ClippingNode::enter(GLState& /* _state */ , const DrawModes::DrawMode& /* _drawmode */ ) 
{
  // one clipping plane
  if (slice_width_ == 0.0)
  {
    glClipPlane(GL_CLIP_PLANE0, offset_plane0_);
    ACG::GLState::enable(GL_CLIP_PLANE0);
  }


  // two planes -> slice
  else 
  {
    glClipPlane(GL_CLIP_PLANE0, offset_plane0_);
    ACG::GLState::enable(GL_CLIP_PLANE0);
    glClipPlane(GL_CLIP_PLANE1, offset_plane1_);
    ACG::GLState::enable(GL_CLIP_PLANE1);
  }
}


//----------------------------------------------------------------------------


void ClippingNode::leave(GLState& /* _state */ , const DrawModes::DrawMode& /* _drawmode */ )
{
  ACG::GLState::disable(GL_CLIP_PLANE0);
  if (slice_width_ > 0.0)
    ACG::GLState::disable(GL_CLIP_PLANE1);
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
