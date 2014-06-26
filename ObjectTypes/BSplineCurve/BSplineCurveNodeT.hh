/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/


//=============================================================================
//
// CLASS BSplineCurveNodeT
// Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


#ifndef ACG_BSPLINECURVENODET_HH
#define ACG_BSPLINECURVENODET_HH


//== INCLUDES =================================================================

#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>

#include <QGLWidget>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================

/** \class BSPlineCurveNodeT BSplineCurveNodeT.hh <ObjectTypes/BSplineCurve/BSplineCurveNodeT.hh>

    Brief Description.

    A more elaborate description follows.
*/

template <class BSplineCurve>
class BSplineCurveNodeT : public BaseNode
{
public:

  // typedefs for easy access
  typedef typename BSplineCurve::Point Point;

  /// Constructor
  BSplineCurveNodeT(BSplineCurve& _bsc,
		    BaseNode*    _parent=0,
		    std::string  _name="<BSplineCurveNode>" ) :
    BaseNode(_parent, _name),
    bsplineCurve_(_bsc),
    pick_radius_(0.1),
    resolution_(16),
    polygon_color_( Vec4f(34.f/255.f, 139.f/255.f, 34.f/255.f, 1.f) ),
    polygon_highlight_color_( Vec4f(1.f, 1.f, 0.f, 1.f) ),
    curve_color_( Vec4f(178.f/255.f, 34.f/255.f, 34.f/255.f, 1.f) ),
    curve_highlight_color_( Vec4f(1.f, 1.f, 1.f, 1.f) ),
    render_control_polygon_(true),
    render_bspline_curve_(true),
    bspline_draw_mode_(NORMAL),
    bspline_selection_draw_mode_(NONE),
    pick_texture_idx_(0),
    pick_texture_res_(256),
    pick_texture_baseidx_(0),
    cp_selection_texture_idx_(0),
    cp_selection_texture_res_(256),
    knot_selection_texture_idx_(0),
    knot_selection_texture_res_(256),
    controlPointSelectionTexture_valid_(false),
    knotVectorSelectionTexture_valid_(false)
  {
    drawMode(DrawModes::WIREFRAME | DrawModes::POINTS);

    cylinder_    = new GLCylinder(16, 1, 1.0f, true, true);
    sphere_      = new GLSphere(5, 5);
    fancySphere_ = new GLSphere(16, 16);
  }


  /// Destructor
  ~BSplineCurveNodeT()
  {
    if (cylinder_)
      delete cylinder_;

    if (sphere_)
      delete sphere_;

    if (fancySphere_)
      delete fancySphere_;
  }

  
  enum BSplineDrawMode {
    NORMAL = 0,
    FANCY = 1
  };

  enum BSplineSelectionDrawMode {
    NONE = 0,
    CONTROLPOINT = 1,
    KNOTVECTOR = 2
  };


  BSplineCurve& bsplineCurve() { return bsplineCurve_; }

  void set_pick_radius( double _pr) { pick_radius_ = _pr; }

  /// static name of this class
  ACG_CLASSNAME(BSplineCurveNodeT);

  /// return available draw modes
  DrawModes::DrawMode availableDrawModes() const;
  
  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// draw lines and normals
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);

  /// picking
  void pick(GLState& _state, PickTarget _target);

  void set_rendering_resolution(int _res){resolution_ = _res;};

  void render_control_polygon(bool _render) {render_control_polygon_ = _render;};
  bool render_control_polygon() {return render_control_polygon_; };

  void render_bspline_curve(bool _render) {render_bspline_curve_ = _render;};
  bool render_bspline_curve() {return render_bspline_curve_; };

  void set_selection_draw_mode(BSplineSelectionDrawMode _mode) {bspline_selection_draw_mode_ = _mode;};
  
  BSplineSelectionDrawMode get_selection_draw_mode() const { return bspline_selection_draw_mode_; }

  void set_bspline_draw_mode(BSplineDrawMode _mode) {bspline_draw_mode_ = _mode;};

  void updateGeometry();
    
  //! Should be a power of 2
  int& pick_texture_res( ) { return pick_texture_res_; }
  
  void cpSelectionTextureValid  (bool _valid){controlPointSelectionTexture_valid_ = _valid;};
  void knotSelectionTextureValid(bool _valid){knotVectorSelectionTexture_valid_   = _valid;};

private:

  void pick_vertices(GLState& _state);
  void pick_spline( GLState& _state, unsigned int _offset );

  void draw_cylinder( const Point& _p0, const Point& _axis, double _r, GLState& _state);
  void draw_sphere  ( const Point& _p0, double _r, GLState& _state, GLSphere* _sphere);

  /// Copy constructor (not used)
  BSplineCurveNodeT(const BSplineCurveNodeT& _rhs);

  /// Assignment operator (not used)
  BSplineCurveNodeT& operator=(const BSplineCurveNodeT& _rhs);

  
  void render(GLState& _state, bool _fill, DrawModes::DrawMode _drawMode);
  
  /// Renders the control polygon
  void drawControlPolygon(DrawModes::DrawMode _drawMode, GLState& _state);
  
  /// Renders the control polygon using cylinders and spheres to include shading effects
  void drawFancyControlPolygon(DrawModes::DrawMode _drawMode, GLState& _state);
  
  /// Renders the spline curve using gluNurbsRenderer
  void drawCurve(GLState& _state);
  
  /// Renders the spline curve by sampling the curve and rendering cylinders in between the samples
  void drawFancyCurve(GLState& _state);

  /// renders a textured cuve using the gluNurbsRenderer to vilualize either the control point ot the knot vector selection
  void drawTexturedCurve(GLState& _state, GLuint _texture_idx);
  
  /** spline curve picking */
  /// generate index and setup texture parameters
  void pick_init_texturing ( );
  /// create texture image
  void pick_create_texture( GLState& _state);
 
  void updateControlPointSelectionTexture(GLState& _state);
  
  void updateKnotVectorSelectionTexture(GLState& _state);
  
  /// generate index and setup texture parameters for selection visualization
  void selection_init_texturing(GLuint & _texture_idx);
  
  /// creates texture to put onto nurbs curve for visualization of control point selection
  void create_cp_selection_texture(GLState& _state);
  /// creates texture to put onto nurbs curve for visualization of knotvector selection
  void create_knot_selection_texture(GLState& _state);
  
  /// draw textured nurbs patch
  void draw_textured_nurbs( GLState& _state);
  
  /// generates a color to highlight the curve from the given color
  ACG::Vec4f generateHighlightColor(ACG::Vec4f _color);

  
private:

  BSplineCurve& bsplineCurve_;

  double pick_radius_;

  int resolution_;

  Vec4f polygon_color_;
  Vec4f polygon_highlight_color_;

  Vec4f curve_color_;
  Vec4f curve_highlight_color_;

  bool render_control_polygon_;
  bool render_bspline_curve_;

  BSplineDrawMode bspline_draw_mode_;
  
  BSplineSelectionDrawMode bspline_selection_draw_mode_;

  std::vector< std::pair< Vec3d, Vec4f > > curve_samples_;
  
  // texturing stuff for picking
  QImage pick_texture_image_;
  GLuint pick_texture_idx_;
  int    pick_texture_res_;
  // used to only re-create pick_texture_image_ if picking indices changed...
  unsigned int pick_texture_baseidx_;
  
  
  // texturing stuff for control point selection highlighting
  QImage cp_selection_texture_image_;
  GLuint cp_selection_texture_idx_;
  int    cp_selection_texture_res_;

  // texturing stuff for knot vector selection highlighting
  QImage knot_selection_texture_image_;
  GLuint knot_selection_texture_idx_;
  int    knot_selection_texture_res_;

  bool controlPointSelectionTexture_valid_;
  bool knotVectorSelectionTexture_valid_;
  
  GLCylinder* cylinder_;
  GLSphere* sphere_;
  GLSphere* fancySphere_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_BSPLINECURVENODET_C)
#define ACG_BSPLINECURVENODET_TEMPLATES
#include "BSplineCurveNodeT.cc"
#endif
//=============================================================================
#endif // ACG_BSPLINECURVENODET_HH defined
//=============================================================================

