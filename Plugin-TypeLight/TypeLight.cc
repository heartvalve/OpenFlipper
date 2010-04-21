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
 *   $Revision: 6753 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-08-05 12:38:20 +0200 (Mi, 05. Aug 2009) $                   *
 *                                                                           *
\*===========================================================================*/


#include "TypeLight.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#define DEF0 "Default Light 0.lgt"
#define DEF1 "Default Light 1.lgt"
#define DEF2 "Default Light 2.lgt"

TypeLightPlugin::TypeLightPlugin() :
    defaultLights_(true) {
  
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
  
  // Disable the build in light management and use this plugins light handling
  PluginFunctions::disableExaminerLightHandling();
  
  // Add default light sources to active scene
  addDefaultLights();
}

int TypeLightPlugin::addDefaultLight(QString _name) {
    
    // new object data struct
    LightObject* object = new LightObject();

    // call the local function to update names
    object->setName( _name );

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
    if(light->name() == DEF0) return;
    if(light->name() == DEF1) return;
    if(light->name() == DEF2) return;
    
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

Q_EXPORT_PLUGIN2( typelightplugin , TypeLightPlugin );

