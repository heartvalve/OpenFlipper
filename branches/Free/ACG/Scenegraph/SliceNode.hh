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
//  CLASS SliceNode
//
//=============================================================================

#ifndef ACG_SCLICENODE_HH
#define ACG_SCLICENODE_HH

//=============================================================================

#include "BaseNode.hh"
#include "DrawModes.hh"

#include <string>

//=============================================================================

namespace ACG {
namespace SceneGraph {

//=============================================================================


class ACGDLLEXPORT SliceNode : public BaseNode
{

public:

  enum Plane {
    NONE     = 1,
    XY_PLANE = 2,
    YZ_PLANE = 3,
    XZ_PLANE = 4
  };

  SliceNode( BaseNode    * _parent = 0,
             std::string   _name   = "<SliceNode>" );

  virtual ~SliceNode();

  void set_visible_box( const Vec3f & _bmin,
                        const Vec3f & _bmax );
  void set_texture_box( const Vec3f & _bmin,
                        const Vec3f & _bmax );

  ACG_CLASSNAME( SliceNode );

  DrawModes::DrawMode availableDrawModes() const;

  void boundingBox( Vec3d & _bbMin, Vec3d & _bbMax );

  void draw( GLState& _state, const DrawModes::DrawMode& _drawMode );
  void draw_frame() const;
  void draw_planes() const;

  void pick( GLState& _state, PickTarget _target );

  bool is_enabled ( Plane _plane ) const;
  void set_enabled( Plane _plane );

  Vec3f cursor() const;
  void  set_cursor( const Vec3f & _cursor );

  void view_frame( bool _view_frame );

private:

  Vec3d        visible_min_;
  Vec3d        visible_max_;

  Vec3f        texture_min_;
  Vec3f        texture_max_;

  Vec3f        cursor_;

  bool         view_frame_;

  Plane enabled_;

};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_SCLICENODE_HH
//=============================================================================
