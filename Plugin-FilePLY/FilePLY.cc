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
  saveBinary_(0),
  saveVertexNormal_(0),
  saveVertexColor_(0),
  saveVertexTexCoord_(0),
  saveFaceNormal_(0),
  saveFaceColor_(0),
  saveDefaultButton_(0),
  triMeshHandling_(0),
  loadVertexNormal_(0),
  loadVertexColor_(0),
  loadVertexTexCoord_(0),
  loadFaceNormal_(0),
  loadFaceColor_(0),
  loadDefaultButton_(0) {
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

size_t FilePLYPlugin::getTypeSize(std::string _type) {
    
    if(_type == "char" || _type == "uchar")
        return sizeof(char);
    else if(_type == "short" || _type == "ushort")
        return sizeof(short);
    else if(_type == "float" || _type == "float32")
        return sizeof(float);
    else if(_type == "int" || _type == "int32" || _type == "uint" || _type == "uint32")
        return sizeof(int);
    else if(_type == "double" || _type == "int64" || _type == "uint64")
        return sizeof(double);
    else
        std::cerr << "Unable to parse SizeType. I got " << _type << std::endl;
    
    // We don't support long type since its size differs depending on
    // the host architecture
    return 0;
}

//-----------------------------------------------------------------------------------------------------

bool FilePLYPlugin::parseHeader(QString _filename, PLYHeader& _header) {
    
    std::ifstream ifs(_filename.toUtf8());
    
    if (!ifs.is_open() || !ifs.good() || ifs.eof()) {
        
        emit log(LOGERR, tr("Error: Could not read header data of specified PLY-file! Aborting."));
        return false;
    }
    
    std::string line;
    std::istringstream sstr;
    
    std::string dString = "";
    std::string dStringPType = "";
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
            if ( dString.find("ascii") != std::string::npos ) {
              _header.binary = false;
            } else {
              _header.binary = true;
              if ( dString.find("big") != std::string::npos ) {
                _header.bigEndian = true;
              } else {
                if ( dString.find("little") == std::string::npos ) {
                  emit log(LOGWARN,tr("Binary PLY file without endian specification. Assuming little endian."));
                }
                _header.bigEndian = false;
              }

            }
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
            // Get property data type
            sstr >> dStringPType;
            // Get property name
            sstr >> dString;
            if(dStringPType == "list") {
                _header.valenceType = dString;
                // Get index type
                sstr >> dString;
                _header.indexType = dString;
            } else if(dString == "x" || dString == "y" || dString == "z") {
                _header.vProps.push_back(PPair("xyz",dStringPType));
            } else if(dString == "nx" || dString == "ny" || dString == "nz") {
                if(lastElement == "vertex") {
                    _header.hasVertexNormals = true;
                    _header.vProps.push_back(PPair("n_xyz",dStringPType));
                } else if(lastElement == "face") {
                    _header.hasFaceNormals = true;
                    _header.fProps.push_back(PPair("n_xyz",dStringPType));
                }
            } else if(dString == "red" || dString == "green" || dString == "blue") {
                if(lastElement == "vertex") {
                    _header.hasVertexColors = true;
                    _header.vProps.push_back(PPair("rgb",dStringPType));
                } else if(lastElement == "face") {
                    _header.hasFaceColors = true;
                    _header.fProps.push_back(PPair("rgb",dStringPType));
                }
            } else if(dString == "alpha" || dString == "a") {
                if(lastElement == "vertex") {
                    _header.hasVertexColorAlpha = true;
                    _header.vProps.push_back(PPair("rgb",dStringPType));
                } else if(lastElement == "face") {
                    _header.hasFaceColorAlpha = true;
                    _header.vProps.push_back(PPair("rgb",dStringPType));
                }
            } else if(dString == "u" || dString == "v") {
                if(lastElement == "vertex") {
                    _header.hasVertexTexCoords = true;
                    _header.vProps.push_back(PPair("uv",dStringPType));
                }
            } else if(dString == "ambient_red" || dString == "ambient_green" || dString == "ambient_blue") {
                if(lastElement == "vertex") {
                    _header.hasVertexColors = true;
                    _header.vProps.push_back(PPair("a_rgb",dStringPType));
                } else if(lastElement == "face") {
                    _header.hasFaceColors = true;
                    _header.fProps.push_back(PPair("a_rgb",dStringPType));
                }
            } else if(dString == "diffuse_red" || dString == "diffuse_green" || dString == "diffuse_blue") {
                if(lastElement == "vertex") {
                    _header.hasVertexColors = true;
                    _header.vProps.push_back(PPair("d_rgb",dStringPType));
                } else if(lastElement == "face") {
                    _header.hasFaceColors = true;
                    _header.fProps.push_back(PPair("d_rgb",dStringPType));
                }
            } else if(dString == "specular_red" || dString == "specular_green" || dString == "specular_blue") {
                if(lastElement == "vertex") {
                    _header.hasVertexColors = true;
                    _header.vProps.push_back(PPair("s_rgb",dStringPType));
                } else if(lastElement == "face") {
                    _header.hasFaceColors = true;
                    _header.fProps.push_back(PPair("s_rgb",dStringPType));
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
int FilePLYPlugin::loadObject(QString _filename, DataType _type) {

  // Create header and initialize with binary zeros
  PLYHeader header = {0,false,0,0,0,0,0,0,std::vector<PPair>(),
                      0,0,0,0,"","",std::vector<PPair>()};

  // Parse header in order to extract important information
  if(!parseHeader(_filename, header)) {
    return -1;
  }

  int objectId = -1;

  // Forced polymesh read
  if ( _type == DATA_POLY_MESH ) {
    objectId = loadPolyMeshObject(_filename, header);

    PolyMeshObject* object(0);
    if(PluginFunctions::getObject( objectId, object )) {

      object->show();
      emit openedFile( objectId );
    }

    // Update viewport
    PluginFunctions::viewAll();

    return objectId;
  } else if ( _type == DATA_TRIANGLE_MESH) {
    // If always open as TriMesh is selected

    objectId = loadTriMeshObject(_filename, header);

    TriMeshObject* object(0);
    if(PluginFunctions::getObject( objectId, object )) {

      object->show();
      emit openedFile( objectId );
    }

    // Update viewport
    PluginFunctions::viewAll();

    return objectId;
  } else {
    emit ( LOGERR, tr("FilePLYPlugin::loadObject(): Tried loading with unknown forced data type"));
  }

  return -1;
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
    PLYHeader header = {0,false,0,0,0,0,0,0,std::vector<PPair>(),
                        0,0,0,0,"","",std::vector<PPair>()};
    
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
            
            // Update viewport
            PluginFunctions::viewAll();

            return objectId;
            
        } else {
            // Load as polymesh
            objectId = loadPolyMeshObject(_filename, header);
            
            PolyMeshObject* object(0);
            if(PluginFunctions::getObject( objectId, object )) {
                
                object->show();
                emit openedFile( objectId );
            }

            // Update viewport
            PluginFunctions::viewAll();

            return objectId;
        }
        
    } else if (triMeshControl == TYPEASK) {      

        // If Ask is selected -> show dialog
        QMessageBox msgBox;
        QPushButton *detectButton = msgBox.addButton(tr("Auto-Detect"), QMessageBox::ActionRole);
//         QPushButton *triButton    = msgBox.addButton(tr("Open as triangle mesh"), QMessageBox::ActionRole);
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

            // Update viewport
            PluginFunctions::viewAll();

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
        
        // Update viewport
        PluginFunctions::viewAll();

        return objectId;
    } else {
        // If always open as TriMesh is selected
        
        objectId = loadTriMeshObject(_filename, header);
        
        TriMeshObject* object(0);
        if(PluginFunctions::getObject( objectId, object )) {
            
            object->show();
            emit openedFile( objectId );
        }
        
        // Update viewport
        PluginFunctions::viewAll();

        return objectId;
    }
    
    // Load object as triangle mesh
    
    objectId = loadTriMeshObject(_filename, header);
    
    TriMeshObject* object(0);
    if(PluginFunctions::getObject( objectId, object )) {
        
        object->show();
        emit openedFile( objectId );

        // Update viewport
        PluginFunctions::viewAll();
    }
    
    return objectId;
};

//-----------------------------------------------------------------------------------------------------

/// load a triangle-mesh with given filename
int FilePLYPlugin::loadTriMeshObject(QString _filename, const PLYHeader& _header){

    int id = -1;
    emit addEmptyObject(DATA_TRIANGLE_MESH, id);
    
    TriMeshObject* object(0);
    if(PluginFunctions::getObject( id, object)) {
        
        if ( PluginFunctions::objectCount() == 1 )
            object->target(true);
        
        object->setFromFileName(_filename);
        
        // Get mesh
        TriMesh* mesh = object->mesh();

        // Reserve Memory to speed up loading
        // Euler formula to calculate number of edges (approximation)
        mesh->reserve(_header.numVertices,(_header.numVertices+_header.numFaces)/2,_header.numFaces);

        if(!_header.binary) {
            // Read ASCII file
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
        
        // Switch to point mode if mesh does not contain one single face
        if(_header.numFaces == 0)
            PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::POINTS);
        
        object->update();

        backupTextureCoordinates(*mesh);

        return object->id();
        
    } else {
        emit log(LOGERR,"Error : Could not create new triangle mesh object.");
        return -1;
    }
}

//-----------------------------------------------------------------------------------------------------

/// load a poly-mesh with given filename
int FilePLYPlugin::loadPolyMeshObject(QString _filename, const PLYHeader& _header){

    int id = -1;
    emit addEmptyObject(DATA_POLY_MESH, id);
    
    PolyMeshObject* object(0);
    if(PluginFunctions::getObject(id, object)) {
        
        if (PluginFunctions::objectCount() == 1 )
            object->target(true);
        
        object->setFromFileName(_filename);
        
        // Get mesh
        PolyMesh* mesh = object->mesh();

        // Reserve Memory to speed up loading
        mesh->reserve(_header.numVertices,_header.numVertices * 4,_header.numFaces);
    
        if(!_header.binary) {
            // Read ASCII file
            if(!readMeshFileAscii(_filename, mesh, _header)) {
                emit log(LOGERR, "Error while reading ascii PLY file!");
                emit deleteObject(id);
                return -1;
            } else {
                emit log(LOGINFO, tr("Successfully loaded file ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
            }
        } else {
            // Read binary file
            if(!readMeshFileBinary(_filename, mesh, _header)) {
                emit log(LOGERR, "Error while reading binary PLY file!");
                emit deleteObject(id);
                return -1;
            } else {
                emit log(LOGINFO, tr("Successfully loaded file ") + object->path() + OpenFlipper::Options::dirSeparator() + object->name() );
            }
        }
        
        // Switch to point mode if mesh does not contain one single face
        if(_header.numFaces == 0)
            PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::POINTS);
        
        object->update();

        backupTextureCoordinates(*mesh);

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
    
    object->setFromFileName(_filename);
    
    bool gui = OpenFlipper::Options::gui() && (saveBinary_ != 0) /*buttons initialized?*/;
    bool binary = ((gui && saveBinary_->isChecked()) ||
                    OpenFlipperSettings().value("FilePLY/Save/Binary",false).toBool());
    
    if ( object->dataType(DATA_POLY_MESH) ) {
        
        PolyMeshObject* polyObj = dynamic_cast<PolyMeshObject*>(object);
        
        if(binary) {
            if(!writeMeshFileBinary(_filename, polyObj->mesh())) {
                emit log(LOGERR, tr("Unable to save ") + _filename);
                return false;
            } else {
                emit log(LOGINFO, tr("Saved object to ") + _filename );
            }
        } else {
            if(!writeMeshFileAscii(_filename, polyObj->mesh())) {
                emit log(LOGERR, tr("Unable to save ") + _filename);
                return false;
            } else {
                emit log(LOGINFO, tr("Saved object to ") + _filename );
            }
        }
        
    } else if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
        
        TriMeshObject* triObj = dynamic_cast<TriMeshObject*>(object);
        
        if(binary) {
            if(!writeMeshFileBinary(_filename, triObj->mesh())) {
                emit log(LOGERR, tr("Unable to save ") + _filename);
                return false;
            } else {
                emit log(LOGINFO, tr("Saved object to ") + _filename );
            }
        } else {
            if(!writeMeshFileAscii(_filename, triObj->mesh())) {
                emit log(LOGERR, tr("Unable to save ") + _filename);
                return false;
            } else {
                emit log(LOGINFO, tr("Saved object to ") + _filename );
            }
        }
    }
    
    return true;
}

//-----------------------------------------------------------------------------------------------------

QWidget* FilePLYPlugin::saveOptionsWidget(QString /*_currentFilter*/) {
    
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
