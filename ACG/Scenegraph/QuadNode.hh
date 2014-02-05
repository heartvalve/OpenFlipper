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


  DrawModes::DrawMode  availableDrawModes() const;

  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);
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
