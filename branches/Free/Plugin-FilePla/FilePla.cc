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

#include "FilePla.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

void FilePlaPlugin::initializePlugin() {
}

QString FilePlaPlugin::getLoadFilters() {
  return QString( tr("Plane files ( *.pla )") );
};

QString FilePlaPlugin::getSaveFilters() {
  return QString( tr("Plane files ( *.pla )") );
};

DataType  FilePlaPlugin::supportedType() {
  DataType type = DATA_PLANE;
  return type;
}

int FilePlaPlugin::loadObject(QString _filename)
{
  int id = -1;
  emit addEmptyObject( DATA_PLANE, id );

  PlaneObject* plane = 0;
  if(PluginFunctions::getObject( id, plane))
  {    
    if( plane )
    {

      QFileInfo fi(_filename);

      if ( fi.exists() ){

        ACG::Vec3f position;
        ACG::Vec3f xDirection;
        ACG::Vec3f yDirection;

        QSettings settings(_filename, QSettings::IniFormat);
        settings.beginGroup("PLANE");

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

          plane->plane().setPlane(position, xDirection, yDirection);

          plane->setFromFileName(_filename);
        }
      }

      emit updatedObject( plane->id() );

    }

  }

  return id;
};

bool FilePlaPlugin::saveObject(int _id, QString _filename)
{

  BaseObjectData*     obj(0);
  if(PluginFunctions::getObject( _id, obj))
  {
    PlaneObject* plane = PluginFunctions::planeObject(obj);
    if( plane )
    {

      obj->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
      obj->setPath(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );

      QSettings settings(_filename, QSettings::IniFormat);
      settings.beginGroup("PLANE");
      settings.setValue("Position0",  plane->planeNode()->position()[0]);
      settings.setValue("Position1",  plane->planeNode()->position()[1]);
      settings.setValue("Position2",  plane->planeNode()->position()[2]);
      settings.setValue("XDirection0",  plane->planeNode()->xDirection()[0]);
      settings.setValue("XDirection1",  plane->planeNode()->xDirection()[1]);
      settings.setValue("XDirection2",  plane->planeNode()->xDirection()[2]);
      settings.setValue("YDirection0",  plane->planeNode()->yDirection()[0]);
      settings.setValue("YDirection1",  plane->planeNode()->yDirection()[1]);
      settings.setValue("YDirection2",  plane->planeNode()->yDirection()[2]);
      settings.endGroup();
    }
  }

  return true;
}

QString FilePlaPlugin::get_unique_name(PlaneObject* _object)
{
  bool name_unique = false;

  int cur_idx = _object->id();

  while(!name_unique)
  {
    name_unique = true;

    QString cur_name = QString(tr("Plane %1.pla").arg( cur_idx ));

    PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DATA_PLANE );
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

  return QString(tr("Plane %1.pla").arg( cur_idx ));
}

Q_EXPORT_PLUGIN2( fileplaplugin , FilePlaPlugin );

