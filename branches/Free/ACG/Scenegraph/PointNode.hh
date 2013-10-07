/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS PointNode
//
//=============================================================================


#ifndef ACG_POINTNODE_HH
#define ACG_POINTNODE_HH


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include "DrawModes.hh"
#include <ACG/GL/VertexDeclaration.hh>
#include <vector>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================

	      

/** \class PointNode PointNode.hh <ACG/Scenegraph/PointNode.hh>

    PointNode renders points and normals.
    
    These elements are internally stored in arrays and rendered using
    OpenGL vertex and normal arrays.
**/

class ACGDLLEXPORT PointNode : public BaseNode
{
public:
   
  // typedefs
  typedef std::vector<ACG::Vec3d>      PointVector;
  typedef PointVector::iterator        PointIter;
  typedef PointVector::const_iterator  ConstPointIter;
  typedef std::vector<ACG::Vec4f>      ColorVector;
  typedef ColorVector::iterator        ColorIter;
  typedef ColorVector::const_iterator  ConstColorIter;


  /// default constructor
  PointNode( BaseNode*         _parent=0,
	     std::string  _name="<PointNode>" )
    : BaseNode(_parent, _name)
  {}
 
  /// destructor
  ~PointNode() {}

  /// static name of this class
  ACG_CLASSNAME(PointNode);

  /// return available draw modes
  DrawModes::DrawMode availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// draw points and normals
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);

  /// draw points and normals via renderer plugin
  void getRenderObjects(IRenderer* _renderer, GLState&  _state , const DrawModes::DrawMode&  _drawMode , const Material* _mat);

  /// reserve mem for _np points and _nn normals
  void reserve(unsigned int _np, unsigned int _nn, unsigned int _nc) {
    points_.reserve(_np); normals_.reserve(_nn); colors_.reserve(_nc);
  }

  /// add point
  void add_point(const ACG::Vec3d& _p) { points_.push_back(_p); }
  /// add normal
  void add_normal(const ACG::Vec3d& _n) { normals_.push_back(_n); }
  /// add color
  void add_color(const ACG::Vec4f& _c) { colors_.push_back(_c); }

  /// how many points?
  size_t n_points() const { return points_.size(); }

  /// clear points
  void clear_points() { points_.clear(); }
  /// clear normals
  void clear_normals() { normals_.clear(); }
  /// clear colors
  void clear_colors() { colors_.clear(); }
  /// clear points and normals and colors
  void clear() { clear_points(); clear_normals(); clear_colors(); }

  /// get point container
  PointVector& points() { return points_; }
  /// get normal container
  PointVector& normals() { return normals_; }
  /// get color container
  ColorVector& colors() { return colors_; }


private:
  
  PointVector  points_, normals_;
  ColorVector  colors_;

  VertexDeclaration vertexDecl_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_POINTNODE_HH defined
//=============================================================================
