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

  /// Structure to hold options for one LightSource
  struct LightSource
  {
    //default Constructor
    LightSource()
    {
      // set OpenGL defaults
      enabled = false;
      fixedPosition = false;

      ambientColor  = Vec4f(0.1,0.1,0.1,1);
      diffuseColor  = Vec4f(1,1,1,1);
      specularColor = Vec4f(1,1,1,1);
      position      = Vec4f(0,0,1,0);
      realPosition  = Vec4f(0,0,1,0);
      spotDirection = Vec3f(0,0,-1);
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
    Vec3f spotDirection;
    float spotExponent;
    float spotCutoff;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
  };


public:

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
  void enable(GLenum _nr)
  { lights_[gl2index(_nr)].enabled = true; }

  /// disable LightSource _nr
  void disable(GLenum _nr)
  { lights_[gl2index(_nr)].enabled = false; }

  /// set position ( _pos = 1) or direction ( _pos = 0) of LightSource
  void set_position(GLenum _nr, Vec4f _pos)
  { lights_[gl2index(_nr)].position = _pos; }

  /// set position for Point-LightSource
  void set_position(GLenum _nr, Vec3f _pos)
  { set_position(_nr, Vec4f(_pos[0], _pos[1], _pos[2], 1)); }

  /// set direction for directional LightSource
  void set_direction(GLenum _nr, Vec3f _pos)
  { set_position(_nr, Vec4f(_pos[0], _pos[1], _pos[2], 0)); }

  /// set Ambient color for LightSource _nr
  void set_ambient_color( GLenum _nr, Vec4f _color)
  { lights_[gl2index(_nr)].ambientColor = _color; }

  /// set Diffuse color for LightSource _nr
  void set_diffuse_color( GLenum _nr, Vec4f _color)
  { lights_[gl2index(_nr)].diffuseColor = _color; }

  /// set Specular color for LightSource _nr
  void set_specular_color( GLenum _nr, Vec4f _color)
  { lights_[gl2index(_nr)].specularColor = _color; }

  /// make LightSource fixed or moveable with ModelViewMatrix
  void fixed_position(GLenum _nr, bool _state)
  { lights_[gl2index(_nr)].fixedPosition = _state; }

private:

  /// return index in vector for GL_LIGHT*
  int gl2index( GLenum _nr)
  { return( _nr - GL_LIGHT0); }

  /// return GL_LIGHT* for light _nr
  GLenum index2gl( int _nr)
  { return( _nr + GL_LIGHT0); }

  /// set _light Options in OpenGL for GL_LIGHT#_index
  void set_parameters(GLenum _index, LightSource& _light);

  /// get _light Options in OpenGL for GL_LIGHT#_index
  void get_parameters(GLenum _index, LightSource& _light);

private:

  /// store LightSources of this node
  std::vector<LightSource> lights_;

  /// save old LightSources
  std::vector<LightSource> lightsSave_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_LIGHT_NODE_HH defined
//=============================================================================

