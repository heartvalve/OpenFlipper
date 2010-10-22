
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

  for (f_it = _mesh.faces_begin(); f_it != f_end; ++f_it){
    
    c  = OpenMesh::color_cast<OpenMesh::Vec4f> (_mesh.color( f_it.handle() ));
    getMaterial(_mesh, f_it.handle(), _objId);
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
                  matStream << "map_Kd textures" << QDir::separator().toAscii()
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
    
    OpenMesh::Vec4f c = OpenMesh::color_cast<OpenMesh::Vec4f> (_mesh.color( _fh ));
    
    // First off, try to fetch texture index of current face/object...
    QString textureIndexProperty;
    emit textureIndexPropertyName(_objId, textureIndexProperty);
    
    int texIndex = 0;
    OpenMesh::FPropHandleT< int > texture_index_property;
    if ( _mesh.get_property_handle(texture_index_property,textureIndexProperty.toStdString()) ) {
        texIndex = _mesh.property(texture_index_property, _fh);
    } else if ( _mesh.get_property_handle(texture_index_property,"f:textureindex") ) {
        texIndex = _mesh.property(texture_index_property, _fh);
    } else if(_mesh.has_face_texture_index()) {
        texIndex = _mesh.texture_index(_fh);
    } else {
        QString texName;
        emit getCurrentTexture(_objId, texName);
        emit textureIndex(texName, _objId, texIndex);
    }
    
    QString filename;
    QString texName;
    
    emit textureName(_objId, texIndex, texName);
    emit textureFilename( _objId, texName, filename );
    
    for (MaterialList::iterator it = materials_.begin(); it != materials_.end(); ++it) {
    
        // No texture has been found
        if(filename == "NOT_FOUND") {
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
    mat.set_map_Kd(filename.toStdString(), texIndex);
    
    materials_.insert(std::pair<std::string, Material>("Material" + mat.material_number(), mat));
    MaterialList::iterator it = materials_.end();
    it--;
    return (*it).second;
}




//-----------------------------------------------------------------------------------------------------

template< class MeshT >
bool FileOBJPlugin::writeMesh(std::ostream& _out, QString _filename, MeshT& _mesh, int _objId ){
  
  unsigned int i, nV, idx;
  Vec3f v, n;
  Vec2f t(0.0f,0.0f);
  typename MeshT::VertexHandle vh;
  std::vector<typename MeshT::VertexHandle> vhandles;
  bool useMaterial = false;
  OpenMesh::Vec4f c;

  bool optionFaceColors      = false;
  bool optionVertexNormals   = false;
  bool optionVertexTexCoords = false;
  bool optionColorAlpha      = false;
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
    optionColorAlpha      = saveAlpha_->isChecked();
  }
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
          for(fh_it=_mesh.fh_iter(f_it.handle()); fh_it; ++fh_it) {
              typename MeshT::TexCoord2D t = _mesh.texcoord2D(fh_it.handle());
              _out << "vt " << t[0] << " " << t[1] << std::endl;
              vtMap.insert(std::pair<typename MeshT::HalfedgeHandle, int>(fh_it.handle(), count));
              count++;
          }
      }
  }

  Material lastMat;
  
  for (f_it = _mesh.faces_begin(); f_it != _mesh.faces_end(); ++f_it){

    if (useMaterial && optionFaceColors) {
       
        Material& material = getMaterial(_mesh, f_it.handle(), _objId);

        // If we are ina a new material block, specify in the file which material to use
        if(lastMat != material) {
            _out << "usemtl " << material << std::endl;
            lastMat = material;
        }
    }

    _out << "f";

    // Write out face information
    for(fh_it=_mesh.fh_iter(f_it.handle()); fh_it; ++fh_it) {
      
        // Write vertex index
        idx = _mesh.to_vertex_handle(fh_it.handle()).idx() + 1;
        _out << " " << idx;

        // Write separator
        _out << "/" ;

        // Write vertex texture coordinate index
        if ( optionVertexTexCoords && _mesh.has_halfedge_texcoords2D()) {
            // Refer to halfedge texture coordinates
            typename std::map<typename MeshT::HalfedgeHandle, int>::iterator it = vtMap.find(fh_it.handle());
            if(it != vtMap.end())
                _out  << (*it).second;
        } else if (optionVertexTexCoords && !_mesh.has_halfedge_texcoords2D() && _mesh.has_vertex_texcoords2D()) {
            // Refer to vertex texture coordinates
            typename std::map<typename MeshT::VertexHandle, int>::iterator it = vtMapV.find(_mesh.to_vertex_handle(fh_it.handle()));
            if(it != vtMapV.end())
                _out  << (*it).second;
        }

        // Write separator
        _out << "/" ;

        // Write vertex normal index
        if ( optionVertexNormals )
        _out << idx;
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
                  if(!testedOnce && dir.exists(fi.absolutePath() + QDir::separator() + "textures")) {
                      emit log(LOGERR, tr("The specified target folder already contains a subfolder called textures. Skipping!"));
                      continue;
                  } else {
                      dir.mkdir("textures");
                      img.save(fi.absolutePath() + QDir::separator() + "textures" + QDir::separator() + img_f.fileName());
                      testedOnce = true;
                  }
                  
              } else {
                  img.save(fi.absolutePath() + QDir::separator() + img_f.fileName());
              }
          }
      }
  }

  materials_.clear();

  return true;
}  
  
  
  
  