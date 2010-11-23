#define FILEOFFPLUGIN_C

#include "FilePLY.hh"

template <class MeshT>
bool FilePLYPlugin::readMeshFileAscii(QString _filename, MeshT* _mesh, const PLYHeader& _header) {
    
    std::string line;
    std::istringstream sstr;
    
    std::string dString = "";
    int         dInt;
    uint        du, dv;
    double      dx, dy, dz;
    uchar       ducr, ducg, ducb;
    
    PolyMesh::VertexHandle currentVertex;
    std::vector<PolyMesh::VertexHandle> vertexIndices;
    
    // Parse file
    std::ifstream ifs(_filename.toUtf8());

    if (!ifs.is_open() | !ifs.good() | ifs.eof()) {
        
        emit log(LOGERR, tr("Error: Could not read PLY data! Aborting."));
        return false;
    }

    // Skip header
    while(!ifs.eof()) {
        sstr.clear();
        std::getline(ifs, line);
        sstr.str(line);
        sstr >> dString;
        if(dString == "end_header") break;
    }
    
    // Get desired properties
    bool nogui = OpenFlipper::Options::nogui();
    // If in no gui mode -> request as much as possible
    bool vNormals   = _header.hasVertexNormals && ((!nogui && loadVertexNormal_->isChecked()) || true);
    bool vColors    = _header.hasVertexColors && ((!nogui && loadVertexColor_->isChecked()) || true);
    bool vTexCoords = _header.hasVertexTexCoords && ((!nogui && loadVertexTexCoord_->isChecked()) || true);
    bool fNormals   = _header.hasFaceNormals && ((!nogui && loadFaceNormal_->isChecked()) || true);
    bool fColors    = _header.hasFaceColors && ((!nogui && loadFaceColor_->isChecked()) || true);
    
    // Request properties
    if(vNormals)
        _mesh->request_vertex_normals();
    if(vColors)
        _mesh->request_vertex_colors();
    if(vTexCoords)
        _mesh->request_vertex_texcoords2D();
    if(fNormals)
        _mesh->request_face_normals();
    if(fColors)
        _mesh->request_face_colors();
    
    unsigned int propIndex;
    bool next;
    
    // Parse vertices
    for(int i = 0; i < _header.numVertices; ++i) {
        
        // Get line
        sstr.clear();
        std::getline(ifs, line);
        sstr.str(line);
        
        propIndex = 0;
        next = false;
        
        while(!next) {
            
            if(propIndex >= _header.vOrder.size())  {
                next = true;
                break;
            }
            
            if(_header.vOrder[propIndex] == "xyz") {
                // Parse xyz-coordinates
                sstr >> dx >> dy >> dz;
                // Add vertex
                currentVertex = _mesh->add_vertex(typename MeshT::Point(dx, dy, dz));
                vertexIndices.push_back(currentVertex);
                // Go over to next property
                propIndex += 3;
            } else if (_header.vOrder[propIndex] == "n_xyz" && vNormals) {
                // Parse vertex normal coordinates
                sstr >> dx >> dy >> dz;
                // Set normal
                _mesh->set_normal(currentVertex, typename MeshT::Normal(dx, dy, dz));
                // Go over to next property
                propIndex +=3;
            } else if (_header.vOrder[propIndex] == "rgb" && vColors) {
                // Parse vertex colors
                sstr >> dx >> dy >> dz;
                // Set color
                _mesh->set_color(currentVertex, typename MeshT::Color(dx, dy, dz));
                // Go over to next property
                propIndex += 3;
            } else if (_header.vOrder[propIndex] == "a_rgb" && vColors) {
                sstr >> dx >> dy >> dz;
                /// \TODO: Make this set the ambient vertex color of overlaying material node
                // Go over to next property
                propIndex += 3;
            } else if (_header.vOrder[propIndex] == "d_rgb" && vColors) {
                sstr >> dx >> dy >> dz;
                // Set color
                _mesh->set_color(currentVertex, typename MeshT::Color(dx, dy, dz));
                // Go over to next property
                propIndex += 3;
            } else if (_header.vOrder[propIndex] == "s_rgb" && vColors) {
                sstr >> dx >> dy >> dz;
                /// \TODO: Make this set the specular vertex color of overlaying material node
                // Go over to next property
                propIndex += 3;
            } else if (_header.vOrder[propIndex] == "uv" && vTexCoords) {
                // Parse uv-texture coordinates
                sstr >> du >> dv;
                // Set texture coordinate
                _mesh->set_texcoord2D(currentVertex, typename MeshT::TexCoord2D(du, dv));
                // Go over to next property
                propIndex += 2;
            } else {
                // Nothing else to parse in this line
                next = true;
                break;
            }
        }
    }
    
    std::vector<typename MeshT::VertexHandle> vlist;
    int index;
    typename MeshT::FaceHandle currentFace;
    
    // Parse faces
    for(int i = 0; i < _header.numFaces; ++i) {
        
        // Get line
        sstr.clear();
        std::getline(ifs, line);
        sstr.str(line);
        
        // Get number of vertices per face
        sstr >> dInt;
        vlist.clear();
        for(int c = 0; c < dInt; ++c) {
            sstr >> index;
            vlist.push_back(vertexIndices[index]);
        }
        
        // Add face
        currentFace = _mesh->add_face(vlist);
        
        propIndex = 0;
        next = false;
        
        // Search for further props in same line
        while(!next || _header.fOrder.size() != 0) {
            
            if(propIndex >= _header.fOrder.size())  {
                next = true;
                break;
            }
            
            if (_header.fOrder[propIndex] == "n_xyz" && fNormals) {
                // Parse face normal coordinates
                sstr >> dx >> dy >> dz;
                // Set normal
                _mesh->set_normal(currentFace, typename MeshT::Normal(dx, dy, dz));
                // Go over to next property
                propIndex +=3;
            } else if (_header.fOrder[propIndex] == "rgb" && fColors) {
                // Parse face colors
                sstr >> dx >> dy >> dz;
                // Set color
                _mesh->set_color(currentFace, typename MeshT::Color(dx, dy, dz));
                // Go over to next property
                propIndex += 3;
            } else if (_header.fOrder[propIndex] == "a_rgb" && fColors) {
                sstr >> dx >> dy >> dz;
                /// \TODO: Make this set the ambient face color of overlaying material node
                // Go over to next property
                propIndex += 3;
            } else if (_header.fOrder[propIndex] == "d_rgb" && fColors) {
                sstr >> dx >> dy >> dz;
                // Set color
                _mesh->set_color(currentFace, typename MeshT::Color(dx, dy, dz));
                // Go over to next property
                propIndex += 3;
            } else if (_header.fOrder[propIndex] == "s_rgb" && fColors) {
                sstr >> dx >> dy >> dz;
                /// \TODO: Make this set the specular face color of overlaying material node
                // Go over to next property
                propIndex += 3;
            } else {
                // Nothing else to parse in this line
                next = true;
                break;
            }
        }
    }       
    
    // Update normals
    _mesh->update_normals();
    
    return true;
}

//===============================================================================================

template <class MeshT>
bool FilePLYPlugin::readMeshFileBinary(QString _filename, MeshT* _mesh, const PLYHeader& _header) {
    
    // Get desired properties
    bool nogui = OpenFlipper::Options::nogui();
    // If in no gui mode -> request as much as possible
    bool vNormals   = _header.hasVertexNormals && ((!nogui && loadVertexNormal_->isChecked()) || true);
    bool vColors    = _header.hasVertexColors && ((!nogui && loadVertexColor_->isChecked()) || true);
    bool vTexCoords = _header.hasVertexTexCoords && ((!nogui && loadVertexTexCoord_->isChecked()) || true);
    bool fNormals   = _header.hasFaceNormals && ((!nogui && loadFaceNormal_->isChecked()) || true);
    bool fColors    = _header.hasFaceColors && ((!nogui && loadFaceColor_->isChecked()) || true);
    
    OpenMesh::IO::Options opt = OpenMesh::IO::Options::Default;
    
    // Set binary
    opt += OpenMesh::IO::Options::Binary;
    
    if(vNormals)
        opt += OpenMesh::IO::Options::VertexNormal;
    if(vColors)
        opt += OpenMesh::IO::Options::VertexColor;
    if(vTexCoords)
        opt += OpenMesh::IO::Options::VertexTexCoord;
    if(fNormals)
        opt += OpenMesh::IO::Options::FaceNormal;
    if(fColors)
        opt += OpenMesh::IO::Options::FaceColor;
    
    std::string filename = std::string( _filename.toUtf8() );
        
    // load file
    /// \TODO: Replace by own routine (make plugin independant from OpenMesh::IO)
    if (!OpenMesh::IO::read_mesh(*_mesh, filename, opt)) {
        return false;
    }
    
    return true;
}