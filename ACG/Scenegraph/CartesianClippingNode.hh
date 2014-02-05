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
//  CLASS CartesianClippingNode
//
//=============================================================================

#ifndef ACG_CARTESIANCLIPPINGNODE_HH
#define ACG_CARTESIANCLIPPINGNODE_HH

//=============================================================================

#include "BaseNode.hh"
#include <string>
#include <fstream>

//=============================================================================

namespace ACG {
namespace SceneGraph {
  
//=============================================================================

  
class ACGDLLEXPORT CartesianClippingNode : public BaseNode
{
public:

  enum Plane {
    XY_PLANE = 0,
    YZ_PLANE = 1,
    XZ_PLANE = 2,
    NONE     = 3,
  };

  CartesianClippingNode( BaseNode*           _parent = 0,
			 const std::string&  _name = "<ClippingNode>" );

  /// Destructor.
  virtual ~CartesianClippingNode() {}

  /// set class name
  ACG_CLASSNAME(CartesianClippingNode);

  /// begin clipping
  void enter( GLState & _state, const DrawModes::DrawMode& _drawmode );

  /// stop clipping
  void leave( GLState & _state, const DrawModes::DrawMode& _drawmode );

  /// set position
  void set_cursor( const Vec3f & _pos );

  /// get position
  const Vec3f & cursor() const;

  void set_enabled( Plane _plane );
  bool is_enabled ( Plane _plane ) const;

private:

  Vec3f cursor_;
  Plane enabled_;

  GLdouble  xy_plane_[4], yz_plane_[4], xz_plane_[4];
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_CARTESIANCLIPPINGNODE_HH defined
//=============================================================================

