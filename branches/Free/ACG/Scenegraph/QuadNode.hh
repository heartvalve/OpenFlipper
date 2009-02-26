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
//  CLASS QuadNode
//
//=============================================================================

#ifndef ACG_QUADNODE_HH
#define ACG_QUADNODE_HH

//=============================================================================

#include "BaseNode.hh"
#include <vector>

//=============================================================================

namespace ACG {
namespace SceneGraph {
	      
//=============================================================================


class ACGDLLEXPORT QuadNode : public BaseNode
{

public:

  typedef std::vector<ACG::Vec3f> PointVector;
   
  QuadNode( BaseNode*    _parent=0,
		std::string  _name="<QuadNode>" );
  virtual ~QuadNode();
  
  ACG_CLASSNAME(QuadNode);


  unsigned int availableDrawModes() const;

  void boundingBox(Vec3f& _bbMin, Vec3f& _bbMax);
  void draw(GLState& _state, unsigned int _drawMode);
  void pick(GLState& _state, PickTarget _target);

  void add_quad( const ACG::Vec3f & _p0,
		 const ACG::Vec3f & _p1,
		 const ACG::Vec3f & _p2,
		 const ACG::Vec3f & _p3 )
  {
    point_.push_back( _p0 );
    point_.push_back( _p1 );
    point_.push_back( _p2 );  
    point_.push_back( _p3 );

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
#endif // ACG_QUADNODE_HH defined
//=============================================================================
