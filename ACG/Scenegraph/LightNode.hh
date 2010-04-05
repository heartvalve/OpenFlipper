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
 *   $Revision: 8548 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2010-02-12 14:40:13 +0100 (Fr, 12. Feb 2010) $                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS LightNode
//
//=============================================================================

#ifndef ACG_LIGHT_NODE_HH
#define ACG_LIGHT_NODE_HH


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include "../GL/gl.hh"
#include <string>
#include <vector>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {
  

//== CLASS DEFINITION =========================================================

  
/** \class LightNode LightNode.hh <ACG/Scenegraph/LightNode.hh>

    Set LightSources (0 to 7) for this node and all its children.  
    All changes will be done in the enter() method undone
    in the leave() method.
**/





class ACGDLLEXPORT LightNode : public BaseNode
{

  public:
  
  /// Structure to hold options for one LightSource
  struct LightSource
  {
    //default Constructor
    LightSource()
    {
      // set OpenGL defaults
      enabled = false;
      fixedPosition = false;

      ambientColor  = Vec4f(0.1,0.1,0.1,1.0);
      diffuseColor  = Vec4f(1.0,1.0,1.0,1.0);
      specularColor = Vec4f(1.0,1.0,1.0,1.0);
      position      = Vec4f(0.0,0.0,1.0,0.0);
      realPosition  = Vec4f(0.0,0.0,1.0,0.0);
      spotDirection = Vec3d(0.0,0.0,-1.0);
      spotExponent  = 0;
      spotCutoff    = 180;

      constantAttenuation  = 1;
      linearAttenuation    = 0;
      quadraticAttenuation = 0;
    }

    bool enabled;
    bool fixedPosition;
    Vec4f ambientColor;
    Vec4f diffuseColor;
    Vec4f specularColor;
    Vec4f position;
    Vec4f realPosition;
    Vec3d spotDirection;
    float spotExponent;
    float spotCutoff;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
  };




  /// Default constructor. Applies all properties.
  LightNode( BaseNode*           _parent = 0,
		   const std::string&  _name = "<LightNode>");

  /// Destructor.
  virtual ~LightNode() {}

  ACG_CLASSNAME(LightNode);

  /// set current Light Sources
  void enter(GLState& _state, DrawModes::DrawMode _drawmode);
  /// restores original Light Sources
  void leave(GLState& _state, DrawModes::DrawMode _drawmode);

  /// enable LightSource _nr
  void enable()
  { light_.enabled = true; }

  /// disable LightSource _nr
  void disable()
  { light_.enabled = false; }
  
  bool enabled() {
    return light_.enabled;
  }

  /// set position for LightSource
  void position( Vec3d _pos)
  { light_.position = Vec4d( _pos[0],_pos[1],_pos[2],1.0); }

  /// Get the position of the LightSource
  Vec3d position() {
    return Vec3d(light_.position[0],light_.position[1],light_.position[2]);
  }
  
  /// set direction for LightSource ( this switches the light source to directional mode
  void direction( Vec3d _pos)
  { light_.position = Vec4d( _pos[0],_pos[1],_pos[2],0.0); }

  /// set direction for directional LightSource
  void spotDirection( Vec3d _pos)
  { light_.spotDirection = _pos; }
  
  /// get direction for directional LightSource
  Vec3d spotDirection( ) { 
    return Vec3d(light_.spotDirection[0],light_.spotDirection[1],light_.spotDirection[2]); 
  }  

  /// set Ambient color for LightSource _nr
  void ambientColor(  Vec4f _color)
  { light_.ambientColor = _color; }
  
  /// get Ambient color for LightSource _nr
  Vec4f ambientColor()
  { return light_.ambientColor; }

  /// set Diffuse color for LightSource _nr
  void diffuseColor(  Vec4f _color)
  { light_.diffuseColor = _color; }

  /// get Diffuse color for LightSource _nr
  Vec4f diffuseColor()
  { return light_.diffuseColor; }
  
  /// set Specular color for LightSource _nr
  void specularColor(  Vec4f _color)
  { light_.specularColor = _color; }
  
  /// get Specular color for LightSource _nr
  Vec4f specularColor()
  { return light_.specularColor; }

  /// make LightSource fixed or moveable with ModelViewMatrix
  void fixedPosition( bool _state)
  { light_.fixedPosition = _state; }
  
  bool fixedPosition() {
    return light_.fixedPosition;   
  }
  
  void spotExponent(float _exponent) {
     light_.spotExponent = _exponent;
  }
  
  float spotExponent() {
     return light_.spotExponent;
  }
  
  void spotCutoff(float _cutoff) {
     light_.spotCutoff = _cutoff;
  }
  
  float spotCutoff() {
     return light_.spotCutoff;
  }
  
  void constantAttenuation(float _constantAttenuation) {
     light_.constantAttenuation = _constantAttenuation;
  }
  
  float constantAttenuation() {
     return light_.constantAttenuation;
  }
  
  void linearAttenuation(float _linearAttenuation) {
     light_.linearAttenuation = _linearAttenuation;
  }
  
  float linearAttenuation() {
     return light_.linearAttenuation;
  }
  
  void quadraticAttenuation(float _quadraticAttenuation) {
     light_.quadraticAttenuation = _quadraticAttenuation;
  }
  
  float quadraticAttenuation() {
     return light_.quadraticAttenuation;
  }
  

private:

  /// set _light Options in OpenGL for GL_LIGHT#_index
  void setParameters(GLenum _index, LightSource& _light);

  /// get _light Options in OpenGL for GL_LIGHT#_index
  void getParameters(GLenum _index, LightSource& _light);

private:

  GLenum      lightId_;
  
  /// store LightSources of this node
  LightSource light_;

  /// save old LightSources
  LightSource lightSave_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_LIGHT_NODE_HH defined
//=============================================================================

