/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS LineNode
//
//=============================================================================


#ifndef ACG_LINENODE_HH
#define ACG_LINENODE_HH


//== INCLUDES =================================================================

#include "MaterialNode.hh"
#include "DrawModes.hh"
#include <vector>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================



/** \class LineNode LineNode.hh <ACG/Scenegraph/LineNode.hh>

    LineNode renders a set of line segments or polylines.

    LineNode renders a set of line segments or one connected polyline,
    depending on the LineMode, that can be set using the
    set_line_mode(LineMode) method.
**/

class ACGDLLEXPORT LineNode : public MaterialNode
{
public:

  // typedefs
  typedef std::vector<Vec3f>           PointVector;
  typedef PointVector::iterator        PointIter;
  typedef PointVector::const_iterator  ConstPointIter;
  typedef std::vector<ACG::Vec3uc>     ColorVector;
  typedef ColorVector::iterator        ColorIter;
  typedef ColorVector::const_iterator  ConstColorIter;

  /// Line mode: draw line segments (every 2 points) or ONE polyline.
  enum LineMode { LineSegmentsMode, PolygonMode };



  /// default constructor
  LineNode( LineMode     _mode,
	    BaseNode*    _parent=0,
	    std::string  _name="<LineNode>" ) :
    MaterialNode(_parent,
		 _name,
		 MaterialNode::BaseColor |
		 MaterialNode::LineWidth),
    line_mode_(_mode)
  {
    drawMode(DrawModes::WIREFRAME);
  }

  /// destructor
  ~LineNode() {}


  /// set line mode (see LineNode::LineMode)
  void set_line_mode(LineMode _mode) { line_mode_ = _mode; }



  /// static name of this class
  ACG_CLASSNAME(LineNode);

  /// return available draw modes
  DrawModes::DrawMode  availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// draw lines and normals
  void draw(GLState& _state, DrawModes::DrawMode _drawMode);




  /// reserve mem for _n lines
  void reserve_lines(unsigned int _n) { points_.reserve(2*_n); }

  /// reserve mem for _n points
  void reserve_points(unsigned int _n) { points_.reserve(_n); }

  /// clear points/lines and colors
  void clear() { clear_points(); clear_colors(); }
  /// clear points/lines
  void clear_points() { points_.clear(); }
  /// clear colors
  void clear_colors() { colors_.clear(); }


  /// add point (for LineMode == PolygonMode)
  void add_point(const Vec3f& _v) { points_.push_back(_v); }

  /// add line (for LineMode == LineSegmentsMode)
  void add_line(const Vec3f& _v0, const Vec3f& _v1) {
    add_point(_v0);  add_point(_v1);
  }
  /// add color (only for LineMode == LineSegmentsMode)
  void add_color(const ACG::Vec3uc& _c) { colors_.push_back(_c); }


  /// number of points
  unsigned int n_points() const { return points_.size(); }

  /// return reference to point vector
  const PointVector& points() const { return points_; }

  /// get color container
  ColorVector& colors() { return colors_; }


  /// STL conformance
  void push_back(const Vec3f& _v) { points_.push_back(_v); }
  typedef Vec3f         value_type;
  typedef Vec3f&        reference;
  typedef const Vec3f&  const_reference;


protected:

  PointVector  points_;
  ColorVector  colors_;

  LineMode     line_mode_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_LINENODE_HH defined
//=============================================================================

