
#include <QtGui>
#include <QFileInfo>
#include <QSettings>

#include "FilePlane.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

void FilePlanePlugin::initializePlugin() {
}

QString FilePlanePlugin::getLoadFilters() {
  return QString( tr("Plane files ( *.pla )") );
};

QString FilePlanePlugin::getSaveFilters() {
  return QString( tr("Plane files ( *.pla )") );
};

DataType  FilePlanePlugin::supportedType() {
  DataType type = DATA_PLANE;
  return type;
}


int FilePlanePlugin::addEmpty( ){
  // new object data struct
  PlaneObject * object = new PlaneObject(dynamic_cast < ACG::SceneGraph::SeparatorNode* >( PluginFunctions::getRootNode() ));

  if ( PluginFunctions::objectCount() == 1 )
    object->target(true);

  if (PluginFunctions::targetCount() == 0 )
    object->target(true);

  QString name = get_unique_name(object);

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  object->update();

  object->show();

  emit log(LOGINFO,object->getObjectinfo());

  emit emptyObjectAdded (object->id() );

  return object->id();
}

int FilePlanePlugin::loadObject(QString _filename)
{
  int id = addEmpty();

  BaseObjectData*     obj(0);
  if(PluginFunctions::getObject( id, obj))
  {
    PlaneObject* plane = PluginFunctions::planeObject(obj);

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

          plane->planeNode()->setPosition(position, xDirection, yDirection);

          obj->setFromFileName(_filename);
        }
      }
    }
    emit updatedObject(obj->id() );
  }

  return id;
};

bool FilePlanePlugin::saveObject(int _id, QString _filename)
{

  BaseObjectData*     obj(0);
  if(PluginFunctions::getObject( _id, obj))
  {
    PlaneObject* plane = PluginFunctions::planeObject(obj);
    if( plane )
    {

      obj->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
      obj->path(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );

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

QString FilePlanePlugin::get_unique_name(PlaneObject* _object)
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

Q_EXPORT_PLUGIN2( fileplaneplugin , FilePlanePlugin );

