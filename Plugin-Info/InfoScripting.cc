#include "InfoPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ACG/Geometry/Algorithms.hh>
#include <MeshTools/MeshInfoT.hh>


//------------------------------------------------------------------------------

/** \brief set the descriptions for scripting slots
 * 
 */
void InfoPlugin::setDescriptions(){

  emit setSlotDescription("vertexCount(int)","get total number of vertices for a given object",
                          QStringList("objectID"), QStringList("id of an object"));

  emit setSlotDescription("edgeCount(int)","get total number of edges for a given object",
                          QStringList("objectID"), QStringList("id of an object"));

  emit setSlotDescription("faceCount(int)","get total number of faces for a given object",
                          QStringList("objectID"), QStringList("id of an object"));

  emit setSlotDescription("boundaryCount(int)","get number of boundaries for a given object",
                          QStringList("objectID"), QStringList("id of an object"));

  emit setSlotDescription("componentCount(int)","get number of components for a given object",
                          QStringList("objectID"), QStringList("id of an object"));

  emit setSlotDescription("genus(int)","get the genus of a given object",
                          QStringList("objectID"), QStringList("id of an object"));

  emit setSlotDescription("cog(int)","get the center of gravity for a given object",
                          QStringList("objectID"), QStringList("id of an object"));

  emit setSlotDescription("boundingBoxMin(int)","get minimum point of the axis-aligned bounding box",
                          QStringList("objectID"), QStringList("id of an object"));

  emit setSlotDescription("boundingBoxMax(int)","get maximum point of the axis-aligned bounding box",
                          QStringList("objectID"), QStringList("id of an object"));

  emit setSlotDescription("boundingBoxSize(int)","get the size of the axis-aligned bounding box",
                          QStringList("objectID"), QStringList("id of an object"));


  emit setSlotDescription("edgeLength(int,int)","Get the length of an edge",
                          QString("ObjectId,EdgeHandle").split(","),
                          QString("id of the object, handle of an edge").split(","));

  emit setSlotDescription("faceArea(int,int)","Get the area of a face",
                          QString("ObjectId,FaceHandle").split(","),
                          QString("id of the object, handle of a face").split(","));

  emit setSlotDescription("aspectRatio(int,int)","Get the aspect ratio of a face",
                          QString("ObjectId,FaceHandle").split(","),
                          QString("id of the object, handle of a face").split(","));

  emit setSlotDescription("vertexValence(int,int)","Get the valence of a vertex",
                          QString("ObjectId,VertexHandle").split(","),
                          QString("id of the object, handle of a vertex").split(","));

}


//------------------------------------------------------------------------------

/** \brief get total number of vertices for a given object
 * 
 * @param _id object id
 * @return number of vertices or -1 if an error occured
 */
int InfoPlugin::vertexCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    return mesh->n_vertices();

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    return mesh->n_vertices();
  }
}


//------------------------------------------------------------------------------

/** \brief get total number of edges for a given object
 * 
 * @param _id object id
 * @return number of edges or -1 if an error occured
 */
int InfoPlugin::edgeCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    return mesh->n_edges();

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    return mesh->n_edges();
  }
}


//------------------------------------------------------------------------------

/** \brief get total number of faces for a given object
 * 
 * @param _id object id
 * @return number of faces or -1 if an error occured
 */
int InfoPlugin::faceCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    return mesh->n_faces();

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    return mesh->n_faces();
  }
}


//------------------------------------------------------------------------------

/** \brief get the number of boundaries for a given object
 * 
 * @param _id object id
 * @return number of boundaries or -1 if an error occured
 */
int InfoPlugin::boundaryCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    return MeshInfo::boundaryCount(mesh);

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    return MeshInfo::boundaryCount(mesh);
  }
}


//------------------------------------------------------------------------------

/** \brief get the number of components for a given object
 * 
 * @param _id object id
 * @return number of components or -1 if an error occured
 */
int InfoPlugin::componentCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    return MeshInfo::componentCount(mesh);

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    return MeshInfo::componentCount(mesh);
  }
}


//------------------------------------------------------------------------------

/** \brief get the genus of the given object
 * 
 * @param _id id of an object
 * @return the genus
 */
int InfoPlugin::genus(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }
///TODO this formula only works for closed objects: fix it
    return (1 - (mesh->n_vertices() - mesh->n_edges() + mesh->n_faces() ) / 2);

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    return (1 - (mesh->n_vertices() - mesh->n_edges() + mesh->n_faces() ) / 2);
  }
}


//------------------------------------------------------------------------------

/** \brief get the center of gravity 
 * 
 * @param _id id of an object
 * @return the center of gravity
 */
Vector InfoPlugin::cog(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return Vector();

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return Vector();
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return Vector();
    }

    return MeshInfo::cog(*mesh);

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return Vector();
    }

    return MeshInfo::cog(*mesh);
  }
}


//------------------------------------------------------------------------------

/** \brief get minimum point of the (axis aligned) bounding box
 * 
 * @param _id id of an object
 * @return minimum point of the bounding box
 */
Vector InfoPlugin::boundingBoxMin(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return Vector();

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return Vector();
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(*mesh, min, max);

    return min;

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(*mesh, min, max);

    return min;
  }
}


//------------------------------------------------------------------------------

/** \brief get maximum point of the (axis aligned) bounding box
 * 
 * @param _id id of an object
 * @return maximum point of the bounding box
 */
Vector InfoPlugin::boundingBoxMax(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return Vector();

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return Vector();
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(*mesh, min, max);

    return max;

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(*mesh, min, max);

    return max;
  }
}


//------------------------------------------------------------------------------

/** \brief get the size of the (axis aligned) bounding box
 * 
 * @param _id id of an object
 * @return size of the bounding box
 */
Vector InfoPlugin::boundingBoxSize(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return Vector();

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return Vector();
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(*mesh, min, max);

    return (max - min);

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(*mesh, min, max);

    return (max - min);
  }
}


//------------------------------------------------------------------------------

/** \brief get the length of an edge
 * 
 * @param _id id of an object
 * @param _edgeHandle edge handle
 * @return edge length or -1 if an error occured
 */
double InfoPlugin::edgeLength(int _id, int _edgeHandle)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1.0;

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return -1.0;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1.0;
    }

    TriMesh::EdgeHandle eh( _edgeHandle );

    if ( !eh.is_valid() ) {
      emit log(LOGERR,"Unable to get edge handle");
      return -1.0;
    }

    TriMesh::HalfedgeHandle hh = mesh->halfedge_handle( eh, 0 );
    TriMesh::Point p0 = mesh->point( mesh->from_vertex_handle(hh) );
    TriMesh::Point p1 = mesh->point( mesh->to_vertex_handle(hh) );

    return (p0 - p1).norm();

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1.0;
    }

    PolyMesh::EdgeHandle eh( _edgeHandle );

    if ( !eh.is_valid() ) {
      emit log(LOGERR,"Unable to get edge handle");
      return -1.0;
    }

    PolyMesh::HalfedgeHandle hh = mesh->halfedge_handle( eh, 0 );
    PolyMesh::Point p0 = mesh->point( mesh->from_vertex_handle(hh) );
    PolyMesh::Point p1 = mesh->point( mesh->to_vertex_handle(hh) );

    return (p0 - p1).norm();
  }
}


//------------------------------------------------------------------------------

/** \brief get the area of a face
 * 
 * @param _id id of an object
 * @param _faceHandle face handle
 * @return face area or -1 if an error occured
 */
double InfoPlugin::faceArea(int _id, int _faceHandle)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1.0;

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return -1.0;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1.0;
    }

    TriMesh::FaceHandle fh( _faceHandle );

    if ( !fh.is_valid() ) {
      emit log(LOGERR,"Unable to get face handle");
      return -1.0;
    }

    TriMesh::FaceVertexIter fv_it = mesh->fv_iter(fh);

    TriMesh::Point v0 = mesh->point( fv_it.handle() );
    ++fv_it;
    TriMesh::Point v1 = mesh->point( fv_it.handle() );
    ++fv_it;
    TriMesh::Point v2 = mesh->point( fv_it.handle() );

    return ACG::Geometry::triangleArea( v0, v1, v2 );

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1.0;
    }

    PolyMesh::FaceHandle fh( _faceHandle );

    if ( !fh.is_valid() ) {
      emit log(LOGERR,"Unable to get face handle");
      return -1.0;
    }

    PolyMesh::FaceVertexIter fv_it;

    std::vector< PolyMesh::Point > vertices;

    for (fv_it = mesh->fv_iter(fh); fv_it; ++fv_it)
      vertices.push_back( mesh->point( fv_it.handle() ) );

///TODO implement polygonArea
  emit log(LOGERR,"Not implemented yet");
  return -1.0;
//     return ACG::Geometry::polygonArea( vertices );
  }
}


//------------------------------------------------------------------------------

/** \brief get the aspect ratio of a face
 * 
 * @param _id id of an object
 * @param _faceHandle face handle
 * @return aspect ratio or -1 if an error occured
 */
double InfoPlugin::aspectRatio(int _id, int _faceHandle)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1.0;

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return -1.0;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1.0;
    }

    TriMesh::FaceHandle fh( _faceHandle );

    if ( !fh.is_valid() ) {
      emit log(LOGERR,"Unable to get face handle");
      return -1.0;
    }

    TriMesh::FaceVertexIter fv_it = mesh->fv_iter(fh);

    TriMesh::Point v0 = mesh->point( fv_it.handle() );
    ++fv_it;
    TriMesh::Point v1 = mesh->point( fv_it.handle() );
    ++fv_it;
    TriMesh::Point v2 = mesh->point( fv_it.handle() );

    return ACG::Geometry::aspectRatio( v0, v1, v2 );

  } else {

    emit log(LOGERR,"Aspect ratio can only be calculated for triangle meshes");
    return -1.0;
  }
}


//------------------------------------------------------------------------------

/** \brief get vertex valence
 * 
 * @param _id id of an object
 * @param _vertexHandle vertex handle
 * @return vertex valence or -1 if an error occured
 */
int InfoPlugin::vertexValence  (int _id, int _vertexHandle)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, "Unable to get object");
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    TriMesh::VertexHandle vh( _vertexHandle );

    if ( !vh.is_valid() ) {
      emit log(LOGERR,"Unable to get vertex handle");
      return -1;
    }

    //check valence
    int valence = 0;
    TriMesh::VertexVertexIter vv_it;
  
    for (vv_it=mesh->vv_iter( vh ); vv_it; ++vv_it)
      valence++;

    return valence;

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR,"Unable to get mesh");
      return -1;
    }

    PolyMesh::VertexHandle vh( _vertexHandle );

    if ( !vh.is_valid() ) {
      emit log(LOGERR,"Unable to get vertex handle");
      return -1;
    }

    //check valence
    int valence = 0;
    PolyMesh::VertexVertexIter vv_it;
  
    for (vv_it=mesh->vv_iter( vh ); vv_it; ++vv_it)
      valence++;

    return valence;
  }
}


