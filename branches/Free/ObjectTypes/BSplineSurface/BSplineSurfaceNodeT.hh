/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


//=============================================================================
//
// CLASS BSplineSurfaceNodeT
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================


#ifndef ACG_BSPLINESURFACENODET_HH
#define ACG_BSPLINESURFACENODET_HH


//== INCLUDES =================================================================

#include <ACG/Scenegraph/MaterialNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>

#include <QGLWidget>
#include <QImage>
//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================


/** \class BSPlineSurfaceNodeT BSplineSurfaceNodeT.hh <ACG/.../BSplineSurfaceNodeT.hh>
  Brief Description.

  A more elaborate description follows.
*/

template <class BSplineSurface>
class BSplineSurfaceNodeT : public MaterialNode
{
  public:

  // typedefs for easy access
  typedef typename BSplineSurface::Point Point;

  /// Constructor
  BSplineSurfaceNodeT(BSplineSurface& _bss,
                      BaseNode*    _parent=0,
                      std::string  _name="<BSplineSurfaceNode>" ) :
    MaterialNode(_parent,
                 _name, MaterialNode::None),
    bsplineSurface_(_bss)
  {
    resolution_ = 16;
    render_control_net_     = false;
    render_bspline_surface_ = true;
    adaptive_sampling_      = false;

    controlnet_color_           = Vec4f(34.0/255.0, 139.0/255.0, 34.0/255.0, 1.0);
    controlnet_highlight_color_ = Vec4f(0,1,0,1);
    surface_color_             = Vec4f(178.0/255.0, 34.0/255.0, 34.0/255.0, 1.0);
    surface_highlight_color_   = Vec4f(1.0, 127.0/255.0, 0.0, 1.0);
    
    pick_init_texturing();
    selection_init_texturing();
  }

  /// Destructor
  ~BSplineSurfaceNodeT() {}

  void set_random_color();

  BSplineSurface& bsplineSurface() { return bsplineSurface_; }

  void set_pick_radius( double _pr) { pick_radius_ = _pr; }

  /// static name of this class
  ACG_CLASSNAME(BSplineSurfaceNodeT);

  /// return available draw modes
  DrawModes::DrawMode availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// draw lines and normals
  void draw(GLState& _state, DrawModes::DrawMode _drawMode);

  /// picking
  void pick(GLState& _state, PickTarget _target);

  void set_rendering_resolution(int _res){resolution_ = _res;};

  void render_control_net(bool _render) {render_control_net_ = _render;};

  void render_bspline_surface(bool _render) {render_bspline_surface_ = _render;};

  void adaptive_sampling(bool _adaptive){adaptive_sampling_ = _adaptive;};

  void updateSelectionTexture();
  
  //! Should be a power of 2
  int& pick_texture_res( ) { return pick_texture_res_; }


private:

  void pick_vertices( GLState& _state );
  void pick_spline( GLState& _state );
  void pick_surface( GLState& _state, unsigned int _offset );

  void draw_cylinder( const Point& _p0, const Point& _axis, double _r, GLState& _state);
  void draw_sphere( const Point& _p0, double _r, GLState& _state);

  /// Copy constructor (not used)
  BSplineSurfaceNodeT(const BSplineSurfaceNodeT& _rhs);

  /// Assignment operator (not used)
  BSplineSurfaceNodeT& operator=(const BSplineSurfaceNodeT& _rhs);

  void render(GLState& _state, bool _fill);

  void drawGluNurbsMode(GLState& _state, bool _fill);
  
  void drawTexturedGluNurbsMode(GLState& _state);

  void drawControlNet(GLState& _state);


  /** spline surface u,v-parameter picking */
  /// generate index and setup texture parameters
  void pick_init_texturing ( );
  /// create texture image
  void pick_create_texture( GLState& _state);

  /// draw textured nurbs patch
  void pick_draw_textured_nurbs( GLState& _state);
  
  
  /// generate index and setup texture parameters for selection visualization
  void selection_init_texturing();
  /// creates texture to put onto nurbs curve for selection visualization
  void create_selection_texture();
  
  /// draw textured nurbs patch
  void draw_textured_nurbs( GLState& _state);

  
private:

  BSplineSurface& bsplineSurface_;

  double pick_radius_;

  int resolution_;

  Vec4f controlnet_color_;
  Vec4f controlnet_highlight_color_;

  Vec4f surface_color_;
  Vec4f surface_highlight_color_;

  bool render_control_net_;
  bool render_bspline_surface_;

  bool adaptive_sampling_;

  QImage pick_texture_image_;
  GLuint pick_texture_idx_;
  int    pick_texture_res_;
  // used to only re-create pick_texture_image_ if picking indices changed...
  unsigned int pick_texture_baseidx_;
  
  // texturing stuff for selection highlighting
  QImage selection_texture_image_;
  GLuint selection_texture_idx_;
  int    selection_texture_res_;
  
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_BSPLINESURFACENODET_C)
#define ACG_BSPLINESURFACENODET_TEMPLATES
#include "BSplineSurfaceNodeT.cc"
#endif
//=============================================================================
#endif // ACG_BSPLINESURFACENODET_HH defined
//=============================================================================

