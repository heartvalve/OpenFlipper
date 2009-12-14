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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/





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

#include <QMessageBox>

/// Constructor
FilePolyMeshPlugin::FilePolyMeshPlugin()
  : loadOptions_(0),
    triMeshHandling_(0) {
}

/// load a poly-mesh with given filename
int FilePolyMeshPlugin::loadObject(QString _filename){

    int id = -1;
    emit addEmptyObject(DATA_POLY_MESH, id);
    
    PolyMeshObject* object(0);
    if(PluginFunctions::getObject( id, object)) {
        
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
            emit deleteObject( object->id() );
            return -1;
            
        } else {
            
            int triMeshControl = 1; // 1 == keep polyMesh .. do nothing
            
            if ( OpenFlipper::Options::gui() ){
                if ( triMeshHandling_ != 0 ){
                    triMeshControl = triMeshHandling_->currentIndex();
                }else
                    triMeshControl = 0;
            }
            
            //check if it's actually a triangle mesh
            if (triMeshControl != 1){ // 1 == do nothing
                
                PolyMesh& mesh = *( object->mesh() );
                
                bool isTriangleMesh = true;
                
                for ( PolyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end() ; ++f_it) {
                    
                    // Count number of vertices for the current face
                    uint count = 0;
                    for ( PolyMesh::FaceVertexIter fv_it( mesh,f_it); fv_it; ++fv_it )
                        ++count;
                    
                    // Check if it is a triangle. If not, this is really a poly mesh
                    if ( count != 3 ) {
                        isTriangleMesh = false;
                        break;
                    }
                    
                }
                
                // Mesh loaded as polymesh is actually a triangle mesh. Ask the user to reload as triangle mesh or keep it as poly mesh.
                if ( isTriangleMesh )
                    
                    if ( triMeshControl == 0){ //ask what to do
                        
                        QMessageBox::StandardButton result = QMessageBox::question ( 0,
                            tr("TriMesh loaded as PolyMesh"),
                            tr("You opened the mesh as a poly mesh but actually its a triangle mesh. \nShould it be opened as a triangle mesh?"),
                            (QMessageBox::Yes | QMessageBox::No ),
                            QMessageBox::Yes );
                            // User decided to reload as triangle mesh
                            if ( result == QMessageBox::No )
                            isTriangleMesh = false;
                    }
                    
                    if ( isTriangleMesh ){
                        std::cerr << "Plugin FilePolyMesh : Reloading mesh as Triangle Mesh\n";
                        
                        bool pluginAvailable = false;
                        
                        emit pluginExists("filetrianglemesh",pluginAvailable);
                        
                        if (pluginAvailable){
                            
                            emit deleteObject( object->id() );
                            
                            return RPC::callFunctionValue<int>("filetrianglemesh","loadObject",_filename);
                        }
                    }
            }
        }
        
        object->mesh()->update_normals();
        
        object->update();
        
        object->show();
        
        emit log(LOGINFO,object->getObjectinfo());
        
        emit openedFile( object->id() );
        
        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new poly mesh object.");
        return -1;
    }
}

/// load a poly-mesh with given filename
int FilePolyMeshPlugin::loadPolyMeshObject(QString _filename){

    int id = -1;
    emit addEmptyObject(DATA_POLY_MESH, id);
    
    PolyMeshObject* object(0);
    if(PluginFunctions::getObject( id, object)) {
        
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
            emit deleteObject( object->id() );
            return -1;
            
        }
        
        object->mesh()->update_normals();
        
        object->update();
        
        object->show();
        
        emit log(LOGINFO,object->getObjectinfo());
        
        emit openedFile( object->id() );
        
        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new poly mesh object.");
        return -1;
    }
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
      emit log(LOGINFO, tr("Saved object to ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
      return true;
    }else{
      emit log(LOGERR, tr("Unable to save ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
      return false;
    }
  }else{

    emit log(LOGERR, tr("Unable to save (object isn't a poly mesh)"));
    return false;
  }
}

///return file-extension filters for loading objects
QString FilePolyMeshPlugin::getLoadFilters() {
  
  QString filters = QString( OpenMesh::IO::IOManager().qt_read_filters().c_str() );
  
  // Remove support for OFF-files since they are now handled by a separate plugin
  //filters.replace(";;Object File Format ( *.off )", "");
  //filters.replace("*.off", "");
  
  return filters;
}

///return file-extension filters for saving objects
QString FilePolyMeshPlugin::getSaveFilters() {
  
  QString filters = QString( OpenMesh::IO::IOManager().qt_write_filters().c_str() );

  // Remove support for OFF-files since they are now handled by a separate plugin
  //filters.replace(";;no description ( *.off )", "");
  //filters.replace("*.off", "");

  return filters;
}

///return supported DataType
DataType  FilePolyMeshPlugin::supportedType() {
  DataType type = DATA_POLY_MESH;
  return type;
}

void FilePolyMeshPlugin::loadIniFileLast( INIFile& _ini ,int _id ) {
  BaseObjectData* baseObject;
  if ( !PluginFunctions::getObject(_id,baseObject) ) {
    emit log(LOGERR,tr("Cannot find object for id ") + QString::number(_id) + tr(" in saveFile") );
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
    emit log(LOGERR,tr("Cannot find object for id ") + QString::number(_id) + tr(" in saveFile") );
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

QWidget* FilePolyMeshPlugin::saveOptionsWidget(QString _currentFilter) {

  return 0;
}

QWidget* FilePolyMeshPlugin::loadOptionsWidget(QString /*_currentFilter*/) {

  if (loadOptions_ == 0){
    //generate widget
    loadOptions_ = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);

    
    QLabel* label = new QLabel(tr("If PolyMesh is a Triangle Mesh:"));
    
    layout->addWidget(label);
    
    triMeshHandling_ = new QComboBox();
    triMeshHandling_->addItem( tr("Ask") );
    triMeshHandling_->addItem( tr("Open as PolyMesh") );
    triMeshHandling_->addItem( tr("Open as TriangleMesh") );

    layout->addWidget(triMeshHandling_);
    
    loadOptions_->setLayout(layout);

  }

  return loadOptions_;
}


Q_EXPORT_PLUGIN2( filepolymeshplugin , FilePolyMeshPlugin );

