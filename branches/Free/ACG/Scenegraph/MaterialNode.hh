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


/** \class Material

    Class to store the properties of a material.
**/

class ACGDLLEXPORT Material {
    
public:
    /// Default constructor
    Material() :
        base_color_(GLState::default_base_color),
        ambient_color_(GLState::default_ambient_color),
        diffuse_color_(GLState::default_diffuse_color),
        specular_color_(GLState::default_specular_color),
        shininess_(GLState::default_shininess),
        point_size_(1.0),
        line_width_(1.0),
        round_points_(false),
        lines_smooth_(false),
        alpha_test_(false),
        alpha_clip_(0),
        blending_(false),
        blend_param1_(GL_ONE),
        blend_param2_(GL_ZERO),
        color_material_(true),
        backface_culling_(false),
        multiSampling_(true) {};
    
    /// Deconstructor
    virtual ~Material() {};
    
    /// Copy constructor
    Material(const Material& _m) :
        base_color_(_m.base_color_),
        ambient_color_(_m.ambient_color_),
        diffuse_color_(_m.diffuse_color_),
        specular_color_(_m.specular_color_),
        shininess_(_m.shininess_),
        mirror_(_m.mirror_),
        point_size_(_m.point_size_),
        line_width_(_m.line_width_),
        round_points_(_m.round_points_),
        lines_smooth_(_m.lines_smooth_),
        alpha_test_(_m.alpha_test_),
        alpha_clip_(_m.alpha_clip_),
        blending_(_m.blending_),
        blend_param1_(_m.blend_param1_),
        blend_param2_(_m.blend_param2_),
        color_material_(_m.color_material_),
        backface_culling_(_m.backface_culling_),
        multiSampling_(_m.multiSampling_) {};
        
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
    void color_material( const bool _cm) { color_material_ = _cm; }
    /// Enable Color Material
    void enable_color_material() { color_material_ = true; }
    /// Disable Color Material
    void disable_color_material() { color_material_ = false; }
    /// get colorMaterial state
    bool color_material() { return color_material_; }

    /// set shininess
    void set_shininess(float _s) { shininess_ = _s; }
    /// get shininess
    float shininess() const { return shininess_; }
    
    /// set mirror
    void set_mirror(double _m) { mirror_ = _m; }
    /// get mirror
    double mirror() const { return mirror_; }
    
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
    
    /// enable alpha test (draw pixels if alpha >= _clip)
    void enable_alpha_test(float _clip) {
      alpha_test_ = true; alpha_clip_ = _clip;
    }

    /// disable alpha test
    void disable_alpha_test() { alpha_test_ = false; }

    /// Return state of Alpha test
    bool alpha_test() { return alpha_test_; };
    
    /// Enable Multisampling
    void enable_multisampling() {
      multiSampling_ = true;
    }

    /// enable alpha test (draw pixels if alpha >= _clip)
    void disable_multisampling() {
      multiSampling_ = false;
    }

    /// Get state of multisampling
    bool multi_sampling() {
      return multiSampling_;
    }

    /// Set state of multisampling
    void set_multisampling( bool _state ) {
      multiSampling_ = _state;
    }

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

private:
    
    Vec4f    base_color_;
    Vec4f    ambient_color_;
    Vec4f    diffuse_color_;
    Vec4f    specular_color_;
    
    float    shininess_;
    double   mirror_;
    float    point_size_;
    float    line_width_;
    
    bool     round_points_;
    bool     lines_smooth_;
    
    bool     alpha_test_;
    float    alpha_clip_;
    
    bool     blending_;
    GLenum   blend_param1_;
    GLenum   blend_param2_;
    
    bool     color_material_;
    bool     backface_culling_;
    
    bool     multiSampling_;
};


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
    void enter(GLState& _state, DrawModes::DrawMode _drawmode);
    /// restores original GL-color and GL-material
    void leave(GLState& _state, DrawModes::DrawMode _drawmode);


    /** \brief Do nothing in picking*/
    void enterPick(GLState& _state, PickTarget _target, DrawModes::DrawMode _drawMode );

    /** \brief Do nothing in picking */
    void leavePick(GLState& _state, PickTarget _target, DrawModes::DrawMode _drawMode );

    /** @} */

    //===========================================================================
    /** @name Color and material settings ( Applied to all objects below this node )
      * @{ */
    //===========================================================================

    /// set color (base, ambient, diffuse, specular) based on _c
    void set_color(const Vec4f& _c) {
      material_.set_color(_c);
    }

    /// set the base color
    void set_base_color(const Vec4f& _c) { material_.set_base_color(_c); }
    /// get the base color
    const Vec4f& base_color() const { return material_.base_color(); }

    /// set the ambient color.
    void set_ambient_color(const Vec4f& _a) { material_.set_ambient_color(_a); }
    /// get the ambient color.
    const Vec4f& ambient_color() const { return material_.ambient_color(); }

    /// set the diffuse color.
    void set_diffuse_color(const Vec4f& _d) { material_.set_diffuse_color(_d); }
    /// get the diffuse color.
    const Vec4f& diffuse_color() const { return material_.diffuse_color(); }

    /// set the specular color
    void set_specular_color(const Vec4f& _s) { material_.set_specular_color(_s); }
    /// get the specular color
    const Vec4f& specular_color() const { return material_.specular_color(); }

    /// Set colorMaterial
    void colorMaterial( const bool _cm) { material_.color_material(_cm); }
    /// Enable Color Material
    void enable_color_material() { material_.enable_color_material(); }
    /// Disable Color Material
    void disable_color_material() { material_.disable_color_material(); }
    /// get colorMaterial state
    bool colorMaterial() { return material_.color_material(); }

    /// set shininess
    void set_shininess(float _s) { material_.set_shininess(_s); }
    /// get shininess
    float shininess() const { return material_.shininess(); }
    
    /// set mirror
    void set_mirror(double _m) { material_.set_mirror(_m); }
    /// get mirror
    double mirror() const { return material_.mirror(); }

  /** @} */

  //===========================================================================
  /** @name Point/Line controls
    * @{ */
  //===========================================================================
    /// set point size (default: 1.0)
    void set_point_size(float _sz) { material_.set_point_size(_sz); }
    /// get point size
    float point_size() const { return material_.point_size(); }

    /// set line width (default: 1.0)
    void set_line_width(float _sz) { material_.set_line_width(_sz); }
    /// get line width
    float line_width() const { return material_.line_width(); }

    /// set round points enabled
    void set_round_points(bool _b) { material_.set_round_points(_b); }
    /// get round points enabled
    bool round_points() const { return material_.round_points(); }

    /// set: smooth lines enabled
    void set_line_smooth(bool _b) { material_.set_line_smooth(_b); }
    /// get: rsmooth lines enabled
    bool line_smooth() const { return material_.line_smooth(); }

  /** @} */

  //===========================================================================
  /** @name Tests
    * @{ */
  //===========================================================================

    /// enable alpha test (draw pixels if alpha >= _clip)
    void enable_alpha_test(float _clip) { material_.enable_alpha_test(_clip); }

    /// disable alpha test
    void disable_alpha_test() { material_.disable_alpha_test(); }

    /// Return state of Alpha test
    bool alpha_test() { return material_.alpha_test(); };

  /** @} */

  //===========================================================================
  /** @name Other Rendering options
    * @{ */
  //===========================================================================

    /// Enable Multisampling
    void enable_multisampling() { material_.enable_multisampling(); }

    /// enable alpha test (draw pixels if alpha >= _clip)
    void disable_multisampling() { material_.disable_multisampling(); }

    /// Get state of multisampling
    bool multiSampling() { return material_.multi_sampling(); }

    /// Set state of multisampling
    void set_multisampling( bool _state ) { material_.set_multisampling(_state); }

  /** @} */

  ///get current alpha value for alpha_test
  float alpha_value(){ return material_.alpha_value(); };

  bool blending() { return material_.blending(); };

  GLenum blending_param1() { return material_.blending_param1(); };
  GLenum blending_param2() { return material_.blending_param2(); };

  /// enable blending with Parameters (_p1, _p2)
  void enable_blending(GLenum _p1 = GL_SRC_ALPHA,
		       GLenum _p2 = GL_ONE_MINUS_SRC_ALPHA) {
      material_.enable_blending(_p1, _p2);
  }
  /// disable blending
  void disable_blending() { material_.disable_blending(); }

  bool backface_culling() { return material_.backface_culling(); };

  /// enable backface culling (not active by default, see applyProperties)
  void enable_backface_culling() { material_.enable_backface_culling(); }
  
  /// disable backface culling (not active by default, see applyProperties)
  void disable_backface_culling() { material_.disable_backface_culling(); }

  /// get properties that will be applied (OR'ed ApplyProperties)
  unsigned int applyProperties() const { return applyProperties_; }

  /// set properties that will be applied (OR'ed ApplyProperties)
  void applyProperties(unsigned int _applyProperties) {
    applyProperties_ = _applyProperties;
  }
  
  /// Get material object reference
  ACG::SceneGraph::Material& material() { return material_; }
  
  /// Set material object
  void set_material(const ACG::SceneGraph::Material& _m) { material_ = _m; }

private:

    /// OR'ed ApplyProperties
    int applyProperties_;

    /// Local material class that actually stores the properties
    ACG::SceneGraph::Material material_;

    Vec4f    base_color_backup_;
    Vec4f    ambient_color_backup_;
    Vec4f    diffuse_color_backup_;
    Vec4f    specular_color_backup_;
     
    float    shininess_backup_;
    double   mirror_backup_;
    float    point_size_backup_;
    float    line_width_backup_;
    
    bool     round_points_backup_;
    bool     lines_smooth_backup_;
    
    bool     alpha_test_backup_;
    float    alpha_clip_backup_;
    
    bool     blending_backup_;
    GLenum   blend_param1_backup_;
    GLenum   blend_param2_backup_;
    
    bool     color_material_backup_;
    bool     backface_culling_backup_;
    
    bool     multiSampling_backup_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_MATERIAL_NODE_HH defined
//=============================================================================

