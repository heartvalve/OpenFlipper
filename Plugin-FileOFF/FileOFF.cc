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
#include <QSettings>
#include <QPushButton>

#include "FileOFF.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

/// Constructor
FileOFFPlugin::FileOFFPlugin()
: loadOptions_(0),
  saveOptions_(0),
  triMeshHandling_(0) {
}

//-----------------------------------------------------------------------------------------------------

void FileOFFPlugin::initializePlugin() {
}

//-----------------------------------------------------------------------------------------------------

QString FileOFFPlugin::getLoadFilters() {
    return QString( tr("Object File Format files ( *.off )") );
};

//-----------------------------------------------------------------------------------------------------

QString FileOFFPlugin::getSaveFilters() {
    return QString( tr("Object File Format files ( *.off )") );
};

//-----------------------------------------------------------------------------------------------------

DataType  FileOFFPlugin::supportedType() {
    DataType type = DATA_POLY_MESH | DATA_TRIANGLE_MESH;
    return type;
}

//-----------------------------------------------------------------------------------------------------

int FileOFFPlugin::loadObject(QString _filename) {

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
        
        //set options
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::loadingSettings() &&
            !OpenFlipper::Options::loadingRecentFile() && loadOptions_ != 0){
            
            if (loadVertexColor_->isChecked())
                opt += OpenMesh::IO::Options::VertexColor;
            
            if (loadFaceColor_->isChecked())
                opt += OpenMesh::IO::Options::FaceColor;
            
            // ColorAlpha is only checked if loading binary off's
            if (loadAlpha_->isChecked())
                opt += OpenMesh::IO::Options::ColorAlpha;
            
            if (loadNormals_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (loadTexCoords_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
        } else {
            
            // Let openmesh try to read everything it can
            opt += OpenMesh::IO::Options::VertexColor;
            opt += OpenMesh::IO::Options::FaceColor;
            opt += OpenMesh::IO::Options::VertexNormal;
            opt += OpenMesh::IO::Options::VertexTexCoord;
        }
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FileOFF : Read error for Poly Mesh\n";
            emit deleteObject( object->id() );
            return -1;
            
        } else {
            
            int triMeshControl = 1; // 1 == keep polyMesh .. do nothing
            
            if ( OpenFlipper::Options::gui() ){
                if ( triMeshHandling_ != 0 ){
                    triMeshControl = triMeshHandling_->currentIndex();
                } else
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
                        std::cerr << "Plugin FileOFF : Reloading mesh as Triangle Mesh\n";
                        
                        emit deleteObject( object->id() );
                        
                        return loadTriMeshObject(_filename);
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
};

//-----------------------------------------------------------------------------------------------------

/// load a triangle-mesh with given filename
int FileOFFPlugin::loadTriMeshObject(QString _filename){

    int id = -1;
    emit addEmptyObject(DATA_TRIANGLE_MESH, id);
    
    TriMeshObject* object(0);
    if(PluginFunctions::getObject( id, object)) {
        
        if ( PluginFunctions::objectCount() == 1 )
            object->target(true);
        
        object->setFromFileName(_filename);
        
        // call the local function to update names
        QFileInfo f(_filename);
        object->setName( f.fileName() );
        
        std::string filename = std::string( _filename.toUtf8() );
        
        //set options
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::loadingSettings() &&
            !OpenFlipper::Options::loadingRecentFile() && loadOptions_ != 0){
            
            if (loadVertexColor_->isChecked())
                opt += OpenMesh::IO::Options::VertexColor;
            
            if (loadFaceColor_->isChecked())
                opt += OpenMesh::IO::Options::FaceColor;
            
            //ColorAlpha is only checked if loading binary off's
            if (loadAlpha_->isChecked())
                opt += OpenMesh::IO::Options::ColorAlpha;
            
            if (loadNormals_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (loadTexCoords_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
        }else{
            //let openmesh try to read everything it can
            opt += OpenMesh::IO::Options::VertexColor;
            opt += OpenMesh::IO::Options::FaceColor;
            opt += OpenMesh::IO::Options::VertexNormal;
            opt += OpenMesh::IO::Options::VertexTexCoord;
        }
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FileOFF : Read error for Triangle Mesh\n";
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
        emit log(LOGERR,"Error : Could not create new triangle mesh object.");
        return -1;
    }
}

//-----------------------------------------------------------------------------------------------------

/// load a poly-mesh with given filename
int FileOFFPlugin::loadPolyMeshObject(QString _filename){

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
        
        //set options
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::loadingSettings() &&
            !OpenFlipper::Options::loadingRecentFile() && loadOptions_ != 0){
            
            if (loadVertexColor_->isChecked())
                opt += OpenMesh::IO::Options::VertexColor;
            
            if (loadFaceColor_->isChecked())
                opt += OpenMesh::IO::Options::FaceColor;
            
            // ColorAlpha is only checked if loading binary off's
            if (loadAlpha_->isChecked())
                opt += OpenMesh::IO::Options::ColorAlpha;
            
            if (loadNormals_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (loadTexCoords_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
        } else {
            
            // Let openmesh try to read everything it can
            opt += OpenMesh::IO::Options::VertexColor;
            opt += OpenMesh::IO::Options::FaceColor;
            opt += OpenMesh::IO::Options::VertexNormal;
            opt += OpenMesh::IO::Options::VertexTexCoord;
        }
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FileOFF : Read error for Poly Mesh\n";
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

//-----------------------------------------------------------------------------------------------------

bool FileOFFPlugin::saveObject(int _id, QString _filename)
{
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
    } else if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
        
        object->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
        object->path(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );
        
        TriMeshObject* triObj = dynamic_cast<TriMeshObject* >( object );
        
        OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
        
        if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0){
            
            if (saveBinary_->isChecked())
                opt += OpenMesh::IO::Options::Binary;
            
            if (saveVertexColor_->isChecked()){
                opt += OpenMesh::IO::Options::VertexColor;
            }
            
            if (saveFaceColor_->isChecked()){
                opt += OpenMesh::IO::Options::FaceColor;
            }
            
            if (saveAlpha_->isChecked()){
                opt += OpenMesh::IO::Options::ColorAlpha;
            }
            
            if (saveNormals_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (saveTexCoords_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
        }
        
        if (OpenMesh::IO::write_mesh(*triObj->mesh(), filename.c_str(),opt) ) {
            emit log(LOGINFO, tr("Saved object to ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
            return true;
        } else {
            emit log(LOGERR, tr("Unable to save ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
            return false;
        }
    } else {
        emit log(LOGERR, tr("Unable to save (object is not a compatible mesh type)"));
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileOFFPlugin::saveOptionsWidget(QString _currentFilter) {
    
    if (saveOptions_ == 0){
        //generate widget
        saveOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        
        saveBinary_ = new QCheckBox("Save Binary");
        layout->addWidget(saveBinary_);
        
        saveVertexColor_ = new QCheckBox("Save Vertex Colors");
        layout->addWidget(saveVertexColor_);
        
        saveFaceColor_ = new QCheckBox("Save Face Colors");
        layout->addWidget(saveFaceColor_);
        
        saveAlpha_ = new QCheckBox("Save Color Alpha");
        layout->addWidget(saveAlpha_);
        
        saveNormals_ = new QCheckBox("Save Normals");
        layout->addWidget(saveNormals_);
        
        saveTexCoords_ = new QCheckBox("Save TexCoords");
        layout->addWidget(saveTexCoords_);
        
        saveDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(saveDefaultButton_);       
        
        saveOptions_->setLayout(layout);
        
        connect(saveDefaultButton_, SIGNAL(clicked()), this, SLOT(slotSaveDefault()));
        
        saveBinary_->setChecked( OpenFlipperSettings().value("FileOff/Save/Binary",true).toBool() );
        saveVertexColor_->setChecked( OpenFlipperSettings().value("FileOff/Save/VertexColor",true).toBool() );
        saveFaceColor_->setChecked( OpenFlipperSettings().value("FileOff/Save/FaceColor",true).toBool() );
        saveAlpha_->setChecked( OpenFlipperSettings().value("FileOff/Save/Alpha",true).toBool() );
        saveNormals_->setChecked( OpenFlipperSettings().value("FileOff/Save/Normals",true).toBool() );
        saveTexCoords_->setChecked( OpenFlipperSettings().value("FileOff/Save/TexCoords",true).toBool() );
        
    } 
    
    return saveOptions_;
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileOFFPlugin::loadOptionsWidget(QString /*_currentFilter*/) {
    
    if (loadOptions_ == 0){
        //generate widget
        loadOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        
        QLabel* label = new QLabel(tr("If PolyMesh is a Triangle Mesh:"));
        
        layout->addWidget(label);
        
        /// \todo : possible options ?:
        // Triangle Mesh in File -> Load as Triangle Mesh
        // Triangle Mesh in File -> Load as Poly Mesh
        // Poly Mesh in File     -> Load as Triangle Mesh
        // Poly Mesh in File     -> Load as Poly Mesh
        
        triMeshHandling_ = new QComboBox();
        triMeshHandling_->addItem( tr("Ask") );
        triMeshHandling_->addItem( tr("Open as PolyMesh") );
        triMeshHandling_->addItem( tr("Open as TriangleMesh") );
        
        layout->addWidget(triMeshHandling_);
        
        loadVertexColor_ = new QCheckBox("Load Vertex Colors");
        layout->addWidget(loadVertexColor_);
        
        loadFaceColor_ = new QCheckBox("Load Face Colors");
        layout->addWidget(loadFaceColor_);
        
        loadAlpha_ = new QCheckBox("Load Color Alpha");
        layout->addWidget(loadAlpha_);
        
        loadNormals_ = new QCheckBox("Load Normals");
        layout->addWidget(loadNormals_);
        
        loadTexCoords_ = new QCheckBox("Load TexCoords");
        layout->addWidget(loadTexCoords_);
 
        loadDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(loadDefaultButton_);
        
        loadOptions_->setLayout(layout);
        
        connect(loadDefaultButton_, SIGNAL(clicked()), this, SLOT(slotLoadDefault()));
        
        
        triMeshHandling_->setCurrentIndex(OpenFlipperSettings().value("FileOff/Load/TriMeshHandling",2).toInt() );
        
        loadVertexColor_->setChecked( OpenFlipperSettings().value("FileOff/Load/VertexColor",true).toBool() );
        loadFaceColor_->setChecked( OpenFlipperSettings().value("FileOff/Load/FaceColor",true).toBool()  );
        loadAlpha_->setChecked( OpenFlipperSettings().value("FileOff/Load/Alpha",true).toBool()  );
        loadNormals_->setChecked( OpenFlipperSettings().value("FileOff/Load/Normals",true).toBool()  );
        loadTexCoords_->setChecked( OpenFlipperSettings().value("FileOff/Load/TexCoords",true).toBool()  );
    }
    
    return loadOptions_;
}

void FileOFFPlugin::slotLoadDefault() {
  OpenFlipperSettings().setValue( "FileOff/Load/VertexColor", loadVertexColor_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Load/FaceColor",   loadFaceColor_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Load/Alpha",       loadAlpha_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Load/Normals",     loadNormals_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Load/TexCoords",   loadTexCoords_->isChecked()  );

  OpenFlipperSettings().setValue("FileOff/Load/TriMeshHandling", triMeshHandling_->currentIndex() );
}


void FileOFFPlugin::slotSaveDefault() {
  OpenFlipperSettings().setValue( "FileOff/Save/Binary",      saveBinary_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Save/VertexColor", saveVertexColor_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Save/FaceColor",   saveFaceColor_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Save/Alpha",       saveAlpha_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Save/Normals",     saveNormals_->isChecked()  );
  OpenFlipperSettings().setValue( "FileOff/Save/TexCoords",   saveTexCoords_->isChecked()  );
}

Q_EXPORT_PLUGIN2( fileoffplugin , FileOFFPlugin );

