
#include <QtGui>
#include <QFileInfo>

#include "FilePolyLine.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

void FilePolyLinePlugin::initializePlugin() {
  // Data type has already been added by polyline type plugin
}

QString FilePolyLinePlugin::getLoadFilters() {
  return QString( tr("Poly-Line files ( *.pol )" ));
};

QString FilePolyLinePlugin::getSaveFilters() {
  return QString( tr("Poly-Line files ( *.pol )" ));
};

DataType  FilePolyLinePlugin::supportedType() {
  DataType type = DATA_POLY_LINE;
  return type;
}

int FilePolyLinePlugin::loadObject(QString _filename)
{
  int id = -1;
  emit addEmptyObject( DATA_POLY_LINE, id );

  PolyLineObject* pol(0);
  if(PluginFunctions::getObject( id, pol))
  {
    pol->line()->load(_filename.toAscii());

    pol->setFromFileName(_filename);

    pol->setName(QFileInfo(_filename).fileName());
  }

  emit openedFile( pol->id() );

  return id;
};

bool FilePolyLinePlugin::saveObject(int _id, QString _filename)
{
  BaseObjectData*     obj(0);
  if(PluginFunctions::getObject( _id, obj))
  {
    PolyLineObject* pol = PluginFunctions::polyLineObject(obj);
    if( pol)
    {
      std::cerr << "call Line Function\n";
      std::cerr << (const char*) _filename.toAscii() << std::endl;

      obj->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
      obj->setPath(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );

      pol->line()->save((const char*) _filename.toAscii());
    }
  }

  return true;
}

void FilePolyLinePlugin::loadIniFileLast( INIFile& _ini ,int _id ) {
  BaseObjectData* baseObject;
  if ( !PluginFunctions::getObject(_id,baseObject) ) {
    emit log(LOGERR,tr("Cannot find object for id %1 in saveFile!").arg(_id));
    return;
  }

  PolyLineObject* object = PluginFunctions::polyLineObject(baseObject);

  if ( object ) {
     ACG::Vec4f col(0.0,0.0,0.0,0.0);

    if ( _ini.get_entryVecf( col, object->name() , "BaseColor" ) )
      object->materialNode()->set_base_color(col);
  }

}

void FilePolyLinePlugin::saveIniFile( INIFile& _ini ,int _id) {
  BaseObjectData* baseObject;
  if ( !PluginFunctions::getObject(_id,baseObject) ) {
    emit log(LOGERR,tr("Cannot find object for id %1 in saveFile!").arg(_id) );
    return;
  }

  PolyLineObject* object = PluginFunctions::polyLineObject(baseObject);

  if ( object ) {
      _ini.add_entryVec( object->name() ,
                         "BaseColor" ,
                         object->materialNode()->base_color() ) ;
  }
}

Q_EXPORT_PLUGIN2( filepolylineplugin , FilePolyLinePlugin );

