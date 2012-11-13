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


#include <QtGui>
#include <QFileInfo>
#include <QSettings>

#include "FileLgt.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

void FileLightPlugin::initializePlugin() {
}

QString FileLightPlugin::getLoadFilters() {
  return QString( tr("Light definition files ( *.lgt )") );
};

QString FileLightPlugin::getSaveFilters() {
  return QString( tr("Light definition files ( *.lgt )") );
};

DataType  FileLightPlugin::supportedType() {
  DataType type = DATA_LIGHT;
  return type;
}

int FileLightPlugin::loadObject(QString _filename)
{
  int id = -1;
  emit addEmptyObject( DATA_LIGHT, id );

  LightObject* lightObject = 0;
  if(PluginFunctions::getObject( id, lightObject))
  {    
    if( lightObject )
    {

      QFileInfo fi(_filename);

      if ( fi.exists() ){
        LightSource* light = PluginFunctions::lightSource(lightObject);

        QString name = lightObject->name();

        QSettings settings(_filename, QSettings::IniFormat);
        settings.beginGroup("LIGHT");

        if ( settings.contains("Name") ) {
          
          QString name = settings.value("Name").toString();
          
          lightObject->setName( name );
        } else {
          lightObject->setFromFileName(_filename);
        }        
        
        if (settings.contains("PositionX")) {
          ACG::Vec3d position;
          position[0] = settings.value("PositionX").toDouble();
          position[1] = settings.value("PositionY").toDouble();
          position[2] = settings.value("PositionZ").toDouble();

          light->position(position);
        }

        if (settings.contains("DirectionX")) {
          ACG::Vec3d direction;
          direction[0] = settings.value("DirectionX").toDouble();
          direction[1] = settings.value("DirectionY").toDouble();
          direction[2] = settings.value("DirectionZ").toDouble();

          light->direction(direction);
        }

        if (settings.contains("SpotDirectionX")) {
          ACG::Vec3d spotDirection;
          spotDirection[0] = settings.value("SpotDirectionX").toDouble();
          spotDirection[1] = settings.value("SpotDirectionY").toDouble();
          spotDirection[2] = settings.value("SpotDirectionZ").toDouble();

          light->spotDirection(spotDirection);
        }

        if (settings.contains("AmbientColorR")) {
          ACG::Vec4f ambientColor;
          ambientColor[0] = settings.value("AmbientColorR").toDouble();
          ambientColor[1] = settings.value("AmbientColorG").toDouble();
          ambientColor[2] = settings.value("AmbientColorB").toDouble();
          if (settings.contains("AmbientColorA"))
            ambientColor[3] = settings.value("AmbientColorA").toDouble();
          else
            ambientColor[3] = 1.0f;

          light->ambientColor(ambientColor);
        }

        if (settings.contains("DiffuseColorR")) {
          ACG::Vec4f diffuseColor;
          diffuseColor[0] = settings.value("DiffuseColorR").toDouble();
          diffuseColor[1] = settings.value("DiffuseColorG").toDouble();
          diffuseColor[2] = settings.value("DiffuseColorB").toDouble();
          if (settings.contains("DiffuseColorA"))
            diffuseColor[3] = settings.value("DiffuseColorA").toDouble();
          else
            diffuseColor[3] = 1.0f;

          light->diffuseColor(diffuseColor);
        }

        if (settings.contains("SpecularColorR")) {
          ACG::Vec4f specularColor;
          specularColor[0] = settings.value("SpecularColorR").toDouble();
          specularColor[1] = settings.value("SpecularColorG").toDouble();
          specularColor[2] = settings.value("SpecularColorB").toDouble();
          if (settings.contains("SpecularColorA"))
            specularColor[3] = settings.value("SpecularColorA").toDouble();
          else
            specularColor[3] = 1.0f;

          light->specularColor(specularColor);
        }

        if (settings.contains("FixedPosition")) {

          bool fixedPosition = settings.value("FixedPosition").toBool();

          light->fixedPosition(fixedPosition);
        }

        if (settings.contains("SpotExponent")) {

          float spotExponent = settings.value("SpotExponent").toDouble();

          light->spotExponent(spotExponent);
        }

        if (settings.contains("SpotCutOff")) {

          float spotCutOff = settings.value("SpotCutOff").toDouble();

          light->spotCutoff(spotCutOff);
        }

        if (settings.contains("ConstantAttenuation")) {

          float cAttenuation = settings.value("ConstantAttenuation").toDouble();

          light->constantAttenuation(cAttenuation);
        }

        if (settings.contains("LinearAttenuation")) {

          float lAttenuation = settings.value("LinearAttenuation").toDouble();

          light->linearAttenuation(lAttenuation);
        }

        if (settings.contains("QuadraticAttenuation")) {

          float qAttenuation = settings.value("QuadraticAttenuation").toDouble();

          light->quadraticAttenuation(qAttenuation);
        }

        if (settings.contains("Radius")) {

          float radius = settings.value("Radius").toDouble();

          light->radius(radius);
        }

        if (settings.contains("Enabled")) {

          bool enabled = settings.value("Enabled").toBool();

          enabled ? light->enable() : light->disable();
        }
        
        settings.endGroup();
      }

      emit updatedObject( lightObject->id(), UPDATE_ALL );
      emit openedFile( lightObject->id() );

    }

  }

  return id;
};

bool FileLightPlugin::saveObject(int _id, QString _filename)
{

  BaseObjectData*     obj(0);
  if(PluginFunctions::getObject( _id, obj))
  {
    LightObject* lightObject = PluginFunctions::lightObject(obj);
    if( lightObject )
    {
        LightSource* light = PluginFunctions::lightSource(lightObject);

        QSettings settings(_filename, QSettings::IniFormat);
        settings.beginGroup("LIGHT");

        settings.setValue("Name",lightObject->name());
        
        if(!light->directional()) {
            settings.setValue("PositionX", light->position()[0]);
            settings.setValue("PositionY", light->position()[1]);
            settings.setValue("PositionZ", light->position()[2]);
        } else {
            settings.setValue("DirectionX", light->direction()[0]);
            settings.setValue("DirectionY", light->direction()[1]);
            settings.setValue("DirectionZ", light->direction()[2]);
        }

        settings.setValue("SpotDirectionX", light->spotDirection()[0]);
        settings.setValue("SpotDirectionY", light->spotDirection()[1]);
        settings.setValue("SpotDirectionZ", light->spotDirection()[2]);
        
        settings.setValue("AmbientColorR", light->ambientColor()[0]);
        settings.setValue("AmbientColorG", light->ambientColor()[1]);
        settings.setValue("AmbientColorB", light->ambientColor()[2]);
        settings.setValue("AmbientColorA", light->ambientColor()[3]);
        
        settings.setValue("DiffuseColorR", light->diffuseColor()[0]);
        settings.setValue("DiffuseColorG", light->diffuseColor()[1]);
        settings.setValue("DiffuseColorB", light->diffuseColor()[2]);
        settings.setValue("DiffuseColorA", light->diffuseColor()[3]);
        
        settings.setValue("SpecularColorR", light->specularColor()[0]);
        settings.setValue("SpecularColorG", light->specularColor()[1]);
        settings.setValue("SpecularColorB", light->specularColor()[2]);
        settings.setValue("SpecularColorA", light->specularColor()[3]);
        
        settings.setValue("FixedPosition", light->fixedPosition());
        
        settings.setValue("SpotExponent", light->spotExponent());
        
        settings.setValue("SpotCutOff", light->spotCutoff());
        
        settings.setValue("ConstantAttenuation", light->constantAttenuation());
        
        settings.setValue("LinearAttenuation", light->linearAttenuation());
        
        settings.setValue("QuadraticAttenuation", light->quadraticAttenuation());
        
        settings.setValue("Radius", light->radius());
        
        settings.setValue("Enabled", light->enabled());
                
        settings.endGroup();
        
        obj->setFromFileName(_filename);
        obj->setName(obj->filename());
    }
  }

  return true;
}

Q_EXPORT_PLUGIN2( filelightplugin , FileLightPlugin );

