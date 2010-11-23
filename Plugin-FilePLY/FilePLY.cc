/*===========================================================================*\
*                                                                           *
*                              OpenFlipper                                  *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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

// Defines for the type handling drop down box
#define TYPEAUTODETECT 0
#define TYPEASK        1
#define TYPEPOLY       2
#define TYPETRIANGLE   3

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

bool FilePLYPlugin::parseHeader(QString _filename, PLYHeader& _header) {
    
    std::ifstream ifs(_filename.toUtf8());
    
    if (!ifs.is_open() | !ifs.good() | ifs.eof()) {
        
        emit log(LOGERR, tr("Error: Could not read header data of specified PLY-file! Aborting."));
        return false;
    }
    
    std::string line;
    std::istringstream sstr;
    
    std::string dString = "";
    int         dInt;
    std::string lastElement = "";
    
    while(dString != "end_header" && !ifs.eof()) {
        
        // Get whole line
        sstr.clear();
        std::getline(ifs, line);
        sstr.str(line);
        sstr >> dString;
        
        if(dString == "ply") {
            continue;
        } else if(dString == "format") {
            // Determine file format (either ascii or binary)
            // format ascii/binary version
            sstr >> dString;
            _header.binary = (dString != "ascii");
            // Skip version
            continue;
        } else if(dString == "element") {
            // Get number of elements
            // element vertex/face/edge number
            sstr >> dString;
            sstr >> dInt;
            if(dString == "vertex") {
                lastElement = dString;
                _header.numVertices = dInt;
            } else if (dString == "face") {
                lastElement = dString;
                _header.numFaces = dInt;
            }
            // Skip processing of rest of line
            continue;
        } else if(dString == "property") {
            // Discard property type
            sstr >> dString;
            // Get property name
            sstr >> dString;
            if(dString == "x" || dString == "y" || dString == "z") {
                _header.vOrder.push_back("xyz");
            } else if(dString == "nx" || dString == "ny" || dString == "nz") {
                if(lastElement == "vertex") {
                    _header.hasVertexNormals = true;
                    _header.vOrder.push_back("n_xyz");
                } else if(lastElement == "face") {
                    _header.hasFaceNormals = true;
                    _header.fOrder.push_back("n_xyz");
                }
            } else if(dString == "red" || dString == "green" || dString == "blue") {
                if(lastElement == "vertex") {
                    _header.hasVertexColors = true;
                    _header.vOrder.push_back("rgb");
                } else if(lastElement == "face") {
                    _header.hasFaceColors = true;
                    _header.fOrder.push_back("rgb");
                }
            } else if(dString == "u" || dString == "v") {
                if(lastElement == "vertex") {
                    _header.hasVertexTexCoords = true;
                    _header.vOrder.push_back("uv");
                }
            } else if(dString == "ambient_red" || dString == "ambient_green" || dString == "ambient_blue") {
                if(lastElement == "vertex") {
                    _header.hasVertexColors = true;
                    _header.hasVertexColorsADS = true;
                    _header.vOrder.push_back("a_rgb");
                } else if(lastElement == "face") {
                    _header.hasFaceColors = true;
                    _header.hasFaceColorsADS = true;
                    _header.fOrder.push_back("a_rgb");
                }
            } else if(dString == "diffuse_red" || dString == "diffuse_green" || dString == "diffuse_blue") {
                if(lastElement == "vertex") {
                    _header.hasVertexColors = true;
                    _header.hasVertexColorsADS = true;
                    _header.vOrder.push_back("d_rgb");
                } else if(lastElement == "face") {
                    _header.hasFaceColors = true;
                    _header.hasFaceColorsADS = true;
                    _header.fOrder.push_back("d_rgb");
                }
            } else if(dString == "specular_red" || dString == "specular_green" || dString == "specular_blue") {
                if(lastElement == "vertex") {
                    _header.hasVertexColors = true;
                    _header.hasVertexColorsADS = true;
                    _header.vOrder.push_back("s_rgb");
                } else if(lastElement == "face") {
                    _header.hasFaceColors = true;
                    _header.hasFaceColorsADS = true;
                    _header.fOrder.push_back("s_rgb");
                }
            }
        } else {
            continue;
        }
    }
    
    // Now we're at 'end_header'
    // We skip the next numVertices lines in order to
    // get to the face definitions (unless there are any)
    // If the file is binary, we don't know the exact
    // number of bytes to skip in order to reach the
    // face definitions -> assume polymesh here
    if(_header.numFaces == 0 || _header.binary) {
        // We're done here
        return true;
    }
    
    for(int i = 0; i < _header.numVertices; ++i) {
        std::getline(ifs, line);
    }
    
    std::getline(ifs, line);
    
    if(ifs.eof()) {
        emit log(LOGERR, "Could not read face valence. Something is wrong with your file!");
        ifs.close();
        return false;
    }
    
    sstr.clear();
    sstr.str(line);
    // Get number of vertices per face
    sstr >> dInt;
    
    _header.isTriangleMesh = (dInt == 3);
    
    ifs.close();
    return true;
}

//-----------------------------------------------------------------------------------------------------

int FilePLYPlugin::loadObject(QString _filename) {

    int triMeshControl = TYPEAUTODETECT; // 0 == Auto-Detect
    
    if ( OpenFlipper::Options::gui() ){
        if ( triMeshHandling_ != 0 ){
          triMeshControl = triMeshHandling_->currentIndex();
        } else {
          triMeshControl = TYPEAUTODETECT;
        }
    }
    
    // Create header and initialize with binary zeros
    PLYHeader header = {0,0,0,0,0,0,0,std::vector<std::string>(),
                        0,0,0,0,std::vector<std::string>()};
    
    // Parse header in order to extract important information
    if(!parseHeader(_filename, header)) {
        return -1;
    }
    
    int objectId = -1;
    
    if(triMeshControl == TYPEAUTODETECT) {
        
        if(header.isTriangleMesh) {
            // Load as trianglemesh
            objectId = loadTriMeshObject(_filename, header);
        
            TriMeshObject* object(0);
            if(PluginFunctions::getObject( objectId, object )) {
                
                object->show();
                emit openedFile( objectId );
            }
            
            return objectId;
            
        } else {
            // Load as polymesh
            objectId = loadPolyMeshObject(_filename, header);
            
            PolyMeshObject* object(0);
            if(PluginFunctions::getObject( objectId, object )) {
                
                object->show();
                emit openedFile( objectId );
            }
            return objectId;
        }
        
    } else if (triMeshControl == TYPEASK) {      

        // If Ask is selected -> show dialog
        QMessageBox msgBox;
        QPushButton *detectButton = msgBox.addButton(tr("Auto-Detect"), QMessageBox::ActionRole);
        QPushButton *triButton    = msgBox.addButton(tr("Open as triangle mesh"), QMessageBox::ActionRole);
        QPushButton *polyButton   = msgBox.addButton(tr("Open as poly mesh"), QMessageBox::ActionRole);
        
        msgBox.setWindowTitle( tr("Mesh types in file") );
        msgBox.setText( tr("You are about to open a file containing one or more mesh types. \n\n Which mesh type should be used?") );
        msgBox.setDefaultButton( detectButton );
        msgBox.exec();
        
        if ((msgBox.clickedButton() == polyButton) ||
            (msgBox.clickedButton() == detectButton && !header.isTriangleMesh)) {
            
            // Detection requested and detected as polymesh
            objectId = loadPolyMeshObject(_filename, header);
            
            PolyMeshObject* object(0);
            if(PluginFunctions::getObject( objectId, object )) {
                
                object->show();
                emit openedFile( objectId );
            }
            return objectId;
        }
                                                                     
    } else if (triMeshControl == TYPEPOLY) {
        // If always open as PolyMesh is selected
        
        objectId = loadPolyMeshObject(_filename, header);
        
        PolyMeshObject* object(0);
        if(PluginFunctions::getObject( objectId, object )) {
            
            object->show();
            emit openedFile( objectId );
        }
        
        return objectId;
    } else {
        // If always open as TriMesh is selected
        
        objectId = loadTriMeshObject(_filename, header);
        
        TriMeshObject* object(0);
        if(PluginFunctions::getObject( objectId, object )) {
            
            object->show();
            emit openedFile( objectId );
        }
        
        return objectId;
    }
    
    // Load object as triangle mesh
    
    objectId = loadTriMeshObject(_filename, header);
    
    TriMeshObject* object(0);
    if(PluginFunctions::getObject( objectId, object )) {
        
        object->show();
        emit openedFile( objectId );
    }
    
    return objectId;
};

//-----------------------------------------------------------------------------------------------------

/// load a triangle-mesh with given filename
int FilePLYPlugin::loadTriMeshObject(QString _filename, const PLYHeader _header){

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
        
        // Get mesh
        TriMesh* mesh = object->mesh();
    
        if(!_header.binary) {
            // Read ascii file
            if(!readMeshFileAscii(_filename, mesh, _header)) {
                emit log(LOGERR, "Error while reading ascii PLY file!");
                emit deleteObject(id);
                return -1;
            }
        } else {
            // Read binary file
            if(!readMeshFileBinary(_filename, mesh, _header)) {
                emit log(LOGERR, "Error while reading binary PLY file!");
                emit deleteObject(id);
                return -1;
            }
        }
        
        // Switch to point mode if mesh does not contain one signle face
        if(_header.numFaces == 0)
            PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::POINTS);
        
        object->update();
        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new triangle mesh object.");
        return -1;
    }
}

//-----------------------------------------------------------------------------------------------------

/// load a poly-mesh with given filename
int FilePLYPlugin::loadPolyMeshObject(QString _filename, const PLYHeader _header){

    int id = -1;
    emit addEmptyObject(DATA_POLY_MESH, id);
    
    PolyMeshObject* object(0);
    if(PluginFunctions::getObject(id, object)) {
        
        if (PluginFunctions::objectCount() == 1 )
            object->target(true);
        
        object->setFromFileName(_filename);
        
        // call the local function to update names
        QFileInfo f(_filename);
        object->setName( f.fileName() );
        
        // Get mesh
        PolyMesh* mesh = object->mesh();
    
        if(!_header.binary) {
            // Read ascii file
            if(!readMeshFileAscii(_filename, mesh, _header)) {
                emit log(LOGERR, "Error while reading ascii PLY file!");
                emit deleteObject(id);
                return -1;
            }
        } else {
            // Read binary file
            if(!readMeshFileBinary(_filename, mesh, _header)) {
                emit log(LOGERR, "Error while reading binary PLY file!");
                emit deleteObject(id);
                return -1;
            }
        }
        
        // Switch to point mode if mesh does not contain one signle face
        if(_header.numFaces == 0)
            PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::POINTS);
        
        object->update();
        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new poly mesh object.");
        return -1;
    }
    
    return id;
}

//-----------------------------------------------------------------------------------------------------

bool FilePLYPlugin::saveObject(int _id, QString _filename)
{
    BaseObjectData* object;
    PluginFunctions::getObject(_id,object);
    
    std::string filename = std::string( _filename.toUtf8() );
    
    if ( object->dataType( DATA_POLY_MESH ) ) {
        
        object->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
        object->setPath(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );
        
        PolyMeshObject* polyObj = dynamic_cast<PolyMeshObject* >( object );
        
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
        
        if (OpenMesh::IO::write_mesh(*polyObj->mesh(), filename.c_str(), opt) ){
            emit log(LOGINFO, tr("Saved object to ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
            return true;
        } else {
            emit log(LOGERR, tr("Unable to save ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name());
            return false;
        }
    } else if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
        
        object->setName(_filename.section(OpenFlipper::Options::dirSeparator(),-1));
        object->setPath(_filename.section(OpenFlipper::Options::dirSeparator(),0,-2) );
        
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
        
        if (OpenMesh::IO::write_mesh(*triObj->mesh(), filename.c_str(), opt) ) {
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
        
        saveVertexColor_ = new QCheckBox("Save Vertex Colors");
        layout->addWidget(saveVertexColor_);
        
        saveVertexTexCoord_ = new QCheckBox("Save Vertex TexCoords");
        layout->addWidget(saveVertexTexCoord_);
        
        saveFaceNormal_ = new QCheckBox("Save Face Normals");
        layout->addWidget(saveFaceNormal_);
        
        saveFaceColor_ = new QCheckBox("Save Face Colors");
        layout->addWidget(saveFaceColor_);
        
        saveDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(saveDefaultButton_);       
        
        saveOptions_->setLayout(layout);
        
        connect(saveDefaultButton_, SIGNAL(clicked()), this, SLOT(slotSaveDefault()));
        
        saveBinary_->setChecked( OpenFlipperSettings().value("FilePLY/Save/Binary",true).toBool() );
        saveVertexNormal_->setChecked( OpenFlipperSettings().value("FilePLY/Save/Normals",true).toBool() );
        saveVertexColor_->setChecked( OpenFlipperSettings().value("FilePLY/Save/VertexColor",true).toBool() );
        saveVertexTexCoord_->setChecked( OpenFlipperSettings().value("FilePLY/Save/TexCoords",true).toBool() );
        saveFaceNormal_->setChecked( OpenFlipperSettings().value("FilePLY/Save/FaceNormal",true).toBool() );
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
                
        loadVertexColor_ = new QCheckBox("Load Vertex Colors");
        layout->addWidget(loadVertexColor_);
        
        loadVertexTexCoord_ = new QCheckBox("Load Vertex TexCoords");
        layout->addWidget(loadVertexTexCoord_);
        
        loadFaceNormal_ = new QCheckBox("Load Face Normals");
        layout->addWidget(loadFaceNormal_);
        
        loadFaceColor_ = new QCheckBox("Load Face Colors");
        layout->addWidget(loadFaceColor_);
        
        loadDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(loadDefaultButton_);
        
        loadOptions_->setLayout(layout);
        
        connect(loadDefaultButton_, SIGNAL(clicked()), this, SLOT(slotLoadDefault()));
        
        
        triMeshHandling_->setCurrentIndex(OpenFlipperSettings().value("FilePLY/Load/TriMeshHandling",TYPEAUTODETECT).toInt() );
        
        loadVertexNormal_->setChecked( OpenFlipperSettings().value("FilePLY/Load/Normals",true).toBool()  );
        loadVertexColor_->setChecked( OpenFlipperSettings().value("FilePLY/Load/VertexColor",true).toBool() );
        loadVertexTexCoord_->setChecked( OpenFlipperSettings().value("FilePLY/Load/TexCoords",true).toBool()  );
        loadFaceNormal_->setChecked( OpenFlipperSettings().value("FilePLY/Load/FaceNormal",true).toBool()  );
        loadFaceColor_->setChecked( OpenFlipperSettings().value("FilePLY/Load/FaceColor",true).toBool()  );
    }
    
    return loadOptions_;
}

void FilePLYPlugin::slotLoadDefault() {
    
    OpenFlipperSettings().setValue( "FilePLY/Load/Normals",     loadVertexNormal_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Load/VertexColor", loadVertexColor_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Load/TexCoords",   loadVertexTexCoord_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Load/FaceNormal",  loadFaceNormal_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Load/FaceColor",   loadFaceColor_->isChecked()  );

    OpenFlipperSettings().setValue( "FilePLY/Load/TriMeshHandling", triMeshHandling_->currentIndex() );
    
    OpenFlipperSettings().setValue( "Core/File/UseLoadDefaults", true );
}


void FilePLYPlugin::slotSaveDefault() {
    
    OpenFlipperSettings().setValue( "FilePLY/Save/Binary",      saveBinary_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Save/TexCoords",   saveVertexTexCoord_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Save/Normals",     saveVertexNormal_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Save/VertexColor", saveVertexColor_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Save/FaceNormal",  saveFaceNormal_->isChecked()  );
    OpenFlipperSettings().setValue( "FilePLY/Save/FaceColor",   saveFaceColor_->isChecked()  );
}

Q_EXPORT_PLUGIN2( fileplyplugin , FilePLYPlugin );