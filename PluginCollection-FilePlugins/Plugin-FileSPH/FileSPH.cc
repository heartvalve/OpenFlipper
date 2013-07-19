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

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QFileInfo>
#include <QSettings>

#include "FileSPH.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>


void FileSPHPlugin::initializePlugin() {
}

QString FileSPHPlugin::getLoadFilters() {
  return QString( tr("Sphere definition files ( *.sph )") );
};

QString FileSPHPlugin::getSaveFilters() {
  return QString( tr("Sphere definition files ( *.sph )") );
};

DataType  FileSPHPlugin::supportedType() {
  DataType type = DATA_SPHERE;
  return type;
}

int FileSPHPlugin::loadObject(QString _filename)
{
  int id = -1;
  emit addEmptyObject( DATA_SPHERE, id );

  SphereObject* sphereObject = 0;
  if(PluginFunctions::getObject( id, sphereObject))
  {    
    if( sphereObject )
    {

      QFileInfo fi(_filename);

      if ( fi.exists() ){
        SphereNode* sphere = PluginFunctions::sphereNode(sphereObject);

        QString name = sphereObject->name();
        
        QSettings settings(_filename, QSettings::IniFormat);
        settings.beginGroup("SPHERE");
        
        if ( settings.contains("Name") ) {
          
          QString name = settings.value("Name").toString();
          
          sphereObject->setName( name );
        } else {
          sphereObject->setFromFileName(_filename);
        }        
        

        
        if ( settings.contains("PositionX") ) {

          ACG::Vec3d position;
          position[0] = settings.value("PositionX").toDouble();
          position[1] = settings.value("PositionY").toDouble();
          position[2] = settings.value("PositionZ").toDouble();
          
          sphere->get_primitive(0).position = position;
        }
        
        if ( settings.contains("Size") ){
          
          double size = settings.value("Size").toDouble();
          
          sphere->get_primitive(0).size = size;
        }
        
        if ( settings.contains("Slices") ){
          
          int slices = settings.value("Slices").toDouble();
          
          sphere->get_primitive(0).slices = slices;
        }
        
        if ( settings.contains("Stacks") ){
          
          int stacks = settings.value("Stacks").toDouble();
          
          sphere->get_primitive(0).stacks = stacks;
        }
        
        settings.endGroup();
      }

      emit updatedObject( sphereObject->id(), UPDATE_ALL );
      emit openedFile( sphereObject->id() );

    }


  }

  return id;
};


bool FileSPHPlugin::saveObject(int _id, QString _filename)
{

  BaseObjectData*     obj(0);
  if(PluginFunctions::getObject( _id, obj))
  {
    SphereObject* sphereObject = PluginFunctions::sphereObject(obj);
    if( sphereObject )
    {
      SphereNode* sphere = PluginFunctions::sphereNode(sphereObject);

      QSettings settings(_filename, QSettings::IniFormat);
      settings.beginGroup("SPHERE");

      settings.setValue("Name",sphereObject->name());
      
      settings.setValue("PositionX", sphere->get_primitive(0).position[0]);
      settings.setValue("PositionY", sphere->get_primitive(0).position[1]);
      settings.setValue("PositionZ", sphere->get_primitive(0).position[2]);

      settings.setValue("Size",   sphere->get_primitive(0).size);
      settings.setValue("Slices", sphere->get_primitive(0).slices);
      settings.setValue("Stacks", sphere->get_primitive(0).stacks);

      settings.endGroup();
      
      obj->setFromFileName(_filename);
      obj->setName(obj->filename());
    }
  }

  return true;
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( filesphplugin , FileSPHPlugin );
#endif


