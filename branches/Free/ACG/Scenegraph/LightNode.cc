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
//  CLASS LightNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "LightNode.hh"


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

GLenum nextLight = GL_LIGHT0;  
  
//== IMPLEMENTATION ========================================================== 


LightNode::LightNode( BaseNode*            _parent, 
		      const std::string&   _name) 
  : BaseNode(_parent, _name),
    lightId_(0)
{
  lightId_ = nextLight;
  nextLight++;
  //std::cerr << "Maximum light sources : " << GL_MAX_LIGHTS << std::endl;
}

    
//----------------------------------------------------------------------------

void LightNode::enter(GLState& _state, DrawModes::DrawMode /* _drawmode */ ) 
{


  /// transfer GL-preferences to lightSave_
  getParameters(lightId_, lightSave_);
  
  // save old light
  lightSave_.enabled = glIsEnabled(lightId_);

  if(light_.enabled) {
    // correct Position for fixed Lights
    if(light_.fixedPosition) {
      light_.realPosition = _state.inverse_modelview() * light_.position;
//       std::cerr << "New Light pos :" << _state.inverse_modelview().transform_vector(light_.position) << std::endl;
    } else {
      light_.realPosition = light_.position;
//       std::cerr << "New Light pos :" << light_.position << std::endl;
    }

    

    glEnable(lightId_);
    setParameters(lightId_, light_);
  } else 
    glDisable(lightId_);

}


//----------------------------------------------------------------------------


void LightNode::leave(GLState& /* _state */ , DrawModes::DrawMode /* _drawmode*/ )
{
  // restore old enabled light
  if(lightSave_.enabled)
  {
    glEnable(lightId_);
    setParameters(lightId_, lightSave_);
  }
  else glDisable(lightId_);
}

//----------------------------------------------------------------------------

void LightNode::setParameters(GLenum _index, LightSource& _light)
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
//   std::cerr << " set GL_AMBIENT  : " << _index << " "  << _light.ambientColor << std::endl;
//   std::cerr << " set GL_DIFFUSE  : " << _index << " "  << _light.diffuseColor << std::endl;
//   std::cerr << " set GL_SPECULAR  : " << _index << " "  << _light.specularColor << std::endl;
//   
//   std::cerr << " set GL_POSITION  : " << _index << " "  << _light.realPosition << std::endl;
//   std::cerr << " set GL_SPOT_DIRECTION  : " << _index << " "  << _light.spotDirection << std::endl;
//   
//   std::cerr << " set GL_SPOT_EXPONENT  : " << _index << " "  << _light.spotExponent << std::endl;
//   std::cerr << " set GL_SPOT_CUTOFF  : " << _index << " "  << _light.spotCutoff << std::endl;
//   std::cerr << " set GL_CONSTANT_ATTENUATION  : " << _index << " "  << _light.constantAttenuation << std::endl;
//   std::cerr << " set GL_LINEAR_ATTENUATION  : " << _index << " "  << _light.linearAttenuation << std::endl;
//   std::cerr << " set GL_QUADRATIC_ATTENUATION  : " << _index << " "  << _light.quadraticAttenuation << std::endl;
//   std::cerr << "===============================================================================" << std::endl;
}

//----------------------------------------------------------------------------

void LightNode::getParameters(GLenum _index, LightSource& _light)
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
//   std::cerr << " get GL_AMBIENT  : " << _index << " "  << _light.ambientColor << std::endl;
//   std::cerr << " get GL_DIFFUSE  : " << _index << " "  << _light.diffuseColor << std::endl;
//   std::cerr << " get GL_SPECULAR  : " << _index << " "  << _light.specularColor << std::endl;
//   
//   std::cerr << " get GL_POSITION  : " << _index << " "  << _light.realPosition << std::endl;
//   std::cerr << " get GL_SPOT_DIRECTION  : " << _index << " "  << _light.spotDirection << std::endl;
//     
//   std::cerr << " get GL_SPOT_EXPONENT  : " << _index << " "  << _light.spotExponent << std::endl;
//   std::cerr << " get GL_SPOT_CUTOFF  : " << _index << " "  << _light.spotCutoff << std::endl;
//   std::cerr << " get GL_CONSTANT_ATTENUATION  : " << _index << " "  << _light.constantAttenuation << std::endl;
//   std::cerr << " get GL_LINEAR_ATTENUATION  : " << _index << " "  << _light.linearAttenuation << std::endl;
//   std::cerr << " get GL_QUADRATIC_ATTENUATION  : " << _index << " "  << _light.quadraticAttenuation << std::endl;
}
//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
