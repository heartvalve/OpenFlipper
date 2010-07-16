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
 *   $Revision: 6753 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-08-05 12:38:20 +0200 (Mi, 05. Aug 2009) $                   *
 *                                                                           *
\*===========================================================================*/


#include "TypeLight.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/BasePlugin/PluginFunctionsViewControls.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

#define DEF0 "Default Light 0.lgt"
#define DEF1 "Default Light 1.lgt"
#define DEF2 "Default Light 2.lgt"

TypeLightPlugin::TypeLightPlugin() :
    defaultLights_(true),
    lightButton_(0),
    contextmenu_(0),
    onlyTargets_(false),
    lastPoint_hitSphere_(false),
    ratioTrackballs_(1.0) {
        
        // Reset transformation matrix
        light_matrix_.identity();  
}

bool TypeLightPlugin::registerType() {
  addDataType("Light",tr("Light"));
  setTypeIcon( "Light", "LightType.png");
  
  return true;
}

void TypeLightPlugin::slotAllCleared() {
    
    // Add default lights to active scene
    addDefaultLights();
}

void TypeLightPlugin::removeDefaultLights() {
    
    int light0 = PluginFunctions::getObjectId(DEF0);
    int light1 = PluginFunctions::getObjectId(DEF1);
    int light2 = PluginFunctions::getObjectId(DEF2);
    
    if(light0 > 0)
        emit deleteObject(light0);
    if(light1 > 0)
        emit deleteObject(light1);
    if(light2 > 0)
        emit deleteObject(light2);
    
    defaultLights_ = false;
}

void TypeLightPlugin::addDefaultLights() {
    
    // Test if light sources already exist
    int light0 = PluginFunctions::getObjectId(DEF0);
    int light1 = PluginFunctions::getObjectId(DEF1);
    int light2 = PluginFunctions::getObjectId(DEF2);
    
    if(light0 == -1) {
        // Create light 0
        light0 = addDefaultLight(DEF0);
        
        BaseObjectData* obj0(0);
        
        if(PluginFunctions::getObject( light0, obj0 )) {
            
            LightObject* lightObject0 = PluginFunctions::lightObject(obj0);
            LightSource* lightSrc0 = PluginFunctions::lightSource(lightObject0);
            
            if( lightSrc0 ) { 
                lightSrc0->direction(ACG::Vec3d(0.0,0.0,1.0) );
                lightSrc0->diffuseColor( ACG::Vec4f(0.7,0.7,0.7,0.0));
                lightSrc0->specularColor(ACG::Vec4f(0.7,0.7,0.7,0.0));
                lightSrc0->enable();
                lightSrc0->fixedPosition(true);
                lightObject0->setName(DEF0);
            }
         
            emit updatedObject(light0, UPDATE_ALL);
        }
    }
    
    if(light1 == -1) {
    
        // Create light 1
        light1 = addDefaultLight(DEF1);
  
        BaseObjectData* obj1(0);
        if(PluginFunctions::getObject( light1, obj1 )) {
            
            LightObject* lightObject1 = PluginFunctions::lightObject(obj1);
            LightSource* lightSrc1 = PluginFunctions::lightSource(lightObject1);
    
            if( lightSrc1 ) {
                lightSrc1->direction(ACG::Vec3d(-1.0,  1.0, 0.7) );
                lightSrc1->diffuseColor( ACG::Vec4f(0.7,0.7,0.7,0.0));
                lightSrc1->specularColor(ACG::Vec4f(0.7,0.7,0.7,0.0));
                lightSrc1->enable();
                lightSrc1->fixedPosition(true);
                lightObject1->setName(DEF1);
            }
        
            emit updatedObject(light1, UPDATE_ALL);
        }
    }
  
    if(light2 == -1) {
        //Generate the default lights
        light2 = addDefaultLight(DEF2);
  
        BaseObjectData* obj2(0);
        if(PluginFunctions::getObject( light2, obj2 )) {
    
            LightObject* lightObject2 = PluginFunctions::lightObject(obj2);
            LightSource* lightSrc2 = PluginFunctions::lightSource(lightObject2);
    
            if( lightSrc2 ) {
                lightSrc2->direction(ACG::Vec3d( 1.0,  1.0, 0.7) );
                lightSrc2->diffuseColor(ACG::Vec4f(0.7,0.7,0.7,0.0));
                lightSrc2->specularColor(ACG::Vec4f(0.7,0.7,0.7,0.0));
                lightSrc2->enable();
                lightSrc2->fixedPosition(true);
                lightObject2->setName(DEF2);
            }
        
            emit updatedObject(light2, UPDATE_ALL);
        }
    }
    
    defaultLights_ = true;
}

void TypeLightPlugin::pluginsInitialized(){
  
    // Add toolbar icon with context menu
    QToolBar* viewerToolbar = 0;

    emit getToolBar( "Viewer Toolbar", viewerToolbar );

    if ( viewerToolbar == 0 ) {
        emit log(LOGERR,"Unable to get Viewer Toolbar!");
    } else {
        // Create toolbar action
        lightButton_ = new QToolButton( viewerToolbar );
        lightButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"light-mode.png") );
        lightButton_->setMinimumSize( 16, 16 );
        lightButton_->setMaximumSize( 32, 32 );
        lightButton_->setToolTip(tr("Switch to <b>light</b> mode."));
        lightButton_->setWhatsThis(tr(
                      "Switch to <b>light</b> mode.<br>"
                      "Rotate lights using left mouse button."));

        connect( lightButton_, SIGNAL( clicked() ), this, SLOT( setLightMode() ) );
        
        QList< QAction *> toolbarActions = viewerToolbar->actions();
        QAction* pickAction = 0;
        for ( int i = 0 ; i < toolbarActions.size() ; ++i ) {
          if ( toolbarActions[i]->text() == "Pick" )
            pickAction = toolbarActions[i];
        }
      
        if (pickAction)
          viewerToolbar->insertWidget( pickAction, lightButton_ )->setText(tr("Light"));
        else
          viewerToolbar->addWidget( lightButton_ )->setText(tr("Light"));
        
        // Create context menu
        QActionGroup* group = new QActionGroup(lightButton_);
        QAction* radioOne = new QAction("All lights", group);
        radioOne->setCheckable(true);
        radioOne->setChecked((onlyTargets_ ? false : true));
        QAction* radioTwo  = new QAction("Target lights", group);
        radioTwo->setCheckable(true);
        radioTwo->setChecked((onlyTargets_ ? true : false));
        contextmenu_ = new QMenu("Choose target light sources", 0);
        contextmenu_->addActions(group->actions());
        
        connect( radioOne, SIGNAL(triggered(bool)), this, SLOT(allLights(bool)) );
        connect( radioTwo, SIGNAL(triggered(bool)), this, SLOT(targetLights(bool)) );
    }
    
    // Disable the build in light management and use this plugins light handling
    PluginFunctions::disableExaminerLightHandling();
  
    // Add default light sources to active scene
    addDefaultLights();
}

void TypeLightPlugin::allLights(bool _b) {
    onlyTargets_ = !_b;
}

void TypeLightPlugin::targetLights(bool _b) {
    onlyTargets_ = _b;
}

void TypeLightPlugin::setLightMode() {
    
    // Reset transformation matrix
    light_matrix_.identity();
    
    // Context menu requested
    contextmenu_->exec(QPoint(lightButton_->mapToGlobal(QPoint(0, lightButton_->height()))));
    
    PluginFunctions::actionMode(Viewer::LightMode);
}

int TypeLightPlugin::addDefaultLight(QString _name) {
    
    // new object data struct
    LightObject* object = new LightObject();

    // call the local function to update names
    object->setName( _name );

    object->defaultLight(true);
    
    object->update();

    object->hide();
    
    emit emptyObjectAdded (object->id() );

    return object->id();
}

int TypeLightPlugin::addEmpty() {
    
  // new object data struct
  LightObject * object = new LightObject();

  QString name = get_unique_name(object);

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  object->defaultLight(false);
  
  object->update();

  object->hide();
  
  emit emptyObjectAdded (object->id() );

  return object->id();
}

void TypeLightPlugin::addedEmptyObject(int _id) {
    
    BaseObject* obj = 0;
    PluginFunctions::getObject(_id, obj);
    
    if(!obj) return;
    
    LightObject* light = 0;
    light = dynamic_cast<LightObject*>(obj);
    
    if(!light) return;
    
    // Skip default light sources
    if(light->defaultLight()) return;
    
    removeDefaultLights();
    lightSources_.push_back(_id);
}

void TypeLightPlugin::objectDeleted(int _id) {
    
    for(uint i = 0; i < lightSources_.size(); ++i) {
        if(lightSources_[i] == _id) lightSources_.erase(lightSources_.begin() + i);
    }
    
    if(lightSources_.empty() && !defaultLights_) {
        addDefaultLights();
    }
    
}

QString TypeLightPlugin::get_unique_name(LightObject* _object) {
    
    bool name_unique = false;

    int cur_idx = _object->id();

    while(!name_unique)
    {
        name_unique = true;

        QString cur_name = QString(tr("Light %1.lgt").arg( cur_idx ));

        PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DATA_LIGHT );
        for(; o_it != PluginFunctions::objectsEnd(); ++o_it) {
            
            if( o_it->name() == cur_name) {
                name_unique = false;
                cur_idx += 10;
                break;
            }
        }
    }

    return QString(tr("Light %1.lgt").arg( cur_idx ));
}

void TypeLightPlugin::slotMouseEventLight(QMouseEvent* _event) {
    
    // Only react if in light mode
    if(PluginFunctions::actionMode() == Viewer::LightMode) {
    
        // Get gl state
        ACG::GLState& state = PluginFunctions::viewerProperties().glState();
    
        QPoint pos (_event->x(), _event->y());
        
        switch (_event->type())
        {
            case QEvent::MouseButtonPress:
            {
              lastPoint_hitSphere_ = mapToSphere( lastPoint2D_= pos,
                         lastPoint3D_, state.viewport_width(), state.viewport_height() );
              updateLights();
              break;
            }
            
            case QEvent::MouseMove:
            {
              // rotate lights
              if (_event->buttons() & Qt::LeftButton)
              {
                QPoint newPoint2D = pos;

                if ( (newPoint2D.x() < 0) || (newPoint2D.x() > (int)state.viewport_width()) ||
                     (newPoint2D.y() < 0) || (newPoint2D.y() > (int)state.viewport_height()) )
                 return;


                ACG::Vec3d newPoint3D;
                bool newPoint_hitSphere = mapToSphere( newPoint2D, newPoint3D, state.viewport_width(), state.viewport_height() );
                int active = PluginFunctions::activeExaminer();

                ACG::Vec3d axis(1.0,0.0,0.0);
                double angle(0.0);

                if ( lastPoint_hitSphere_ )
                {
                    if ( newPoint_hitSphere ) {
                        
                        axis = lastPoint3D_ % newPoint3D;
                        axis.normalize();
                        axis = state.modelview().transform_vector(axis);
                        double cos_angle = ( lastPoint3D_ | newPoint3D );
                        
                        if ( fabs(cos_angle) < 1.0 ) {
                            angle = acos( cos_angle );
                            angle *= 180 / M_PI;
                        }
                    }
                 
                    rotateLights(axis, angle);
                }

                lastPoint2D_ = newPoint2D;
                lastPoint3D_ = newPoint3D;
                lastPoint_hitSphere_ = newPoint_hitSphere;

              }
              break;
            }

        default: // avoid warning
          break;
        }
    }
    
    // interaction
    ACG::SceneGraph::MouseEventAction action(_event,PluginFunctions::viewerProperties().glState());
    PluginFunctions::traverse(action);
}

bool TypeLightPlugin::mapToSphere(const QPoint& _v2D, ACG::Vec3d& _v3D, int _width, int _height) const
{
    if ( (_v2D.x() >= 0) && (_v2D.x() < _width) &&
         (_v2D.y() >= 0) && (_v2D.y() < _height) ) {
    
        double x = 2.0 * _v2D.x() / (double)_width - 1;
        double y = 1 - 2.0 * _v2D.y() / (double)_height;
        
        //double x   = (double)(_v2D.x() - ((double)_width / 2.0))  / (double)_width;
        //double y   = (double)(((double)_height / 2.0) - _v2D.y()) / (double)_height;
        
        //double sinx         = sin(0.5 * M_PI * x);
        //double siny         = sin(0.5 * M_PI * y);
        
        double x2y2   =  x*x + y*y; // sinx * sinx + siny * siny;

        _v3D[0] = x; //*/sinx;
        _v3D[1] = y; //*/siny;
        _v3D[2] = x2y2 < 1.0 ? sqrt(1.0 - x2y2) : 0.0;
        
        _v3D.normalize();
        
        return true;
        
    } else {
        return false;
    }
}

void TypeLightPlugin::rotateLights(ACG::Vec3d& _axis, double _angle) {
    
    ACG::GLState& state = PluginFunctions::viewerProperties().glState();
    ACG::Vec3d c = PluginFunctions::trackBallCenter( ACG::Vec3d(), PluginFunctions::activeExaminer() );
    ACG::Vec3d s_c = PluginFunctions::sceneCenter( PluginFunctions::activeExaminer() );
    int active = PluginFunctions::activeExaminer();
    
    double max_s_radius = 0.0;
    ratioTrackballs_ = 1.0;
    
    for ( PluginFunctions::ObjectIterator o_it((onlyTargets_ ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        LightObject* lightObject = PluginFunctions::lightObject(o_it);
        if(lightObject != 0) {
            
            LightSource* source = PluginFunctions::lightSource(lightObject);
            if(source != 0) {
                
                // Update max light radius
                if(!source->directional()) {
                    
                    // Get light source's position
                    ACG::Vec3d p = source->position();
                    
                    // Small trackball's radius
                    double s_radius = (p - c).length();
                    if(s_radius > max_s_radius) max_s_radius = s_radius;
                }
            }
        }
    }
    
    /*
    Compute ratio of the trackballs' radia:
    Choose light source that is most distant to the virtual trackball's
    rotation center. Compute its corresponding trackball's radius
    as (position - trackball_center).length().
    Now when moving the trackball that exactly fills the screen, we
    scale the angle by which the light sources should be rotated by
    the ratio between the "big" screen-size trackball and the farthest
    light source's trackball.
    */
    
    // Big trackball's radius
    ACG::Vec3d e = PluginFunctions::eyePos(active);
    double b_radius = ((e + (c-e).normalize()*state.near_plane()) - c).length() * 0.5;
    // Compute ratio
    double ratio = max_s_radius != 0.0 ? b_radius/max_s_radius : 0.0;
    if(ratio > ratioTrackballs_) ratioTrackballs_ = ratio;
    
    // Rotate
    light_matrix_.rotate(_angle * ratioTrackballs_, _axis[0], _axis[1], _axis[2]);
    
    // Transform positions
    updateLights();
}

void TypeLightPlugin::updateLights() {
    
    ACG::Vec3d c = PluginFunctions::trackBallCenter( ACG::Vec3d(), PluginFunctions::activeExaminer() );
    
    for ( PluginFunctions::ObjectIterator o_it((onlyTargets_ ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        LightObject* lightObject = PluginFunctions::lightObject(o_it);
        if(lightObject != 0) {
            
            LightSource* source = PluginFunctions::lightSource(lightObject);
            if(source != 0) {

                // Rotate light source relatively to trackball center:
                
                // Get light source's position
                ACG::Vec3d p = source->position();
                // Vector point from trackball center to light source position
                ACG::Vec3d r = p - c;
                // Rotate this vector
                r = light_matrix_.transform_vector(r);
                // ... and set new position.
                source->position(c + r);
                
                emit updatedObject(lightObject->id(), UPDATE_ALL);
            }
        }
    }
    
    // Reset light source transformation matrix
    light_matrix_.identity();
}

Q_EXPORT_PLUGIN2( typelightplugin , TypeLightPlugin );

