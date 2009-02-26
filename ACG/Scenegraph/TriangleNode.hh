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
//  CLASS TriangleNode
//
//=============================================================================

#ifndef ACG_TRIANGLENODE_HH
#define ACG_TRIANGLENODE_HH

//=============================================================================

#include "BaseNode.hh"

#include <vector>

//=============================================================================

namespace ACG {
namespace SceneGraph {
	      
//=============================================================================


class ACGDLLEXPORT TriangleNode : public BaseNode
{

public:

  typedef std::vector<ACG::Vec3f> PointVector;
   
  TriangleNode( BaseNode*    _parent=0,
		std::string  _name="<TriangleNode>" );
  virtual ~TriangleNode();
  
  ACG_CLASSNAME(TriangleNode);


  unsigned int availableDrawModes() const;

  void boundingBox(Vec3f& _bbMin, Vec3f& _bbMax);
  void draw(GLState& _state, unsigned int _drawMode);
  void pick(GLState& _state, PickTarget _target);

  void add_triangle( const ACG::Vec3f & _p0,
		     const ACG::Vec3f & _p1,
		     const ACG::Vec3f & _p2 )
  {
    point_.push_back( _p0 );
    point_.push_back( _p1 );
    point_.push_back( _p2 );

    ACG::Vec3f n = ( _p1 - _p0 ) % ( _p2 - _p1 );
    if ( n.norm() > 0.00001 )
      n.normalize();
    else
      n = Vec3f( 0, 0, 0 );
    normal_.push_back( n );
  }

  void clear()
  {
    point_.clear();
    normal_.clear();
  }

  int n_triangles() const { return point_.size() / 3; }

  void triangle( int _i,
		 ACG::Vec3f & _p0,
		 ACG::Vec3f & _p1,
		 ACG::Vec3f & _p2 ) const
  {
    _p0 = point_[ 3 * _i + 0 ];
    _p1 = point_[ 3 * _i + 1 ];
    _p2 = point_[ 3 * _i + 2 ];
  }

protected:

  enum FaceMode { FACE_NORMALS, FACE_COLORS, PER_VERTEX };

  void draw_vertices();
  void draw_faces();
  void draw_wireframe();

  PointVector point_;
  PointVector normal_;

};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_TRIMESHNODE_HH defined
//=============================================================================
