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
//  CLASS LineNode
//
//=============================================================================


#ifndef ACG_LINENODE_HH
#define ACG_LINENODE_HH


//== INCLUDES =================================================================

#include <ACG/Scenegraph/MaterialNode.hh>
#include "DrawModes.hh"
#include <ACG/GL/VertexDeclaration.hh>
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
  typedef ACG::Vec3uc                    Color;
  typedef ACG::Vec4f                     Color4f;
  typedef std::vector<Vec3d>             PointVector;
  typedef PointVector::iterator          PointIter;
  typedef PointVector::const_iterator    ConstPointIter;
  typedef std::vector<ACG::Vec3uc>       ColorVector;
  typedef ColorVector::iterator          ColorIter;
  typedef ColorVector::const_iterator    ConstColorIter;
  typedef std::vector<Color4f>           Color4fVector;
  typedef Color4fVector::iterator        Color4fIter;
  typedef Color4fVector::const_iterator  ConstColor4fIter;

  /// Line mode: draw line segments (every 2 points) or ONE polyline.
  enum LineMode { LineSegmentsMode, PolygonMode };



  /// default constructor
  LineNode( LineMode     _mode,
	          BaseNode*    _parent=0,
	          std::string  _name="<LineNode>" );

  /// destructor
  ~LineNode();

  /// set line mode (see LineNode::LineMode)
  void set_line_mode(LineMode _mode);


  /// static name of this class
  ACG_CLASSNAME(LineNode);

  /// return available draw modes
  DrawModes::DrawMode  availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);
  
  
  /// set depth function (needed for lasso selection so that the line can be draw in pseudo-2D)
  void enter(GLState& _state, const DrawModes::DrawMode& _drawMode);

  /// draw lines and normals
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);
  
  /// reset depth function to what it was before enter()
  void leave(GLState& _state, const DrawModes::DrawMode& _drawMode);


  /// reserve mem for _n lines
  void reserve_lines(unsigned int _n) { points_.reserve(2*_n); }
  /// reserve mem for _n points
  void reserve_points(unsigned int _n) { points_.reserve(_n); }

  /// clear points/lines and colors
  void clear();

  /// clear points/lines
  void clear_points();

  /// clear colors
  void clear_colors();
  
  /// Override material node's set color function in order to locally add color
  void set_color(const Vec4f& _c);

  /// add point (for LineMode == PolygonMode)
  void add_point(const Vec3d& _v);

  /// add line (for LineMode == LineSegmentsMode)
  void add_line(const Vec3d& _v0, const Vec3d& _v1);

  /// add color (only for LineMode == LineSegmentsMode)
  void add_color(const ACG::Vec3uc& _c);

  /// add color 4f (only for LineMode == LineSegmentsMode)
  void add_color(const Color4f _c);

  /// number of points
  size_t n_points() const { return points_.size(); }

  /**\brief return reference to point vector
   *
   * If you change something here, you need to call updateVBO() in order
   * to tell the system, that your data arrays changed
   */
  const PointVector& points() const { return points_; }

  /**\brief get and set color container
   *
   * If you change something here, you need to call updateVBO() in order
   * to tell the system, that your data arrays changed
   */
  ColorVector& colors() { return colors_; }
  
  /// get and set always on top
  bool& alwaysOnTop() { updateVBO_ = true; return draw_always_on_top;  }

  void updateVBO() { updateVBO_ = true; };

  /// STL conformance
  void push_back(const Vec3d& _v) { points_.push_back(_v); updateVBO_ = true; }
  typedef Vec3d         value_type;
  typedef Vec3d&        reference;
  typedef const Vec3d&  const_reference;

  /** \brief Add the objects to the given renderer
   *
   * @param _renderer The renderer which will be used. Add your geometry into this class
   * @param _state    The current GL State when this object is called
   * @param _drawMode The active draw mode
   * @param _mat      Current material
   */
  void getRenderObjects(IRenderer* _renderer, GLState&  _state , const DrawModes::DrawMode&  _drawMode , const ACG::SceneGraph::Material* _mat);

protected:
  /// creates the vbo only if update was requested
  void createVBO();

  PointVector   points_;
  ColorVector   colors_;
  Color4fVector colors4f_;

  LineMode     line_mode_;
  
  bool	       draw_always_on_top;
  GLint	       prev_depth_;

  // Vertex buffer object used in this node
  unsigned int vbo_;

  // True if points changed and the vbo has to be updated
  bool         updateVBO_;

  ACG::VertexDeclaration vertexDecl_;

};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_LINENODE_HH defined
//=============================================================================

