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
Vec3f OBJImporter::vertex(unsigned int _index){
  
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
void OBJImporter::setObject(BaseObject* _object, int _groupId) {

    if((unsigned int)_groupId >= triMeshes_.size()) {
        std::cerr << "Error: Group does not exist!" << std::endl;
        return;
    }

    if(PluginFunctions::polyMeshObject(_object->id()) != NULL) {

        polyMeshes_[_groupId] = PluginFunctions::polyMeshObject(_object->id());
        addUsedVertices(_groupId);

    } else if(PluginFunctions::triMeshObject(_object->id()) != NULL) {

        triMeshes_[_groupId] = PluginFunctions::triMeshObject(_object->id());
        addUsedVertices(_groupId);
    }
#ifdef ENABLE_BSPLINECURVE_SUPPORT
    else if(PluginFunctions::bsplineCurveObject(PluginFunctions::baseObjectData(_object)) != NULL) {

        bSplineCurves_[_groupId] = PluginFunctions::bsplineCurveObject(PluginFunctions::baseObjectData(_object));
    }
#endif
#ifdef ENABLE_BSPLINESURFACE_SUPPORT
    else if(PluginFunctions::bsplineSurfaceObject(PluginFunctions::baseObjectData(_object)) != NULL) {

        bSplineSurfaces_[_groupId] = PluginFunctions::bsplineSurfaceObject(PluginFunctions::baseObjectData(_object));
    }
#endif
    else {
        std::cerr << "Error: Cannot add object. Type is unknown!" << std::endl;
    }
}

//-----------------------------------------------------------------------------

/// get id of the active object
int OBJImporter::currentObject(){

    return currentGroup_;
}

//-----------------------------------------------------------------------------

/// get the active polyMesh
PolyMesh* OBJImporter::currentPolyMesh(){

    return polyMeshes_[currentGroup_]->mesh();
}

//-----------------------------------------------------------------------------

/// get the active triMesh
TriMesh* OBJImporter::currentTriMesh(){

    return triMeshes_[currentGroup_]->mesh();
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINECURVE_SUPPORT

BSplineCurve* OBJImporter::currentCurve(){

    return bSplineCurves_[currentGroup_]->splineCurve();
}
  
#endif

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINESURFACE_SUPPORT

BSplineSurface* OBJImporter::currentSurface(){

    return bSplineSurfaces_[currentGroup_]->splineSurface();
}
  
#endif

//-----------------------------------------------------------------------------

/// add all vertices that are used to the mesh (in correct order)
void OBJImporter::addUsedVertices(int _groupId) {

    if (isTriangleMesh(_groupId)) {

        //handle triangle meshes
        TriMesh* curMesh = triMeshes_[_groupId]->mesh();
        if (curMesh == NULL)
            return;

        // add all vertices to the mesh

        if(usedVertices_.size() <= (unsigned int)_groupId) return;

        for(std::map<int, VertexHandle>::const_iterator it = usedVertices_[_groupId].begin();
                it != usedVertices_[_groupId].end(); ++it) {

            if (it->first >= (int)vertices_.size()) {
                std::cerr << "Error: Vertex ID too large" << std::endl;
                continue;
            }

            if (vertexMapTri_[_groupId].find(it->first) == vertexMapTri_[_groupId].end()) {
                vertexMapTri_[_groupId].insert(std::pair<int, TriMesh::VertexHandle>(it->first, curMesh->add_vertex((TriMesh::Point)vertices_[it->first])));
            }
        }

    } else if (isPolyMesh(_groupId)) {

        //handle triangle meshes
        PolyMesh* curMesh = polyMeshes_[_groupId]->mesh();
        if (curMesh == NULL)
            return;

        //add all vertices to the mesh

        if(usedVertices_.size() <= (unsigned int)currentObject()) return;

        for(std::map<int, VertexHandle>::const_iterator it = usedVertices_[_groupId].begin();
                it != usedVertices_[_groupId].end(); ++it) {

            if (it->first >= (int)vertices_.size()) {
                std::cerr << "Error: Vertex ID too large" << std::endl;
                continue;
            }

            if (vertexMapPoly_[_groupId].find(it->first) == vertexMapPoly_[_groupId].end()) {
                vertexMapPoly_[_groupId].insert(std::pair<int, PolyMesh::VertexHandle>(it->first, curMesh->add_vertex((PolyMesh::Point)vertices_[it->first])));
            }
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
      if ( vertexMapTri_[currentGroup_].find( _vh ) != vertexMapTri_[currentGroup_].end() )
        currentTriMesh()->set_texcoord2D( vertexMapTri_[currentGroup_][_vh], texCoords_[ _texCoordID ] );

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
      if ( vertexMapPoly_[currentGroup_].find( _vh ) != vertexMapPoly_[currentGroup_].end() )
        currentPolyMesh()->set_texcoord2D( vertexMapPoly_[currentGroup_][_vh], texCoords_[ _texCoordID ] );

    }else{
      std::cerr << "Error: TexCoord ID too large" << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------

/// set vertex normal
void OBJImporter::setNormal(int _index, int _normalID){
  
  if ( isTriangleMesh( currentObject() ) ){
  
    //handle triangle meshes
    if ( !currentTriMesh() ) return;
    
    if ( _normalID < (int) normals_.size() )
    {
      if ( vertexMapTri_[currentGroup_].find( _index ) != vertexMapTri_[currentGroup_].end() )
      {
        TriMesh::VertexHandle vh = vertexMapTri_[currentGroup_][_index];
        TriMesh::Normal normal =  static_cast<TriMesh::Normal>( normals_[ _normalID ] );
        TriMesh* currentMesh = currentTriMesh();
        currentMesh->set_normal( vh , normal );
        objectOptions_[ currentGroup_ ] |= NORMALS;

        storedTriHENormals_[vh] = normal;
      }

    }else{
      std::cerr << "Error: normal ID too large" << std::endl;
    }
  
  } else if ( isPolyMesh( currentObject() ) ){
    
    //handle poly meshes
    if ( !currentPolyMesh() ) return;
    
    if ( _normalID < (int) normals_.size() )
    {
      if ( vertexMapPoly_[currentGroup_].find( _index ) != vertexMapPoly_[currentGroup_].end() )
      {
        PolyMesh::VertexHandle vh = vertexMapPoly_[currentGroup_][_index];
        PolyMesh::Point normal =  static_cast<PolyMesh::Point>( normals_[ _normalID ] );
        PolyMesh* currentMesh = currentPolyMesh();
        currentMesh->set_normal( vh , normal );
        objectOptions_[ currentGroup_ ] |= NORMALS;

        storedPolyHENormals_[vh] = normal;
      }

    }else{
      std::cerr << "Error: normal ID too large" << std::endl;
    }
    
  }
}

//-----------------------------------------------------------------------------

bool OBJImporter::addFace(const VHandles& _indices, OpenMesh::FaceHandle &_outFH, std::vector< TriMesh::VertexHandle > &_outTriVertices, std::vector< PolyMesh::VertexHandle > &_outPolyVertices)
{
  if ( isTriangleMesh( currentObject() ) ){

    //handle triangle meshes
    if ( !currentTriMesh() ) return false;

    addedFacesTri_[currentGroup_].clear();

    for (unsigned int i=0; i < _indices.size(); i++){

      if ( vertexMapTri_[currentGroup_].find( _indices[i] ) != vertexMapTri_[currentGroup_].end() ){

        _outTriVertices.push_back( vertexMapTri_[currentGroup_][ _indices[i] ] );

      }else{
        std::cerr << "Error: cannot add face. undefined index (" <<  _indices[i] << ")" << std::endl;
        return false;
      }
    }

    size_t n_faces = currentTriMesh()->n_faces();

    OpenMesh::FaceHandle fh = currentTriMesh()->add_face( _outTriVertices );

    //remember all new faces
    if(!fh.is_valid()) {
      // Store non-manifold face
      invalidFaces_.push_back(_outTriVertices);

    } else {
      // Store recently added face
      for( size_t i=0; i < currentTriMesh()->n_faces()-n_faces; ++i )
        addedFacesTri_[currentGroup_].push_back( TriMesh::FaceHandle(n_faces+i) );

      //write stored normals into the half edges
      if (!currentTriMesh()->has_halfedge_normals())
        currentTriMesh()->request_halfedge_normals();

      //iterate over all incoming haldedges of the added face
      for (TriMesh::FaceHalfedgeIter fh_iter = currentTriMesh()->fh_begin(fh);
          fh_iter != currentTriMesh()->fh_end(fh); ++fh_iter)
      {
        //and write the normals to it
        TriMesh::HalfedgeHandle heh = fh_iter.current_halfedge_handle();
        TriMesh::VertexHandle vh = currentTriMesh()->to_vertex_handle(heh);
        std::map<TriMesh::VertexHandle,TriMesh::Normal>::iterator iter = storedTriHENormals_.find(vh);
        if (iter != storedTriHENormals_.end())
          currentTriMesh()->set_normal(heh,iter->second);
      }
      storedTriHENormals_.clear();
    }

    _outFH = fh;
    return true;

  } else if ( isPolyMesh( currentObject() ) ){

    //handle poly meshes
    if ( !currentPolyMesh() ) return false;

    for (unsigned int i=0; i < _indices.size(); i++){

      if ( vertexMapPoly_[currentGroup_].find( _indices[i] ) != vertexMapPoly_[currentGroup_].end() ){

        _outPolyVertices.push_back( vertexMapPoly_[currentGroup_][ _indices[i] ] );

      }else{
        std::cerr << "Error: cannot add face. undefined index (" <<  _indices[i] << ")" << std::endl;
        return false;
      }
    }

    if(!vertexListIsManifold(_outPolyVertices)) {
      std::cerr << "Face consists of multiple occurrences of the same vertex!" << std::endl;
      return false;
    }

    OpenMesh::FaceHandle fh = currentPolyMesh()->add_face( _outPolyVertices );

    if(!fh.is_valid()) {
      // Store non-manifold face
      invalidFaces_.push_back(_outPolyVertices);
    } else {
      addedFacePoly_ = fh;

      //write stored normals into the half edges
      if (!currentPolyMesh()->has_halfedge_normals())
        currentPolyMesh()->request_halfedge_normals();

      //iterate over all incoming haldedges of the added face
      for (PolyMesh::FaceHalfedgeIter fh_iter = currentPolyMesh()->fh_begin(fh);
          fh_iter != currentPolyMesh()->fh_end(fh); ++fh_iter)
      {
        //and write the normals to it
        PolyMesh::HalfedgeHandle heh = fh_iter.current_halfedge_handle();
        PolyMesh::VertexHandle vh = currentPolyMesh()->to_vertex_handle(heh);
        std::map<PolyMesh::VertexHandle,PolyMesh::Normal>::iterator iter = storedTriHENormals_.find(vh);
        if (iter != storedTriHENormals_.end())
          currentPolyMesh()->set_normal(heh,iter->second);
      }
      storedPolyHENormals_.clear();

    }

    _outFH = fh;
    return true;
  }
  return false;
}

/// add a face with indices _indices refering to vertices
void OBJImporter::addFace(const VHandles& _indices){
  OpenMesh::FaceHandle fh;
  std::vector< TriMesh::VertexHandle > triVertices;
  std::vector< PolyMesh::VertexHandle > polyVertices;
  addFace(_indices,fh,triVertices,polyVertices);

}

//-----------------------------------------------------------------------------

bool OBJImporter::vertexListIsManifold(const std::vector<PolyMesh::VertexHandle>& _vertices) const {

    std::set<PolyMesh::VertexHandle> check;
    for(std::vector<PolyMesh::VertexHandle>::const_iterator v_it = _vertices.begin();
            v_it != _vertices.end(); ++v_it) {
        check.insert(*v_it);
    }

    return (check.size() == _vertices.size());
}

//-----------------------------------------------------------------------------

/// add face and texture coordinates
void OBJImporter::addFace(const VHandles& _indices, const std::vector<int>& _face_texcoords){
  
  
  OpenMesh::FaceHandle fh;
  std::vector< TriMesh::VertexHandle > triVertices;
  std::vector< PolyMesh::VertexHandle > polyVertices;
  if(!addFace(_indices, fh, triVertices, polyVertices) || !fh.is_valid())
    return;

  if ( isTriangleMesh(currentObject()) )
  {
    //perhaps request texCoords for the mesh
    if ( !currentTriMesh()->has_halfedge_texcoords2D() )
      currentTriMesh()->request_halfedge_texcoords2D();

    //now add texCoords

    // get first halfedge handle
    TriMesh::HalfedgeHandle cur_heh = currentTriMesh()->halfedge_handle( addedFacesTri_[currentGroup_][0] );
    TriMesh::HalfedgeHandle end_heh = currentTriMesh()->prev_halfedge_handle(cur_heh);

    // find start heh
    while( currentTriMesh()->to_vertex_handle(cur_heh) != triVertices[0] && cur_heh != end_heh )
      cur_heh = currentTriMesh()->next_halfedge_handle( cur_heh);

    for(unsigned int i=0; i<_face_texcoords.size(); ++i)
    {
      if ( _face_texcoords[i] < (int)texCoords_.size() ){

        PolyMesh::TexCoord2D tex = OpenMesh::vector_cast<TriMesh::TexCoord2D>( texCoords_[ _face_texcoords[i] ] );
        currentTriMesh()->set_texcoord2D(cur_heh, tex);

        cur_heh = currentTriMesh()->next_halfedge_handle(cur_heh);

      } else
        std::cerr << "Error: cannot set texture coordinates. undefined index." << std::endl;
    }
  }else if (isPolyMesh(currentObject()))
  {
    addedFacePoly_ = fh;

    //perhaps request texCoords for the mesh
    if ( !currentPolyMesh()->has_halfedge_texcoords2D() )
      currentPolyMesh()->request_halfedge_texcoords2D();

    //now add texCoords

    if ( addedFacePoly_.is_valid() ) {
      // get first halfedge handle
      PolyMesh::HalfedgeHandle cur_heh = currentPolyMesh()->halfedge_handle( addedFacePoly_ );
      PolyMesh::HalfedgeHandle end_heh = currentPolyMesh()->prev_halfedge_handle(cur_heh);

      // find start heh
      while( currentPolyMesh()->to_vertex_handle(cur_heh) != polyVertices[0] && cur_heh != end_heh )
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
        
          // Add texture index property if it doesn't exist yet
          if (! currentTriMesh()->get_property_handle(indexProperty,TEXTUREINDEX) )
            currentTriMesh()->add_property(indexProperty,TEXTUREINDEX);
        }
      }

      for (unsigned int i=0; i < addedFacesTri_[currentGroup_].size(); i++){
      
        if ( mat.has_Kd() ) {
          bool colorAllowed = ! ( objectOptions_[ currentObject() ] & FORCE_NOCOLOR );
          
          if ( currentTriMesh()->has_face_colors() && colorAllowed ){

            TriMesh::Color color = OpenMesh::color_cast< OpenMesh::Vec4f >(mat.Kd() );

            // Get alpha if available
            if (mat.has_Tr() ) {
              color[3] = mat.Tr();
            } else {
              color[3] = 1.0;
            }

            currentTriMesh()->set_color(addedFacesTri_[currentGroup_][i], color  );
            objectOptions_[ currentObject() ] |= FACECOLOR;
          }
        }

        bool textureAllowed = ! ( objectOptions_[ currentObject() ] & FORCE_NOTEXTURES );

        // Set the texture index in the face index property
        if ( mat.has_Texture() ) {

          if ( hasTexture( currentObject() ) && textureAllowed )
            currentTriMesh()->property(indexProperty, addedFacesTri_[currentGroup_][i]) = mat.map_Kd_index();

        } else {
          // If we don't have the info, set it to no texture
          if ( hasTexture( currentObject() ) && textureAllowed )
            currentTriMesh()->property(indexProperty, addedFacesTri_[currentGroup_][i]) = 0;
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

          if (! currentPolyMesh()->get_property_handle(indexProperty,TEXTUREINDEX) )
            currentPolyMesh()->add_property(indexProperty,TEXTUREINDEX);
        }
      }
      
      if ( mat.has_Kd() ) {
        bool colorAllowed = ! ( objectOptions_[ currentObject() ] & FORCE_NOCOLOR );
        
        if ( currentPolyMesh()->has_face_colors() && colorAllowed && addedFacePoly_.is_valid()  ){

          TriMesh::Color color = OpenMesh::color_cast< OpenMesh::Vec4f >(mat.Kd() );

          // Get alpha if available
          if (mat.has_Tr() ) {
            color[3] = mat.Tr();
          } else {
            color[3] = 1.0;
          }

          currentPolyMesh()->set_color(addedFacePoly_, color );
          objectOptions_[ currentObject() ] |= FACECOLOR;
        }
      }

      bool textureAllowed = ! ( objectOptions_[ currentObject() ] & FORCE_NOTEXTURES );

      // Set the texture index in the face index property
      if ( mat.has_Texture() ) {

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
  
  for (unsigned int i=0; i < objectOptions_.size(); i++){
    
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

unsigned int OBJImporter::n_vertices(){
  return vertices_.size();
}

//-----------------------------------------------------------------------------

unsigned int OBJImporter::n_normals(){
  return normals_.size();
}

//-----------------------------------------------------------------------------

unsigned int OBJImporter::n_texCoords(){
  return texCoords_.size();
}

//-----------------------------------------------------------------------------

unsigned int OBJImporter::objectCount(){
  return groupNames_.size();
}

//-----------------------------------------------------------------------------

BaseObject* OBJImporter::object(int _objectID) {

  if (objectCount() == 0 || objectCount() <= (unsigned int)_objectID)
    return NULL;

  if(triMeshes_[_objectID] != NULL)             return triMeshes_[_objectID];
  else if(polyMeshes_[_objectID] != NULL)       return polyMeshes_[_objectID];
#ifdef ENABLE_BSPLINECURVE_SUPPORT
  else if(bSplineCurves_[_objectID] != NULL)    return bSplineCurves_[_objectID];
#endif
#ifdef ENABLE_BSPLINESURFACE_SUPPORT
  else if(bSplineSurfaces_[_objectID] != NULL)  return bSplineSurfaces_[_objectID];
#endif

  return NULL;
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

void OBJImporter::setObjectOptions(ObjectOptions _options) {

    while((unsigned int)currentGroup_ >= objectOptions_.size()) {
        objectOptions_.push_back(NONE);
    }

    objectOptions_[currentGroup_] = _options;
}

//-----------------------------------------------------------------------------

std::vector< OBJImporter::ObjectOptions >& OBJImporter::objectOptions(){
  return objectOptions_;
}

//-----------------------------------------------------------------------------

// check if object with given id has given option
bool OBJImporter::hasOption( unsigned int _id, ObjectOptions _option ){
  
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

int OBJImporter::addGroup(const QString& _groupName) {

    QString group = _groupName.trimmed();
    int id = groupId(group);
    if(id == -1) {
        groupNames_.push_back(group);
        vertexMapTri_.push_back(std::map<int,TriMesh::VertexHandle>());
        vertexMapPoly_.push_back(std::map<int,TriMesh::VertexHandle>());
        addedFacesTri_.push_back(std::vector< TriMesh::FaceHandle>());

        triMeshes_.push_back(NULL);
        polyMeshes_.push_back(NULL);
#ifdef ENABLE_BSPLINECURVE_SUPPORT
        bSplineCurves_.push_back(NULL);
#endif
#ifdef ENABLE_BSPLINESURFACE_SUPPORT
        bSplineSurfaces_.push_back(NULL);
#endif
        return groupNames_.size() - 1;
    }
    return id;
}

//-----------------------------------------------------------------------------

int OBJImporter::groupId(const QString& _groupName) const {

    for(unsigned int i = 0; i < groupNames_.size(); ++i) {
        if(groupNames_[i] == _groupName.trimmed()) return i;
    }
    return -1;
}

//-----------------------------------------------------------------------------

const QString OBJImporter::groupName(const int _grpId) const {

    if((unsigned int)_grpId < groupNames_.size()) {
        return groupNames_[_grpId];
    }
    return QString();
}

//-----------------------------------------------------------------------------

void OBJImporter::setGroupName(const int _grp, const QString& _name) {

    if((unsigned int)_grp >= groupNames_.size()) return;

    groupNames_[_grp] = _name;
}

//-----------------------------------------------------------------------------

void OBJImporter::setCurrentGroup(const int _current) {

    currentGroup_ = _current;
}

//-----------------------------------------------------------------------------

int OBJImporter::currentGroup() const {

    return currentGroup_;
}

//-----------------------------------------------------------------------------

void OBJImporter::finish() {

	// Duplicate vertices of non-manifold faces
	// and add them as new isolated face
	if(invalidFaces_.empty()) return;

	if (isTriangleMesh(currentObject())) {

		// Handle triangle meshes
		if ( !currentTriMesh() ) return;

		for(std::vector<OMVHandles>::iterator it = invalidFaces_.begin();
				it != invalidFaces_.end(); ++it) {

			OMVHandles& vhandles = *it;

			// double vertices
			for (unsigned int j = 0; j < vhandles.size(); ++j)
			{
			  TriMesh::Point p = currentTriMesh()->point(vhandles[j]);
			  vhandles[j] = currentTriMesh()->add_vertex(p);
			  // DO STORE p, reference may not work since vertex array
			  // may be relocated after adding a new vertex !

			  // Mark vertices of failed face as non-manifold
			  if (currentTriMesh()->has_vertex_status()) {
				  currentTriMesh()->status(vhandles[j]).set_fixed_nonmanifold(true);
			  }
			}

			// add face
			OpenMesh::FaceHandle fh = currentTriMesh()->add_face(vhandles);

			// Mark failed face as non-manifold
			if (currentTriMesh()->has_face_status())
				currentTriMesh()->status(fh).set_fixed_nonmanifold(true);

			// Mark edges of failed face as non-two-manifold
			if (currentTriMesh()->has_edge_status()) {
				TriMesh::FaceEdgeIter fe_it = currentTriMesh()->fe_iter(fh);
				for(; fe_it; ++fe_it) {
					currentTriMesh()->status(fe_it).set_fixed_nonmanifold(true);
				}
			}
		}

	} else {

		// Handle Polymeshes
		if ( !currentPolyMesh() ) return;

		for(std::vector<OMVHandles>::iterator it = invalidFaces_.begin();
				it != invalidFaces_.end(); ++it) {

			OMVHandles& vhandles = *it;

			// double vertices
			for (unsigned int j = 0; j < vhandles.size(); ++j)
			{
			  PolyMesh::Point p = currentPolyMesh()->point(vhandles[j]);
			  vhandles[j] = currentPolyMesh()->add_vertex(p);
			  // DO STORE p, reference may not work since vertex array
			  // may be relocated after adding a new vertex !

			  // Mark vertices of failed face as non-manifold
			  if (currentPolyMesh()->has_vertex_status()) {
				  currentPolyMesh()->status(vhandles[j]).set_fixed_nonmanifold(true);
			  }
			}

			// add face
			OpenMesh::FaceHandle fh = currentPolyMesh()->add_face(vhandles);

			// Mark failed face as non-manifold
			if (currentPolyMesh()->has_face_status())
				currentPolyMesh()->status(fh).set_fixed_nonmanifold(true);

			// Mark edges of failed face as non-two-manifold
			if (currentPolyMesh()->has_edge_status()) {
				PolyMesh::FaceEdgeIter fe_it = currentPolyMesh()->fe_iter(fh);
				for(; fe_it; ++fe_it) {
					currentPolyMesh()->status(fe_it).set_fixed_nonmanifold(true);
				}
			}
		}
	}

	// Clear faces
	invalidFaces_.clear();
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINECURVE_SUPPORT
void OBJImporter::setCurveGroupId(const unsigned int _count, const int _id) {
  curvesMap_[_count] = _id;
}
#endif

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINECURVE_SUPPORT
int OBJImporter::getCurveGroupId(const unsigned int _count) {
  return curvesMap_[_count];
}
#endif

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINECURVE_SUPPORT
void OBJImporter::setCurveParentId(const int _curveGroup, const int _parentGroup) {
  curveParentGroupMap_[_curveGroup] = _parentGroup;
}
#endif

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINECURVE_SUPPORT
int OBJImporter::getCurveParentId(const int _curveGroup) {
  return curveParentGroupMap_[_curveGroup];
}
#endif

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
void OBJImporter::setSurfaceGroupId(const unsigned int _count, const int _id) {
  surfacesMap_[_count] = _id;
}
#endif

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
int OBJImporter::getSurfaceGroupId(const unsigned int _count) {
  return surfacesMap_[_count];
}
#endif

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
void OBJImporter::setSurfaceParentId(const int _surfaceGroup, const int _parentGroup) {
  surfaceParentGroupMap_[_surfaceGroup] = _parentGroup;
}
#endif

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINESURFACE_SUPPORT
int OBJImporter::getSurfaceParentId(const int _surfaceGroup) {
  return surfaceParentGroupMap_[_surfaceGroup];
}
#endif

//-----------------------------------------------------------------------------

//used materials
const std::vector<std::string> OBJImporter::usedMaterials(unsigned int _objectID){

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
  for (unsigned int i=0; i < usedMaterials_[ currentObject() ].size(); i++ )
    if ( usedMaterials_[ currentObject() ][i] == _materialName )
      return;
  
  usedMaterials_[ currentObject() ].push_back( _materialName );
  
  objectOptions_[ currentObject() ] |= TEXTURE;
}

///used vertices
void OBJImporter::useVertex(int _vertex_index){

    while(currentGroup_ >= (int)usedVertices_.size()) {
        usedVertices_.push_back(std::map<int,VertexHandle>());
    }

    usedVertices_[currentGroup_].insert(std::pair<int,VertexHandle>(_vertex_index,-1));

//  while( (int)usedVertices_.size() - 1 < (int) objectOptions_.size() )
//    usedVertices_.push_back( std::set<VertexHandle>() );
//
//  usedVertices_[ objectOptions_.size() ].insert( _vertex );
}

//-----------------------------------------------------------------------------

