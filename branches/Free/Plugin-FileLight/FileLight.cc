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
*   $Revision: 8545 $                                                       *
*   $Author: moebius $                                                       *
*   $Date: 2010-02-12 14:30:50 +0100 (Fr, 12. Feb 2010) $                   *
*                                                                           *
\*===========================================================================*/


#include <QtGui>
#include <QFileInfo>
#include <QSettings>

#include "FileLight.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

void FileLightPlugin::initializePlugin() {
}

QString FileLightPlugin::getLoadFilters() {
  return QString( tr("Light files ( *.lgt )") );
};

QString FileLightPlugin::getSaveFilters() {
  return QString( tr("Light files ( *.lgt )") );
};

DataType  FileLightPlugin::supportedType() {
  DataType type = DATA_LIGHT;
  return type;
}

int FileLightPlugin::loadObject(QString _filename)
{
  int id = -1;
  emit addEmptyObject( DATA_LIGHT, id );

  LightObject* light = 0;
  if(PluginFunctions::getObject( id, light))
  {    
    if( light )
    {

      QFileInfo fi(_filename);

      if ( fi.exists() ){

        ACG::Vec3f position;
        ACG::Vec3f xDirection;
        ACG::Vec3f yDirection;

        QSettings settings(_filename, QSettings::IniFormat);
        settings.beginGroup("LIGHT");

        if ( settings.contains("Position0") ){

          position[0] = settings.value("Position0").toDouble();
          position[1] = settings.value("Position1").toDouble();
          position[2] = settings.value("Position2").toDouble();
          xDirection[0] = settings.value("XDirection0").toDouble();
          xDirection[1] = settings.value("XDirection1").toDouble();
          xDirection[2] = settings.value("XDirection2").toDouble();
          yDirection[0] = settings.value("YDirection0").toDouble();
          yDirection[1] = settings.value("YDirection1").toDouble();
          yDirection[2] = settings.value("YDirection2").toDouble();
          settings.endGroup();

//           light->lightNode()->setPosition(position, xDirection, yDirection);

          light->setFromFileName(_filename);
        }
      }
    }
    emit updatedObject( light->id() );
  }

  return id;
};

bool FileLightPlugin::saveObject(int _id, QString _filename)
{

  BaseObjectData*     obj(0);
  if(PluginFunctions::getObject( _id, obj))
  {
    LightObject* light = PluginFunctions::lightObject(obj);
    if( light )
    {

      obj->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
      obj->setPath(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );

      QSettings settings(_filename, QSettings::IniFormat);
      settings.beginGroup("LIGHT");
//       settings.setValue("Position0",  light->lightNode()->position()[0]);
//       settings.setValue("Position1",  light->lightNode()->position()[1]);
//       settings.setValue("Position2",  light->lightNode()->position()[2]);
//       settings.setValue("XDirection0",  plane->planeNode()->xDirection()[0]);
//       settings.setValue("XDirection1",  plane->planeNode()->xDirection()[1]);
//       settings.setValue("XDirection2",  plane->planeNode()->xDirection()[2]);
//       settings.setValue("YDirection0",  plane->planeNode()->yDirection()[0]);
//       settings.setValue("YDirection1",  plane->planeNode()->yDirection()[1]);
//       settings.setValue("YDirection2",  plane->planeNode()->yDirection()[2]);
      settings.endGroup();
    }
  }

  return true;
}

QString FileLightPlugin::get_unique_name(LightObject* _object)
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

Q_EXPORT_PLUGIN2( filelightplugin , FileLightPlugin );

