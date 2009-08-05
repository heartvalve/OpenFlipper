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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS MaterialNode
//
//=============================================================================

#ifndef ACG_MATERIAL_NODE_HH
#define ACG_MATERIAL_NODE_HH


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include <string>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class MaterialNode MaterialNode.hh <ACG/Scenegraph/MaterialNode.hh>

    Set material and some other stuff like alphatest and blending
    for this node and all its children.
    All changes will be done in the enter() method undone
    in the leave() method.
**/

class ACGDLLEXPORT MaterialNode : public BaseNode
{
public:

  /// Apply which properties? Others will be ignored. Values may be OR'ed.
  enum ApplyProperties
  {
    /// apply nothing
    None=0,
    /// apply all properites
    All=0xffff,
    /// apply base color
    BaseColor=1,
    /// apply material (ambient, diffuse, specular, shininess)
    Material=2,
    /// apply point size
    PointSize=4,
    /// apply line width
    LineWidth=8,
    /// draw smooth (round) points using glPoint()
    RoundPoints=16,
    /// draw smooth lines using glLine()
    LineSmooth=32,
    /// use alpha test
    AlphaTest=64,
    /// use blending
    Blending=128,
    /// backface culling
    BackFaceCulling=256,
    /// Color Material ( Only when a drawmode using shading and lighting is enabled )
    ColorMaterial=512,
    /// MultiSampling
    MultiSampling=1024
  };



  /// Default constructor. Applies all properties.
  MaterialNode( BaseNode*           _parent = 0,
		          const std::string&  _name = "<MaterialNode>",
		          unsigned int        _applyProperties = (All & ~BackFaceCulling));

  /// Destructor.
  virtual ~MaterialNode() {};

  /// read MaterialFile
  void read( std::istream & _is);

  //===========================================================================
  /** @name Scenegraph functions
    * @{ */
  //===========================================================================

    ACG_CLASSNAME(MaterialNode);

    /// set current GL-color and GL-material
    void enter(GLState& _state, unsigned int _drawmode);
    /// restores original GL-color and GL-material
    void leave(GLState& _state, unsigned int _drawmode);


    /** \brief Do nothing in picking*/
    void enterPick(GLState& _state, PickTarget _target, unsigned int _drawMode );

    /** \brief Do nothing in picking */
    void leavePick(GLState& _state, PickTarget _target, unsigned int _drawMode );

  /** @} */

  //===========================================================================
  /** @name Color and material settings ( Applied to all objects below this node )
    * @{ */
  //===========================================================================

    /// set color (base, ambient, diffuse, specular) based on _c
    void set_color(const Vec4f& _c) {
      Vec4f c;
      set_base_color(_c);
      c = _c * 0.2f;  c[3]=_c[3];  set_ambient_color(c);
      c = _c * 0.6f;  c[3]=_c[3];  set_diffuse_color(c);
      c = _c * 0.8f;  c[3]=_c[3];  set_specular_color(c);
    }

    /// set the base color
    void set_base_color(const Vec4f& _c) { base_color_ = _c;}
    /// get the base color
    const Vec4f& base_color() const { return base_color_; }

    /// set the ambient color.
    void set_ambient_color(const Vec4f& _a) { ambient_color_ = _a; }
    /// get the ambient color.
    const Vec4f& ambient_color() const { return ambient_color_; }

    /// set the diffuse color.
    void set_diffuse_color(const Vec4f& _d) { diffuse_color_ = _d; }
    /// get the diffuse color.
    const Vec4f& diffuse_color() const { return diffuse_color_; }

    /// set the specular color
    void set_specular_color(const Vec4f& _s) { specular_color_ = _s; }
    /// get the specular color
    const Vec4f& specular_color() const { return specular_color_; }

    /// Set colorMaterial
    void colorMaterial( const bool _colorMaterial) { color_material_ = _colorMaterial; }
    /// Enable Color Material
    void enable_color_material() { color_material_ = true; }
    /// Disable Color Material
    void disable_color_material() { color_material_ = false; }
    /// get colorMaterial state
    bool colorMaterial() { return color_material_; }

    /// set shininess
    void set_shininess(float _s) { shininess_ = _s; }
    /// get shininess
    float shininess() const { return shininess_; }

  /** @} */

  //===========================================================================
  /** @name Point/Line controls
    * @{ */
  //===========================================================================
    /// set point size (default: 1.0)
    void set_point_size(float _sz) { point_size_ = _sz; }
    /// get point size
    float point_size() const { return point_size_; }

    /// set line width (default: 1.0)
    void set_line_width(float _sz) { line_width_ = _sz; }
    /// get line width
    float line_width() const { return line_width_; }

    /// set: round points enabled
    void set_round_points(bool _b) { round_points_ = _b; }
    /// get: round points enabled
    bool round_points() const { return round_points_; }

    /// set: smooth lines enabled
    void set_line_smooth(bool _b) { lines_smooth_ = _b; }
    /// get: rsmooth lines enabled
    bool line_smooth() const { return lines_smooth_; }

  /** @} */

  //===========================================================================
  /** @name Tests
    * @{ */
  //===========================================================================

    /// enable alpha test (draw pixels if alpha >= _clip)
    void enable_alpha_test(float _clip) {
      alpha_test_ = true; alpha_clip_ = _clip;
    }

    /// disable alpha test
    void disable_alpha_test() { alpha_test_ = false; }

    /// Return state of Alpha test
    bool alpha_test() { return alpha_test_; };

  /** @} */

  //===========================================================================
  /** @name Other Rendering options
    * @{ */
  //===========================================================================

    /// Enable Multisampling
    void enable_multisampling() {
      multiSampling_ = true;
    }

    /// enable alpha test (draw pixels if alpha >= _clip)
    void disable_multisampling() {
      multiSampling_ = false;
    }

    /// Get state of multisampling
    bool multiSampling() {
      return multiSampling_;
    }

    /// Set state of multisampling
    void set_multisampling( bool _state ) {
      multiSampling_ = _state;
    }

  /** @} */






  ///get current alpha value for alpha_test
  float alpha_value(){ return alpha_clip_; };

  bool blending() { return blending_; };

  GLenum blending_param1() { return blend_param1_; };
  GLenum blending_param2() { return blend_param2_; };

  /// enable blending with Parameters (_p1, _p2)
  void enable_blending(GLenum _p1 = GL_SRC_ALPHA,
		       GLenum _p2 = GL_ONE_MINUS_SRC_ALPHA)
  { blending_ = true; blend_param1_ = _p1; blend_param2_ = _p2; }
  /// disable blending
  void disable_blending() { blending_ = false; }

  bool backface_culling() { return backface_culling_; };

  /// enable backface culling (not active by default, see applyProperties)
  void enable_backface_culling() { backface_culling_ = true; }
  /// disable backface culling (not active by default, see applyProperties)
  void disable_backface_culling() { backface_culling_ = false; }



    /// get properties that will be applied (OR'ed ApplyProperties)
  unsigned int applyProperties() const { return applyProperties_; }

  /// set properties that will be applied (OR'ed ApplyProperties)
  void applyProperties(unsigned int _applyProperties) {
    applyProperties_ = _applyProperties;
  }

private:

  /// OR'ed ApplyProperties
  int applyProperties_;

  Vec4f    base_color_,      base_color_backup_;
  Vec4f    ambient_color_,   ambient_color_backup_;
  Vec4f    diffuse_color_,   diffuse_color_backup_;
  Vec4f    specular_color_,  specular_color_backup_;

  float    shininess_,       shininess_backup_;
  float    point_size_,      point_size_backup_;
  float    line_width_,      line_width_backup_;

  bool     round_points_,    round_points_backup_;
  bool     lines_smooth_,    lines_smooth_backup_;

  bool     alpha_test_,      alpha_test_backup_;
  float    alpha_clip_,      alpha_clip_backup_;

  bool     blending_,        blending_backup_;
  GLenum   blend_param1_,    blend_param1_backup_;
  GLenum   blend_param2_,    blend_param2_backup_;

  bool     color_material_,  color_material_backup_;
  bool     backface_culling_,backface_culling_backup_;

  bool     multiSampling_,   multiSampling_backup_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_MATERIAL_NODE_HH defined
//=============================================================================

