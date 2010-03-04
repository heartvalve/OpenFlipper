#include "OBJImporter.hh"

#include <OpenMesh/Core/Utils/vector_cast.hh>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/// base class needs virtual destructor
OBJImporter::~OBJImporter(){
  
}

//-----------------------------------------------------------------------------

/// add a vertex with coordinate \c _point
VertexHandle OBJImporter::addVertex(const Vec3f& _point){
  vertices_.push_back( _point );
  
  return vertices_.size()-1;
}

/// get vertex with given index
Vec3f OBJImporter::vertex(uint _index){
  
  if ( vertices_.size() > _index )
    return vertices_[ _index ];
  else
    return Vec3f();
}

//-----------------------------------------------------------------------------

/// add texture coordinates
int OBJImporter::addTexCoord(const Vec2f& _coord){
  texCoords_.push_back( _coord );
  
  return texCoords_.size()-1;
}

//-----------------------------------------------------------------------------

/// add a normal
int OBJImporter::addNormal(const Vec3f& _normal){
  normals_.push_back( _normal );
  
  return normals_.size()-1;
}

//-----------------------------------------------------------------------------

/// set degree U direction
void OBJImporter::setDegreeU(int _degree){
  degreeU_ = _degree;
}

//-----------------------------------------------------------------------------

/// set degree V direction
void OBJImporter::setDegreeV(int _degree){
  degreeV_ = _degree;
}


//-----------------------------------------------------------------------------

/// get current degree
int OBJImporter::degreeU(){
  return degreeU_;
}

//-----------------------------------------------------------------------------

/// get current degree
int OBJImporter::degreeV(){
  return degreeV_;
}

//-----------------------------------------------------------------------------

/// add a mesh
void OBJImporter::addObject( BaseObject* _object ){

  PolyMeshObject* polyMeshObj = dynamic_cast< PolyMeshObject* > (_object);
  TriMeshObject*  triMeshObj  = dynamic_cast< TriMeshObject*  > (_object);
  
#ifdef ENABLE_BSPLINECURVE_SUPPORT
  BSplineCurveObject* curveObject = dynamic_cast< BSplineCurveObject*  > (_object);
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
  BSplineSurfaceObject* surfaceObject = dynamic_cast< BSplineSurfaceObject*  > (_object);
#endif

  if ( polyMeshObj ){

    polyMeshes_.push_back( polyMeshObj->mesh() );
    objects_.push_back( _object );

    addUsedVertices();
    
  } else if ( triMeshObj ){
    
    triMeshes_.push_back( triMeshObj->mesh() );
    objects_.push_back( _object );
    
    addUsedVertices();
    
  }

#ifdef ENABLE_BSPLINECURVE_SUPPORT
  else if ( curveObject ){
    
    curves_.push_back( curveObject->splineCurve() );
    objects_.push_back( _object );  
  }
#endif

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
  else if ( surfaceObject ){
    
    surfaces_.push_back( surfaceObject->splineSurface() );
    objects_.push_back( _object );  
  }
#endif

  else {
    std::cerr << "Error: Cannot add object. Type is unknown!" << std::endl;
  }
}

//-----------------------------------------------------------------------------

/// get id of the active object
int OBJImporter::currentObject(){
  return objects_.size()-1;
}

//-----------------------------------------------------------------------------

/// get the active polyMesh
PolyMesh* OBJImporter::currentPolyMesh(){
  if (polyMeshes_.size() == 0)
    return 0;
  else
    return polyMeshes_.back();
}

//-----------------------------------------------------------------------------

/// get the active triMesh
TriMesh* OBJImporter::currentTriMesh(){
  if (triMeshes_.size() == 0)
    return 0;
  else
    return triMeshes_.back();
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINECURVE_SUPPORT

BSplineCurve* OBJImporter::currentCurve(){
  if (curves_.size() == 0)
    return 0;
  else
    return curves_.back();
}
  
#endif

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINESURFACE_SUPPORT

BSplineSurface* OBJImporter::currentSurface(){
  if (surfaces_.size() == 0)
    return 0;
  else
    return surfaces_.back();
}
  
#endif

//-----------------------------------------------------------------------------

/// add all vertices that are used to the mesh (in correct order)
void OBJImporter::addUsedVertices(){

  
  if ( isTriangleMesh( currentObject() ) ){

    //handle triangle meshes
    if ( !currentTriMesh() ) return;
    
    //add all vertices to the mesh
    std::set<VertexHandle>::iterator it;

    for ( it=usedVertices_[ currentObject() ].begin() ; it != usedVertices_[ currentObject() ].end(); it++ ){
  
      if ( *it >= (int)vertices_.size() ){
        std::cerr << "Error: Vertex ID too large" << std::endl;
        continue;
      }

      if ( vertexMapTri_.find( *it ) == vertexMapTri_.end() )
        vertexMapTri_[ *it ] = currentTriMesh()->add_vertex( (TriMesh::Point) vertices_[*it] );
    }
      
  } else if ( isPolyMesh( currentObject() ) ){

    //handle poly meshes
    if ( !currentPolyMesh() ) return;

    //add all vertices to the mesh
    std::set<VertexHandle>::iterator it;

    for ( it=usedVertices_[ currentObject() ].begin() ; it != usedVertices_[ currentObject() ].end(); it++ ){
        
      if ( *it >= (int)vertices_.size() ){
        std::cerr << "Error: Vertex ID too large" << std::endl;
        return;
      }

      if ( vertexMapPoly_.find( *it ) == vertexMapPoly_.end() )
        vertexMapPoly_[ *it ] = currentPolyMesh()->add_vertex( (PolyMesh::Point) vertices_[*it] );
    }
  }
}

//-----------------------------------------------------------------------------

/// set vertex texture coordinate
void OBJImporter::setVertexTexCoord(VertexHandle _vh, int _texCoordID){
  
  if ( isTriangleMesh( currentObject() ) ){
  
    //handle triangle meshes
    if ( !currentTriMesh() ) return;
    
    if ( _texCoordID < (int) texCoords_.size() ){
    
      //perhaps request texCoords for the mesh
      if ( !currentTriMesh()->has_vertex_texcoords2D() )
        currentTriMesh()->request_vertex_texcoords2D();
      
      //set the texCoords
      if ( vertexMapTri_.find( _vh ) != vertexMapTri_.end() )
        currentTriMesh()->set_texcoord2D( vertexMapTri_[_vh], texCoords_[ _texCoordID ] ); 

    }else{
      std::cerr << "Error: TexCoord ID too large" << std::endl;
    }
  
  } else if ( isPolyMesh( currentObject() ) ){
    
    //handle poly meshes
    if ( !currentPolyMesh() ) return;
    
    if ( _texCoordID < (int) texCoords_.size() ){

      //perhaps request texCoords for the mesh
      if ( !currentPolyMesh()->has_vertex_texcoords2D() )
        currentPolyMesh()->request_vertex_texcoords2D();
      
      //set the texCoords
      if ( vertexMapPoly_.find( _vh ) != vertexMapPoly_.end() )
        currentPolyMesh()->set_texcoord2D( vertexMapPoly_[_vh], texCoords_[ _texCoordID ] ); 

    }else{
      std::cerr << "Error: TexCoord ID too large" << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------

/// set vertex normal
void OBJImporter::setNormal(VertexHandle _vh, int _normalID){
  
  if ( isTriangleMesh( currentObject() ) ){
  
    //handle triangle meshes
    if ( !currentTriMesh() ) return;
    
    if ( _normalID < (int) normals_.size() ){
    
      if ( vertexMapTri_.find( _vh ) != vertexMapTri_.end() ){
        currentTriMesh()->set_normal( vertexMapTri_[_vh], (TriMesh::Point) normals_[ _normalID ] ); 
        objectOptions_[ objects_.size() - 1 ] |= NORMALS;
      }

    }else{
      std::cerr << "Error: normal ID too large" << std::endl;
    }
  
  } else if ( isPolyMesh( currentObject() ) ){
    
    //handle poly meshes
    if ( !currentPolyMesh() ) return;
    
    if ( _normalID < (int) normals_.size() ){
    
      if ( vertexMapPoly_.find( _vh ) != vertexMapPoly_.end() ){
        currentPolyMesh()->set_normal( vertexMapPoly_[_vh], (PolyMesh::Point) normals_[ _normalID ] ); 
        objectOptions_[ objects_.size() - 1 ] |= NORMALS;
      }

    }else{
      std::cerr << "Error: normal ID too large" << std::endl;
    }
    
  }
}

//-----------------------------------------------------------------------------

/// add a face with indices _indices refering to vertices
void OBJImporter::addFace(const VHandles& _indices){
  
  if ( isTriangleMesh( currentObject() ) ){
  
    //handle triangle meshes
    if ( !currentTriMesh() ) return;
    
    addedFacesTri_.clear();
    
    std::vector< TriMesh::VertexHandle > vertices;
    
    for (uint i=0; i < _indices.size(); i++){

      if ( vertexMapTri_.find( _indices[i] ) != vertexMapTri_.end() ){
    
        vertices.push_back( vertexMapTri_[ _indices[i] ] );

      }else{
        std::cerr << "Error: cannot add face. undefined index (" <<  _indices[i] << ")" << std::endl;
        return;
      }
    }
    
    size_t n_faces = currentTriMesh()->n_faces();
    
    currentTriMesh()->add_face( vertices ); 
    
    //remember all new faces
    for( size_t i=0; i < currentTriMesh()->n_faces()-n_faces; ++i )
      addedFacesTri_.push_back( TriMesh::FaceHandle(n_faces+i) );
    
    
  } else if ( isPolyMesh( currentObject() ) ){
  
    //handle poly meshes
    if ( !currentPolyMesh() ) return;
    
    std::vector< PolyMesh::VertexHandle > vertices;
    
    for (uint i=0; i < _indices.size(); i++){
      
      if ( vertexMapPoly_.find( _indices[i] ) != vertexMapPoly_.end() ){
    
        vertices.push_back( vertexMapPoly_[ _indices[i] ] );

      }else{
        std::cerr << "Error: cannot add face. undefined index (" <<  _indices[i] << ")" << std::endl;
        return;
      }
    }
    
    size_t n_faces = currentPolyMesh()->n_faces();
    
    addedFacePoly_ = currentPolyMesh()->add_face( vertices ); 
  }
}

//-----------------------------------------------------------------------------

/// add face and texture coordinates
void OBJImporter::addFace(const VHandles& _indices, const std::vector<int>& _face_texcoords){
  
  if ( isTriangleMesh( currentObject() ) ){
  
    //handle triangle meshes
    if ( !currentTriMesh() ) return;
    
    addedFacesTri_.clear();
    
    std::vector< TriMesh::VertexHandle > vertices;
    
    for (uint i=0; i < _indices.size(); i++){

      if ( vertexMapTri_.find( _indices[i] ) != vertexMapTri_.end() ){
    
        vertices.push_back( vertexMapTri_[ _indices[i] ] );

      }else{
        std::cerr << "Error: cannot add face. undefined index (" <<  _indices[i] << ")" << std::endl;
        return;
      }
    }
    
    size_t n_faces = currentTriMesh()->n_faces();
    
    currentTriMesh()->add_face( vertices ); 
    
    //remember all new faces
    for( size_t i=0; i < currentTriMesh()->n_faces()-n_faces; ++i )
      addedFacesTri_.push_back( TriMesh::FaceHandle(n_faces+i) );

    
    
    //perhaps request texCoords for the mesh
    if ( !currentTriMesh()->has_halfedge_texcoords2D() )
      currentTriMesh()->request_halfedge_texcoords2D();
    
    //now add texCoords
  
    // get first halfedge handle
    TriMesh::HalfedgeHandle cur_heh = currentTriMesh()->halfedge_handle( addedFacesTri_[0] );
    TriMesh::HalfedgeHandle end_heh = currentTriMesh()->prev_halfedge_handle(cur_heh);

    // find start heh
    while( currentTriMesh()->to_vertex_handle(cur_heh) != vertices[0] && cur_heh != end_heh )
      cur_heh = currentTriMesh()->next_halfedge_handle( cur_heh);

    for(unsigned int i=0; i<_face_texcoords.size(); ++i)
    {
      if ( _face_texcoords[i] < (int)texCoords_.size() ){

        PolyMesh::TexCoord2D tex = OpenMesh::vector_cast<TriMesh::TexCoord2D>( texCoords_[ _face_texcoords[i] ] );
        currentTriMesh()->set_texcoord2D(cur_heh, tex);

        cur_heh = currentTriMesh()->next_halfedge_handle(cur_heh);

      }else
        std::cerr << "Error: cannot set texture coordinates. undefined index." << std::endl;
    }
    
    
  } else if ( isPolyMesh( currentObject() ) ){

    //handle poly meshes
    if ( !currentPolyMesh() ) return;
    
    std::vector< PolyMesh::VertexHandle > vertices;
    
    for (uint i=0; i < _indices.size(); i++){

      if ( vertexMapPoly_.find( _indices[i] ) != vertexMapPoly_.end() ){
    
        vertices.push_back( vertexMapPoly_[ _indices[i] ] );

      }else{
        std::cerr << "Error: cannot add face. undefined index." << std::endl;
        return;
      }
    }
    
    size_t n_faces = currentPolyMesh()->n_faces();
    
    addedFacePoly_ = currentPolyMesh()->add_face( vertices ); 
    
    //perhaps request texCoords for the mesh
    if ( !currentPolyMesh()->has_halfedge_texcoords2D() )
      currentPolyMesh()->request_halfedge_texcoords2D();
    
    //now add texCoords
  
    if ( addedFacePoly_.is_valid() ) {
      // get first halfedge handle
      PolyMesh::HalfedgeHandle cur_heh = currentPolyMesh()->halfedge_handle( addedFacePoly_ );
      PolyMesh::HalfedgeHandle end_heh = currentPolyMesh()->prev_halfedge_handle(cur_heh);

      // find start heh
      while( currentPolyMesh()->to_vertex_handle(cur_heh) != vertices[0] && cur_heh != end_heh )
        cur_heh = currentPolyMesh()->next_halfedge_handle( cur_heh);

      for(unsigned int i=0; i<_face_texcoords.size(); ++i)
      {
        if ( _face_texcoords[i] < (int)texCoords_.size() ){

          PolyMesh::TexCoord2D tex = OpenMesh::vector_cast<PolyMesh::TexCoord2D>( texCoords_[ _face_texcoords[i] ] );
          currentPolyMesh()->set_texcoord2D(cur_heh, tex);

          cur_heh = currentPolyMesh()->next_halfedge_handle(cur_heh);

        }else
          std::cerr << "Error: cannot set texture coordinates. undefined index." << std::endl;
      }
    }
  }
}

//-----------------------------------------------------------------------------

void OBJImporter::addMaterial(std::string _materialName){

  
  if ( isTriangleMesh( currentObject() ) ){
  
    //handle triangle meshes
    if ( !currentTriMesh() ) return;
    
  
    if ( materials_.find( _materialName ) != materials_.end() ){
      
      Material& mat = materials_[ _materialName ];

      //get textureIndex Property
      OpenMesh::FPropHandleT< int > indexProperty;

      if ( hasTexture( currentObject() ) ){

        bool textureAllowed = ! ( objectOptions_[ currentObject() ] & FORCE_NOTEXTURES );
        
        if ( textureAllowed ){
        
          if (! currentTriMesh()->get_property_handle(indexProperty,"OriginalTexIndexMapping") )
            currentTriMesh()->add_property(indexProperty,"OriginalTexIndexMapping");
        }
      }

      for (uint i=0; i < addedFacesTri_.size(); i++){
      
        if ( mat.has_Kd() ) {
          bool colorAllowed = ! ( objectOptions_[ currentObject() ] & FORCE_NOCOLOR );
          
          if ( currentTriMesh()->has_face_colors() && colorAllowed ){
            currentTriMesh()->set_color(addedFacesTri_[i], OpenMesh::color_cast< TriMesh::Color >(mat.Kd() ) );
            objectOptions_[ currentObject() ] |= FACECOLOR;
          }
        }

        bool textureAllowed = ! ( objectOptions_[ currentObject() ] & FORCE_NOTEXTURES );

        // Set the texture index in the face index property
        if ( mat.has_map_Kd() ) {

          if ( hasTexture( currentObject() ) && textureAllowed )
            currentTriMesh()->property(indexProperty, addedFacesTri_[i]) = mat.map_Kd_index();

        } else {
          // If we don't have the info, set it to no texture
          if ( hasTexture( currentObject() ) && textureAllowed )
            currentTriMesh()->property(indexProperty, addedFacesTri_[i]) = 0;
        }
      }
    }
    
  } else if ( isPolyMesh( currentObject() ) ){
  
    //handle poly meshes
    if ( !currentPolyMesh() ) return;
    
    if ( materials_.find( _materialName ) != materials_.end() ){
      
      Material& mat = materials_[ _materialName ];
      
      //get textureIndex Property
      OpenMesh::FPropHandleT< int > indexProperty;

      if ( hasTexture( currentObject() ) ){

        bool textureAllowed = ! ( objectOptions_[ currentObject() ] & FORCE_NOTEXTURES );
        
        if ( textureAllowed ){

          if (! currentPolyMesh()->get_property_handle(indexProperty,"OriginalTexIndexMapping") )
            currentPolyMesh()->add_property(indexProperty,"OriginalTexIndexMapping");
        }
      }
      
      if ( mat.has_Kd() ) {
        bool colorAllowed = ! ( objectOptions_[ currentObject() ] & FORCE_NOCOLOR );
        
        if ( currentPolyMesh()->has_face_colors() && colorAllowed && addedFacePoly_.is_valid()  ){
          currentPolyMesh()->set_color(addedFacePoly_, OpenMesh::color_cast< PolyMesh::Color >(mat.Kd() ));
          objectOptions_[ currentObject() ] |= FACECOLOR;
        }
      }

      bool textureAllowed = ! ( objectOptions_[ currentObject() ] & FORCE_NOTEXTURES );

      // Set the texture index in the face index property
      if ( mat.has_map_Kd() ) {

        if ( hasTexture( currentObject() ) && textureAllowed && addedFacePoly_.is_valid())
          currentPolyMesh()->property(indexProperty, addedFacePoly_) = mat.map_Kd_index();
        
      } else {
        // If we don't have the info, set it to no texture
        if ( hasTexture( currentObject() ) && textureAllowed && addedFacePoly_.is_valid())
          currentPolyMesh()->property(indexProperty, addedFacePoly_) = 0;
      }
    }
  }
}

//-----------------------------------------------------------------------------

// force all meshes to be opened with specific type
void OBJImporter::forceMeshType( ObjectOptions _meshType ){
  
  for (uint i=0; i < objectOptions_.size(); i++){
    
    bool isMesh = (objectOptions_[i] & TRIMESH) | (objectOptions_[i] & POLYMESH);
    bool correctType = objectOptions_[i] & _meshType;
    
    if ( isMesh && !correctType )
      objectOptions_[i] = _meshType;
  }
}

//-----------------------------------------------------------------------------

bool OBJImporter::isTriangleMesh(int _objectID){
  return objectOptions_[ _objectID ] & TRIMESH;
}

//-----------------------------------------------------------------------------

bool OBJImporter::isPolyMesh(int _objectID){
  return objectOptions_[ _objectID ] & POLYMESH;
}

//-----------------------------------------------------------------------------

bool OBJImporter::isCurve(int _objectID){
  return objectOptions_[ _objectID ] & CURVE;
}

//-----------------------------------------------------------------------------

bool OBJImporter::isSurface(int _objectID){
  return objectOptions_[ _objectID ] & SURFACE;
}

//-----------------------------------------------------------------------------

bool OBJImporter::hasNormals(int _objectID){
  
  return objectOptions_[ _objectID ] & NORMALS;
}

//-----------------------------------------------------------------------------

bool OBJImporter::hasTexture(int _objectID){
  
  return objectOptions_[ _objectID ] & TEXTURE;
}

//-----------------------------------------------------------------------------

bool OBJImporter::hasTextureCoords(int _objectID){
  return objectOptions_[ _objectID ] & TEXCOORDS;
}

//-----------------------------------------------------------------------------

uint OBJImporter::n_vertices(){
  return vertices_.size();
}

//-----------------------------------------------------------------------------

uint OBJImporter::n_normals(){
  return normals_.size();
}

//-----------------------------------------------------------------------------

uint OBJImporter::n_texCoords(){
  return texCoords_.size();
}

//-----------------------------------------------------------------------------

uint OBJImporter::objectCount(){
  return objects_.size();
}

//-----------------------------------------------------------------------------

BaseObject* OBJImporter::object(int _objectID){
  if (objects_.size() == 0 || (int)objects_.size() <= _objectID)
    return 0;
  else
    return objects_[ _objectID ];
}

//-----------------------------------------------------------------------------

MaterialList& OBJImporter::materials(){
  return materials_;
}

//-----------------------------------------------------------------------------

QString OBJImporter::path(){
  return path_;
}

//-----------------------------------------------------------------------------

void OBJImporter::setPath(QString _path){
  path_ = _path;
}

//-----------------------------------------------------------------------------

void OBJImporter::addObjectOptions(ObjectOptions _options){
  objectOptions_.push_back( _options );
}

//-----------------------------------------------------------------------------

std::vector< OBJImporter::ObjectOptions >& OBJImporter::objectOptions(){
  return objectOptions_;
}

//-----------------------------------------------------------------------------

// check if object with given id has given option
bool OBJImporter::hasOption( uint _id, ObjectOptions _option ){
  
  if (_id >= objectOptions_.size())
    return false;
  
  return objectOptions_[_id] & _option;
}

//-----------------------------------------------------------------------------

void OBJImporter::setObjectName(int _objectID, QString _name){

  BaseObject* obj = object( _objectID );
  
  if ( obj != 0 )
    obj->setName( _name );
}

//-----------------------------------------------------------------------------

void OBJImporter::setGroup(QString _groupName){
  groupName_ = _groupName;
}

//-----------------------------------------------------------------------------

QString OBJImporter::group(){
  return groupName_;
}

//-----------------------------------------------------------------------------

//used materials
const std::vector<std::string> OBJImporter::usedMaterials(uint _objectID){

  if (_objectID >= usedMaterials_.size())
    return std::vector<std::string>();
  else
    return usedMaterials_[ _objectID ];
}

//-----------------------------------------------------------------------------

void OBJImporter::useMaterial( std::string _materialName ){

  while( (int)usedMaterials_.size() - 1 < currentObject() )
    usedMaterials_.push_back( std::vector<std::string>() );
  
  //check that it is not added already
  for (uint i=0; i < usedMaterials_[ currentObject() ].size(); i++ )
    if ( usedMaterials_[ currentObject() ][i] == _materialName )
      return;
  
  usedMaterials_[ currentObject() ].push_back( _materialName );
  
  objectOptions_[ currentObject() ] |= TEXTURE;
}

///used vertices
void OBJImporter::useVertex(VertexHandle _vertex){

  while( (int)usedVertices_.size() - 1 < (int) objectOptions_.size() )
    usedVertices_.push_back( std::set<VertexHandle>() );

  usedVertices_[ objectOptions_.size() ].insert( _vertex );
}

//-----------------------------------------------------------------------------

