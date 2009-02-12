//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================





#include <QtGui>
#include <QFileInfo>

#include "FilePolyMesh.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/common/GlobalOptions.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>


/// Add an empty poly-mesh
int FilePolyMeshPlugin::addEmpty( ){

  // new object data struct
  MeshObject< PolyMesh, DATA_POLY_MESH >* object;

  object = new MeshObject< PolyMesh, DATA_POLY_MESH >( dynamic_cast < ACG::SceneGraph::SeparatorNode* >( PluginFunctions::getRootNode() ) );

  // Set pointers for tree structure
  object->setParent(PluginFunctions::objectRoot());
  PluginFunctions::objectRoot()->appendChild(object);

  if (PluginFunctions::targetCount() == 0 )
    object->target(true);


  QString name = "PolyMesh " + QString::number( object->id() ) + ".off";

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  object->update();

  object->show();

  emit log(LOGINFO,object->getObjectinfo());

  emit emptyObjectAdded (object->id() );

  return object->id();
}

/// load a poly-mesh with given filename
int FilePolyMeshPlugin::loadObject(QString _filename){

  // new object data struct
  MeshObject< PolyMesh, DATA_POLY_MESH >* object;

  object = new MeshObject< PolyMesh, DATA_POLY_MESH >( dynamic_cast < ACG::SceneGraph::SeparatorNode* >( PluginFunctions::getRootNode() ) );

  // Set pointers for tree structure
  object->setParent(PluginFunctions::objectRoot());
  PluginFunctions::objectRoot()->appendChild(object);

  if (PluginFunctions::objectCount() == 1 )
    object->target(true);

  object->setFromFileName(_filename);

  // call the local function to update names
  QFileInfo f(_filename);
  object->setName( f.fileName() );

  std::string filename = std::string( _filename.toUtf8() );

  // load file
  bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename );
  if (!ok)
  {
    std::cerr << "Plugin FilePolyMesh : Read error for Poly Mesh\n";
    PluginFunctions::deleteObject( object->id() );
    return -1;
  }

  object->mesh()->update_normals();

  object->update();

  object->show();

  emit log(LOGINFO,object->getObjectinfo());

  emit openedFile( object->id() );

  return object->id();
}

/// Save object with given id
bool FilePolyMeshPlugin::saveObject(int _id, QString _filename){
  BaseObjectData* object;
  PluginFunctions::getObject(_id,object);

  std::string filename = std::string( _filename.toUtf8() );

  if ( object->dataType( DATA_POLY_MESH ) ) {
    object->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
    object->path(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );

    PolyMeshObject* polyObj = dynamic_cast<PolyMeshObject* >( object );

    if (OpenMesh::IO::write_mesh(*polyObj->mesh(), filename.c_str()) ){
      emit log(LOGINFO, "Saved object to " + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
      return true;
    }else{
      emit log(LOGERR, "Unable to save " + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
      return false;
    }
  }else{

    emit log(LOGERR, "Unable to save (object isn't a poly mesh)");
    return false;
  }
}

///return file-extension filters for loading objects
QString FilePolyMeshPlugin::getLoadFilters() {
  return QString( OpenMesh::IO::IOManager().qt_read_filters().c_str() );
}

///return file-extension filters for saving objects
QString FilePolyMeshPlugin::getSaveFilters() {
  return QString( OpenMesh::IO::IOManager().qt_write_filters().c_str() );
}

///return supported DataType
DataType  FilePolyMeshPlugin::supportedType() {
  DataType type = DATA_POLY_MESH;
  return type;
}

void FilePolyMeshPlugin::loadIniFileLast( INIFile& _ini ,int _id ) {
  BaseObjectData* baseObject;
  if ( !PluginFunctions::getObject(_id,baseObject) ) {
    emit log(LOGERR,"Cannot find object for id " + QString::number(_id) + " in saveFile" );
    return;
  }

  PolyMeshObject* object = PluginFunctions::polyMeshObject(baseObject);

  if ( object ) {
    std::vector< QString > draw_modes;
    if( _ini.get_entry(draw_modes, object->name(), "MaterialDrawMode") )
      object->materialNode()->drawMode( ListToDrawMode(draw_modes) );

    if( _ini.get_entry(draw_modes, object->name(), "MeshDrawMode") )
      object->meshNode()->drawMode( ListToDrawMode(draw_modes) );

    ACG::Vec4f col(0.0,0.0,0.0,0.0);

    if ( _ini.get_entryVecf( col, object->name() , "BaseColor" ) )
      object->materialNode()->set_base_color(col);

    if ( _ini.get_entryVecf( col, object->name() , "AmbientColor" ) )
      object->materialNode()->set_ambient_color(col);

    if ( _ini.get_entryVecf( col, object->name() , "DiffuseColor" ) )
      object->materialNode()->set_diffuse_color(col);

    if ( _ini.get_entryVecf( col, object->name() , "SpecularColor" ) )
      object->materialNode()->set_specular_color(col);

    int size = 1;
    if ( _ini.get_entry( size, object->name() , "PointSize" ) )
      object->materialNode()->set_point_size(size);
    if ( _ini.get_entry( size, object->name() , "LineWidth" ) )
      object->materialNode()->set_line_width(size);
  }

}

void FilePolyMeshPlugin::saveIniFile( INIFile& _ini ,int _id) {
  BaseObjectData* baseObject;
  if ( !PluginFunctions::getObject(_id,baseObject) ) {
    emit log(LOGERR,"Cannot find object for id " + QString::number(_id) + " in saveFile" );
    return;
  }

  PolyMeshObject* object = PluginFunctions::polyMeshObject(baseObject);

  if ( object ) {
    _ini.add_entry( object->name() ,
                    "MaterialDrawMode" ,
                    drawModeToList( object->materialNode()->drawMode()) );
    _ini.add_entry( object->name() ,
                    "MeshDrawMode" ,
                    drawModeToList( object->meshNode()->drawMode()) );
    _ini.add_entryVec( object->name() ,
                      "BaseColor" ,
                      object->materialNode()->base_color()) ;
    _ini.add_entryVec( object->name() ,
                      "AmbientColor" ,
                      object->materialNode()->ambient_color()) ;
    _ini.add_entryVec( object->name() ,
                      "DiffuseColor" ,
                      object->materialNode()->diffuse_color());
    _ini.add_entryVec( object->name() ,
                      "SpecularColor" ,
                      object->materialNode()->specular_color());

    _ini.add_entry( object->name() ,
                    "PointSize" ,
                    object->materialNode()->point_size());
    _ini.add_entry( object->name() ,
                    "LineWidth" ,
                    object->materialNode()->line_width());
  }
}


Q_EXPORT_PLUGIN2( filepolymeshplugin , FilePolyMeshPlugin );

