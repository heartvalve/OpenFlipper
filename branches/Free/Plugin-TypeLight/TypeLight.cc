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

TypeLightPlugin::TypeLightPlugin() {
  
}

bool TypeLightPlugin::registerType() {
  addDataType("Light",tr("Light"));
  setTypeIcon( "Light", "LightType.png");
  
   
  
  
//   int objectId1 = -1;
//   int objectId2 = -1;
//   emit addEmptyObject(objectId0);
//   emit addEmptyObject(objectId1);
//   emit addEmptyObject(objectId2);
  

  
  return true;
}

void TypeLightPlugin::pluginsInitialized(){
  
  // Disable the build in light management and use this plugins light handling
  PluginFunctions::disableExaminerLightHandling();
  
  // Generate the default lights
  int objectId0 = addEmpty();
  
  BaseObjectData* obj0(0);
  if(PluginFunctions::getObject( objectId0, obj0))
  {
    LightObject* lightObject0 = PluginFunctions::lightObject(obj0);
    LightSource* light0 = PluginFunctions::lightSource(lightObject0);
    
    if( light0 ) { 
      light0->direction(ACG::Vec3d(0.0,0.0,1.0) );
      light0->diffuseColor( ACG::Vec4f(0.7,0.7,0.7,0.0));
      light0->specularColor(ACG::Vec4f(0.7,0.7,0.7,0.0));
      light0->enable();
      light0->fixedPosition(true);
      std::cerr << "Set light 0 " << std::endl;
    }
    
  }
  
  emit updatedObject(objectId0,UPDATE_ALL);
  
  // Generate the default lights
  int objectId1 = addEmpty();
  
  BaseObjectData* obj1(0);
  if(PluginFunctions::getObject( objectId1, obj1))
  {
    LightObject* lightObject1 = PluginFunctions::lightObject(obj1);
    LightSource* light1 = PluginFunctions::lightSource(lightObject1);
    
    if( light1 ) {
      light1->direction(ACG::Vec3d(-1.0,  1.0, 0.7) );
      light1->diffuseColor( ACG::Vec4f(0.7,0.7,0.7,0.0));
      light1->specularColor(ACG::Vec4f(0.7,0.7,0.7,0.0));
      light1->enable();
      light1->fixedPosition(true);
    }
  }
  
  emit updatedObject(objectId1,UPDATE_ALL);
  
  //Generate the default lights
  int objectId2 = addEmpty();
  
  BaseObjectData* obj2(0);
  if(PluginFunctions::getObject( objectId2, obj2))
  {
    LightObject* lightObject2 = PluginFunctions::lightObject(obj2);
    LightSource* light2 = PluginFunctions::lightSource(lightObject2);
    
    if( light2 ) {
      light2->direction(ACG::Vec3d( 1.0,  1.0, 0.7) );
      light2->diffuseColor(ACG::Vec4f(0.7,0.7,0.7,0.0));
      light2->specularColor(ACG::Vec4f(0.7,0.7,0.7,0.0));
      light2->enable();
      light2->fixedPosition(true);
    }
  }
  
  emit updatedObject(objectId2,UPDATE_ALL);
   
}

int TypeLightPlugin::addEmpty(){
    
  // new object data struct
  LightObject * object = new LightObject();

  QString name = get_unique_name(object);

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  object->update();

  
  object->show();

  emit emptyObjectAdded (object->id() );

  return object->id();
}

QString TypeLightPlugin::get_unique_name(LightObject* _object)
{
  bool name_unique = false;

  int cur_idx = _object->id();

  while(!name_unique)
  {
    name_unique = true;

    QString cur_name = QString(tr("Light %1.lgt").arg( cur_idx ));

    PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DATA_LIGHT );
    for(; o_it != PluginFunctions::objectsEnd(); ++o_it)
    {
      if( o_it->name() == cur_name)
      {
        name_unique = false;
        cur_idx += 10;
        break;
      }
    }
  }

  return QString(tr("Light %1.lgt").arg( cur_idx ));
}

Q_EXPORT_PLUGIN2( typelightplugin , TypeLightPlugin );

