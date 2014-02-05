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
//  CLASS LightSourceNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "LightSourceNode.hh"


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

  
//== IMPLEMENTATION ========================================================== 


LightSourceNode::LightSourceNode( BaseNode*            _parent, 
				  const std::string&   _name) 
  : BaseNode(_parent, _name)
{
  lights_.resize(8);
  lightsSave_.resize(8);
  enable(GL_LIGHT0);;
}

    
//----------------------------------------------------------------------------

void LightSourceNode::enter(GLState& _state, const DrawModes::DrawMode& /* _drawmode */ ) 
{
  // save old lights
  for(unsigned int i=0; i<lightsSave_.size(); i++)
  {
    // save only if enabled
    if(glIsEnabled(index2gl(i)))
    {
      lightsSave_[i].enabled = true;

      /// transfer GL-preferences to lightsSave_
      get_parameters(index2gl(i), lightsSave_[i]);
    }
    else lightsSave_[i].enabled = false;
  }

  // set new lights
  for(unsigned int i=0; i<lights_.size(); i++)
  {
    if(lights_[i].enabled)
    {
      // correct Position for fixed Lights
      if(lights_[i].fixedPosition)
	lights_[i].realPosition = 
	  _state.inverse_modelview()*lights_[i].position;
      else lights_[i].realPosition = lights_[i].position;

      ACG::GLState::enable(index2gl(i));
      set_parameters(index2gl(i), lights_[i]);
    }
    else ACG::GLState::disable(index2gl(i));

  }
}


//----------------------------------------------------------------------------


void LightSourceNode::leave(GLState& /* _state */ , const DrawModes::DrawMode& /* _drawmode*/ )
{
  // restore old enabled lights
  for(unsigned int i=0; i<lights_.size(); i++)
  {
    if(lightsSave_[i].enabled)
    {
      ACG::GLState::enable(index2gl(i));
      set_parameters(index2gl(i), lightsSave_[i]);
    }
    else ACG::GLState::disable(index2gl(i));
  }
}

//----------------------------------------------------------------------------

void LightSourceNode::set_parameters(GLenum _index, LightSource& _light)
{

  // set preferences of _light for GL_LIGHT#_index
  glLightfv(_index, GL_AMBIENT,  (GLfloat *)_light.ambientColor.data());
  glLightfv(_index, GL_DIFFUSE,  (GLfloat *)_light.diffuseColor.data());
  glLightfv(_index, GL_SPECULAR,  (GLfloat *)_light.specularColor.data());

  glLightfv(_index, GL_POSITION,  (GLfloat *)_light.realPosition.data());
  glLightfv(_index, GL_SPOT_DIRECTION,  (GLfloat *)_light.spotDirection.data());

  glLightf(_index, GL_SPOT_EXPONENT,  _light.spotExponent);
  glLightf(_index, GL_SPOT_CUTOFF,  _light.spotCutoff);
  glLightf(_index, GL_CONSTANT_ATTENUATION,  _light.constantAttenuation);
  glLightf(_index, GL_LINEAR_ATTENUATION,  _light.linearAttenuation);
  glLightf(_index, GL_QUADRATIC_ATTENUATION,  _light.quadraticAttenuation);
}

//----------------------------------------------------------------------------

void LightSourceNode::get_parameters(GLenum _index, LightSource& _light)
{
  // get preferences of GL_LIGHT#_index and store them in _light
  glGetLightfv(_index, GL_AMBIENT,  (GLfloat *)_light.ambientColor.data());
  glGetLightfv(_index, GL_DIFFUSE,  (GLfloat *)_light.diffuseColor.data());
  glGetLightfv(_index, GL_SPECULAR,  (GLfloat *)_light.specularColor.data());
  glGetLightfv(_index, GL_POSITION,  (GLfloat *)_light.position.data());
  glGetLightfv(_index, GL_SPOT_DIRECTION,  (GLfloat *)_light.spotDirection.data());

  glGetLightfv(_index, GL_SPOT_EXPONENT,  &_light.spotExponent);
  glGetLightfv(_index, GL_SPOT_CUTOFF,  &_light.spotCutoff);
  glGetLightfv(_index, GL_CONSTANT_ATTENUATION,  &_light.constantAttenuation);
  glGetLightfv(_index, GL_LINEAR_ATTENUATION,  &_light.linearAttenuation);
  glGetLightfv(_index, GL_QUADRATIC_ATTENUATION,  &_light.quadraticAttenuation);
}
//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
