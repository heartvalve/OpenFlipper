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

#define FILEOFFPLUGIN_C

#include <OpenMesh/Core/Utils/color_cast.hh>

#include "FilePLY.hh"

#include <iostream>
#include <fstream>
#include <sstream>

template <class MeshT>
bool FilePLYPlugin::readMeshFileAscii(QString _filename, MeshT* _mesh, const PLYHeader& _header) {

    std::string line;
    std::istringstream sstr;

    std::string dString = "";
    int         dInt;
    uint        du, dv;
    double      dx, dy, dz;
    double      alpha;

    PolyMesh::VertexHandle currentVertex;
    std::vector<PolyMesh::VertexHandle> vertexIndices;

    // Parse file
    std::ifstream ifs(_filename.toUtf8());

    if (!ifs.is_open() || !ifs.good() || ifs.eof()) {

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
    bool gui = OpenFlipper::Options::gui() && (loadVertexNormal_ != 0) /*buttons initialized*/;
    // If in no gui mode -> request as much as possible
    bool vNormals   =  ((gui && loadVertexNormal_->isChecked()) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Load/Normals",true).toBool()));
    bool vColors    =  ((gui && loadVertexColor_->isChecked()) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Load/VertexColor",true).toBool()));
    bool vTexCoords =  ((gui && loadVertexTexCoord_->isChecked()) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Load/TexCoords",true).toBool()));
    bool fNormals   =  ((gui && loadFaceNormal_->isChecked()) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Load/FaceNormal",true).toBool()));
    bool fColors    =  ((gui && loadFaceColor_->isChecked()) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Load/FaceColor",true).toBool()));

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

    // Reserve enough space for all vertices to avoid resizing and save time
    vertexIndices.reserve(_header.numVertices);

    // Parse vertices
    for(int i = 0; i < _header.numVertices; ++i) {

        // Get line
        sstr.clear();
        std::getline(ifs, line);
        sstr.str(line);

        propIndex = 0;
        next = false;

        while(!next) {

            if(propIndex >= _header.vProps.size())  {
                next = true;
                break;
            }

            if(_header.vProps[propIndex].first == "xyz") {
                // Parse xyz-coordinates
                sstr >> dx >> dy >> dz;
                // Add vertex
                currentVertex = _mesh->add_vertex(typename MeshT::Point(dx, dy, dz));
                vertexIndices.push_back(currentVertex);
                // Go over to next property
                propIndex += 3;
            } else if (_header.vProps[propIndex].first == "n_xyz" && vNormals) {
                // Parse vertex normal coordinates
                sstr >> dx >> dy >> dz;
                // Set normal
                if ( vNormals ) _mesh->set_normal(currentVertex, typename MeshT::Normal(dx, dy, dz));
                // Go over to next property
                propIndex +=3;
            } else if (_header.vProps[propIndex].first == "rgb" && vColors) {

                bool divide = false;
                if (getTypeSize(_header.vProps[propIndex].second) == 1 ||
                    getTypeSize(_header.vProps[propIndex].second) == 4)
                  divide = true;

                // Parse vertex colors
                sstr >> dx >> dy >> dz;

                if (divide) {
                  dx /= 255.0;
                  dy /= 255.0;
                  dz /= 255.0;
                }

                if(_header.hasVertexColorAlpha) {
                  sstr >> alpha;

                  if (divide)
                    alpha /= 255.0;
                    propIndex++;
                } else {
                  alpha = 1.0;
                }

                // Set color
                if ( vColors ) _mesh->set_color(currentVertex, typename MeshT::Color(dx, dy, dz,alpha));

                // Go over to next property
                propIndex += 3;

            } else if (_header.vProps[propIndex].first == "a_rgb" && vColors) {
                sstr >> dx >> dy >> dz;
                /// \todo Make this set the ambient vertex color of overlaying material node
                // Go over to next property
                propIndex += 3;
            } else if (_header.vProps[propIndex].first == "d_rgb" && vColors) {

                bool divide = false;
                if (getTypeSize(_header.vProps[propIndex].second) == 1 ||
                    getTypeSize(_header.vProps[propIndex].second) == 4)
                  divide = true;

                // Parse vertex colors
                sstr >> dx >> dy >> dz;

                if (divide) {
                  dx /= 255.0;
                  dy /= 255.0;
                  dz /= 255.0;
                }

                // Set color
                if ( vColors ) _mesh->set_color(currentVertex, typename MeshT::Color(dx, dy, dz, 1.0));

                // Go over to next property
                propIndex += 3;
            } else if (_header.vProps[propIndex].first == "s_rgb" && vColors) {
                sstr >> dx >> dy >> dz;
                /// \todo Make this set the specular vertex color of overlaying material node
                // Go over to next property
                propIndex += 3;
            } else if (_header.vProps[propIndex].first == "uv" && vTexCoords) {
                // Parse uv-texture coordinates
                sstr >> du >> dv;
                // Set texture coordinate
                if ( vTexCoords )_mesh->set_texcoord2D(currentVertex, typename MeshT::TexCoord2D(du, dv));
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

            if ( (unsigned int)index < vertexIndices.size() )
              vlist.push_back(vertexIndices[index]);
            else {
              emit log(LOGERR, tr("Illegal index in index List for face %1 vertex %2/%3. Vertices available: %4 ; Index: %5  ").arg(i).arg(c+1).arg(dInt).arg(vertexIndices.size()).arg(index+1));
              return false;
            }
        }

        // Add face
        currentFace = _mesh->add_face(vlist);

        propIndex = 0;
        next = false;

        // Search for further props in same line
        while(!next || _header.fProps.size() != 0) {

            if(propIndex >= _header.fProps.size())  {
                next = true;
                break;
            }

            if (_header.fProps[propIndex].first == "n_xyz" && fNormals) {
                // Parse face normal coordinates
                sstr >> dx >> dy >> dz;
                // Set normal
                if ( fNormals ) _mesh->set_normal(currentFace, typename MeshT::Normal(dx, dy, dz));
                // Go over to next property
                propIndex +=3;
            } else if (_header.fProps[propIndex].first == "rgb" && fColors) {

                bool divide = false;
                if (getTypeSize(_header.fProps[propIndex].second) == 1 ||
                    getTypeSize(_header.fProps[propIndex].second) == 4)
                  divide = true;

                // Parse face colors
                sstr >> dx >> dy >> dz;

                if (divide) {
                  dx /= 255.0;
                  dy /= 255.0;
                  dz /= 255.0;
                }

                if(_header.hasFaceColorAlpha) {
                  sstr >> alpha;

                  if (divide)
                    alpha /= 255.0;
                  propIndex++;
                } else {
                  alpha = 1.0;
                }

                // Set color
                if (fColors) _mesh->set_color(currentFace, typename MeshT::Color(dx, dy, dz,alpha));
                // Go over to next property
                propIndex += 3;
            } else if (_header.fProps[propIndex].first == "a_rgb" && fColors) {
                sstr >> dx >> dy >> dz;
                /// \todo Make this set the ambient face color of overlaying material node
                // Go over to next property
                propIndex += 3;
            } else if (_header.fProps[propIndex].first == "d_rgb" && fColors) {

                bool divide = false;
                if (getTypeSize(_header.fProps[propIndex].second) == 1 ||
                    getTypeSize(_header.fProps[propIndex].second) == 4)
                  divide = true;

                // Parse face colors
                sstr >> dx >> dy >> dz;

                if (divide) {
                  dx /= 255.0;
                  dy /= 255.0;
                  dz /= 255.0;
                }

                // Set color
                if (fColors) _mesh->set_color(currentFace, typename MeshT::Color(dx, dy, dz,1.0));
                // Go over to next property
                propIndex += 3;
            } else if (_header.fProps[propIndex].first == "s_rgb" && fColors) {
                sstr >> dx >> dy >> dz;
                /// \todo Make this set the specular face color of overlaying material node
                // Go over to next property
                propIndex += 3;
            } else {
                // Nothing else to parse in this line
                next = true;
                break;
            }
        }
    }

    // Update normals if we do not have them or did not read them
    if ( !_header.hasFaceNormals || !vNormals)
      _mesh->update_normals();

    ifs.close();
    return true;
}

//===============================================================================================

template <class MeshT>
bool FilePLYPlugin::readMeshFileBinary(QString _filename, MeshT* _mesh, const PLYHeader& _header) {

    std::string line;
    std::istringstream sstr;

    std::string dString = "";

    std::vector<PolyMesh::VertexHandle> vertexIndices;

    // Data types
    OpenMesh::Vec3f     v3f;
    OpenMesh::Vec3d     v3d;
    OpenMesh::Vec4uc    v4uc;
    OpenMesh::Vec4f     v4f;
    OpenMesh::Vec3i     v3i;
    OpenMesh::Vec4i     v4i(0,0,0,0);
    OpenMesh::Vec2i     v2i;
    OpenMesh::Vec2f     v2f;
    OpenMesh::Vec2d     v2d;

    // Open file stream as binary
    std::ifstream ifs(_filename.toUtf8(), std::ios_base::binary);

    if (!ifs.is_open() || !ifs.good() || ifs.eof()) {

        emit log(LOGERR, tr("Error: Could not read PLY data! Aborting."));
        return false;
    }

    // Skip header
    while(!ifs.eof()) {
        sstr.clear();
        std::getline(ifs, line);
        sstr.str(line);
        sstr >> dString;
        if(dString == "end_header") {
            break;
        }
    }

    // Get desired properties
    bool gui = OpenFlipper::Options::gui() && (loadVertexNormal_ != 0) /*buttons initialized*/;
    // If in no gui mode -> request as much as possible
    bool vNormals   =  ((gui && loadVertexNormal_->isChecked()) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Load/Normals",true).toBool()));
    bool vColors    =  ((gui && loadVertexColor_->isChecked()) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Load/VertexColor",true).toBool()));
    bool vTexCoords =  ((gui && loadVertexTexCoord_->isChecked()) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Load/TexCoords",true).toBool()));
    bool fNormals   =  ((gui && loadFaceNormal_->isChecked()) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Load/FaceNormal",true).toBool()));
    bool fColors    =  ((gui && loadFaceColor_->isChecked()) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Load/FaceColor",true).toBool()));

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
    unsigned int numProps = _header.vProps.size();
    typename MeshT::VertexHandle currentVertex;

    // Reserve enough space for all vertices to avoid resizing and save time
    vertexIndices.reserve(_header.numVertices);

    // Read in vertices
    for(int i = 0; i < _header.numVertices; ++i) {

        propIndex = 0;

        while(propIndex < numProps) {
            if(_header.vProps[propIndex].first == "xyz") {
                // Get x-, y-, z-coordinates
                if(getTypeSize(_header.vProps[propIndex].second) == 4) {
                    if(_header.vProps[propIndex].second == "int" || _header.vProps[propIndex].second == "int32" ||
                       _header.vProps[propIndex].second == "uint" || _header.vProps[propIndex].second == "uint32") {
                        readValue(ifs, v3i[0],_header.bigEndian);
                        readValue(ifs, v3i[1],_header.bigEndian);
                        readValue(ifs, v3i[2],_header.bigEndian);
                        currentVertex = _mesh->add_vertex(typename MeshT::Point(v3i));
                    } else {
                        readValue(ifs, v3f[0],_header.bigEndian);
                        readValue(ifs, v3f[1],_header.bigEndian);
                        readValue(ifs, v3f[2],_header.bigEndian);
                        currentVertex = _mesh->add_vertex(typename MeshT::Point(v3f));
                    }
                    vertexIndices.push_back(currentVertex);
                    propIndex += 3;
                } else if (getTypeSize(_header.vProps[propIndex].second) == 8) {
                    readValue(ifs, v3d[0],_header.bigEndian);
                    readValue(ifs, v3d[1],_header.bigEndian);
                    readValue(ifs, v3d[2],_header.bigEndian);
                    currentVertex = _mesh->add_vertex(typename MeshT::Point(v3d));
                    vertexIndices.push_back(currentVertex);
                    propIndex += 3;
                } else {
                    emit log(LOGERR, tr("Vertices have unsupported data size in binary PLY file. Aborting!"));
                    ifs.close();
                    return false;
                }
            } else if(_header.vProps[propIndex].first == "uv") {
                // Get vertex texcoords
                if(getTypeSize(_header.vProps[propIndex].second) == 4) {
                    if(_header.vProps[propIndex].second == "int" || _header.vProps[propIndex].second == "int32" ||
                       _header.vProps[propIndex].second == "uint" || _header.vProps[propIndex].second == "uint32") {
                        readValue(ifs, v2i[0],_header.bigEndian);
                        readValue(ifs, v2i[1],_header.bigEndian);
                        if(vTexCoords) _mesh->set_texcoord2D(currentVertex, typename MeshT::TexCoord2D(v2i));
                    } else {
                        readValue(ifs, v2f[0],_header.bigEndian);
                        readValue(ifs, v2f[1],_header.bigEndian);
                        if(vTexCoords) _mesh->set_texcoord2D(currentVertex, typename MeshT::TexCoord2D(v2f));
                    }
                    propIndex += 2;
                } else if (getTypeSize(_header.vProps[propIndex].second) == 8) {
                    readValue(ifs, v2d[0],_header.bigEndian);
                    readValue(ifs, v2d[1],_header.bigEndian);
                    if(vTexCoords) _mesh->set_texcoord2D(currentVertex, typename MeshT::TexCoord2D(v2d));
                    propIndex += 2;
                } else {
                    emit log(LOGERR, tr("Vertex texture coordinates have unsupported data size in binary PLY file. Aborting!"));
                    ifs.close();
                    return false;
                }
            } else if(_header.vProps[propIndex].first == "n_xyz") {
                // Get vertex normal
                if(getTypeSize(_header.vProps[propIndex].second) == 4) {
                    if(_header.vProps[propIndex].second == "int" || _header.vProps[propIndex].second == "int32" ||
                       _header.vProps[propIndex].second == "uint" || _header.vProps[propIndex].second == "uint32") {
                        readValue(ifs, v3i[0],_header.bigEndian);
                        readValue(ifs, v3i[1],_header.bigEndian);
                        readValue(ifs, v3i[2],_header.bigEndian);
                        if(vNormals) _mesh->set_normal(currentVertex, typename MeshT::Normal(v3i));
                    } else {
                        readValue(ifs, v3f[0],_header.bigEndian);
                        readValue(ifs, v3f[1],_header.bigEndian);
                        readValue(ifs, v3f[2],_header.bigEndian);
                        if(vNormals) _mesh->set_normal(currentVertex, typename MeshT::Normal(v3f));
                    }
                    propIndex += 3;
                } else if (getTypeSize(_header.vProps[propIndex].second) == 8) {
                    readValue(ifs, v3d[0],_header.bigEndian);
                    readValue(ifs, v3d[1],_header.bigEndian);
                    readValue(ifs, v3d[2],_header.bigEndian);
                    if(vNormals) _mesh->set_normal(currentVertex, typename MeshT::Normal(v3d));
                    propIndex += 3;
                } else {
                    emit log(LOGERR, tr("Vertex normals have unsupported data size in binary PLY file. Aborting!"));
                    ifs.close();
                    return false;
                }
            } else if(_header.vProps[propIndex].first == "rgb") {
                // Get vertex color
                if(getTypeSize(_header.vProps[propIndex].second) == 1) {
                    readValue(ifs, v4uc[0],_header.bigEndian);
                    readValue(ifs, v4uc[1],_header.bigEndian);
                    readValue(ifs, v4uc[2],_header.bigEndian);
                    if(_header.hasVertexColorAlpha) {
                        readValue(ifs, v4uc[3],_header.bigEndian);
                        propIndex++;
                    } else {
                      v4uc[3] = 255;
                    }
                    if(vColors) _mesh->set_color(currentVertex, OpenMesh::color_cast<typename MeshT::Color>(v4uc));
                    propIndex += 3;
                } else if (getTypeSize(_header.vProps[propIndex].second) == 4) {
                    if(_header.vProps[propIndex].second == "int" || _header.vProps[propIndex].second == "int32" ||
                       _header.vProps[propIndex].second == "uint" || _header.vProps[propIndex].second == "uint32") {
                        readValue(ifs, v4i[0],_header.bigEndian);
                        readValue(ifs, v4i[1],_header.bigEndian);
                        readValue(ifs, v4i[2],_header.bigEndian);
                        if(_header.hasVertexColorAlpha) {
                            readValue(ifs, v4i[3],_header.bigEndian);
                            propIndex++;
                        } else {
                          v4i[3] = 255;
                        }
                        if(vColors) _mesh->set_color(currentVertex, OpenMesh::color_cast<typename MeshT::Color>(v4i) );
                    } else {
                        readValue(ifs, v4f[0],_header.bigEndian);
                        readValue(ifs, v4f[1],_header.bigEndian);
                        readValue(ifs, v4f[2],_header.bigEndian);
                        if(_header.hasVertexColorAlpha) {
                            readValue(ifs, v4f[3],_header.bigEndian);
                            propIndex++;
                        } else {
                          v4f[3] = 1.0;
                        }

                        if(vColors) _mesh->set_color(currentVertex, v4f );
                    }
                    propIndex += 3;
                } else {
                    emit log(LOGERR, tr("Vertex colors have unsupported data size in binary PLY file. Aborting!"));
                    ifs.close();
                    return false;
                }
            } else if(_header.vProps[propIndex].first == "diffuse_rgb") {
                // Get diffuse vertex color
                if(getTypeSize(_header.vProps[propIndex].second) == 1) {
                    readValue(ifs, v4uc[0],_header.bigEndian);
                    readValue(ifs, v4uc[1],_header.bigEndian);
                    readValue(ifs, v4uc[2],_header.bigEndian);
                    if(_header.hasVertexColorAlpha) {
                        readValue(ifs, v4uc[3],_header.bigEndian);
                        propIndex++;
                    } else {
                      v4uc[3] = 255;
                    }
                    if(vColors) _mesh->set_color(currentVertex, OpenMesh::color_cast<typename MeshT::Color>(v4uc));
                    propIndex += 3;
                } else if (getTypeSize(_header.vProps[propIndex].second) == 4) {
                    if(_header.vProps[propIndex].second == "int" || _header.vProps[propIndex].second == "int32" ||
                       _header.vProps[propIndex].second == "uint" || _header.vProps[propIndex].second == "uint32") {
                        readValue(ifs, v4i[0],_header.bigEndian);
                        readValue(ifs, v4i[1],_header.bigEndian);
                        readValue(ifs, v4i[2],_header.bigEndian);
                        if(_header.hasVertexColorAlpha) {
                            readValue(ifs, v4i[3],_header.bigEndian);
                            propIndex++;
                        } else {
                          v4i[3] = 255;
                        }
                        if(vColors) _mesh->set_color(currentVertex, OpenMesh::color_cast<typename MeshT::Color>(v4i) );
                    } else {
                        readValue(ifs, v4f[0],_header.bigEndian);
                        readValue(ifs, v4f[1],_header.bigEndian);
                        readValue(ifs, v4f[2],_header.bigEndian);
                        if(_header.hasVertexColorAlpha) {
                            readValue(ifs, v4f[3],_header.bigEndian);
                            propIndex++;
                        } else {
                          v4f[3] = 1.0;
                        }
                        if(vColors) _mesh->set_color(currentVertex, v4f );
                    }
                    propIndex += 3;
                } else {
                    emit log(LOGERR, tr("Vertex colors have unsupported data size in binary PLY file. Aborting!"));
                    ifs.close();
                    return false;
                }
            } else {
                // Skip (sizeof unknown property) bytes
                // ambient_ and specular_ colors are not supported at this time
                ifs.seekg(getTypeSize(_header.vProps[propIndex].second), std::ios_base::cur);
                propIndex++;
            }
        }
    }

    numProps = _header.fProps.size();
    uint val;
    uint index;

    uchar v_uc;
    uchar i_uc;

    std::vector<typename MeshT::VertexHandle> vertex_list;
    typename MeshT::FaceHandle currentFace;

    // Read in faces
    for(int i = 0; i < _header.numFaces; ++i) {

        // Read in face valence
        vertex_list.clear();
        if(getTypeSize(_header.valenceType) == 1) {
            readValue(ifs, v_uc,_header.bigEndian);
            val = (unsigned int)v_uc;
        } else if (getTypeSize(_header.valenceType) == 4) {
          readValue(ifs, val,_header.bigEndian);
        } else {
            emit log(LOGERR, tr("Face valence has unsupported data size. Aborting!"));
            ifs.close();
            return false;
        }

        for(uint j = 0; j < val; ++j) {
            // Read vertex index
            if(getTypeSize(_header.indexType) == 1) {
                readValue(ifs, i_uc,_header.bigEndian);
                index = (unsigned int)i_uc;
            } else if(getTypeSize(_header.indexType) == 4) {
                readValue(ifs, index,_header.bigEndian);
            } else {
                emit log(LOGERR, tr("Vertex index type for face definitions has unsupported data size. Aborting!"));
                ifs.close();
                return false;
            }

            if ( index < vertexIndices.size() )
              vertex_list.push_back(vertexIndices[index]);
            else {
              emit log(LOGERR, tr("Illegal index in index List for face %1 vertex %2/%3. Vertices available: %4 ; Index: %5  ").arg(i).arg(j+1).arg(val).arg(vertexIndices.size()).arg(index+1));
              return false;
            }
        }

        currentFace = _mesh->add_face(vertex_list);

        // Read in face properties
        propIndex = 0;
        while(propIndex < numProps) {
             if(_header.fProps[propIndex].first == "n_xyz") {
                // Get face normal
                if(getTypeSize(_header.fProps[propIndex].second) == 4) {
                    if(_header.fProps[propIndex].second == "int" || _header.fProps[propIndex].second == "int32" ||
                       _header.fProps[propIndex].second == "uint" || _header.fProps[propIndex].second == "uint32") {
                        readValue(ifs, v3i[0],_header.bigEndian);
                        readValue(ifs, v3i[1],_header.bigEndian);
                        readValue(ifs, v3i[2],_header.bigEndian);
                        if(fNormals) _mesh->set_normal(currentFace, typename MeshT::Normal(v3i));
                    } else {
                        readValue(ifs, v3f[0],_header.bigEndian);
                        readValue(ifs, v3f[1],_header.bigEndian);
                        readValue(ifs, v3f[2],_header.bigEndian);
                        if(fNormals) _mesh->set_normal(currentFace, typename MeshT::Normal(v3f));
                    }
                    propIndex += 3;
                } else if (getTypeSize(_header.fProps[propIndex].second) == 8) {
                    readValue(ifs, v3d[0],_header.bigEndian);
                    readValue(ifs, v3d[1],_header.bigEndian);
                    readValue(ifs, v3d[2],_header.bigEndian);
                    if(fNormals) _mesh->set_normal(currentFace, typename MeshT::Normal(v3d));
                    propIndex += 3;
                } else {
                    emit log(LOGERR, tr("Face normals have unsupported data size in binary PLY file. Aborting!"));
                    ifs.close();
                    return false;
                }
            } else if(_header.fProps[propIndex].first == "rgb") {
                // Get vertex color
                if(getTypeSize(_header.fProps[propIndex].second) == 1) {
                    readValue(ifs, v4uc[0],_header.bigEndian);
                    readValue(ifs, v4uc[1],_header.bigEndian);
                    readValue(ifs, v4uc[2],_header.bigEndian);
                    if(_header.hasFaceColorAlpha) {
                        readValue(ifs, v4uc[3],_header.bigEndian);
                        propIndex++;
                    } else {
                      v4uc[3] = 255;
                    }
                    if(fColors) _mesh->set_color(currentFace, OpenMesh::color_cast<typename MeshT::Color>(v4uc));
                    propIndex += 3;
                } else if (getTypeSize(_header.fProps[propIndex].second) == 4) {
                    if(_header.fProps[propIndex].second == "int" || _header.fProps[propIndex].second == "int32" ||
                       _header.fProps[propIndex].second == "uint" || _header.fProps[propIndex].second == "uint32") {
                        readValue(ifs, v4i[0],_header.bigEndian);
                        readValue(ifs, v4i[1],_header.bigEndian);
                        readValue(ifs, v4i[2],_header.bigEndian);
                        if(_header.hasFaceColorAlpha) {
                            readValue(ifs, v4i[3],_header.bigEndian);
                            propIndex++;
                        } else {
                          v4i[3] = 255;
                        }
                        if(fColors) _mesh->set_color(currentFace, OpenMesh::color_cast<typename MeshT::Color>(v4i));
                    } else {
                        readValue(ifs, v4f[0],_header.bigEndian);
                        readValue(ifs, v4f[1],_header.bigEndian);
                        readValue(ifs, v4f[2],_header.bigEndian);
                        if(_header.hasFaceColorAlpha) {
                            readValue(ifs, v4f[3],_header.bigEndian);
                            propIndex++;
                        } else {
                          v4f[3] = 1.0;
                        }
                        if(fColors) _mesh->set_color(currentFace, v4f);
                    }
                    propIndex += 3;
                } else {
                    emit log(LOGERR, tr("Face colors have unsupported data size in binary PLY file. Aborting!"));
                    ifs.close();
                    return false;
                }
            } else if(_header.fProps[propIndex].first == "diffuse_rgb") {
                // Get diffuse vertex color
                if(getTypeSize(_header.fProps[propIndex].second) == 1) {
                    readValue(ifs, v4uc[0],_header.bigEndian);
                    readValue(ifs, v4uc[1],_header.bigEndian);
                    readValue(ifs, v4uc[2],_header.bigEndian);
                    if(_header.hasFaceColorAlpha) {
                        readValue(ifs, v4uc[3],_header.bigEndian);
                        propIndex++;
                    } else {
                      v4uc[3] = 255;
                    }
                    if(fColors) _mesh->set_color(currentFace, OpenMesh::color_cast<typename MeshT::Color>(v4uc));
                    propIndex += 3;
                } else if (getTypeSize(_header.fProps[propIndex].second) == 4) {
                    if(_header.fProps[propIndex].second == "int" || _header.fProps[propIndex].second == "int32" ||
                       _header.fProps[propIndex].second == "uint" || _header.fProps[propIndex].second == "uint32") {
                        readValue(ifs, v4i[0],_header.bigEndian);
                        readValue(ifs, v4i[1],_header.bigEndian);
                        readValue(ifs, v4i[2],_header.bigEndian);
                        if(_header.hasFaceColorAlpha) {
                            readValue(ifs, v4i[3],_header.bigEndian);
                            propIndex++;
                        } else {
                          v4i[3] = 255;
                        }
                        if(fColors) _mesh->set_color(currentFace, OpenMesh::color_cast<typename MeshT::Color>(v4i));
                    } else {
                        readValue(ifs, v4f[0],_header.bigEndian);
                        readValue(ifs, v4f[1],_header.bigEndian);
                        readValue(ifs, v4f[2],_header.bigEndian);
                        if(_header.hasFaceColorAlpha) {
                            readValue(ifs, v4f[3],_header.bigEndian);
                            propIndex++;
                        } else {
                          v4f[3] = 1.0;
                        }
                        if(fColors) _mesh->set_color(currentFace, v4f);
                    }
                    propIndex += 3;
                } else {
                    emit log(LOGERR, tr("Face colors have unsupported data size in binary PLY file. Aborting!"));
                    ifs.close();
                    return false;
                }
            } else {
                // Skip (sizeof unknown property) bytes
                // ambient_ and specular_ colors are not supported at this time
                ifs.seekg(getTypeSize(_header.fProps[propIndex].second), std::ios_base::cur);
                propIndex++;
            }
        }
    }

    // Update normals if we do not have them or did not read them
    if ( !_header.hasFaceNormals || !vNormals)
      _mesh->update_normals();

    ifs.close();
    return true;
}

//===============================================================================================

template <class MeshT>
void FilePLYPlugin::writeHeader(std::ofstream& _os, MeshT* _mesh, bool _binary) {

    // Get desired properties
    bool gui = OpenFlipper::Options::gui() && (saveVertexNormal_ != 0) /*buttons initialized*/;
    // If in no gui mode -> request as much as possible
    bool vNormals   = _mesh->has_vertex_normals() && (((gui && saveVertexNormal_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/Normals",true).toBool()));
    bool vColors    = _mesh->has_vertex_colors() && (((gui && saveVertexColor_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/VertexColor",true).toBool()));
    bool vTexCoords = _mesh->has_vertex_texcoords2D() && (((gui && saveVertexTexCoord_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/TexCoords",true).toBool()));
    bool fNormals   = _mesh->has_face_normals() && (((gui && saveFaceNormal_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/FaceNormal",true).toBool()));
    bool fColors    = _mesh->has_face_colors() && (((gui && saveFaceColor_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/FaceColor",true).toBool()));

    // Write general information
    _os << "ply\n";
    _os << "format " << (_binary ? "binary_little_endian" : "ascii") << " 1.0\n";
    _os << "comment =================================\n";
    _os << "comment Exported via OpenFlipper " << OpenFlipper::Options::coreVersion().toStdString() << "\n";
    _os << "comment www.openflipper.org\n";
    _os << "comment =================================\n";

    // Vertex block
    _os << "element vertex " << _mesh->n_vertices() << "\n";

    // Vertex coordinates
    _os << "property float x\n";
    _os << "property float y\n";
    _os << "property float z\n";

    // Vertex normals
    if(vNormals) _os << "property float nx\n";
    if(vNormals) _os << "property float ny\n";
    if(vNormals) _os << "property float nz\n";

    // Vertex colors
    if(vColors) _os << "property uchar red\n";
    if(vColors) _os << "property uchar green\n";
    if(vColors) _os << "property uchar blue\n";

    // Vertex texcoords
    if(vTexCoords) _os << "property int32 u\n";
    if(vTexCoords) _os << "property int32 v\n";

    // Face block
    _os << "element face " << _mesh->n_faces() << "\n";

    // Vertex index list
    _os << "property list uchar int32 vertex_index\n";

    // Face normals
    if(fNormals) _os << "property float nx\n";
    if(fNormals) _os << "property float ny\n";
    if(fNormals) _os << "property float nz\n";

    // Face colors
    if(fColors) _os << "property uchar red\n";
    if(fColors) _os << "property uchar green\n";
    if(fColors) _os << "property uchar blue\n";

    // End of header
    _os << "end_header\n";
}

//===============================================================================================

template <class MeshT>
bool FilePLYPlugin::writeMeshFileAscii(QString _filename, MeshT* _mesh) {

    // Get desired properties
    bool gui = OpenFlipper::Options::gui() && (saveVertexNormal_ != 0) /*buttons initialized*/;
    // If in no gui mode -> request as much as possible
    bool vNormals   = _mesh->has_vertex_normals() && (((gui && saveVertexNormal_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/Normals",true).toBool()));
    bool vColors    = _mesh->has_vertex_colors() && (((gui && saveVertexColor_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/VertexColor",true).toBool()));
    bool vTexCoords = _mesh->has_vertex_texcoords2D() && (((gui && saveVertexTexCoord_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/TexCoords",true).toBool()));
    bool fNormals   = _mesh->has_face_normals() && (((gui && saveFaceNormal_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/FaceNormal",true).toBool()));
    bool fColors    = _mesh->has_face_colors() && (((gui && saveFaceColor_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/FaceColor",true).toBool()));

    // Open file stream
    std::ofstream ofs(_filename.toUtf8());

    if (!ofs.is_open() || !ofs.good()) {

        emit log(LOGERR, tr("Error: Could not open PLY file for writing! Check permissions."));
        return false;
    }

    // Write out header
    writeHeader(ofs, _mesh, false);

    if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0) {
      ofs.precision(savePrecision_->value());
    };

    // Create map: VertexHandle -> int (line number in file)
    std::map<typename MeshT::VertexHandle,unsigned int> vMap;

    // Temporary types
    typename MeshT::Point       p;
    typename MeshT::Normal      n;
    typename MeshT::Color       c;
    typename MeshT::TexCoord2D  t;

    // Write vertices and properties
    unsigned int i = 0;
    for(typename MeshT::VertexIter v_it = _mesh->vertices_begin(); v_it != _mesh->vertices_end(); ++v_it) {

        p = _mesh->point(v_it);

        // Write vertex coords
        ofs << p;

        // Write vertex normal coords
        if(vNormals) {
            n = _mesh->normal(v_it);
            ofs << " " << n;
        }

        // Write vertex color
        if(vColors) {
            c = _mesh->color(v_it);
            ofs << " " << OpenMesh::color_cast<OpenMesh::Vec3uc>(c);
        }

        // Write vertex texcoord
        if(vTexCoords) {
            t = _mesh->texcoord2D(v_it);
            ofs << " " << t;
        }

        ofs << "\n";

        vMap.insert(std::pair<typename MeshT::VertexHandle,unsigned int>(v_it, i));
        ++i;
    }

    // Write faces and properties
    for(typename MeshT::FaceIter f_it = _mesh->faces_begin(); f_it != _mesh->faces_end(); ++f_it) {

        // Write face valence
        ofs << _mesh->valence(f_it);

        // Write vertex indices
        for(typename MeshT::FaceVertexIter fv_it = _mesh->fv_iter(f_it); fv_it; ++fv_it) {
            ofs << " " << vMap[fv_it.handle()];
        }

        // Write face normal
        if(fNormals) {
            n = _mesh->normal(f_it);
            ofs << " " << n;
        }

        // Write face color
        if(fColors) {
            c = _mesh->color(f_it);
            ofs << " " << OpenMesh::color_cast<OpenMesh::Vec3uc>(c);
        }

        ofs << "\n";
    }

    ofs.close();
    return true;
}

//===============================================================================================

template <class MeshT>
bool FilePLYPlugin::writeMeshFileBinary(QString _filename, MeshT* _mesh) {

    // Get desired properties
    bool gui = OpenFlipper::Options::gui() && (saveVertexNormal_ != 0) /*buttons initialized*/;
    // If in no gui mode -> request as much as possible
    bool vNormals   = _mesh->has_vertex_normals() && (((gui && saveVertexNormal_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/Normals",true).toBool()));
    bool vColors    = _mesh->has_vertex_colors() && (((gui && saveVertexColor_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/VertexColor",true).toBool()));
    bool vTexCoords = _mesh->has_vertex_texcoords2D() && (((gui && saveVertexTexCoord_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/TexCoords",true).toBool()));
    bool fNormals   = _mesh->has_face_normals() && (((gui && saveFaceNormal_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/FaceNormal",true).toBool()));
    bool fColors    = _mesh->has_face_colors() && (((gui && saveFaceColor_->isChecked())) ||
                        (!gui && OpenFlipperSettings().value("FilePLY/Save/FaceColor",true).toBool()));

    // Open file stream as binary
    std::ofstream ofs(_filename.toUtf8(), std::ios_base::binary);

    if (!ofs.is_open() || !ofs.good()) {

        emit log(LOGERR, tr("Error: Could not open PLY file for writing! Check permissions."));
        return false;
    }

    writeHeader(ofs, _mesh, true);

    // Create map: VertexHandle -> int (line number in file)
    std::map<typename MeshT::VertexHandle,int> vMap;

    // Temporary types
    typename MeshT::Point       p;
    typename MeshT::Normal      n;
    typename MeshT::Color       c;
    typename MeshT::TexCoord2D  t;
    OpenMesh::Vec3uc            v3uc;

    // Write vertices and properties
    int i = 0;
    for(typename MeshT::VertexIter v_it = _mesh->vertices_begin(); v_it != _mesh->vertices_end(); ++v_it) {

        p = _mesh->point(v_it);

        // Write coordinates
        writeValue(ofs, (float)p[0]);
        writeValue(ofs, (float)p[1]);
        writeValue(ofs, (float)p[2]);

        // Write normals
        if(vNormals) {
            n = _mesh->normal(v_it);

            // Write coordinates
            writeValue(ofs, (float)n[0]);
            writeValue(ofs, (float)n[1]);
            writeValue(ofs, (float)n[2]);
        }

        // Write colors
        if(vColors) {
            c = _mesh->color(v_it);

            v3uc = OpenMesh::vector_cast<OpenMesh::Vec3uc>(c);

            // Write coordinates
            writeValue(ofs, v3uc[0]);
            writeValue(ofs, v3uc[1]);
            writeValue(ofs, v3uc[2]);
        }

        // Write texture coords
        if(vTexCoords) {
            t = _mesh->texcoord2D(v_it);

            // Write coordinates
            writeValue(ofs, (uint)t[0]);
            writeValue(ofs, (uint)t[1]);
        }

        vMap.insert(std::pair<typename MeshT::VertexHandle,int>(v_it, i));
        ++i;
    }

    // Write faces and properties
    for(typename MeshT::FaceIter f_it = _mesh->faces_begin(); f_it != _mesh->faces_end(); ++f_it) {

        // Write face valence
        writeValue(ofs, (uchar)_mesh->valence(f_it));

        // Write vertex indices
        for(typename MeshT::FaceVertexIter fv_it = _mesh->fv_iter(f_it); fv_it; ++fv_it) {
            writeValue(ofs, vMap[fv_it.handle()]);
        }

        // Write face normal
        if(fNormals) {
            n = _mesh->normal(f_it);

            // Write coordinates
            writeValue(ofs, (float)n[0]);
            writeValue(ofs, (float)n[1]);
            writeValue(ofs, (float)n[2]);
        }

        // Write face color
        if(fColors) {
            c = _mesh->color(f_it);

            v3uc = OpenMesh::vector_cast<OpenMesh::Vec3uc>(c);

            // Write coordinates
            writeValue(ofs, v3uc[0]);
            writeValue(ofs, v3uc[1]);
            writeValue(ofs, v3uc[2]);
        }
    }

    ofs.close();
    return true;
}

//===============================================================================================

/// creates a backup of the original per vertex/face texture coordinates
template <class MeshT>
void FilePLYPlugin::backupTextureCoordinates(MeshT& _mesh) {

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
