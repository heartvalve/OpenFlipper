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
//  CLASS LightNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "LightNode.hh"
#include <ACG/GL/IRenderer.hh>

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

  ambientColor_         = Vec4f(0.1f,0.1f,0.1f,1.f);
  diffuseColor_         = Vec4f(1.f,1.f,1.f,1.f);
  specularColor_        = Vec4f(1.f,1.f,1.f,1.f);
  
  position_             = Vec4f(0.f,0.f,1.f,0.f);
  realPosition_         = Vec4f(0.f,0.f,1.f,0.f);
  
  spotDirection_        = Vec3d(0.0,0.0,-1.0);
  realSpotDirection_    = Vec3d(0.0,0.0,-1.0);
  
  // Holds initial light source position
  // converted to camera coordinates
  initialPosition_      = Vec4f(0.f, 0.f, 0.f, 0.f);
  initialSpotDirection_ = Vec3d(0.0, 0.0, -1.0);
  initialPositionInit_  = false;
  
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

    sphere_ = new ACG::GLSphere(10, 10);
    cone_ = new ACG::GLCone(10, 10, 1.0f, 1.0f, false, true);
}

//----------------------------------------------------------------------------

LightNode::~LightNode() {
  if (sphere_)
    delete sphere_;

  if (cone_)
    delete cone_;
}
    
//----------------------------------------------------------------------------

void LightNode::getLightSource(LightSource* _light) const
{
  memcpy(_light, &light_, sizeof(LightSource));
}

//----------------------------------------------------------------------------

void LightNode::getLightSourceViewSpace( LightSource* _light ) const
{
  memcpy(_light, &transformedLight_, sizeof(LightSource));
}

//----------------------------------------------------------------------------

void LightNode::boundingBox(ACG::Vec3d& _bbMin, ACG::Vec3d& _bbMax) {
    
    if( visualize_ && !light_.directional() ) {
        ACG::Vec3d r;
        if(light_.fixedPosition())
            r = ACG::Vec3d((double)light_.realPosition_[0],
                           (double)light_.realPosition_[1],
                           (double)light_.realPosition_[2]);
        else
            r = light_.position();
        _bbMin.minimize( r - Vec3d(light_.radius()*3) );
        _bbMax.maximize( r + Vec3d(light_.radius()*3) );
    }
}

//----------------------------------------------------------------------------

void LightNode::draw(GLState& _state, const DrawModes::DrawMode& /*_drawMode*/) {
    
    // Visualize light node
    if(visualize_ && !light_.directional()) {
        
        // Get initial camera coords of light if in fixed
        // mode and if they haven't been computed yet
        if(light_.fixedPosition_ && !light_.initialPositionInit_) {
            light_.initialPosition_ = _state.modelview() * light_.position_;
            light_.initialSpotDirection_ = _state.modelview().transform_vector(light_.spotDirection());
            light_.initialPositionInit_ = true;
        }
        
        if(light_.fixedPosition_) {
             light_.realPosition_ = _state.inverse_modelview() * light_.initialPosition_;
             light_.realSpotDirection_ = _state.inverse_modelview().transform_vector(light_.initialSpotDirection_);
         } else {
             light_.realPosition_ = light_.position_;
             light_.realSpotDirection_ = light_.spotDirection_;
         }
         
         ACG::Vec3f p = ACG::Vec3f(light_.realPosition_[0],
                                   light_.realPosition_[1],
                                   light_.realPosition_[2]);
         ACG::Vec3d spotDir = light_.realSpotDirection_;          
         
         ACG::Vec4f ac = light_.ambientColor();
         ACG::Vec4f dc = light_.diffuseColor();
         ACG::Vec4f sc = light_.specularColor();
         
         // Make light sources appear as bright as possible
         float max = 0;
         for(int i = 0; i < 3; ++i) {
             if(ac[i] > max) max = ac[i];
         }
         ac += ACG::Vec4f(1.0f - max);
         max = 0;
         for(int i = 0; i < 3; ++i) {
             if(dc[i] > max) max = dc[i];
         }
         dc += ACG::Vec4f(1.0f - max);
         max = 0;
         for(int i = 0; i < 3; ++i) {
             if(sc[i] > max) max = sc[i];
         }
         sc += ACG::Vec4f(1.0f - max);
      
         // Backup variables
         GLboolean lighting_backup;
 
         // Origin
         _state.push_modelview_matrix();
         // Transform to light origin and direction
         _state.translate(p[0], p[1], p[2]);
             
         // Set lighting
         glGetBooleanv(GL_LIGHTING, &lighting_backup);
         ACG::GLState::enable(GL_LIGHTING);
 
         // Make light directional just for the drawing
         // of itself
         bool backup_directional = light_.directional();
         ACG::Vec3d backup_position = light_.position();
         
         // Get light id
         lightId_ = lightSourceHandle->getLight(this);
    
         // Return if we don't have a valid light source
         if(lightId_ == GL_INVALID_ENUM) {
             
             // Reset all stored attributes before returning
             if(!lighting_backup) ACG::GLState::disable(GL_LIGHTING);
 
             _state.pop_modelview_matrix();
             
             return;
         }
         
         glLightf(lightId_, GL_SPOT_EXPONENT, 0.0f);
         float pos[4];
         pos [0] = backup_position[0];
         pos [1] = backup_position[1];
         pos [2] = backup_position[2];
         pos [3] = 0.0f;

         glLightfv(lightId_, GL_POSITION, pos);
         
         // Set colors
         float gl_ac[] = {ac[0], ac[1], ac[2], ac[3]};
         glLightfv(lightId_, GL_AMBIENT, gl_ac);
         float gl_dc[] = {dc[0], dc[1], dc[2], dc[3]};
         glLightfv(lightId_, GL_DIFFUSE, gl_dc);
         float gl_sc[] = {sc[0], sc[1], sc[2], sc[3]};
         glLightfv(lightId_, GL_SPECULAR, gl_sc);
         
         ACG::GLState::enable(lightId_);
         
         sphere_->draw(_state, light_.radius());
 
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
 
             // Inverse normal orientation
             cone_->setNormalOrientation(ACG::GLPrimitive::INSIDE);
             cone_->setBottomRadius(light_.radius()/6.0f);
             cone_->setTopRadius(light_.radius()/6.0f);
             cone_->draw(_state, light_.radius()*2.0f);
             _state.translate(0.0, 0.0, light_.radius()*2);
             // Draw arrow tip
             cone_->setBottomRadius(light_.radius()/2.0f);
             cone_->setTopRadius(0.0f);
             cone_->draw(_state, light_.radius());
         }
         
         // Free light id
         lightSourceHandle->removeLight(this);
 
         // Undo state changes
         
         if(!backup_directional) {
             light_.position(backup_position);
         }
 
         // Lighting
         if(!lighting_backup) ACG::GLState::disable(GL_LIGHTING);
 
         _state.pop_modelview_matrix();
     }

}

void LightNode::pick(GLState& _state, PickTarget _target) {

    GLenum prev_depth = _state.depthFunc();
    
    if (_target == PICK_FACE ||
       _target == PICK_ANYTHING) {
        
        // Visualize light node
        if(visualize_ && !light_.directional()) {
            
            // Get initial camera coords of light if in fixed
            // mode and if they haven't been computed yet
            if(light_.fixedPosition_ && !light_.initialPositionInit_) {
                light_.initialPosition_ = _state.modelview() * light_.position_;
                light_.initialSpotDirection_ = _state.modelview().transform_vector(light_.spotDirection());
                light_.initialPositionInit_ = true;
            }
            
            if(light_.fixedPosition_) {
                 light_.realPosition_ = _state.inverse_modelview() * light_.initialPosition_;
                 light_.realSpotDirection_ = _state.inverse_modelview().transform_vector(light_.initialSpotDirection_);
             } else {
                 light_.realPosition_ = light_.position_;
                 light_.realSpotDirection_ = light_.spotDirection_;
             }
             
             // Enable depth test but store original status
             glPushAttrib(GL_DEPTH_BUFFER_BIT);
             ACG::GLState::enable(GL_DEPTH_TEST);
             ACG::GLState::depthFunc(GL_LEQUAL);
             
             _state.pick_set_maximum(1);
             _state.pick_set_name(0);
             
             ACG::Vec3f p = ACG::Vec3f(light_.realPosition_[0],
                                       light_.realPosition_[1],
                                       light_.realPosition_[2]);
             ACG::Vec3d spotDir = light_.realSpotDirection_;          
             ACG::Vec4f c = light_.ambientColor() * light_.brightness_;
          
             // Origin
             _state.push_modelview_matrix();
             // Transform to light origin and direction
             _state.translate(p[0], p[1], p[2]);
                 
             sphere_->draw(_state, light_.radius());

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

                 cone_->setNormalOrientation(ACG::GLPrimitive::OUTSIDE);
                 cone_->setBottomRadius(light_.radius()/6.0f);
                 cone_->setTopRadius(light_.radius()/6.0f);
                 cone_->draw(_state, light_.radius()*2.0f);
                 _state.translate(0.0, 0.0, light_.radius()*2);
                 // Draw arrow tip
                 cone_->setBottomRadius(light_.radius()/2.0f);
                 cone_->setTopRadius(0.0f);
                 cone_->draw(_state, light_.radius());
             }

             _state.pop_modelview_matrix();
             
             ACG::GLState::depthFunc(prev_depth);
         }  
    }
}

void LightNode::enter(GLState& _state, const DrawModes::DrawMode& /* _drawmode */ ) 
{
    if(visualize_) return;
    
    // Get initial camera coords of light if in fixed
    // mode and if they haven't been computed yet
    if(light_.fixedPosition_ && !light_.initialPositionInit_) {
        light_.initialPosition_ = _state.modelview() * light_.position_;
        light_.initialSpotDirection_ = _state.modelview().transform_vector(light_.spotDirection());
        light_.initialPositionInit_ = true;
    }
    
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
            light_.realPosition_ = _state.inverse_modelview() * light_.initialPosition_;
            light_.realSpotDirection_ = _state.inverse_modelview().transform_vector(light_.initialSpotDirection_);
            //std::cerr << "New Light pos :" << _state.inverse_modelview().transform_vector(light_.position) << std::endl;
        } else {
            light_.realPosition_ = light_.position_;
            light_.realSpotDirection_ = light_.spotDirection_;
            //std::cerr << "New Light pos :" << light_.position << std::endl;
        }

        // transform to view space for shader pipeline
        transformedLight_ = light_;
        transformedLight_.position_ = _state.modelview() * light_.realPosition_;
        transformedLight_.spotDirection_ = _state.modelview().transform_vector(light_.realSpotDirection_);

        ACG::GLState::enable(lightId_);
        setParameters(lightId_, light_);
    } else {
        ACG::GLState::disable(lightId_);
    }
}


//----------------------------------------------------------------------------


void LightNode::leave(GLState& /* _state */ , const DrawModes::DrawMode& /* _drawmode*/ )
{
    if(visualize_) return;
    
    // Return if we don't have a valid light source
    if(lightId_ == GL_INVALID_ENUM) return;
      
    // restore old enabled light
    if(lightSave_.enabled_) {
        ACG::GLState::enable(lightId_);
        setParameters(lightId_, lightSave_);
    }
    else {
        ACG::GLState::disable(lightId_);
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
  
  bool directional = _light.directional();
  
  Vec4f& p = _light.realPosition_;
  GLfloat realPos[4] = {(float)p[0], (float)p[1], (float)p[2], (directional ? 0.0f : 1.0f)};
  
  glLightfv(_index, GL_POSITION,  realPos);
  
  if(!directional) glLightfv(_index, GL_SPOT_DIRECTION,  dir);
  if(!directional) glLightf(_index, GL_SPOT_EXPONENT,  _light.spotExponent_);
  if(!directional) glLightf(_index, GL_SPOT_CUTOFF,  _light.spotCutoff_);
  
  glLightf(_index, GL_CONSTANT_ATTENUATION,  _light.constantAttenuation_);
  glLightf(_index, GL_LINEAR_ATTENUATION,  _light.linearAttenuation_);
  glLightf(_index, GL_QUADRATIC_ATTENUATION,  _light.quadraticAttenuation_);
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
}


void LightNode::getRenderObjects( IRenderer* _renderer, GLState& _state , const DrawModes::DrawMode& _drawMode , const Material* _mat )
{

  // fill IRenderer light struct with light data in view-space
  IRenderer::LightData light;

  if (transformedLight_.directional())
    light.ltype = ACG::SG_LIGHT_DIRECTIONAL;
  else if (transformedLight_.spotCutoff() > 179.5f)
    light.ltype = ACG::SG_LIGHT_POINT;
  else
    light.ltype = ACG::SG_LIGHT_SPOT;

#define V4toV3(v) (ACG::Vec3f(v[0], v[1], v[2]))

  light.diffuse = V4toV3(transformedLight_.diffuseColor());
  light.ambient = V4toV3(transformedLight_.ambientColor());
  light.specular = V4toV3(transformedLight_.specularColor());
  
  light.pos = V4toV3(transformedLight_.position());
  light.dir = V4toV3(transformedLight_.direction());

  light.atten[0] = transformedLight_.constantAttenuation();
  light.atten[1] = transformedLight_.linearAttenuation();
  light.atten[2] = transformedLight_.quadraticAttenuation();

  light.spotCutoffExponent[0] = transformedLight_.spotCutoff();
  light.spotCutoffExponent[1] = transformedLight_.spotExponent();

  _renderer->addLight(light);

}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
