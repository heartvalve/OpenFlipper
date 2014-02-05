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
//  CLASS CoordFrameNode
//
//=============================================================================


#ifndef ACG_COORDFRAMENODE_HH
#define ACG_COORDFRAMENODE_HH


//== INCLUDES =================================================================


#include "MaterialNode.hh"
#include <vector>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================

	      

/** \class CoordFrameNode CoordFrameNode.hh <ACG/.../CoordFrameNode.hh>

    Scenegraph Node.
  
    A more elaborate description follows.
**/
class ACGDLLEXPORT CoordFrameNode : public MaterialNode
{
public:
   
  /// Default constructor
  CoordFrameNode(BaseNode*  _parent=0,
	     const std::string&  _name="<CoordFrameNode>" );
 
  /// Destructor
  ~CoordFrameNode() {}


  /// implement className()
  ACG_CLASSNAME(CoordFrameNode);
  /// return available draw modes
  DrawModes::DrawMode  availableDrawModes() const;
  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);
  /// drawing the primitive
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);


  /// update bounding box (compute in from BB of children)
  void update_bounding_box();
  /// set bounding box
  void set_bounding_box(const Vec3f& _bb_min, const Vec3f& _bb_max);
  /// get bounding box
  const Vec3d& bb_min() const { return bb_min_; }
  /// get bounding box
  const Vec3d& bb_max() const { return bb_max_; }


  /// get x-plane container
  const std::vector<float>&  x_planes() const { return x_planes_; }
  /// get y-plane container
  const std::vector<float>&  y_planes() const { return y_planes_; }
  /// get z-plane container
  const std::vector<float>&  z_planes() const { return z_planes_; }


  /// set x-plane container
  void set_x_planes(const std::vector<float>& _planes) { x_planes_ = _planes; }
  /// set y-plane container
  void set_y_planes(const std::vector<float>& _planes) { y_planes_ = _planes; }
  /// set z-plane container
  void set_z_planes(const std::vector<float>& _planes) { z_planes_ = _planes; }

  
  /// add (x == _x)-plane
  void add_x_plane(float _x) { x_planes_.push_back(_x); }
  /// add (y == _y)-plane
  void add_y_plane(float _y) { y_planes_.push_back(_y); }
  /// add (z == _z)-plane
  void add_z_plane(float _z) { z_planes_.push_back(_z); }


  /// del (x == _x)-plane
  void del_x_plane(float _x) { 
    x_planes_.erase(std::find(x_planes_.begin(), x_planes_.end(), _x));
  }
  /// del (y == _y)-plane
  void del_y_plane(float _y) { 
    y_planes_.erase(std::find(y_planes_.begin(), y_planes_.end(), _y));
  }
  /// del (z == _z)-plane
  void del_z_plane(float _z) { 
    z_planes_.erase(std::find(z_planes_.begin(), z_planes_.end(), _z));
  }



 

private:

  /// Copy constructor (not used)
  CoordFrameNode(const CoordFrameNode& _rhs);
  /// Assignment operator (not used)
  CoordFrameNode& operator=(const CoordFrameNode& _rhs);


  // extend of bounding box
  Vec3d bb_min_, bb_max_;

  // planes in x-, y-, z-direction
  std::vector<float>  x_planes_, y_planes_, z_planes_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_COORDFRAMENODE_HH defined
//=============================================================================

