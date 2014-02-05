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
//  CLASS ClippingNode
//
//=============================================================================


#ifndef ACG_CLIPPING_NODE_HH
#define ACG_CLIPPING_NODE_HH


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include <string>
#include <fstream>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {
  

//== CLASS DEFINITION =========================================================

  
/** \class ClippingNode ClippingNode.hh <ACG/Scenegraph/ClippingNode.hh>

    Set material and some other stuff for this node and all its
    children.  All changes will be done in the enter() method undone
    in the leave() method.
**/

class ACGDLLEXPORT ClippingNode : public BaseNode
{
public:

  /// Default constructor. Applies all properties.
  ClippingNode( BaseNode*           _parent = 0,
		const std::string&  _name = "<ClippingNode>" )
    : BaseNode(_parent, _name),
      slice_width_(0),
      offset_(0)
  {
    offset_plane0_[0] = 0.0;
    offset_plane0_[1] = 0.0;
    offset_plane0_[2] = 0.0;
    offset_plane0_[3] = 0.0;

  }


  /// Destructor.
  virtual ~ClippingNode() {}

  /// set class name
  ACG_CLASSNAME(ClippingNode);

  /// set texture
  void enter(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /// restores original texture (or no-texture)
  void leave(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /// set position and normal of plane
  void set_plane(const Vec3f& _position, const Vec3f& _normal, float _eps=0.0);

  /// get position
  const Vec3f& position() const { return position_; }

  /// get normal
  const Vec3f& normal() const { return normal_; }

  /// get slice width
  float slice_width() const { return slice_width_; }

  /// sweep plane along normal by _dist
  void set_offset(float _dist);

  
private:

  Vec3f     position_, normal_;
  GLdouble  plane0_[4], plane1_[4], offset_plane0_[4], offset_plane1_[4];
  float     slice_width_, offset_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_CLIPPING_NODE_HH defined
//=============================================================================
