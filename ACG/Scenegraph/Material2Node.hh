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
//  CLASS Material2Node
//
//=============================================================================

#ifndef ACG_MATERIAL2NODE_HH
#define ACG_MATERIAL2NODE_HH

//=============================================================================

#include "BaseNode.hh"
#include <string>

//=============================================================================

namespace ACG {
namespace SceneGraph {
  
//=============================================================================


class ACGDLLEXPORT Material2Node : public BaseNode
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
    /// use alpha test
    AlphaTest=32,
    /// use blending
    Blending=64,
    /// backface culling
    BackFaceCulling=128
  };
  

  enum Side
  {
    FrontSide = 0,
    BackSide  = 1,
  };

  /// Default constructor. Applies all properties.
  Material2Node( BaseNode*           _parent = 0,
		const std::string&  _name = "<Material2Node>",
		unsigned int        _applyProperties = (All & 
							~BackFaceCulling));

  /// Destructor.
  virtual ~Material2Node() {};

    
  ACG_CLASSNAME(Material2Node);

  /// read MaterialFile
  void read( std::istream & _is);


  /// set current GL-color and GL-material
  void enter(GLState& _state, unsigned int _drawmode);
  /// restores original GL-color and GL-material
  void leave(GLState& _state, unsigned int _drawmode);

  
  /// get properties that will be applied (OR'ed ApplyProperties)
  unsigned int applyProperties() const { return applyProperties_; }
  /// set properties that will be applied (OR'ed ApplyProperties)
  void applyProperties(unsigned int _applyProperties) { 
    applyProperties_ = _applyProperties; 
  }

  
  /// set color (base, ambient, diffuse, specular) based on _c
  void set_color( const Vec4f & _c,
		  Side _side = FrontSide )
  {
    Vec4f c;
    set_base_color( _c, _side );
    c = _c * 0.2f;  c[3]=_c[3];  set_ambient_color( c, _side );
    c = _c * 0.6f;  c[3]=_c[3];  set_diffuse_color( c, _side );
    c = _c * 0.8f;  c[3]=_c[3];  set_specular_color( c, _side );
  }

  /// set the base color
  void set_base_color( const Vec4f & _c,
		       Side          _side = FrontSide )
  {
    base_color_[ _side ] = _c;
  }

  /// get the base color
  const Vec4f& base_color( Side _side = FrontSide ) const 
  {
    return base_color_[ _side ];
  }
  
  /// set the ambient color.
  void set_ambient_color( const Vec4f& _a,
			  Side          _side = FrontSide )
  {
    ambient_color_[ _side ] = _a;
  }
  
  /// get the ambient color.
  const Vec4f& ambient_color( Side _side = FrontSide ) const
  {
    return ambient_color_[ _side ];
  }  

  /// set the diffuse color.
  void set_diffuse_color( const Vec4f& _d,
			  Side _side = FrontSide )
  {
    diffuse_color_[ _side ] = _d;
  }

  /// get the diffuse color.
  const Vec4f& diffuse_color( Side _side = FrontSide ) const 
  {
    return diffuse_color_[ _side ];
  }
  
  /// set the specular color
  void set_specular_color( const Vec4f& _s,
			   Side _side = FrontSide )
  {
    specular_color_[ _side ] = _s; 
  }

  /// get the specular color
  const Vec4f& specular_color( Side _side = FrontSide ) const 
  {
    return specular_color_[ _side ];
  }

  /// set shininess
  void set_shininess(float _s) { shininess_ = _s; }
  /// get shininess
  float shininess() const { return shininess_; }

  
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

  /// enable alpha test (draw pixels if alpha >= _clip)
  void enable_alpha_test(float _clip) { 
    alpha_test_ = true; alpha_clip_ = _clip; 
  }
  /// disable alpha test
  void disable_alpha_test() { alpha_test_ = false; }

  /// enable blending with Parameters (_p1, _p2)
  void enable_blending(GLenum _p1 = GL_SRC_ALPHA, 
		       GLenum _p2 = GL_ONE_MINUS_SRC_ALPHA) 
  { blending_ = true; blend_param1_ = _p1; blend_param2_ = _p2; }
  /// disable blending
  void disable_blending() { blending_ = false; }

  /// enable backface culling (not active by default, see applyProperties)
  void enable_backface_culling() { backface_culling_ = true; }
  /// disable backface culling (not active by default, see applyProperties)
  void disable_backface_culling() { backface_culling_ = false; }

  /// enable two-sided material
  void set_twosided( bool _twosided ) { twosided_ = _twosided; }

private:

  /// OR'ed ApplyProperties
  int applyProperties_;
  
  Vec4f    base_color_[2],      base_color_backup_[2];
  Vec4f    ambient_color_[2],   ambient_color_backup_[2];
  Vec4f    diffuse_color_[2],   diffuse_color_backup_[2];
  Vec4f    specular_color_[2],  specular_color_backup_[2];

  float    shininess_,       shininess_backup_;
  float    point_size_,      point_size_backup_;
  float    line_width_,      line_width_backup_;

  bool     round_points_, round_points_backup_;

  bool     alpha_test_, alpha_test_backup_;
  float    alpha_clip_, alpha_clip_backup_;
  
  bool     blending_, blending_backup_;
  GLenum   blend_param1_, blend_param1_backup_;
  GLenum   blend_param2_, blend_param2_backup_;

  bool     backface_culling_, backface_culling_backup_;

  bool     twosided_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_MATERIAL2NODE_HH defined
//=============================================================================

