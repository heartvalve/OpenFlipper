/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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

#include "TypeLight.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/BasePlugin/PluginFunctionsViewControls.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

#define DEF0 "Default Light 0.lgt"
#define DEF1 "Default Light 1.lgt"
#define DEF2 "Default Light 2.lgt"

TypeLightPlugin::TypeLightPlugin() :
        defaultLights_(true),
        toolbar_(0),
        lightOptions_(0),
        lightAction_(0),
        moveMode_(0),
        translateMode_(0),
        rotateMode_(0),
        allLightsMode_(0),
        onlyTargets_(false),
        planeDepth_(0.0f),
        transVec_(0.0),
        rotation_(true),
        radius_(0.0),
        depth_(0.0f),
        lightCenter_(0.0),
        lightRadius_(0.0),
        lightId_(-1),
        hitLightTrackball_(false),
        hitTrackball_(false)
{

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
                lightSrc0->diffuseColor( ACG::Vec4f(0.55,0.55,0.55,0.0));
                lightSrc0->specularColor(ACG::Vec4f(0.55,0.55,0.55,0.0));
                lightSrc0->enable();
                lightSrc0->brightness(1.0);
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
                lightSrc1->diffuseColor( ACG::Vec4f(0.55,0.55,0.55,0.0));
                lightSrc1->specularColor(ACG::Vec4f(0.55,0.55,0.55,0.0));
                lightSrc1->enable();
                lightSrc1->brightness(1.0);
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
                lightSrc2->diffuseColor(ACG::Vec4f(0.55,0.55,0.55,0.0));
                lightSrc2->specularColor(ACG::Vec4f(0.55,0.55,0.55,0.0));
                lightSrc2->enable();
                lightSrc2->brightness(1.0);
                lightSrc2->fixedPosition(true);
                lightObject2->setName(DEF2);
            }
        
            emit updatedObject(light2, UPDATE_ALL);
        }
    }
    
    defaultLights_ = true;
}

void TypeLightPlugin::showReducedUi(bool reduced) {
    viewerToolbarAction_->setVisible(!reduced);
}

void TypeLightPlugin::pluginsInitialized(){
  
  if ( OpenFlipper::Options::gui() ){
    //PICKMODES
    emit addPickMode("Separator");
    emit addHiddenPickMode("MoveLights");
    emit setPickModeMouseTracking ("MoveLights", true);
    emit addHiddenPickMode("TranslateLights");
    emit setPickModeMouseTracking ("TranslateLights", true);
    emit addHiddenPickMode("RotateLights");
    emit setPickModeMouseTracking ("RotateLights", true);
    
    // Create tool bar
    toolbar_ = new QToolBar(tr("Transform light sources"));
    
    lightAction_ = new QAction(tr("<B>Light mode</B><br>Transform light sources in 3D."), toolbar_);
    lightAction_->setStatusTip(tr("Transform light sources in 3D."));
    lightAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"light-mode.png") );
    lightAction_->setCheckable(true);
    toolbar_->addAction(lightAction_);
    
    connect(lightAction_, SIGNAL(triggered(bool)), this, SLOT(slotLightModeRequest(bool)) );

    // Add toolbar icon with context menu
    QToolBar* viewerToolbar = 0;

    emit getToolBar( "Viewer Toolbar", viewerToolbar );

    if ( viewerToolbar == 0 ) {
        emit log(LOGERR,"Unable to get Viewer Toolbar!");
    } else {
        
        // Search for picking action...
        QList< QAction *> toolbarActions = viewerToolbar->actions();
        QAction* pickAction = 0;
        for ( int i = 0 ; i < toolbarActions.size() ; ++i ) {
            if ( toolbarActions[i]->text() == "Pick" )
                pickAction = toolbarActions[i];
        }
      
        // And append light mode button if found.
        if (pickAction)
            viewerToolbarAction_ = viewerToolbar->insertWidget( pickAction, toolbar_ );
        else
            viewerToolbarAction_ = viewerToolbar->addWidget( toolbar_ );
        viewerToolbarAction_->setText(tr("Light"));
        
    }
    
    lightOptions_ = new QToolBar(tr("Transform light sources"));
    lightOptions_->setAttribute(Qt::WA_AlwaysShowToolTips, true);
    
    QActionGroup* pickGroup = new QActionGroup(lightOptions_);
    pickGroup->setExclusive (false);

    moveMode_ = new QAction(tr("Move light source"), pickGroup);
    moveMode_->setStatusTip(tr("Move light source on virtual trackball around trackball center."));
    moveMode_->setToolTip(tr("Move light source on virtual trackball around trackball center."));
    moveMode_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"light-move.png") );
    moveMode_->setCheckable(true);
    lightOptions_->addAction(moveMode_);

    translateMode_ = new QAction(tr("Translate light source"), pickGroup);
    translateMode_->setStatusTip(tr("Translate light source on plane parallely to viewing plane."));
    translateMode_->setToolTip(tr("Translate light source on plane parallely to viewing plane."));
    translateMode_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"light-translate.png") );
    translateMode_->setCheckable(true);
    lightOptions_->addAction(translateMode_);

    rotateMode_ = new QAction(tr("Rotate spotlight direction"), pickGroup);
    rotateMode_->setStatusTip(tr("Rotate the spotlight direction of a light source."));
    rotateMode_->setToolTip(tr("Rotate the spotlight direction of a light source."));
    rotateMode_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"light-rotate.png") );
    rotateMode_->setCheckable(true);
    lightOptions_->addAction(rotateMode_);
    
    lightOptions_->addSeparator ();
    
    allLightsMode_ = new QAction(tr("All lights"), lightOptions_);
    allLightsMode_->setStatusTip(tr("Transform all light sources if checked. If not, only apply to target light sources."));
    allLightsMode_->setToolTip(tr("Transform all light sources if checked. If not, only apply to target light sources."));
    allLightsMode_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"light-alllights.png") );
    allLightsMode_->setCheckable(true);
    lightOptions_->addAction(allLightsMode_);
        
    connect(pickGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotPickModeRequest(QAction*)) );
    connect(allLightsMode_, SIGNAL(triggered(bool)), this, SLOT(slotSetAllOrTarget(bool)) );

    emit setPickModeToolbar ("MoveLights", lightOptions_);
    emit setPickModeToolbar ("TranslateLights", lightOptions_);
    emit setPickModeToolbar ("RotateLights", lightOptions_);
    
  }
  // Disable the build in light management and use this plugins light handling
  PluginFunctions::disableExaminerLightHandling();

  // Add default light sources to active scene
  addDefaultLights();
}

void TypeLightPlugin::slotPickModeChanged (const std::string& /*_pickMode*/) {
    
    if(PluginFunctions::actionMode() != Viewer::LightMode)
        lightAction_->setChecked(false);
}

void TypeLightPlugin::slotLightModeRequest(bool /*_checked*/) {
    
    PluginFunctions::actionMode(Viewer::LightMode);
    PluginFunctions::pickMode("MoveLights");

    lightAction_->setChecked(true);
    
    // Chose "MoveLights" pick mode per default
    moveMode_->setChecked(true);
    translateMode_->setChecked(false);
    rotateMode_->setChecked(false);
}

void TypeLightPlugin::slotPickModeRequest(QAction* _action) {
    
    PluginFunctions::actionMode(Viewer::LightMode);
    
    if (_action == moveMode_){
        PluginFunctions::pickMode("MoveLights");

        lightAction_->setChecked(true);
        moveMode_->setChecked(true);
        translateMode_->setChecked(false);
        rotateMode_->setChecked(false);
        
    } else if (_action == translateMode_){
        PluginFunctions::pickMode("TranslateLights");

        lightAction_->setChecked(true);
        moveMode_->setChecked(false);
        translateMode_->setChecked(true);
        rotateMode_->setChecked(false);
        
    } else if (_action == rotateMode_){
        PluginFunctions::pickMode("RotateLights");

        lightAction_->setChecked(true);
        moveMode_->setChecked(false);
        translateMode_->setChecked(false);
        rotateMode_->setChecked(true);   
    }
}

void TypeLightPlugin::slotSetAllOrTarget(bool _checked) {
    
    allLightsMode_->setChecked(_checked);
    onlyTargets_ = !_checked;
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

  QString name = QString(tr("New Light %1.lgt").arg( object->id() ));

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  object->defaultLight(false);
  
  object->update();

  object->enablePicking(true);
  
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

float TypeLightPlugin::findDepth() {
    
    // Init depth
    float d = FLT_MAX;
    ACG::GLState& state = PluginFunctions::viewerProperties().glState();
    
    for ( PluginFunctions::ObjectIterator o_it((onlyTargets_ ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        LightObject* lightObject = PluginFunctions::lightObject(o_it);
        if(lightObject != 0) {
            
            LightSource* source = PluginFunctions::lightSource(lightObject);
            if(source != 0) {
                
                ACG::Vec3d z = state.project(source->position());
                
                if(!source->directional() && z[2] < d) {
                    
                    // z-value of light source
                    d = z[2];
                }
            }
        }
    }
    
    if(d == FLT_MAX) return 0.0f;
    
    return d;
}

double TypeLightPlugin::getFarthestRadius() {
    
    // Init distance
    double d = 0.0;
    ACG::Vec3d c = PluginFunctions::trackBallCenter( PluginFunctions::activeExaminer() );
    
    for ( PluginFunctions::ObjectIterator o_it((onlyTargets_ ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        LightObject* lightObject = PluginFunctions::lightObject(o_it);
        if(lightObject != 0) {
            
            LightSource* source = PluginFunctions::lightSource(lightObject);
            if(source != 0) {
                
                if(!source->directional()) {
                    
                    double tmp_d = fabs((source->position() - c).norm());
                    if(tmp_d > d) d = tmp_d;
                    
                }
            }
        }
    }
    
    return d;
}

void TypeLightPlugin::slotMouseEventLight(QMouseEvent* _event) {
    
    // Only react if in light mode
    if(PluginFunctions::actionMode() == Viewer::LightMode) {
        // Get gl state
        ACG::GLState& state = PluginFunctions::viewerProperties().glState();
        
        // Invert screen y-axis since the OpenGL y-axis is inverted
        QPoint pos (_event->x(), state.viewport_height() - _event->y());
        
        switch (_event->type())
        {
            case QEvent::MouseButtonPress:
            {
                // Reset transformation
                light_matrix_.identity();
                
                if(PluginFunctions::pickMode() == "MoveLights" && _event->buttons() & Qt::LeftButton) {
                    
                    radius_ = getFarthestRadius();
                    
                    depth_ = findDepth();
                    
                    // Trackball rotation of light source
                    hitTrackball_ = computeClickOnTrackball(pos, lastPoint3D_, state);
                       
                    rotation_ = true;
                    
                } else if (PluginFunctions::pickMode() == "TranslateLights" && _event->buttons() & Qt::LeftButton) {
                    // Translation in plane orthogonal to viewing plane
                    lastPoint2D_ = pos;
                    
                    rotation_ = false;
                    
                } else if(PluginFunctions::pickMode() == "RotateLights" && _event->buttons() & Qt::LeftButton) {
                    
                    QPoint p(_event->x(), _event->y());
                    
                    unsigned int id = 0;
                    unsigned int t = 0;
                    ACG::Vec3d v;
                    PluginFunctions::scenegraphPick(PluginFunctions::activeExaminer(), ACG::SceneGraph::PICK_ANYTHING, p, id, t, &v);
                    
                    // Get picked light node
                    if(id != 0) {
                        BaseObjectData* obj = 0;
                        PluginFunctions::getPickedObject(id, obj);
                    
                        if(obj != 0) {
                            LightObject* light = 0;
                            light = PluginFunctions::lightObject(obj);
                            
                            if(light) {
                            
                                lightCenter_ = light->lightSource()->position();
                                
                                ACG::Vec3d bbMin, bbMax;
                                light->lightNodeVis()->boundingBox(bbMin, bbMax);
                                lightRadius_ = (bbMin - bbMax).length()/4;
                                lightId_     = id;
                                
                                // Set depth
                                depth_ = state.project(light->lightSource()->position())[2];
                            }
                        }
                    }
                    
                    if(lightId_ == -1) break;
                    
                    hitLightTrackball_ = computeClickOnLightTrackball(pos, lastPoint3D_, state);
                }
                
                break;
            }
            
            case QEvent::MouseButtonRelease:
            {
                lightId_ = -1;
                hitLightTrackball_  = false;
                hitTrackball_       = false;
            }
            
            case QEvent::MouseMove:
            {
                
                if (PluginFunctions::pickMode() == "MoveLights" && _event->buttons() & Qt::LeftButton) {
                    
                    // rotate lights
                    if ((pos.x() < 0) || (pos.x() > (int)state.viewport_width())  ||
                        (pos.y() < 0) || (pos.y() > (int)state.viewport_height()))
                        break;
                    
                    // Compute first intersection if this is the first
                    // hit of the trackball
                    if(hitTrackball_ == false) {
                        
                        hitTrackball_ = computeClickOnTrackball(pos, lastPoint3D_, state);
                        
                    } else {
                    
                        ACG::Vec3d v1 = lastPoint3D_;
                        v1.normalize();
                        
                        hitTrackball_ = computeClickOnTrackball(pos, lastPoint3D_, state);
                        
                        ACG::Vec3d v2 = lastPoint3D_;
                        v2.normalize();
                        
                        ACG::Vec3d axis = v1 % v2;
                        
                        axis = state.inverse_modelview().transform_vector(axis);
                        
                        axis.normalize();
                        
                        double angle = acos(v1 | v2) * 180/M_PI;
                        
                        rotateLights(axis, angle);
                    }

                } else if (PluginFunctions::pickMode() == "TranslateLights" && _event->buttons() & Qt::LeftButton) {
                    
                    // Get depth of plane along which we want to translate
                    planeDepth_ = findDepth();
                    
                    ACG::Vec3d p0(pos.x(), pos.y(), planeDepth_);
                    p0 = state.unproject(p0);
                    
                    ACG::Vec3d p1(lastPoint2D_.x(), lastPoint2D_.y(), planeDepth_);
                    p1 = state.unproject(p1);
                    
                    // Translation in plane
                    transVec_ = p0 - p1;
                    
                    lastPoint2D_ = pos;
                    
                    updateLights();
                    
                } else if (PluginFunctions::pickMode() == "RotateLights" && _event->buttons() & Qt::LeftButton) {
                    
                    // rotate spot light direction
                    if ( (pos.x() < 0) || (pos.x() > (int)state.viewport_width())  ||
                         (pos.y() < 0) || (pos.y() > (int)state.viewport_height()) ||
                         (lightId_ == -1)) break;
                    
                    // Compute first intersection if this is the first
                    // hit of the trackball
                    if(hitLightTrackball_ == false) {
                        
                        hitLightTrackball_ = computeClickOnLightTrackball(pos, lastPoint3D_, state);
                        
                    } else {
                    
                        ACG::Vec3d v1 = lastPoint3D_;
                        v1.normalize();
                        
                        hitLightTrackball_ = computeClickOnLightTrackball(pos, lastPoint3D_, state);
                        
                        ACG::Vec3d v2 = lastPoint3D_;
                        v2.normalize();
                        
                        ACG::Vec3d axis = v1 % v2;
                        
                        axis = state.inverse_modelview().transform_vector(axis);
                        
                        axis.normalize();
                        
                        double angle = acos(v1 | v2) * 180/M_PI;
                        
                        rotateLightDirection(axis, angle);
                    }
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

bool TypeLightPlugin::computeClickOnTrackball(const QPoint& _v2D, ACG::Vec3d& _clickOnSphere, ACG::GLState& _state) {
    
    bool hit = true;
    
    ACG::Vec3d clickInWorld = _state.unproject(ACG::Vec3d(_v2D.x(), _v2D.y(), depth_));
                    
    clickInWorld = _state.modelview().transform_point(clickInWorld);
   
    ACG::Vec3d c = PluginFunctions::trackBallCenter( PluginFunctions::activeExaminer() );
        
    c = _state.modelview().transform_point(c);
    
    ACG::Vec3d clickRelToC = (clickInWorld - c);
    
    double x = clickRelToC[0];
    double y = clickRelToC[1];
    
    // radius_ contains the radius of the trackball of the most
    // distant light source
    double sq = radius_*radius_ - x*x - y*y;
    
    double z = sq > 0.0 ? sqrt(sq) : 0.0;
    
    if(sq <= 0.0) hit = false;
    
    // Set point on sphere to referenced variable
    _clickOnSphere = ACG::Vec3d(x, y, z);
    
    return hit;
}

bool TypeLightPlugin::computeClickOnLightTrackball(const QPoint& _v2D, ACG::Vec3d& _clickOnSphere, ACG::GLState& _state) {
    
    bool hit = true;
    
    ACG::Vec3d clickInWorld = _state.unproject(ACG::Vec3d(_v2D.x(), _v2D.y(), depth_));
                    
    clickInWorld = _state.modelview().transform_point(clickInWorld);
   
    ACG::Vec3d c = lightCenter_;
        
    c = _state.modelview().transform_point(c);
    
    ACG::Vec3d clickRelToC = (clickInWorld - c);
    
    double x = clickRelToC[0];
    double y = clickRelToC[1];
    
    // radius_ contains the radius of the trackball of the most
    // distant light source
    double sq = lightRadius_*lightRadius_ - x*x - y*y;
    
    double z = sq > 0.0 ? sqrt(sq) : 0.0;
    
    if(sq <= 0.0) hit = false;
    
    // Set point on sphere to referenced variable
    _clickOnSphere = ACG::Vec3d(x, y, z);
    
    return hit;
}

void TypeLightPlugin::rotateLights(ACG::Vec3d& _axis, double _angle) {
    
    // Rotate lights
    light_matrix_.rotate(_angle, _axis[0], _axis[1], _axis[2]);
    
    // Transform positions
    updateLights();
}

void TypeLightPlugin::rotateLightDirection(ACG::Vec3d& _axis, double _angle) {
    
    ACG::GLMatrixd m;
    
    m.identity();
    m.rotate(_angle, _axis[0], _axis[1], _axis[2]);
    
    // Get picked light node
    if(lightId_ != 0) {
        BaseObjectData* obj = 0;
        PluginFunctions::getPickedObject(lightId_, obj);
    
        if(obj != 0) {
            LightObject* light = 0;
            light = PluginFunctions::lightObject(obj);
            
            if(light) {
            
                ACG::Vec3d spot = light->lightSource()->spotDirection();
                spot = m.transform_vector(spot);
                light->lightSource()->spotDirection(spot);
                
                emit updatedObject(light->id(), UPDATE_ALL);
            }
        }
    }
}

void TypeLightPlugin::updateLights() {
    
    ACG::Vec3d c = PluginFunctions::trackBallCenter( PluginFunctions::activeExaminer() );
    
    for ( PluginFunctions::ObjectIterator o_it((onlyTargets_ ? PluginFunctions::TARGET_OBJECTS : PluginFunctions::ALL_OBJECTS));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
        
        LightObject* lightObject = PluginFunctions::lightObject(o_it);
        if(lightObject != 0) {
            
            LightSource* source = PluginFunctions::lightSource(lightObject);
            if(source != 0) {
              
              // If light source is directional, we only care about rotations not translations!  
              if( source->directional() && rotation_) {
                std::cerr << "Not implemented yet! rotate directional light sources" << std::endl;
              }
              
              
              // Skip if light source is directional
              if(!source->directional() && rotation_) {

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
              
              } else if(!source->directional() && !rotation_) {
                  
                  // Translate light on plane
                  
                  // Get light source's position
                  ACG::Vec3d p = source->position();
                  p += transVec_;
                  source->position(p);
                  
                  emit updatedObject(lightObject->id(), UPDATE_ALL);
              }
          }
          
        }
    }
    
    // Reset light source transformation matrix
    light_matrix_.identity();
    
    // Reset translation vector
    transVec_ = ACG::Vec3d(0.0);
}

Q_EXPORT_PLUGIN2( typelightplugin , TypeLightPlugin );

