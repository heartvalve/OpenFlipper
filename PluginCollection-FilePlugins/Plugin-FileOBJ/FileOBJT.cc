/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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


#define FILEOBJPLUGIN_C

#include "FileOBJ.hh"

#include <OpenMesh/Core/Utils/color_cast.hh>
#include <OpenMesh/Core/Geometry/VectorT.hh>


//-----------------------------------------------------------------------------------------------------

template< class MeshT >
bool FileOBJPlugin::writeMaterial(QString _filename, MeshT& _mesh, int _objId )
{
  bool optionColorAlpha      = false;
  bool optionTextures        = false;
  bool optionCopyTextures    = false;
  bool optionCreateTexFolder = false;

  // check options
  if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0) {
      optionColorAlpha      = saveAlpha_->isChecked();
      optionTextures        = saveTextures_->isChecked();
      optionCopyTextures    = saveCopyTextures_->isChecked();
      optionCreateTexFolder = saveCreateTexFolder_->isChecked();
  }
  // \TODO Fetch options from ini states if dialog box is not available

  std::fstream matStream( _filename.toStdString().c_str(), std::ios_base::out );

  if ( !matStream ){

    emit log(LOGERR, tr("writeMaterial : cannot not open file %1").arg(_filename) );
    return false;
  }

  // \TODO Implement setting of all colors (diffuse, ambient and specular)
  // There's only diffuse colors so far
  OpenMesh::Vec4f c;

  materials_.clear();

  //iterate over faces
  typename MeshT::FaceIter f_it;
  typename MeshT::FaceIter f_end = _mesh.faces_end();

  // Prepare materials ( getMaterial handles a list that is set up by this call)
  for (f_it = _mesh.faces_begin(); f_it != f_end; ++f_it){
    getMaterial(_mesh, *f_it, _objId);
  }

  //write the materials
  for(MaterialList::iterator it = materials_.begin(); it != materials_.end(); ++it) {
      Material& mat = (*it).second;
      matStream << "newmtl " << mat << std::endl;
      matStream << "Ka 0.5000 0.5000 0.5000" << std::endl;
      ACG::Vec3f c = mat.Kd();
      matStream << "Kd " << c[0] << " " << c[1] << " " << c[2] << std::endl;
      if(optionColorAlpha) {
          matStream << "Tr " << mat.Tr() << std::endl;
      }
      matStream << "illum 1" << std::endl;

      // Write out texture info
      if(optionTextures && mat.has_Texture()) {
          if(optionCopyTextures) {
              // Use file path in target folder (relative)
              QFileInfo file(mat.map_Kd().c_str());
              if(optionCreateTexFolder) {
                  QFileInfo materialFilename(_filename);

                  matStream << "map_Kd " << materialFilename.baseName().toStdString() << "_textures" << QDir::separator().toLatin1()
                      << file.fileName().toStdString() << std::endl;
              } else {
                  matStream << "map_Kd " << file.fileName().toStdString() << std::endl;
              }
          } else {
              // Use original file path
              matStream << "map_Kd " << mat.map_Kd() << std::endl;
          }
      }

      matStream << std::endl;
  }

  matStream.close();

  return true;
}

//-----------------------------------------------------------------------------------------------------

template< class MeshT >
Material& FileOBJPlugin::getMaterial(MeshT& _mesh, const OpenMesh::FaceHandle& _fh, int _objId)
{
    // check options
    bool optionColorAlpha = false;
    if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0)
        optionColorAlpha = saveAlpha_->isChecked();
    // \TODO Fetch options from ini states if dialog box is not available

    OpenMesh::Vec4f c = _mesh.color( _fh );

    // First off, try to fetch texture index of current face/object...
    if(!textureIndexPropFetched_) {
        emit textureIndexPropertyName(_objId, textureIndexPropertyName_);
        textureIndexPropFetched_ = true;
    }

    int texIndex = -1;
    OpenMesh::FPropHandleT< int > texture_index_property;
    if ( _mesh.get_property_handle(texture_index_property, textureIndexPropertyName_.toStdString()) ) {
        texIndex = _mesh.property(texture_index_property, _fh);
    } else if ( _mesh.get_property_handle(texture_index_property, "f:textureindex") ) {
        texIndex = _mesh.property(texture_index_property, _fh);
    } else if(_mesh.has_face_texture_index()) {
        texIndex = _mesh.texture_index(_fh);
    } else {
        QString texName;
        emit getCurrentTexture(_objId, texName);
        emit textureIndex(texName, _objId, texIndex);
    }

    QString filename;
    bool hasTexture = false;

    if(texIndex != -1) {

        // Search for texture index in local map
        std::map<int,QString>::iterator it = texIndexFileMap_.find(texIndex);

        if(it != texIndexFileMap_.end()) {
            // We already know this file
            filename = (*it).second;
            hasTexture = true;
        } else {
            // A new texture file has been found
            QString texName;
            emit textureName(_objId, texIndex, texName);

            if(texName != "NOT_FOUND") {
                emit textureFilename( _objId, texName, filename );
                // => Add to local map
                texIndexFileMap_.insert(std::pair<int,QString>(texIndex, filename));
                hasTexture = true;
            }
        }
    }

    for (MaterialList::iterator it = materials_.begin(); it != materials_.end(); ++it) {

        // No texture has been found
        if(!hasTexture) {
            // ... just look for diffuse color in materials list
            if(((*it).second).Kd() == ACG::Vec3f(c[0], c[1], c[2]) &&
                ((optionColorAlpha && ((*it).second).Tr() == c[3]) || !optionColorAlpha))
                return (*it).second;
        } else {
            // Texture has been found, look for both, matching texture and color
            QString mKd(((*it).second).map_Kd().c_str());
            if((((*it).second).Kd() == ACG::Vec3f(c[0], c[1], c[2]) &&
                ((optionColorAlpha && ((*it).second).Tr() == c[3]) || !optionColorAlpha)) &&
                (filename == mKd && ((*it).second).map_Kd_index() == texIndex))
                return (*it).second;
        }
    }

    // If not found, add new material(s)
    Material mat;
    // Set diffuse color
    mat.set_Kd(c[0], c[1], c[2]);
    // Add transparency if available
    if(optionColorAlpha) mat.set_Tr(c[3]);
    mat.material_number(materials_.size());
    // Set texture info
    if(hasTexture)
        mat.set_map_Kd(filename.toStdString(), texIndex);

    materials_.insert(std::pair<std::string, Material>("Material" + mat.material_number(), mat));
    MaterialList::iterator it = materials_.end();
    --it;
    return (*it).second;
}




//-----------------------------------------------------------------------------------------------------

template< class MeshT >
bool FileOBJPlugin::writeMesh(std::ostream& _out, QString _filename, MeshT& _mesh, int _objId){

  unsigned int i, nV, idx;
  Vec3f v, n;
  Vec2f t(0.0f,0.0f);
  typename MeshT::VertexHandle vh;
  bool useMaterial = false;
  OpenMesh::Vec4f c;

  bool optionFaceColors      = false;
  bool optionVertexNormals   = false;
  bool optionVertexTexCoords = false;
  bool optionTextures        = false;
  bool optionCopyTextures    = false;
  bool optionCreateTexFolder = false;

  QFileInfo fi(_filename);

  // check options
  if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0) {
    optionFaceColors      = saveFaceColor_->isChecked();
    optionVertexNormals   = saveNormals_->isChecked();
    optionVertexTexCoords = saveTexCoords_->isChecked();
    optionTextures        = saveTextures_->isChecked();
    optionCopyTextures    = saveCopyTextures_->isChecked();
    optionCreateTexFolder = saveCreateTexFolder_->isChecked();
    _out.precision(savePrecision_->value());
  };
  // \TODO Fetch options from ini states if dialog box is not available

  //create material file if needed
  if ( optionFaceColors || optionTextures ){

    QString matFile = fi.absolutePath() + QDir::separator() + fi.baseName() + ".mtl";

    useMaterial = writeMaterial(matFile, _mesh, _objId);
  }

  // Header
  _out << "# " << _mesh.n_vertices() << " vertices, ";
  _out << _mesh.n_faces() << " faces" << std::endl;

  // Material file
  if (useMaterial &&  optionFaceColors )
    _out << "mtllib " << fi.baseName().toStdString() << ".mtl" << std::endl;

  // Store indices of vertices in a map such that
  // they can easily be referenced for face definitions
  // later on
  std::map<typename MeshT::VertexHandle, int> vtMapV;

  int cf = 1;
  // vertex data (point, normals, texcoords)
  for (i=0, nV=_mesh.n_vertices(); i<nV; ++i)
  {
    vh = typename MeshT::VertexHandle(i);
    v  = _mesh.point(vh);
    n  = _mesh.normal(vh);

    if ( _mesh.has_vertex_texcoords2D() && !_mesh.has_halfedge_texcoords2D() )
      t  = _mesh.texcoord2D(vh);

    // Write out vertex coordinates
    _out << "v " << v[0] <<" "<< v[1] <<" "<< v[2] << std::endl;

    // Write out vertex coordinates
    if ( optionVertexNormals)
      _out << "vn " << n[0] <<" "<< n[1] <<" "<< n[2] << std::endl;

    // Write out vertex texture coordinates
    if ( optionVertexTexCoords && _mesh.has_vertex_texcoords2D() && !_mesh.has_halfedge_texcoords2D()) {
      _out << "vt " << t[0] <<" "<< t[1] << std::endl;
      vtMapV.insert(std::pair<typename MeshT::VertexHandle, int>(vh, cf));
      cf++;
    }
  }

  typename MeshT::FaceVertexIter fv_it;
  typename MeshT::FaceHalfedgeIter fh_it;
  typename MeshT::FaceIter f_it;

  // Store indices of vertex coordinate (in obj-file)
  // in map such that the corresponding halfedge
  // can easily be found later on
  std::map<typename MeshT::HalfedgeHandle, int> vtMap;

  // If mesh has halfedge tex coords, write them out instead of vertex texcoords
  if(_mesh.has_halfedge_texcoords2D()) {
      int count = 1;
      for (f_it = _mesh.faces_begin(); f_it != _mesh.faces_end(); ++f_it) {
          for(fh_it=_mesh.fh_iter(*f_it); fh_it.is_valid(); ++fh_it) {
              typename MeshT::TexCoord2D t = _mesh.texcoord2D(*fh_it);
              _out << "vt " << t[0] << " " << t[1] << std::endl;
              vtMap.insert(std::pair<typename MeshT::HalfedgeHandle, int>(*fh_it, count));
              count++;
          }
      }
  }

  Material lastMat;

  // we do not want to write seperators if we only write vertex indices
  bool vertexOnly =     !(optionVertexTexCoords && _mesh.has_halfedge_texcoords2D())
                    &&  !(optionVertexTexCoords && !_mesh.has_halfedge_texcoords2D() && _mesh.has_vertex_texcoords2D())
                    &&  !(optionVertexNormals);

  for (f_it = _mesh.faces_begin(); f_it != _mesh.faces_end(); ++f_it){

    if (useMaterial && optionFaceColors) {

        Material& material = getMaterial(_mesh, *f_it, _objId);

        // If we are ina a new material block, specify in the file which material to use
        if(lastMat != material) {
            _out << "usemtl " << material << std::endl;
            lastMat = material;
        }
    }

    _out << "f";

    // Write out face information
    for(fh_it=_mesh.fh_iter(*f_it); fh_it.is_valid(); ++fh_it) {

        // Write vertex index
        idx = _mesh.to_vertex_handle(*fh_it).idx() + 1;
        _out << " " << idx;

        if (!vertexOnly) {

          // Write separator
          _out << "/" ;

          // Write vertex texture coordinate index
          if ( optionVertexTexCoords && _mesh.has_halfedge_texcoords2D()) {
            // Refer to halfedge texture coordinates
            typename std::map<typename MeshT::HalfedgeHandle, int>::iterator it = vtMap.find(*fh_it);
            if(it != vtMap.end())
              _out  << (*it).second;
          } else if (optionVertexTexCoords && !_mesh.has_halfedge_texcoords2D() && _mesh.has_vertex_texcoords2D()) {
            // Refer to vertex texture coordinates
            typename std::map<typename MeshT::VertexHandle, int>::iterator it = vtMapV.find(_mesh.to_vertex_handle(*fh_it));
            if(it != vtMapV.end())
              _out  << (*it).second;
          }

          // Write vertex normal index
          if ( optionVertexNormals ) {
            // Write separator
            _out << "/" ;

            _out << idx;
          }
        }
    }

    _out << std::endl;
  }

  // Copy texture files (if demanded)
  if(optionCopyTextures) {
      // Only test existence of folder once
      // (for multiple textures)
      bool testedOnce = false;
      for(MaterialList::iterator it = materials_.begin(); it != materials_.end(); ++it) {
          Material& mat = (*it).second;

          if(!mat.has_Texture()) continue;

          QImage img(mat.map_Kd().c_str());
          QFileInfo img_f(mat.map_Kd().c_str());

          if(img.isNull()) {
              // Something happened wrong
              emit log(LOGERR, tr("An error occurred when trying to copy a texture file."));
              continue;
          } else {
              if(optionCreateTexFolder) {
                  // Create folder
                  QDir dir(fi.absolutePath());
                  if(!testedOnce && dir.exists(fi.absolutePath() + QDir::separator() + fi.baseName() + "_textures")) {
                      emit log(LOGERR, tr("The specified target folder already contains a subfolder called textures. Skipping!"));
                      continue;
                  } else {
                      dir.mkdir(fi.baseName() + "_textures");
                      img.save(fi.absolutePath() + QDir::separator() + fi.baseName() + "_textures" + QDir::separator() + img_f.fileName());
                      testedOnce = true;
                  }

              } else {
                  img.save(fi.absolutePath() + QDir::separator() + img_f.fileName());
              }
          }
      }
  }

  materials_.clear();
  texIndexFileMap_.clear();
  textureIndexPropFetched_ = false;

  return true;
}




