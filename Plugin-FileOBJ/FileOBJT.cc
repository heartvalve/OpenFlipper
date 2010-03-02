
#define FILEOBJPLUGIN_C


#include "FileOBJ.hh"

#include <OpenMesh/Core/Utils/color_cast.hh>
#include <OpenMesh/Core/Geometry/VectorT.hh>


//-----------------------------------------------------------------------------------------------------

template< class MeshT >
bool FileOBJPlugin::writeMaterial(QString _filename, MeshT& _mesh )
{
  bool optionColorAlpha = false;

  // check options
  if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0)
    optionColorAlpha = saveAlpha_->isChecked();
  
  
  std::fstream matStream( _filename.toStdString().c_str(), std::ios_base::out );

  if ( !matStream ){
    
    emit log(LOGERR, tr("writeMaterial : cannot not open file %1").arg(_filename) );
    return false; 
  }
    
  
  OpenMesh::Vec4f c;

  materials_.clear();
  
  //iterate over faces
  typename MeshT::FaceIter f_it;
  typename MeshT::FaceIter f_end = _mesh.faces_end();

  for (f_it = _mesh.faces_begin(); f_it != f_end; ++f_it){
    
    c  = OpenMesh::color_cast<OpenMesh::Vec4f> (_mesh.color( f_it.handle() ));
    getMaterial(c);
  }

  //write the materials
  if ( optionColorAlpha )
    for (uint i=0; i < materials_.size(); i++){
      matStream << "newmtl " << "mat" << i << std::endl;
      matStream << "Ka 0.5000 0.5000 0.5000" << std::endl;
      matStream << "Kd " << materials_[i][0] << materials_[i][1] << materials_[i][2] << std::endl;
      matStream << "Tr " << materials_[i][3] << std::endl;
      matStream << "illum 1" << std::endl;
    }
  else
    for (uint i=0; i < materials_.size(); i++){
      matStream << "newmtl " << "mat" << i << std::endl;
      matStream << "Ka 0.5000 0.5000 0.5000" << std::endl;
      matStream << "Kd " << materials_[i][0] << materials_[i][1] << materials_[i][2] << std::endl;
      matStream << "illum 1" << std::endl;
    }

  matStream.close();

  return true;
}


//-----------------------------------------------------------------------------------------------------

template< class MeshT >
bool FileOBJPlugin::writeMesh(std::ostream& _out, QString _filename, MeshT& _mesh ){
  
  unsigned int i, nV, idx;
  Vec3f v, n;
  Vec2f t;
  typename MeshT::VertexHandle vh;
  std::vector<typename MeshT::VertexHandle> vhandles;
  bool useMatrial = false;
  OpenMesh::Vec4f c;

  bool optionFaceColors      = false;
  bool optionVertexNormals   = false;
  bool optionVertexTexCoords = false;
  bool optionColorAlpha      = false;
  
  QFileInfo fi(_filename);
  
  // check options
  if ( !OpenFlipper::Options::savingSettings() && saveOptions_ != 0){
    optionFaceColors      = saveFaceColor_->isChecked();
    optionVertexNormals   = saveNormals_->isChecked();
    optionVertexTexCoords = saveTexCoords_->isChecked();
    optionColorAlpha      = saveAlpha_->isChecked();
    
  }

  
  //create material file if needed
  if ( optionFaceColors ){
    
    QString matFile = fi.absolutePath() + QDir::separator() + fi.baseName() + ".mat";

    useMatrial = writeMaterial(matFile, _mesh);
  }

  // header
  _out << "# " << _mesh.n_vertices() << " vertices, ";
  _out << _mesh.n_faces() << " faces" << std::endl;

  // material file
  if (useMatrial &&  optionFaceColors )
    _out << "mtllib " << fi.baseName().toStdString() << ".mat" << std::endl;

  // vertex data (point, normals, texcoords)
  for (i=0, nV=_mesh.n_vertices(); i<nV; ++i)
  {
    vh = typename MeshT::VertexHandle(i);
    v  = _mesh.point(vh);
    n  = _mesh.normal(vh);
    t  = _mesh.texcoord2D(vh);

    _out << "v " << v[0] <<" "<< v[1] <<" "<< v[2] << std::endl;

    if ( optionVertexNormals )
      _out << "vn " << n[0] <<" "<< n[1] <<" "<< n[2] << std::endl;

    if ( optionVertexTexCoords )
      _out << "vt " << t[0] <<" "<< t[1] << std::endl;
  }

  int lastMat = -1;

  // faces (indices starting at 1 not 0)

  typename MeshT::FaceIter f_it;
  typename MeshT::FaceIter f_end = _mesh.faces_end();

  for (f_it = _mesh.faces_begin(); f_it != f_end; ++f_it){

    if (useMatrial && optionFaceColors ){
      int material = -1;

      c  = OpenMesh::color_cast<OpenMesh::Vec4f> (_mesh.color( f_it.handle() ));
      material = getMaterial(c);

      // if we are ina a new material block, specify in the file which material to use
      if(lastMat != material) {
        _out << "usemtl mat" << material << std::endl;
        lastMat = material;
      }
    }

    _out << "f";


    typename MeshT::FaceVertexIter fv_it;

    for (fv_it=_mesh.fv_iter(f_it); fv_it; ++fv_it){
      
      // Write vertex index
      idx = fv_it.handle().idx() + 1;
      _out << " " << idx;

      // write separator
      _out << "/" ;
      
      // write vertex texture coordinate index
      if ( optionVertexTexCoords )
        _out  << idx;
      
      // write separator
      _out << "/" ;

      // write vertex normal index
      if ( optionVertexNormals )
        _out << idx;
    }

    _out << std::endl;
  }

  materials_.clear();

  return true;
}  
  
  
  
  