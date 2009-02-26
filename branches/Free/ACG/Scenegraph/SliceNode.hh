//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




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
    XZ_PLANE = 4,
  };

  SliceNode( BaseNode    * _parent = 0,
	     std::string   _name   = "<SliceNode>" );

  virtual ~SliceNode();

  void set_visible_box( const Vec3f & _bmin,
			const Vec3f & _bmax );
  void set_texture_box( const Vec3f & _bmin,
			const Vec3f & _bmax );

  ACG_CLASSNAME( SliceNode );

  unsigned int availableDrawModes() const;

  void boundingBox( Vec3f & _bbMin, Vec3f & _bbMax );

  void draw( GLState& _state, unsigned int _drawMode );
  void draw_frame() const;
  void draw_planes() const;

  void pick( GLState& _state, PickTarget _target );

  bool is_enabled ( Plane _plane ) const;
  void set_enabled( Plane _plane );

  Vec3f cursor() const;
  void  set_cursor( const Vec3f & _cursor );

  void view_frame( bool _view_frame );

private:

  Vec3f        visible_min_;
  Vec3f        visible_max_;

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
