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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ========================================================== 

static LightSourceHandle* lightSourceHandle = 0;

//default Constructor
LightSource::LightSource()
{
  // set OpenGL defaults
  enabled_              = false;
  fixedPosition_        = false;

  ambientColor_         = Vec4f(0.1,0.1,0.1,1.0);
  diffuseColor_         = Vec4f(1.0,1.0,1.0,1.0);
  specularColor_        = Vec4f(1.0,1.0,1.0,1.0);
  
  position_             = Vec4f(0.0,0.0,1.0,0.0);
  realPosition_         = Vec4f(0.0,0.0,1.0,0.0);
  
  spotDirection_        = Vec3d(0.0,0.0,-1.0);
  realSpotDirection_    = Vec3d(0.0,0.0,-1.0);
  
  spotExponent_         = 0;
  spotCutoff_           = 180;

  constantAttenuation_  = 1;
  linearAttenuation_    = 0;
  quadraticAttenuation_ = 0;
  
  brightness_           = 1.0f;
  
  radius_               = 0.1f;
}

void LightSource::position( Vec3d _pos){ 
  // Set homogeneous coordinte to 1.0 to get a positional light source
  position_ = Vec4d( _pos[0],_pos[1],_pos[2],1.0); 
}

Vec3d LightSource::position() {
  return Vec3d( position_[0], position_[1], position_[2]);
}

void LightSource::direction( Vec3d _pos) { 
  // Set homogeneous coordinate of position to 0.0 to tell OpenGL
  // that this is a directional light source
  position_ = Vec4d( _pos[0],_pos[1],_pos[2],0.0); 
}

Vec3d LightSource::direction() {
    return Vec3d(position_[0], position_[1], position_[2]);
}

bool LightSource::directional() {
  return ( position_[3] == 0.0  );
}

void LightSource::enable()
{ enabled_ = true; }

void LightSource::disable()
{ enabled_ = false; }

bool LightSource::enabled() {
  return enabled_;
}

void LightSource::spotDirection( Vec3d _pos)
{ spotDirection_ = _pos; }

Vec3d LightSource::spotDirection( ) { 
  return Vec3d(spotDirection_[0],spotDirection_[1],spotDirection_[2]); 
}  

void LightSource::ambientColor(  Vec4f _color)
{ ambientColor_ = _color; }

Vec4f LightSource::ambientColor()
{ return ambientColor_; }

void LightSource::diffuseColor(  Vec4f _color)
{ diffuseColor_ = _color; }

Vec4f LightSource::diffuseColor()
{ return diffuseColor_; }

void LightSource::specularColor(  Vec4f _color)
{ specularColor_ = _color; }

Vec4f LightSource::specularColor()
{ return specularColor_; }

void LightSource::setColor(const Vec4f& _ambient, const Vec4f& _diffuse, const Vec4f& _specular) {
    ambientColor_ = _ambient;
    diffuseColor_ = _diffuse;
    specularColor_ = _specular;
}

void LightSource::fixedPosition( bool _state)
{ fixedPosition_ = _state; }

bool LightSource::fixedPosition() {
  return fixedPosition_;   
}

void LightSource::spotExponent(float _exponent) {
    spotExponent_ = _exponent;
}

float LightSource::spotExponent() {
    return spotExponent_;
}

void LightSource::spotCutoff(float _cutoff) {
    spotCutoff_ = _cutoff;
}

float LightSource::spotCutoff() {
    return spotCutoff_;
}

void LightSource::constantAttenuation(float _constantAttenuation) {
    constantAttenuation_ = _constantAttenuation;
}

float LightSource::constantAttenuation() {
    return constantAttenuation_;
}

void LightSource::linearAttenuation(float _linearAttenuation) {
    linearAttenuation_ = _linearAttenuation;
}

float LightSource::linearAttenuation() {
    return linearAttenuation_;
}

void LightSource::quadraticAttenuation(float _quadraticAttenuation) {
    quadraticAttenuation_ = _quadraticAttenuation;
}

float LightSource::quadraticAttenuation() {
    return quadraticAttenuation_;
}

void LightSource::brightness(float _brightness) {
    brightness_ = _brightness;
}

float LightSource::brightness() const {
    return brightness_;
}

LightNode::LightNode( BaseNode* _parent, 
    const std::string&   _name) 
  : BaseNode(_parent, _name),
    visualize_(false),
    lightId_(GL_INVALID_ENUM) {
  
    if(lightSourceHandle == 0) {
        lightSourceHandle = new LightSourceHandle();
    }
}

//----------------------------------------------------------------------------

LightNode::~LightNode() {
}
    
//----------------------------------------------------------------------------

void LightNode::boundingBox(ACG::Vec3d& _bbMin, ACG::Vec3d& _bbMax) {
    
    if( visualize_ && !light_.directional() ) {
        _bbMin.minimize( light_.position() - Vec3d(light_.radius()*3) );
        _bbMax.maximize( light_.position() + Vec3d(light_.radius()*3) );
    }
}

//----------------------------------------------------------------------------

void LightNode::draw(GLState& _state, DrawModes::DrawMode /*_drawMode*/) {
    
    // Visualize light node
    if(visualize_ && !light_.directional()) {
        
        if(light_.fixedPosition_) {
             light_.realPosition_ = _state.inverse_modelview() * light_.position_;
             light_.realSpotDirection_ = _state.inverse_modelview().transform_vector(light_.spotDirection_);
         } else {
             light_.realPosition_ = light_.position_;
             light_.realSpotDirection_ = light_.spotDirection_;
         }
         
         ACG::Vec3f p = ACG::Vec3f(light_.realPosition_[0],
                                   light_.realPosition_[1],
                                   light_.realPosition_[2]);
         ACG::Vec3d spotDir = light_.realSpotDirection_;          
         ACG::Vec4f c = light_.ambientColor() * light_.brightness_;
      
      // Backup variables
         ACG::Vec4f base_color_backup;
         GLboolean lighting_backup;
 
         GLUquadricObj* quadric = gluNewQuadric();
 
         // Origin
         _state.push_modelview_matrix();
         // Transform to light origin and direction
         _state.translate(p[0], p[1], p[2]);
             
         // Set lighting
         glGetBooleanv(GL_LIGHTING, &lighting_backup);
         glDisable(GL_LIGHTING);
 
         // Set color
         base_color_backup = _state.base_color();
         _state.set_base_color(c);
 
         gluSphere( quadric, light_.radius(), 10, 10 );
 
         // Visualize spot cone (or direction)
         if(light_.spotCutoff() < 180.0f) {
             // Note: if the cutoff angle is 180, the light source
             // is a point light emitting light into all directions equally
             
             // Rotate into light direction
             ACG::Vec3d z = ACG::Vec3d(0.0f, 0.0f, 1.0f);
             ACG::Vec3d spot = spotDir;
             float angle = acos((z | spot)/(z.norm()*spot.norm()));
             angle = angle*360/(2*M_PI);
             ACG::Vec3d rA = z % spot;
             _state.rotate(angle, rA[0], rA[1], rA[2]);
 
             gluCylinder( quadric, light_.radius()/6, light_.radius()/6, light_.radius()*2, 10, 10 );
             _state.translate(0.0, 0.0, light_.radius()*2);
             // Draw arrow tip
             gluCylinder( quadric, light_.radius()/2, 0, light_.radius(), 10, 10 );
         }
 
         // Undo state changes
 
         // Color
         _state.set_base_color(base_color_backup);
 
         // Lighting
         if(lighting_backup) glEnable(GL_LIGHTING);
 
         gluDeleteQuadric(quadric);
 
         _state.pop_modelview_matrix();
     }

}

void LightNode::enter(GLState& _state, DrawModes::DrawMode /* _drawmode */ ) 
{
    if(visualize_) return;
    
    // Get light id
    lightId_ = lightSourceHandle->getLight(this);
    
    // Return if we don't have a valid light source
    if(lightId_ == GL_INVALID_ENUM) return;
    
    /// transfer GL-preferences to lightSave_
    getParameters(lightId_, lightSave_);
  
    // save old light
    lightSave_.enabled_ = glIsEnabled(lightId_);

    if(light_.enabled_ ) {
        // correct Position for fixed Lights
        if(light_.fixedPosition_) {
            light_.realPosition_ = _state.inverse_modelview() * light_.position_;
            light_.realSpotDirection_ = _state.inverse_modelview().transform_vector(light_.spotDirection_);
            //std::cerr << "New Light pos :" << _state.inverse_modelview().transform_vector(light_.position) << std::endl;
        } else {
            light_.realPosition_ = light_.position_;
            light_.realSpotDirection_ = light_.spotDirection_;
            //std::cerr << "New Light pos :" << light_.position << std::endl;
        }

        glEnable(lightId_);
        setParameters(lightId_, light_);
    } else {
        glDisable(lightId_);
    }
}


//----------------------------------------------------------------------------


void LightNode::leave(GLState& /* _state */ , DrawModes::DrawMode /* _drawmode*/ )
{
    if(visualize_) return;
    
    // Return if we don't have a valid light source
    if(lightId_ == GL_INVALID_ENUM) return;
      
    // restore old enabled light
    if(lightSave_.enabled_) {
        glEnable(lightId_);
        setParameters(lightId_, lightSave_);
    }
    else {
        glDisable(lightId_);
    }
    
    // Free light id
    lightSourceHandle->removeLight(this);
}

//----------------------------------------------------------------------------

void LightNode::setParameters(GLenum _index, LightSource& _light)
{

  // Multiply colors by brightness
  Vec4f& a = _light.ambientColor_;
  GLfloat ambient[4] = {a[0]*_light.brightness_, 
                        a[1]*_light.brightness_,
                        a[2]*_light.brightness_,
                        a[3]*_light.brightness_};

  Vec4f& d = _light.diffuseColor_;
  GLfloat diffuse[4] = {d[0]*_light.brightness_,
                        d[1]*_light.brightness_,
                        d[2]*_light.brightness_,
                        d[3]*_light.brightness_};
                        
  Vec4f& s = _light.specularColor_;
  GLfloat specular[4] = {s[0]*_light.brightness_,
                         s[1]*_light.brightness_,
                         s[2]*_light.brightness_,
                         s[3]*_light.brightness_};
  
  // set preferences of _light for GL_LIGHT#_index
  glLightfv(_index, GL_AMBIENT,  ambient);
  glLightfv(_index, GL_DIFFUSE,  diffuse);
  glLightfv(_index, GL_SPECULAR, specular);

  Vec3d& sd = _light.realSpotDirection_;
  GLfloat dir[3] = {(float)sd[0], (float)sd[1], (float)sd[2]};
  
  glLightfv(_index, GL_POSITION,  (GLfloat *)_light.realPosition_.data());
  glLightfv(_index, GL_SPOT_DIRECTION,  dir);

  glLightf(_index, GL_SPOT_EXPONENT,  _light.spotExponent_);
  glLightf(_index, GL_SPOT_CUTOFF,  _light.spotCutoff_);
  glLightf(_index, GL_CONSTANT_ATTENUATION,  _light.constantAttenuation_);
  glLightf(_index, GL_LINEAR_ATTENUATION,  _light.linearAttenuation_);
  glLightf(_index, GL_QUADRATIC_ATTENUATION,  _light.quadraticAttenuation_);
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
  glGetLightfv(_index, GL_AMBIENT,  (GLfloat *)_light.ambientColor_.data());
  glGetLightfv(_index, GL_DIFFUSE,  (GLfloat *)_light.diffuseColor_.data());
  glGetLightfv(_index, GL_SPECULAR,  (GLfloat *)_light.specularColor_.data());
  glGetLightfv(_index, GL_POSITION,  (GLfloat *)_light.position_.data());
  glGetLightfv(_index, GL_SPOT_DIRECTION,  (GLfloat *)_light.spotDirection_.data());

  glGetLightfv(_index, GL_SPOT_EXPONENT,  &_light.spotExponent_);
  glGetLightfv(_index, GL_SPOT_CUTOFF,  &_light.spotCutoff_);
  glGetLightfv(_index, GL_CONSTANT_ATTENUATION,  &_light.constantAttenuation_);
  glGetLightfv(_index, GL_LINEAR_ATTENUATION,  &_light.linearAttenuation_);
  glGetLightfv(_index, GL_QUADRATIC_ATTENUATION,  &_light.quadraticAttenuation_);
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
