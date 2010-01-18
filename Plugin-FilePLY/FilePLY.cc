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
*   $Revision: 7936 $                                                       *
*   $Author: kremer $                                                      *
*   $Date: 2009-12-15 14:01:19 +0100 (Tue, 15 Dec 2009) $                   *
*                                                                           *
\*===========================================================================*/

#include <QtGui>
#include <QFileInfo>
#include <QSettings>
#include <QPushButton>

#include "FilePLY.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

/// Constructor
FilePLYPlugin::FilePLYPlugin()
: loadOptions_(0),
  saveOptions_(0),
  triMeshHandling_(0) {
}

//-----------------------------------------------------------------------------------------------------

void FilePLYPlugin::initializePlugin() {
}

//-----------------------------------------------------------------------------------------------------

QString FilePLYPlugin::getLoadFilters() {
    return QString( tr("Polygon File Format files ( *.ply )") );
};

//-----------------------------------------------------------------------------------------------------

QString FilePLYPlugin::getSaveFilters() {
    return QString( tr("Polygon File Format files ( *.ply )") );
};

//-----------------------------------------------------------------------------------------------------

DataType  FilePLYPlugin::supportedType() {
    DataType type = DATA_POLY_MESH | DATA_TRIANGLE_MESH;
    return type;
}

//-----------------------------------------------------------------------------------------------------

int FilePLYPlugin::loadObject(QString _filename) {

    int triMeshControl = 0; // 0 == Auto-Detect
    
    if ( OpenFlipper::Options::gui() ){
        if ( triMeshHandling_ != 0 ){
            triMeshControl = triMeshHandling_->currentIndex();
        } else {
            triMeshControl = 0;
        }
    }
    
    int objectId = -1;
    
    if(triMeshControl == 0) {
        // If Auto-Detect is selected (triMeshControl == 0)
        objectId = loadPolyMeshObject(_filename);
        
        PolyMeshObject *object = 0;
        if(!PluginFunctions::getObject(objectId, object))
            return -1;
        
        for ( PolyMesh::FaceIter f_it = object->mesh()->faces_begin(); f_it != object->mesh()->faces_end() ; ++f_it) {
            
            // Count number of vertices for the current face
            uint count = 0;
            for ( PolyMesh::FaceVertexIter fv_it( *(object->mesh()),f_it); fv_it; ++fv_it )
                ++count;
            
            // Check if it is a triangle. If not, this is really a poly mesh
            if ( count != 3 ) {
                
                emit openedFile( objectId );
                
                return objectId;
            }
        }
        
    } else if (triMeshControl == 1) {
        // If Ask is selected -> show dialog
        objectId = loadPolyMeshObject(_filename);
        
        PolyMeshObject *object = 0;
        if(!PluginFunctions::getObject(objectId, object))
            return -1;
        
        for ( PolyMesh::FaceIter f_it = object->mesh()->faces_begin(); f_it != object->mesh()->faces_end() ; ++f_it) {
            
            // Count number of vertices for the current face
            uint count = 0;
            for ( PolyMesh::FaceVertexIter fv_it( *(object->mesh()),f_it); fv_it; ++fv_it )
                ++count;
            
            // Check if it is a triangle. If not, this is really a poly mesh
            if ( count != 3 ) {
                
                emit openedFile( objectId );
                
                return objectId;
            }
        }
        
        QMessageBox::StandardButton result = QMessageBox::question ( 0,
            tr("TriMesh loaded as PolyMesh"),
            tr("You opened the mesh as a poly mesh but actually its a triangle mesh. \nShould it be opened as a triangle mesh?"),
            (QMessageBox::Yes | QMessageBox::No ),
            QMessageBox::Yes );
                                                                     
        // User decided to reload as triangle mesh
        if ( result == QMessageBox::No ) {
            
            emit openedFile( objectId );
            
            return objectId;
        }
                                                                     
    } else if (triMeshControl == 2) {
        // If always open as PolyMesh is selected
        
        objectId = loadPolyMeshObject(_filename);
        
        emit openedFile( objectId );
        
        return objectId;
    } else {
        // If always open as TriMesh is selected
        
        objectId = loadTriMeshObject(_filename);
        
        emit openedFile( objectId );
        
        return objectId;
    }
    
    // Load object as triangle mesh
    if(objectId != -1) emit deleteObject(objectId);
    
    objectId = loadTriMeshObject(_filename);
    
    emit openedFile( objectId );
    
    return objectId;
};

//-----------------------------------------------------------------------------------------------------

/// load a triangle-mesh with given filename
int FilePLYPlugin::loadTriMeshObject(QString _filename){

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
            
            if (loadVertexNormal_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (loadVertexTexCoord_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
            if (loadVertexColor_->isChecked())
                opt += OpenMesh::IO::Options::VertexColor;
            
            if (loadFaceColor_->isChecked())
                opt += OpenMesh::IO::Options::FaceColor;
            
        } else {
            
            // Let openmesh try to read everything it can
            opt += OpenMesh::IO::Options::VertexNormal;
            opt += OpenMesh::IO::Options::VertexTexCoord;
            opt += OpenMesh::IO::Options::VertexColor;
            opt += OpenMesh::IO::Options::FaceColor;
            
        }
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FilePLY : Read error for Triangle Mesh\n";
            emit deleteObject( object->id() );
            return -1;
        }
        
        object->mesh()->update_normals();
        
        object->update();
        
        object->show();
        
        emit log(LOGINFO,object->getObjectinfo());
        
        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new triangle mesh object.");
        return -1;
    }
}

//-----------------------------------------------------------------------------------------------------

/// load a poly-mesh with given filename
int FilePLYPlugin::loadPolyMeshObject(QString _filename){

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
            
            if (loadVertexNormal_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (loadVertexTexCoord_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
            if (loadVertexColor_->isChecked())
                opt += OpenMesh::IO::Options::VertexColor;
            
            if (loadFaceColor_->isChecked())
                opt += OpenMesh::IO::Options::FaceColor;
            
        } else {
            
            // Let openmesh try to read everything it can
            opt += OpenMesh::IO::Options::VertexNormal;
            opt += OpenMesh::IO::Options::VertexTexCoord;
            opt += OpenMesh::IO::Options::VertexColor;
            opt += OpenMesh::IO::Options::FaceColor;
            
        }
        
        // load file
        bool ok = OpenMesh::IO::read_mesh( (*object->mesh()) , filename, opt );
        if (!ok)
        {
            std::cerr << "Plugin FilePLY : Read error for Poly Mesh\n";
            emit deleteObject( object->id() );
            return -1;
            
        }
        
        object->mesh()->update_normals();
        
        object->update();
        
        object->show();
        
        emit log(LOGINFO,object->getObjectinfo());
        
        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new poly mesh object.");
        return -1;
    }
}

//-----------------------------------------------------------------------------------------------------

bool FilePLYPlugin::saveObject(int _id, QString _filename)
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
            
            if (saveVertexNormal_->isChecked())
                opt += OpenMesh::IO::Options::VertexNormal;
            
            if (saveVertexTexCoord_->isChecked())
                opt += OpenMesh::IO::Options::VertexTexCoord;
            
            if (saveVertexColor_->isChecked())
                opt += OpenMesh::IO::Options::VertexColor;
            
            if (saveFaceColor_->isChecked())
                opt += OpenMesh::IO::Options::FaceColor;
            
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

QWidget* FilePLYPlugin::saveOptionsWidget(QString _currentFilter) {
    
    if (saveOptions_ == 0){
        //generate widget
        saveOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        
        saveBinary_ = new QCheckBox("Save Binary");
        layout->addWidget(saveBinary_);
        
        saveVertexNormal_ = new QCheckBox("Save Vertex Normals");
        layout->addWidget(saveVertexNormal_);
        
        saveVertexTexCoord_ = new QCheckBox("Save Vertex TexCoords");
        layout->addWidget(saveVertexTexCoord_);
        
        saveVertexColor_ = new QCheckBox("Save Vertex Colors");
        layout->addWidget(saveVertexColor_);
        
        saveFaceColor_ = new QCheckBox("Save Face Colors");
        layout->addWidget(saveFaceColor_);
      
        saveDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(saveDefaultButton_);       
        
        saveOptions_->setLayout(layout);
        
        connect(saveDefaultButton_, SIGNAL(clicked()), this, SLOT(slotSaveDefault()));
        
        saveBinary_->setChecked( OpenFlipperSettings().value("FilePLY/Save/Binary",true).toBool() );
        saveVertexNormal_->setChecked( OpenFlipperSettings().value("FilePLY/Save/Normals",true).toBool() );
        saveVertexTexCoord_->setChecked( OpenFlipperSettings().value("FilePLY/Save/TexCoords",true).toBool() );
        saveVertexColor_->setChecked( OpenFlipperSettings().value("FilePLY/Save/VertexColor",true).toBool() );
        saveFaceColor_->setChecked( OpenFlipperSettings().value("FilePLY/Save/FaceColor",true).toBool() );

    } 
    
    return saveOptions_;
}

//-----------------------------------------------------------------------------------------------------

QWidget* FilePLYPlugin::loadOptionsWidget(QString /*_currentFilter*/) {
    
    if (loadOptions_ == 0){
        //generate widget
        loadOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        
        QLabel* label = new QLabel(tr("If PolyMesh is a Triangle Mesh:"));
        
        layout->addWidget(label);
        
        triMeshHandling_ = new QComboBox();
        triMeshHandling_->addItem( tr("Auto-Detect") );
        triMeshHandling_->addItem( tr("Ask") );
        triMeshHandling_->addItem( tr("Always open as PolyMesh") );
        triMeshHandling_->addItem( tr("Always open as TriangleMesh") );
        
        layout->addWidget(triMeshHandling_);
        
        loadVertexNormal_ = new QCheckBox("Load Vertex Normals");
        layout->addWidget(loadVertexNormal_);
        
        loadVertexTexCoord_ = new QCheckBox("Load Vertex TexCoords");
        layout->addWidget(loadVertexTexCoord_);
                
        loadVertexColor_ = new QCheckBox("Load Vertex Colors");
        layout->addWidget(loadVertexColor_);
        
        loadFaceColor_ = new QCheckBox("Load Face Colors");
        layout->addWidget(loadFaceColor_);

        loadDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(loadDefaultButton_);
        
        loadOptions_->setLayout(layout);
        
        connect(loadDefaultButton_, SIGNAL(clicked()), this, SLOT(slotLoadDefault()));
        
        
        triMeshHandling_->setCurrentIndex(OpenFlipperSettings().value("FilePLY/Load/TriMeshHandling",2).toInt() );
        
        loadVertexNormal_->setChecked( OpenFlipperSettings().value("FilePLY/Load/Normals",true).toBool()  );
        loadVertexTexCoord_->setChecked( OpenFlipperSettings().value("FilePLY/Load/TexCoords",true).toBool()  );
        loadVertexColor_->setChecked( OpenFlipperSettings().value("FilePLY/Load/VertexColor",true).toBool() );
        loadFaceColor_->setChecked( OpenFlipperSettings().value("FilePLY/Load/FaceColor",true).toBool()  );
        
    }
    
    return loadOptions_;
}

void FilePLYPlugin::slotLoadDefault() {
    
    OpenFlipperSettings().setValue( "FilePLY/Load/Normals",     loadVertexNormal_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Load/TexCoords",   loadVertexTexCoord_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Load/VertexColor", loadVertexColor_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Load/FaceColor",   loadFaceColor_->isChecked()  );

    OpenFlipperSettings().setValue( "FilePLY/Load/TriMeshHandling", triMeshHandling_->currentIndex() );
}


void FilePLYPlugin::slotSaveDefault() {
    
    OpenFlipperSettings().setValue( "FilePLY/Save/Binary",      saveBinary_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Save/Normals",     saveVertexNormal_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Save/TexCoords",   saveVertexTexCoord_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Save/VertexColor", saveVertexColor_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Save/FaceColor",   saveFaceColor_->isChecked()  );
  
}

Q_EXPORT_PLUGIN2( fileplyplugin , FilePLYPlugin );