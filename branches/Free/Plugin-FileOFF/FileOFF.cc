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

#include "FileOFF.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>
#include <OpenMesh/Core/Utils/color_cast.hh>

#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>

// Defines for the type handling drop down box
#define TYPEAUTODETECT 0
#define TYPEASK        1
#define TYPEPOLY       2
#define TYPETRIANGLE   3

/// Constructor
FileOFFPlugin::FileOFFPlugin()
: loadOptions_(0),
  saveOptions_(0),
  saveBinary_(0),
  saveVertexColor_(0),
  saveFaceColor_(0),
  saveAlpha_(0),
  saveNormals_(0),
  saveTexCoords_(0),
  saveDefaultButton_(0),
  triMeshHandling_(0),
  loadVertexColor_(0),
  loadFaceColor_(0),
  loadAlpha_(0),
  loadNormals_(0),
  loadTexCoords_(0),
  loadCheckManifold_(0),
  loadDefaultButton_(0),
  userReadOptions_(0),
  userWriteOptions_(0),
  forceTriangleMesh_(false),
  forcePolyMesh_(false),
  readColorComp_(false) {
}

//-----------------------------------------------------------------------------------------------------

void FileOFFPlugin::initializePlugin() {
    
    // Initialize standard options that can then be changed in the file dialogs
    if(OpenFlipperSettings().value("FileOff/Load/VertexColor",true).toBool())
        userReadOptions_ |= OFFImporter::VERTEXCOLOR;
    if(OpenFlipperSettings().value("FileOff/Load/FaceColor",true).toBool())
        userReadOptions_ |= OFFImporter::FACECOLOR;
    if(OpenFlipperSettings().value("FileOff/Load/Alpha",true).toBool())
        userReadOptions_ |= OFFImporter::COLORALPHA;
    if(OpenFlipperSettings().value("FileOff/Load/Normal",true).toBool())
        userReadOptions_ |= OFFImporter::VERTEXNORMAL;
    if(OpenFlipperSettings().value("FileOff/Load/TexCoords",true).toBool())
        userReadOptions_ |= OFFImporter::VERTEXTEXCOORDS;
    
    if(OpenFlipperSettings().value("FileOff/Save/Binary",true).toBool())
        userWriteOptions_ |= OFFImporter::BINARY;
    if(OpenFlipperSettings().value("FileOff/Save/VertexColor",true).toBool())
        userWriteOptions_ |= OFFImporter::VERTEXCOLOR;
    if(OpenFlipperSettings().value("FileOff/Save/FaceColor",true).toBool())
        userWriteOptions_ |= OFFImporter::FACECOLOR;
    if(OpenFlipperSettings().value("FileOff/Save/Alpha",true).toBool())
        userWriteOptions_ |= OFFImporter::COLORALPHA;
    if(OpenFlipperSettings().value("FileOff/Save/Normal",true).toBool())
        userWriteOptions_ |= OFFImporter::VERTEXNORMAL;
    if(OpenFlipperSettings().value("FileOff/Save/TexCoords",true).toBool())
        userWriteOptions_ |= OFFImporter::VERTEXTEXCOORDS;
    
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

DataType FileOFFPlugin::supportedType() {
    DataType type = DATA_POLY_MESH | DATA_TRIANGLE_MESH;
    return type;
}

//-----------------------------------------------------------------------------------------------------

void FileOFFPlugin::trimString( std::string& _string) {
    // Trim Both leading and trailing spaces
    
    size_t start = _string.find_first_not_of(" \t\r\n");
    size_t end   = _string.find_last_not_of(" \t\r\n");
    
    if(( std::string::npos == start ) || ( std::string::npos == end))
        _string = "";
    else
        _string = _string.substr( start, end-start+1 );
}

//-----------------------------------------------------------------------------------------------------

bool FileOFFPlugin::getCleanLine( std::istream& ifs , std::string& _string, bool _skipEmptyLines) {

  // while we are not at the end of the file
  while (true) {

    // get the current line:
    std::getline(ifs,_string);

    // Remove whitespace at beginning and end
    trimString(_string);

    // Check if string is not empty ( otherwise we continue
    if ( _string.size() != 0  ) {

      // Check if string is a comment ( starting with # )
      if ( _string[0] != '#') {
        return true;
      }

    } else {
      if ( !_skipEmptyLines )
        return true;
    }

    if ( ifs.eof() ) {
      std::cerr << "End of file reached while searching for input!" << std::endl;
      return false;
    }

  }

  return false;

}

//-----------------------------------------------------------------------------------------------------

void FileOFFPlugin::updateUserOptions() {
    
    // If the options dialog has not been initialized, keep
    // the initial values
    
    if( OpenFlipper::Options::nogui() )
        return;
    
    // Load options
    if(loadVertexColor_) {
        if(loadVertexColor_->isChecked()) userReadOptions_ |= OFFImporter::VERTEXCOLOR;
        else { if(userReadOptions_ & OFFImporter::VERTEXCOLOR) userReadOptions_ -= OFFImporter::VERTEXCOLOR; }
    }
    if(loadFaceColor_) {
        if(loadFaceColor_->isChecked()) userReadOptions_ |= OFFImporter::FACECOLOR;
        else { if(userReadOptions_ & OFFImporter::FACECOLOR) userReadOptions_ -= OFFImporter::FACECOLOR; }
    }
    if(loadAlpha_) {
        if(loadAlpha_->isChecked()) userReadOptions_ |= OFFImporter::COLORALPHA;
        else { if(userReadOptions_ & OFFImporter::COLORALPHA) userReadOptions_ -= OFFImporter::COLORALPHA; }
    }
    if(loadNormals_) {
        if(loadNormals_->isChecked()) userReadOptions_ |= OFFImporter::VERTEXNORMAL;
        else { if(userReadOptions_ & OFFImporter::VERTEXNORMAL) userReadOptions_ -= OFFImporter::VERTEXNORMAL; }
    }
    if(loadTexCoords_) {
        if(loadTexCoords_->isChecked()) userReadOptions_ |= OFFImporter::VERTEXTEXCOORDS;
        else { if(userReadOptions_ & OFFImporter::VERTEXTEXCOORDS) userReadOptions_ -= OFFImporter::VERTEXTEXCOORDS; }
    }
    
    // Save options
    if(saveBinary_) {
        if(saveBinary_->isChecked()) userWriteOptions_ |= OFFImporter::BINARY;
        else { if(userWriteOptions_ & OFFImporter::BINARY) userWriteOptions_ -= OFFImporter::BINARY; }
    }
    if(saveVertexColor_) {
        if(saveVertexColor_->isChecked()) userWriteOptions_ |= OFFImporter::VERTEXCOLOR;
        else { if(userWriteOptions_ & OFFImporter::VERTEXCOLOR) userWriteOptions_ -= OFFImporter::VERTEXCOLOR; }
    }
    if(saveFaceColor_) {
        if(saveFaceColor_->isChecked()) userWriteOptions_ |= OFFImporter::FACECOLOR;
        else { if(userWriteOptions_ & OFFImporter::FACECOLOR) userWriteOptions_ -= OFFImporter::FACECOLOR; }
    }
    if(saveAlpha_) {
        if(saveAlpha_->isChecked()) userWriteOptions_ |= OFFImporter::COLORALPHA;
        else { if(userWriteOptions_ & OFFImporter::COLORALPHA) userWriteOptions_ -= OFFImporter::COLORALPHA; }
    }
    if(saveNormals_) {
        if(saveNormals_->isChecked()) userWriteOptions_ |= OFFImporter::VERTEXNORMAL;
        else { if(userWriteOptions_ & OFFImporter::VERTEXNORMAL) userWriteOptions_ -= OFFImporter::VERTEXNORMAL; }
    }
    if(saveTexCoords_) {
        if(saveTexCoords_->isChecked()) userWriteOptions_ |= OFFImporter::VERTEXTEXCOORDS;
        else { if(userWriteOptions_ & OFFImporter::VERTEXTEXCOORDS) userWriteOptions_ -= OFFImporter::VERTEXTEXCOORDS; }
    }
}

//-----------------------------------------------------------------------------------------------------

bool FileOFFPlugin::readFileOptions(QString _filename, OFFImporter& _importer) {
    
    /* Constitution of an OFF-file
       ==================================================================
       [ST] [C] [N] [4][n]OFF [BINARY] # comment
       nV nF nE # number of vertices, faces and edges (edges are skipped)
       v[0] v[1] v[2] [n[0] n[1] n[2]] [c[0] c[1] c[1]] [t[0] t[0]]
       ...
       faceValence vIdx[0] ... vIdx[faceValence-1] colorspec
       ...
       ==================================================================
    */
    
    const unsigned int LINE_LEN = 4096;
    
    
    std::ifstream ifs(_filename.toUtf8());
    
    if ( (!ifs.is_open()) || (!ifs.good())) {
        
        emit log(LOGERR, tr("Error: Could not read file options of specified OFF-file! Aborting."));
        return false;
    }
    
    // read 1st line
    char line[LINE_LEN], *p;
    ifs.getline(line, LINE_LEN);
    p = line;
    
    int remainingChars = ifs.gcount();

    // check header: [ST][C][N][4][n]OFF BINARY
    while(remainingChars > 0) {
    
        if ( ( remainingChars > 1 ) && ( p[0] == 'S' && p[1] == 'T') ) {
            _importer.addOption(OFFImporter::VERTEXTEXCOORDS);
            p += 2;
            remainingChars -= 2;
        } else if ( ( remainingChars > 0 ) && ( p[0] == 'C') ) {
            _importer.addOption(OFFImporter::VERTEXCOLOR);
            ++p;
            --remainingChars;
        } else if ( ( remainingChars > 0 ) && ( p[0] == 'N') ) {
            _importer.addOption(OFFImporter::VERTEXNORMAL);
            ++p;
            --remainingChars;
        } else if ( ( remainingChars > 0 ) && (p[0] == '3' ) ) {
            ++p;
            --remainingChars;
        } else if ( ( remainingChars > 0 ) && (p[0] == '4' ) ) {
            /// \todo Implement extended coordinates
            std::cerr << "Error: Extended coordinates are currently not supported!" << std::endl;
            ifs.close();
            return false;
        } else if ( ( remainingChars > 0 ) && (p[0] == 'n' ) ) {
            /// \todo Implement any space dimension
            std::cerr << "Error: n-dimensional coordinates are currently not supported!" << std::endl;
            ifs.close();
            return false;
        } else if ( ( remainingChars >= 3 ) && (p[0] == 'O' && p[1] == 'F' && p[2] == 'F') ) {
            // Skip "OFF " (plus space):
            p += 4;
            remainingChars -= 4;
        } else if ( ( remainingChars >= 6 ) && ( strncmp(p, "BINARY", 6) == 0 ) ) {
            _importer.addOption(OFFImporter::BINARY);
            p += 6;
            remainingChars -= 6;
        } else if ( ( remainingChars > 0 ) && ( p[0] == '#' ) ) {
            // Skip the rest of the line since it's a comment
            remainingChars = 0;
        } else {
            // Skip unknown character or space
            ++p;
            --remainingChars;
        }
    }
    
    // Now extract data type by iterating over
    // the face valences
    
    unsigned int nV, nF, dummy_uint;
    unsigned int vertexCount = 0;
    unsigned int tmp_count = 0;
    std::string trash;
    std::string str;
    std::istringstream sstr;
       
    if(_importer.isBinary()) {
        // Parse BINARY file
        float dummy_f;
        
        // + #Vertices, #Faces, #Edges
        readValue(ifs, nV);
        readValue(ifs, nF);
        readValue(ifs, dummy_uint);
        
        for (uint i=0; i<nV && !ifs.eof(); ++i) {
            // Skip vertices
            for(int i = 0; i < 3; ++i) readValue(ifs, dummy_f);
            
            if ( _importer.hasVertexNormals() ) {
                for(int i = 0; i < 3; ++i) readValue(ifs, dummy_f);
            }
            
            if ( _importer.hasVertexColors() ) {
                for(int i = 0; i < 3; ++i) readValue(ifs, dummy_f);
            }
            
            if ( _importer.hasTextureCoords() ) {
                for(int i = 0; i < 2; ++i) readValue(ifs, dummy_f);
            }
        }
        for (uint i=0; i<nF; ++i) {
            // Get valence of current face
            readValue(ifs, tmp_count);

			if (ifs.eof())
			  break;

            if(tmp_count > vertexCount) vertexCount = tmp_count;
            
            // Skip the rest
            
            // Vertex indices
            for(uint i = 0; i < tmp_count; ++i) readValue(ifs, dummy_uint);
            
            // Get number of color components
            readValue(ifs, tmp_count);
            
            if(!_importer.hasFaceColors() && tmp_count > 0) {
                _importer.addOption(OFFImporter::FACECOLOR);
            }
            
            // Face color
            for (uint i = 0; i < tmp_count; ++i) {
                readValue(ifs, dummy_f);
            }
        }
        
    } else {
        // Parse ASCII file
        
        // Get whole line since there could be comments in it
        getCleanLine(ifs, str);
        sstr.str(str);
        
        // + #Vertices, #Faces, #Edges
        sstr >> nV;
        sstr >> nF;
        sstr >> dummy_uint;
        
        // Skip vertices
        for(unsigned int i = 0; i < nV; ++i) {
          getCleanLine(ifs, trash);
        }
        
        trash = "";
        
        // Count vertices per face
        for(unsigned int i = 0; i < nF; ++i) {
            sstr.clear();
            getCleanLine(ifs, trash);
            sstr.str(trash);
            
            sstr >> tmp_count;
            
            if(tmp_count > vertexCount) vertexCount = tmp_count;
            
            // Skip vertex indices
            for(uint i = 0; i < tmp_count; ++i) {
                sstr >> dummy_uint;
            }
            
            // Look if there's at least one face color specified
            // Note: Comments should not be here, so don't treat them
            if(!_importer.hasFaceColors()) {
                if(!sstr.eof()) {
                    _importer.addOption(OFFImporter::FACECOLOR);
                }
            }
        }
    }
    
    ifs.close();
    
    _importer.maxFaceValence(vertexCount);

    if(vertexCount == 3) {
        _importer.addOption(OFFImporter::TRIMESH);
        _importer.removeOption(OFFImporter::POLYMESH);
    } else if (vertexCount == 0 && nF != 0) {
        // Something went wrong
        return false;
    } else {
        _importer.addOption(OFFImporter::POLYMESH);
        _importer.removeOption(OFFImporter::TRIMESH);
    }
    
    return true;
}

//-----------------------------------------------------------------------------------------------------

bool FileOFFPlugin::readOFFFile(QString _filename, OFFImporter& _importer) {
    
  if ( !QFile(_filename).exists() ){
    emit log(LOGERR, tr("Unable to load OFF file. File not found!")); 
    return false;
  }
  
  if(!readFileOptions(_filename, _importer)) {
      return false;
  }

  // Let's see if the user has specified some options
  updateUserOptions();

  std::ifstream ifile(_filename.toUtf8(), (_importer.isBinary() ? std::ios::binary | std::ios::in
  : std::ios::in) );

  if (!ifile.is_open() || !ifile.good())
  {
      emit log(LOGERR, tr("Cannot open OFF file for reading!"));
      return false;
  }

  assert(ifile);

  int triMeshControl = TYPEAUTODETECT; // 0 == Auto-Detect

  if ( OpenFlipper::Options::gui() ){
      if ( triMeshHandling_ != 0 ){
          triMeshControl = triMeshHandling_->currentIndex();
      } else {
          triMeshControl = TYPEAUTODETECT;
      }
  }

  if ( forceTriangleMesh_ )
    triMeshControl = TYPETRIANGLE;

  if ( forcePolyMesh_ )
    triMeshControl = TYPEPOLY;
 
  DataType type = DATA_TRIANGLE_MESH;

  switch (triMeshControl) {
    case  TYPEAUTODETECT:
      // Auto-detect
      type = _importer.isTriangleMesh() ? DATA_TRIANGLE_MESH : DATA_POLY_MESH;
      break;
      
    case TYPEASK:
        if( !OpenFlipper::Options::nogui() ) {
            // Create message box
            QMessageBox msgBox;
            QPushButton *detectButton = msgBox.addButton(tr("Auto-Detect"), QMessageBox::ActionRole);
            QPushButton *triButton    = msgBox.addButton(tr("Open as triangle mesh"), QMessageBox::ActionRole);
            QPushButton *polyButton   = msgBox.addButton(tr("Open as poly mesh"), QMessageBox::ActionRole);
            
            msgBox.setWindowTitle( tr("Mesh types in file") );
            msgBox.setText( tr("You are about to open a file containing one or more mesh types. \n\n Which mesh type should be used?") );
            msgBox.setDefaultButton( detectButton );
            msgBox.exec();
      
            if (msgBox.clickedButton() == triButton)
                type = DATA_TRIANGLE_MESH;
            else if (msgBox.clickedButton() == polyButton)
                type = DATA_POLY_MESH;
            else
                type = _importer.isTriangleMesh() ? DATA_TRIANGLE_MESH : DATA_POLY_MESH;
            
        } else {
            // No gui mode
            type = _importer.isTriangleMesh() ? DATA_TRIANGLE_MESH : DATA_POLY_MESH;
        }
      
        break;
      
    case TYPEPOLY:
      // Always load as PolyMesh
      type = DATA_POLY_MESH;
      break;
      
    case TYPETRIANGLE:
      // Always load as TriangleMesh
      type = DATA_TRIANGLE_MESH;
      break;
      
    default:
      break;
      
  }
  
  return _importer.isBinary() ? parseBinary(ifile, _importer, type, _filename) : parseASCII(ifile, _importer, type, _filename);
}

//-----------------------------------------------------------------------------------------------------

bool FileOFFPlugin::parseASCII(std::istream& _in, OFFImporter& _importer, DataType _type, QString& _objectName) {
    
    unsigned int                idx;
    unsigned int                nV, nF, dummy;
    OpenMesh::Vec3f             v, n;
    OpenMesh::Vec2f             t;
    OpenMesh::Vec3i             c3;
    OpenMesh::Vec3f             c3f;
    OpenMesh::Vec4i             c4;
    OpenMesh::Vec4f             c4f;
    std::vector<VertexHandle>   vhandles;
    VertexHandle                vh;
    FaceHandle                  fh;
    
    int objectId = -1;
    emit addEmptyObject(_type, objectId);
    
    BaseObject* object(0);
    if(!PluginFunctions::getObject( objectId, object )) {
        emit log(LOGERR, tr("Could not create new object!"));
        return false;
    }
    
    // Set object's name to match file name
    QFileInfo f(_objectName);
    object->setName(f.fileName());
    
    // Set initial object
    _importer.addObject(object);
    
    std::string line;
    std::istringstream sstr;
    
    // read header line
    getCleanLine(_in, line);
    
    // + #Vertices, #Faces, #Edges
    // Note: We use a stringstream because there
    // could be comments in the line
    getCleanLine(_in, line);
    sstr.str(line);
    sstr >> nV;
    sstr >> nF;
    sstr >> dummy;
    
    // Reserve memory
    _importer.reserve(nV, nF * _importer.maxFaceValence() /*Upper bound*/, nF);

    // read vertices: coord [hcoord] [normal] [color] [texcoord]
    for (uint i=0; i<nV && !_in.eof(); ++i) {
        
        // Always read VERTEX
        _in >> v[0] >> v[1] >> v[2];
        
        vh = _importer.addVertex(v);
        
        // perhaps read NORMAL
        if ( _importer.hasVertexNormals() ){
            
            _in >> n[0] >> n[1] >> n[2];
            
            if(userReadOptions_ & OFFImporter::VERTEXNORMAL) {
                int nid = _importer.addNormal(n);
                _importer.setNormal(vh, nid);
            }
        }
        
        sstr.clear();
        getCleanLine(_in, line, false);
        sstr.str(line);
        
        int colorType = getColorType(line, _importer.hasTextureCoords() );
        
        //perhaps read COLOR
        if ( _importer.hasVertexColors() ){
            
            std::string trash;
            
            switch (colorType){
              case 0 : break; //no color
              case 1 : sstr >> trash; break; //one int (isn't handled atm)
              case 2 : sstr >> trash; sstr >> trash; break; //corrupt format (ignore)
              // rgb int
              case 3 : sstr >> c3[0];  sstr >> c3[1];  sstr >> c3[2];
                if ( userReadOptions_ & OFFImporter::VERTEXCOLOR ) {
                  int cidx = _importer.addColor( OpenMesh::color_cast<PolyMesh::Color>( c3 ) );
                  _importer.setVertexColor(vh, cidx);
                }
                break;
              // rgba int
              case 4 : sstr >> c4[0];  sstr >> c4[1];  sstr >> c4[2]; sstr >> c4[3];
                if ( userReadOptions_ & OFFImporter::VERTEXCOLOR ) {
                  int cidx = _importer.addColor( OpenMesh::color_cast<PolyMesh::Color>( c4 ) );
                  _importer.setVertexColor(vh, cidx);
                  _importer.addOption(OFFImporter::COLORALPHA);
                }
                break;
              // rgb floats
              case 5 : sstr >> c3f[0];  sstr >> c3f[1];  sstr >> c3f[2];
                if ( userReadOptions_ & OFFImporter::VERTEXCOLOR ) {
                  int cidx = _importer.addColor( OpenMesh::color_cast<PolyMesh::Color>(c3f) );
                  _importer.setVertexColor(vh, cidx);
                }
                break;
              // rgba floats
              case 6 : sstr >> c4f[0];  sstr >> c4f[1];  sstr >> c4f[2]; sstr >> c4f[3];
                if ( userReadOptions_ & OFFImporter::VERTEXCOLOR ) {
                  int cidx = _importer.addColor( OpenMesh::color_cast<PolyMesh::Color>(c4f) );
                  _importer.setVertexColor(vh, cidx);
                  _importer.addOption(OFFImporter::COLORALPHA);
                }
                break;

              default:
                std::cerr << "Error in file format (colorType = " << colorType << ")\n";
                break;
            }
        }
        
        //perhaps read TEXTURE COORDS
        if ( _importer.hasTextureCoords() ){
            sstr >> t[0]; sstr >> t[1];
            if ( userReadOptions_ & OFFImporter::VERTEXTEXCOORDS ) {
                int tcidx = _importer.addTexCoord(t);
                _importer.setVertexTexCoord(vh, tcidx);
            }
        }
    }
    
    // faces
    // #N <v1> <v2> .. <v(n-1)> [color spec]
    for (uint i=0; i<nF; ++i)
    {
        // nV = number of Vertices for current face
        _in >> nV;
        
        // If number of faces < 3, we have a degenerated face
        // which we don't allow and thus skip
        if (nV < 3) {
          // Read the rest of the line and dump it
          getCleanLine(_in, line, false);
          // Proceed reading
          continue;
        }
        
        vhandles.clear();
        for (uint i=0; i<nV; ++i) {
            _in >> idx;
            vhandles.push_back(VertexHandle(idx));
        }
        
        bool checkManifold = true;
        if(!OpenFlipper::Options::nogui() && loadCheckManifold_ != 0) {
            checkManifold = loadCheckManifold_->isChecked();
        }
        
        // Check for degenerate faces if specified in gui
        if(checkManifold) {
            if(checkDegenerateFace(vhandles)) {
                fh = _importer.addFace(vhandles);
            } else {
                continue;
            }
        } else {
            fh = _importer.addFace(vhandles);
        }
        
        //perhaps read face COLOR
        if ( _importer.hasFaceColors() ){
            
            //take the rest of the line and check how colors are defined
            sstr.clear();
            getCleanLine(_in, line, false);
            sstr.str(line);
            
            int colorType = getColorType(line, false);
            
            std::string trash;
            
            switch (colorType){
                case 0 : break; //no color
                case 1 : sstr >> trash; break; //one int (isn't handled atm)
                case 2 : sstr >> trash; sstr >> trash; break; //corrupt format (ignore)
                    // rgb int
                case 3 : sstr >> c3[0];  sstr >> c3[1];  sstr >> c3[2];
                if ( userReadOptions_ & OFFImporter::FACECOLOR ) {
                    int cidx = _importer.addColor( OpenMesh::color_cast<PolyMesh::Color>( c3 ) );
                    _importer.setFaceColor(fh, cidx);
                }
                break;
                // rgba int
                case 4 : sstr >> c4[0];  sstr >> c4[1];  sstr >> c4[2]; sstr >> c4[3];
                if ( userReadOptions_ & OFFImporter::FACECOLOR ) {
                    int cidx = _importer.addColor( OpenMesh::color_cast<PolyMesh::Color>( c4 ) );
                    _importer.setFaceColor(fh, cidx);
                    _importer.addOption(OFFImporter::COLORALPHA);
                }
                break;
                // rgb floats
                case 5 : sstr >> c3f[0];  sstr >> c3f[1];  sstr >> c3f[2];
                if ( userReadOptions_ & OFFImporter::FACECOLOR ) {
                    int cidx = _importer.addColor( OpenMesh::color_cast<PolyMesh::Color>(c3f) );
                    _importer.setFaceColor(fh, cidx);
                }
                break;
                // rgba floats
                case 6 : sstr >> c4f[0];  sstr >> c4f[1];  sstr >> c4f[2]; sstr >> c4f[3];
                if ( userReadOptions_ & OFFImporter::FACECOLOR ) {
                    int cidx = _importer.addColor( OpenMesh::color_cast<PolyMesh::Color>(c4f) );
                    _importer.setFaceColor(fh, cidx);
                    _importer.addOption(OFFImporter::COLORALPHA);
                }
                break;
                
                default:
                    std::cerr << "Error in file format (colorType = " << colorType << ")\n";
                    break;
            }
        }
    }
    
    // File was successfully parsed.
    return true;
}

//-----------------------------------------------------------------------------------------------------

bool FileOFFPlugin::checkDegenerateFace(const std::vector<VertexHandle>& _v) {
    
    bool check = true;
    int size = _v.size();
    // Check if at least two elements in the list have the same value
    for(int i = 0; i < size; ++i) {
        for(int j = i+1; j < size; ++j) {
            if(_v[i] == _v[j]) check = false;
        }
    }
    return check;
}

//-----------------------------------------------------------------------------------------------------

int FileOFFPlugin::getColorType(std::string& _line, bool _texCoordsAvailable) {
    /*
    0 : no Color
    1 : one int (e.g colormap index)
    2 : two items (error!)
    3 : 3 ints
    4 : 4 ints
    5 : 3 floats
    6 : 4 floats
    */
        
    // Check if we have any additional information here
    if ( _line.size() < 1 )
        return 0;
    
    //first remove spaces at start/end of the line
    trimString(_line);

    //count the remaining items in the line
    size_t found;
    int count = 0;
    
    found=_line.find_first_of(" ");
    while (found!=std::string::npos){
        count++;
        found=_line.find_first_of(" ",found+1);
    }
    
    if (!_line.empty()) count++;
    
    if (_texCoordsAvailable) count -= 2;
    
    if (count == 3 || count == 4){
        //get first item
        found = _line.find(" ");
        std::string c1 = _line.substr (0,found);
        
        if (c1.find(".") != std::string::npos){
   if (count == 3)
       count = 5;
   else
       count = 6;
   }
    }
    return count;
}

//-----------------------------------------------------------------------------------------------------

bool FileOFFPlugin::parseBinary(std::istream& _in, OFFImporter& _importer, DataType _type, QString& _objectName) {
    
    unsigned int                idx;
    unsigned int                nV, nF, dummy;
    float                       dummy_f;
    OpenMesh::Vec3f             v, n;
    OpenMesh::Vec4f             c;
    float                       alpha = 1.0f;
    OpenMesh::Vec2f             t;
    std::vector<VertexHandle>   vhandles;
    VertexHandle                vh;
    FaceHandle                  fh;
    
    int objectId = -1;
    emit addEmptyObject(_type, objectId);
        
    BaseObject* object(0);
    if(!PluginFunctions::getObject( objectId, object )) {
        emit log(LOGERR, tr("Could not create new object!"));
        return false;
    }
    
    // Set object's name to match file name
    QFileInfo f(_objectName);
    object->setName(f.fileName());
    
    // Set initial object
    _importer.addObject(object);
    
    // read header line
    std::string header;
    getCleanLine(_in,header);
    
    // + #Vertices, #Faces, #Edges
    readValue(_in, nV);
    readValue(_in, nF);
    readValue(_in, dummy);
    
    // Reserve memory
    _importer.reserve(nV, nF * _importer.maxFaceValence() /*Upper bound*/, nF);

    // read vertices: coord [hcoord] [normal] [color] [texcoord]
    for (uint i=0; i<nV && !_in.eof(); ++i)
    {
        // Always read Vertex
        readValue(_in, v[0]);
        readValue(_in, v[1]);
        readValue(_in, v[2]);
        
        vh = _importer.addVertex(v);
        
        if ( _importer.hasVertexNormals() ) {
            readValue(_in, n[0]);
            readValue(_in, n[1]);
            readValue(_in, n[2]);
            
            if ( userReadOptions_ & OFFImporter::VERTEXNORMAL ) {
                int nidx = _importer.addNormal(n);
                _importer.setNormal(vh, nidx);
            }
        }
        
        if ( _importer.hasVertexColors() ) {
            // Vertex colors are always without alpha
            readValue(_in, c[0]);
            readValue(_in, c[1]);
            readValue(_in, c[2]);
            c[3] = 1.0;
                
            if ( userReadOptions_ & OFFImporter::VERTEXCOLOR ) {
                int cidx = _importer.addColor( c );
                _importer.setVertexColor(vh, cidx);
            }
        }

        if ( _importer.hasTextureCoords() ) {
            readValue(_in, t[0]);
            readValue(_in, t[1]);
            
            if ( userReadOptions_ & OFFImporter::VERTEXTEXCOORDS ) {
                int tcidx = _importer.addTexCoord(t);
                _importer.setVertexTexCoord(vh, tcidx);
            }
        }
    }
    
    int pos = 0;
    int nB = 0;
    
    // faces
    // #N <v1> <v2> .. <v(n-1)> [color spec]
    for (uint i = 0; i<nF && !_in.eof(); ++i)
    {
        // Get bytes to be read from this point on
        if(i == 0) {
            pos = _in.tellg();
            _in.seekg(0, std::ios::end);
            nB = _in.tellg();
            nB -= pos;
            _in.seekg(pos);
            // nB now holds the total number of bytes to be read
        }
        
        readValue(_in, nV);
        
        // Now that we have the initial face valence
        // we check, if there could possibly be colors
        // after the face specs by checking if
        // the bytes to be read from this point on (nB)
        // equal (nF + nF*nV)*4 (each line of [nV V_1 ... V..nV]).
        // If not, we have more bytes to be read than
        // actual face definitions.
        // So if we have at least nF additional bytes
        // we can read the number of color components after each face
        // definition.
        if(i == 0) {
            // Always make sure that we only deal with
            // integers and floats/doubles
            if(nB % 4 == 0) {
                // Cut down number of bytes to number
                // of elements to be read
                nB /= 4;
                
                nB -= nF + nF*nV;
                
                if(nB <= 0) {
                    // We don't have additional color components
                    // Case nB < 0: Face valence is not constant
                    // throughout the mesh
                    readColorComp_ = false;
                } else {
                    // Not enough additional elements to read
                    // This should actually never happen...
                    // or nB >= nF -> perform extended
                    // face color component test
                    readColorComp_ = extendedFaceColorTest(_in, nV, nF, nB);
                }
            }
        }
        
        // Check if the face has at least valence 3
        // if not, skip the current face
        if (nV < 3) {
            // Read in following vertex indices and dump them
            for (uint j = 0; j < nV; ++j) {
                readValue(_in, dummy);
            }
            // Read in color components if available
            // and dump them
            if (readColorComp_) {
                // Number of color components
                readValue(_in, nV);
                for (uint j = 0; j < nV; ++j) {
                    readValue(_in, dummy_f);
                }
            }
            // Proceed reading
            continue;
        }
        
        // Read vertex indices of current face
        vhandles.clear();
        for (uint j = 0; j < nV; ++j) {
            readValue(_in, idx);
            vhandles.push_back(VertexHandle(idx));
        }
                
        fh = _importer.addFace(vhandles);

        if ( !readColorComp_ ) {
            // Some binary files that were created via an OFF writer
            // that doesn't comply with the OFF specification
            // don't specify the number of color components before
            // the face specs.
            nV = 0;
        } else {
            // nV now holds the number of color components
            readValue(_in, nV);
        }
        
        // valid face color:
        if ( nV == 3 || nV == 4 ) {
            
            // Read standard rgb color
            for(uint k = 0; k < 3; ++k) {
                readValue(_in, c[k]);
                --nV;
            }
            
            // Color has additional alpha value
            if(nV == 1) {
                readValue(_in, alpha);
            }
            
            if(userReadOptions_ & OFFImporter::FACECOLOR) {
                if(userReadOptions_ & OFFImporter::COLORALPHA) {
                    int cidx = _importer.addColor(OpenMesh::Vec4f(c[0], c[1], c[2], alpha));
                    _importer.setFaceColor( fh,  cidx );
                    _importer.addOption(OFFImporter::COLORALPHA);
                } else {
                    int cidx = _importer.addColor(OpenMesh::color_cast<OpenMesh::Vec4f>(c));
                    _importer.setFaceColor( fh,  cidx );
                }
            }
        } else {
            // Skip face colors since they are not in a supported format
            for(uint i = 0; i < nV; ++i) {
                readValue(_in, dummy_f);
            }
        }
    }
    
    // File was successfully parsed.
    return true;
}

//-----------------------------------------------------------------------------------------------------

bool FileOFFPlugin::extendedFaceColorTest(std::istream& _in, uint _nV, uint _nF, int _nB) const {
    
    // Perform the extended and even more reliable color
    // component test. Read an integer n (starting with the face
    // valence) and skip n*4 bytes. Repeat this nF times.
    // After this we have two cases:
    //
    // Case 1: The file contains face color components
    // and we interpreted the number of face components as face
    // valence which results in a bunch of bytes that still are to be read
    // after nF cycles.
    //
    // Case 2: The mesh has varying face valences and has
    // therefor been wrongly detected as containing face color components.
    // If this has happened, the following test will result in
    // no bytes to be left for reading after nF reading cycles.

    uint nV = _nV;
    uint dummy = 0;

    // Get current file pointer
    int pos = _in.tellg();
    
    for(uint k = 0; k < _nF; ++k) {
        // Remember: The first nV has already been read
        if(k != 0)
            readValue(_in, nV);
        
        // Skip the following nV values
        for(uint z = 0; z < nV; ++z) {
            readValue(_in, dummy);
        }
    }
    
    // Get position after all the reading has been done
    int currPos = _in.tellg();
    
    // Reset read pointer to where we were
    _in.seekg(pos);
    
    if(_nB - currPos == 0) {
        // No additional face colors have been specified
        return false;
    }
    
    // We actually have face colors
    return true;
}

//-----------------------------------------------------------------------------------------------------

int FileOFFPlugin::loadObject(QString _filename) {

    OFFImporter importer;
    
    // Parse file
    readOFFFile( _filename, importer );
    
    // Finish importing
    importer.finish();
    
    BaseObject* object = importer.getObject();
    
    if(!object){
      
      forceTriangleMesh_ = false;
      forcePolyMesh_     = false;
      
      return -1;
    }
    
    object->setFromFileName(_filename);

    // Handle new PolyMeshes
    PolyMeshObject* polyMeshObj = dynamic_cast< PolyMeshObject* > (object);
    
    if ( polyMeshObj ){

      if ( !importer.hasVertexNormals() )
        polyMeshObj->mesh()->update_normals();
      else
        polyMeshObj->mesh()->update_face_normals();

      backupTextureCoordinates(*(polyMeshObj->mesh()));

      polyMeshObj->update();
      polyMeshObj->show();
    }
    
    // Handle new TriMeshes
    TriMeshObject* triMeshObj = dynamic_cast< TriMeshObject* > (object);
    
    if ( triMeshObj ){

      if ( !importer.hasVertexNormals() || (userReadOptions_ & OFFImporter::FORCE_NONORMALS) )
        triMeshObj->mesh()->update_normals();
      else
        triMeshObj->mesh()->update_face_normals();

      backupTextureCoordinates(*(triMeshObj->mesh()));

      triMeshObj->update();
      triMeshObj->show();
    }
    
    //general stuff
    emit openedFile( object->id() );
    
    // Update viewport
    PluginFunctions::viewAll();
    
    forceTriangleMesh_ = false;
    forcePolyMesh_     = false;
    
    return object->id();
}

//-----------------------------------------------------------------------------------------------------

int FileOFFPlugin::loadObject(QString _filename, DataType _type) {
  
  forceTriangleMesh_ = false;
  forcePolyMesh_     = false;
  
  if ( _type == DATA_TRIANGLE_MESH )
    forceTriangleMesh_ = true;
  else if ( _type == DATA_POLY_MESH )
    forcePolyMesh_ = true;

  return loadObject(_filename);
  
}

//-----------------------------------------------------------------------------------------------------

bool FileOFFPlugin::saveObject(int _id, QString _filename)
{
    BaseObjectData* object;
    PluginFunctions::getObject(_id,object);
    
    std::string filename = std::string( _filename.toUtf8() );
    
    std::fstream ofs( filename.c_str(), std::ios_base::out );
    
    if (!ofs) {
        
        emit log(LOGERR, tr("saveObject : Cannot not open file %1 for writing!").arg(_filename) );
        return false;
    }
    
    // Get user specified options
    updateUserOptions();
    
    if ( object->dataType( DATA_POLY_MESH ) ) {
        
        object->setFromFileName(_filename);
        
        PolyMeshObject* polyObj = dynamic_cast<PolyMeshObject* >( object );
        
        if (writeMesh(ofs, *polyObj->mesh())){
            emit log(LOGINFO, tr("Saved object to ") + _filename );
            ofs.close();
            return true;
        }else{
            emit log(LOGERR, tr("Unable to save ") + _filename);
            ofs.close();
            return false;
        }
    } else if ( object->dataType( DATA_TRIANGLE_MESH ) ) {
        
        object->setFromFileName(_filename);
        
        TriMeshObject* triObj = dynamic_cast<TriMeshObject* >( object );
                
        if (writeMesh(ofs, *triObj->mesh())) {
            emit log(LOGINFO, tr("Saved object to ") + _filename );
            ofs.close();
            return true;
        } else {
            emit log(LOGERR, tr("Unable to save ") + _filename );
            ofs.close();
            return false;
        }
    } else {
        emit log(LOGERR, tr("Unable to save (object is not a compatible mesh type)"));
        ofs.close();
        return false;
    }
}

//-----------------------------------------------------------------------------------------------------

template <class MeshT>
void FileOFFPlugin::backupTextureCoordinates(MeshT& _mesh) {

    // Create a backup of the original per Vertex texture Coordinates
    if (_mesh.has_vertex_texcoords2D()) {

      OpenMesh::VPropHandleT< typename MeshT::TexCoord2D > oldVertexCoords;
      if (!_mesh.get_property_handle(oldVertexCoords, "Original Per Vertex Texture Coords"))
        _mesh.add_property(oldVertexCoords, "Original Per Vertex Texture Coords");

      for (typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it != _mesh.vertices_end(); ++v_it)
        _mesh.property(oldVertexCoords, v_it) =  _mesh.texcoord2D(v_it);

    }

    // Create a backup of the original per Face texture Coordinates
    if (_mesh.has_halfedge_texcoords2D()) {

      OpenMesh::HPropHandleT< typename MeshT::TexCoord2D > oldHalfedgeCoords;
      if (!_mesh.get_property_handle(oldHalfedgeCoords,"Original Per Face Texture Coords"))
        _mesh.add_property(oldHalfedgeCoords,"Original Per Face Texture Coords");

      for (typename MeshT::HalfedgeIter he_it = _mesh.halfedges_begin(); he_it != _mesh.halfedges_end(); ++he_it)
        _mesh.property(oldHalfedgeCoords, he_it) =  _mesh.texcoord2D(he_it);

    }
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileOFFPlugin::saveOptionsWidget(QString /*_currentFilter*/) {
    
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
        
        saveBinary_->setChecked( OpenFlipperSettings().value("FileOff/Save/Binary",false).toBool() );
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
        
        triMeshHandling_ = new QComboBox();
        triMeshHandling_->addItem( tr("Auto-Detect") );
        triMeshHandling_->addItem( tr("Ask") );
        triMeshHandling_->addItem( tr("Always open as PolyMesh") );
        triMeshHandling_->addItem( tr("Always open as TriangleMesh") );
        
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
        
        loadCheckManifold_ = new QCheckBox("Check for manifold configurations");
        layout->addWidget(loadCheckManifold_);
 
        loadDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(loadDefaultButton_);
        
        loadOptions_->setLayout(layout);
        
        connect(loadDefaultButton_, SIGNAL(clicked()), this, SLOT(slotLoadDefault()));
        
        triMeshHandling_->setCurrentIndex(OpenFlipperSettings().value("FileOff/Load/TriMeshHandling",TYPEAUTODETECT ).toInt() );
        
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
  
  OpenFlipperSettings().setValue( "Core/File/UseLoadDefaults", true );
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

