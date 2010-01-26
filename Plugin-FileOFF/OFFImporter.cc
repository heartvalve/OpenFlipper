#include "OFFImporter.hh"

#include <OpenMesh/Core/Utils/vector_cast.hh>

//-----------------------------------------------------------------------------

/// base class needs virtual destructor
OFFImporter::~OFFImporter(){
  
}

//-----------------------------------------------------------------------------

/// constructor
OFFImporter::OFFImporter() :
polyMesh_(0),
triMesh_(0),
object_(0),
objectOptions_(0) {}

//-----------------------------------------------------------------------------

/// add a mesh
void OFFImporter::addObject( BaseObject* _object ){
    
    PolyMeshObject* polyMeshObj = dynamic_cast< PolyMeshObject* > (_object);
    TriMeshObject*  triMeshObj  = dynamic_cast< TriMeshObject*  > (_object);
    
    if ( polyMeshObj ){
        
        polyMesh_ = polyMeshObj->mesh();
        object_ = _object;
        objectOptions_ |= POLYMESH;
        
    } else if ( triMeshObj ){
        
        triMesh_ = triMeshObj->mesh();
        object_ = _object;
        objectOptions_ |= TRIMESH;
        
    } else {
        std::cerr << "Error: Cannot add object. Type is unknown!" << std::endl;
    }
}

//-----------------------------------------------------------------------------

/// get vertex with given index
Vec3f OFFImporter::vertex(uint _index){
  
  if ( vertices_.size() > _index )
    return vertices_[ _index ];
  else
    return Vec3f();
}

//-----------------------------------------------------------------------------

/// add texture coordinates
int OFFImporter::addTexCoord(const Vec2f& _coord){
  texCoords_.push_back( _coord );
  
  return texCoords_.size()-1;
}

//-----------------------------------------------------------------------------

/// add a normal
int OFFImporter::addNormal(const Vec3f& _normal){
  normals_.push_back( _normal );
  
  return normals_.size()-1;
}

//-----------------------------------------------------------------------------

/// add a color
int OFFImporter::addColor(const Vec4uc& _color) {
    colors_.push_back( _color );
    
    return colors_.size()-1;
}

//-----------------------------------------------------------------------------

/// add a color
int OFFImporter::addColor(const Vec3uc& _color) {
    colors_.push_back( Vec4uc(_color[0], _color[1], _color[2], 255) );
    
    return colors_.size()-1;
}

//-----------------------------------------------------------------------------

/// get mesh as polyMesh
PolyMesh* OFFImporter::polyMesh(){
  if (polyMesh_ == 0)
    return 0;
  else
    return polyMesh_;
}

//-----------------------------------------------------------------------------

/// get mesh as triMesh
TriMesh* OFFImporter::triMesh(){
  if (triMesh_ == 0)
    return 0;
  else
    return triMesh_;
}

//-----------------------------------------------------------------------------

/// set vertex texture coordinate
void OFFImporter::setVertexTexCoord(VertexHandle _vh, int _texCoordID){
  
  if ( isTriangleMesh() ){
  
    //handle triangle meshes
    if ( !triMesh() ) return;
    
    if ( _texCoordID < (int) texCoords_.size() ){
    
      if ( vertexMapTri_.find( _vh ) != vertexMapTri_.end() )
      triMesh()->set_texcoord2D( vertexMapTri_[_vh], texCoords_[ _texCoordID ] );
      objectOptions_ |= VERTEXTEXCOORDS;

    }else{
      std::cerr << "Error: TexCoord ID too large" << std::endl;
    }
  
  } else if ( isPolyMesh() ){
    
    //handle poly meshes
    if ( !polyMesh() ) return;
    
    if ( _texCoordID < (int) texCoords_.size() ){
    
      if ( vertexMapPoly_.find( _vh ) != vertexMapPoly_.end() )
      polyMesh()->set_texcoord2D( vertexMapPoly_[_vh], texCoords_[ _texCoordID ] ); 
      objectOptions_ |= VERTEXTEXCOORDS;

    }else{
      std::cerr << "Error: TexCoord ID too large" << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------

/// set vertex normal
void OFFImporter::setNormal(VertexHandle _vh, int _normalID){
  
  if ( isTriangleMesh() ){
  
    //handle triangle meshes
    if ( !triMesh() ) return;
    
    if ( _normalID < (int) normals_.size() ){
    
      if ( vertexMapTri_.find( _vh ) != vertexMapTri_.end() ){
        triMesh()->set_normal( vertexMapTri_[_vh], (TriMesh::Point) normals_[ _normalID ] ); 
        objectOptions_ |= VERTEXNORMAL;
      }

    }else{
      std::cerr << "Error: normal ID too large" << std::endl;
    }
  
  } else if ( isPolyMesh() ){
    
    //handle poly meshes
    if ( !polyMesh() ) return;
    
    if ( _normalID < (int) normals_.size() ){
    
      if ( vertexMapPoly_.find( _vh ) != vertexMapPoly_.end() ){
        polyMesh()->set_normal( vertexMapPoly_[_vh], (PolyMesh::Point) normals_[ _normalID ] ); 
        objectOptions_ |= VERTEXNORMAL;
      }

    }else{
      std::cerr << "Error: normal ID too large" << std::endl;
    }
    
  }
}

//-----------------------------------------------------------------------------

/// add a vertex with coordinate \c _point
VertexHandle OFFImporter::addVertex(const Vec3f& _point){
    
    vertices_.push_back( _point );
    
    int id = vertices_.size()-1;
    
    if ( isTriangleMesh() ){
        
        //handle triangle meshes
        if ( !triMesh() ) return false;
        vertexMapTri_[ id ] = triMesh()->add_vertex( (TriMesh::Point) vertices_[id] );
        
    } else if ( isPolyMesh() ){
        
        //handle poly meshes
        if ( !polyMesh() ) return false;
        vertexMapPoly_[ id ] = polyMesh()->add_vertex( (PolyMesh::Point) vertices_[id] );
    }
    
    return id;
}

//-----------------------------------------------------------------------------

/// add a face with indices _indices refering to vertices
int OFFImporter::addFace(const VHandles& _indices) {
   
  int faceIndex = -1;
    
  if ( isTriangleMesh() ){
  
    //handle triangle meshes
    if ( !triMesh() ) return -1;
    
    std::vector< TriMesh::VertexHandle > vertices;
    
    for (uint i=0; i < _indices.size(); i++){

      if ( vertexMapTri_.find( _indices[i] ) != vertexMapTri_.end() ){
    
        vertices.push_back( vertexMapTri_[ _indices[i] ] );

      } else {
        std::cerr << "Error: Cannot add face. Undefined index (" <<  _indices[i] << ")" << std::endl;
        return -1;
      }
    }
    
    TriMesh::FaceHandle fh = triMesh()->add_face( vertices ); 
    
    faceMapTri_.push_back( fh );
    
    faceIndex = faceMapTri_.size()-1;
    
  } else if ( isPolyMesh() ){
  
    //handle poly meshes
    if ( !polyMesh() ) return -1;
    
    std::vector< PolyMesh::VertexHandle > vertices;
    
    for (uint i=0; i < _indices.size(); i++){
      
      if ( vertexMapPoly_.find( _indices[i] ) != vertexMapPoly_.end() ){
    
        vertices.push_back( vertexMapPoly_[ _indices[i] ] );

      }else{
        std::cerr << "Error: Cannot add face. Undefined index (" <<  _indices[i] << ")" << std::endl;
        return -1;
      }
    }
    
    PolyMesh::FaceHandle fh = polyMesh()->add_face( vertices );
    
    faceMapPoly_.push_back(fh);
    
    faceIndex = faceMapPoly_.size()-1;
  }
  
  return faceIndex;
}

//-----------------------------------------------------------------------------

bool OFFImporter::isTriangleMesh(){
  return objectOptions_ & TRIMESH;
}

//-----------------------------------------------------------------------------

bool OFFImporter::isPolyMesh(){
  return objectOptions_ & POLYMESH;
}

//-----------------------------------------------------------------------------

bool OFFImporter::isBinary(){
    return objectOptions_ & BINARY;
}

//-----------------------------------------------------------------------------

bool OFFImporter::hasVertexNormals(){
  
  return objectOptions_ & VERTEXNORMAL;
}

//-----------------------------------------------------------------------------

bool OFFImporter::hasTextureCoords(){
  return objectOptions_ & VERTEXTEXCOORDS;
}

//-----------------------------------------------------------------------------

bool OFFImporter::hasVertexColors() {
    return objectOptions_ & VERTEXCOLOR;
}

//-----------------------------------------------------------------------------

bool OFFImporter::hasFaceColors() {
    return objectOptions_ & FACECOLOR;
}

//-----------------------------------------------------------------------------

bool OFFImporter::hasOption(ObjectOptionsE _option) {
    return objectOptions_ & _option;
}

//-----------------------------------------------------------------------------

uint OFFImporter::n_vertices(){
  return vertices_.size();
}

//-----------------------------------------------------------------------------

uint OFFImporter::n_normals(){
  return normals_.size();
}

//-----------------------------------------------------------------------------

uint OFFImporter::n_texCoords(){
  return texCoords_.size();
}

//-----------------------------------------------------------------------------

BaseObject* OFFImporter::getObject(){
  
    return object_;
}

//-----------------------------------------------------------------------------

QString OFFImporter::path(){
  return path_;
}

//-----------------------------------------------------------------------------

void OFFImporter::setPath(QString _path){
  path_ = _path;
}

//-----------------------------------------------------------------------------

void OFFImporter::setObjectOptions(ObjectOptions _options){
  objectOptions_ = _options;
}

//-----------------------------------------------------------------------------

void OFFImporter::addOption(ObjectOptionsE _option) {
    objectOptions_ |= _option;
}

//-----------------------------------------------------------------------------

void OFFImporter::removeOption(ObjectOptionsE _option) {
    if(objectOptions_ & _option) objectOptions_ -= _option;
}

//-----------------------------------------------------------------------------

OFFImporter::ObjectOptions& OFFImporter::objectOptions(){
  return objectOptions_;
}

//-----------------------------------------------------------------------------

void OFFImporter::setObjectName(QString _name){
  
  if ( object_ != 0 )
    object_->setName( _name );
}

//------------------------------------------------------------------------------

/// set RGBA color of vertex
void OFFImporter::setVertexColor(VertexHandle _vh, int _colorIndex) {
    
    // TODO Implement color alpha
    
    if ( isTriangleMesh() ){
        
        //handle triangle meshes
        if ( !triMesh() ) return;
        
        if ( _colorIndex < (int) colors_.size() ){
            
            if ( vertexMapTri_.find( _vh ) != vertexMapTri_.end() ){
                Vec4uc c = colors_[_colorIndex];
                triMesh()->set_color( vertexMapTri_[_vh], TriMesh::Color(c[0], c[1], c[2]) ); 
                objectOptions_ |= VERTEXCOLOR;
            }
            
        }else{
            std::cerr << "Error: Color ID too large" << std::endl;
        }
        
    } else if ( isPolyMesh() ){
        
        //handle poly meshes
        if ( !polyMesh() ) return;
        
        if ( _colorIndex < (int) colors_.size() ){
            
            if ( vertexMapPoly_.find( _vh ) != vertexMapPoly_.end() ){
                Vec4uc c = colors_[_colorIndex];
                polyMesh()->set_color( vertexMapPoly_[_vh], PolyMesh::Color(c[0], c[1], c[2]) ); 
                objectOptions_ |= VERTEXCOLOR;
            }
            
        }else{
            std::cerr << "Error: Color ID too large" << std::endl;
        }
        
    }
}

//------------------------------------------------------------------------------

/// set RGBA color of face
void OFFImporter::setFaceColor(FaceHandle _fh, int _colorIndex) {
    
    if ( isTriangleMesh() ){
        
        //handle triangle meshes
        if ( !triMesh() ) return;
        
        if ( _colorIndex < (int) colors_.size() ){
            
            if ( _fh < (int)faceMapTri_.size() ) {
                Vec4uc c = colors_[_colorIndex];
                triMesh()->set_color( faceMapTri_[_fh], TriMesh::Color(c[0], c[1], c[2]) ); 
                objectOptions_ |= FACECOLOR;
            }
            
        }else{
            std::cerr << "Error: Color ID too large" << std::endl;
        }
        
    } else if ( isPolyMesh() ){
        
        //handle poly meshes
        if ( !polyMesh() ) return;
        
        if ( _colorIndex < (int) colors_.size() ){
            
            if ( _fh < (int)faceMapPoly_.size() ) {
                Vec4uc c = colors_[_colorIndex];
                polyMesh()->set_color( faceMapPoly_[_fh], PolyMesh::Color(c[0], c[1], c[2]) ); 
                objectOptions_ |= FACECOLOR;
            }
            
        }else{
            std::cerr << "Error: Color ID too large" << std::endl;
        }
        
    }
}