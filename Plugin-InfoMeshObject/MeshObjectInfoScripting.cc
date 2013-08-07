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

#include "MeshObjectInfoPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ACG/Geometry/Algorithms.hh>
#include <MeshTools/MeshInfoT.hh>


//------------------------------------------------------------------------------

/** \brief set the descriptions for scripting slots
 * 
 */
void InfoMeshObjectPlugin::setDescriptions(){

  emit setSlotDescription("vertexCount(int)",tr("get total number of vertices for a given object"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("edgeCount(int)",tr("get total number of edges for a given object"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("faceCount(int)",tr("get total number of faces for a given object"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("boundaryCount(int)",tr("get number of boundaries for a given object"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("componentCount(int)",tr("get number of components for a given object"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("genus(int)",tr("get the genus of a given object"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("cog(int)",tr("get the center of gravity for a given object"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("boundingBoxMin(int)",tr("get minimum point of the axis-aligned bounding box"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("boundingBoxMax(int)",tr("get maximum point of the axis-aligned bounding box"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));

  emit setSlotDescription("boundingBoxSize(int)",tr("get the size of the axis-aligned bounding box"),
                          QStringList(tr("objectID")), QStringList(tr("id of an object")));


  emit setSlotDescription("edgeLength(int,int)",tr("Get the length of an edge"),
                          QString(tr("ObjectId,EdgeHandle")).split(","),
                          QString(tr("id of the object, handle of an edge")).split(","));

  emit setSlotDescription("faceArea(int,int)",tr("Get the area of a face"),
                          QString(tr("ObjectId,FaceHandle")).split(","),
                          QString(tr("id of the object, handle of a face")).split(","));

  emit setSlotDescription("aspectRatio(int,int)",tr("Get the aspect ratio of a face"),
                          QString(tr("ObjectId,FaceHandle")).split(","),
                          QString(tr("id of the object, handle of a face")).split(","));

  emit setSlotDescription("vertexValence(int,int)",tr("Get the valence of a vertex"),
                          QString(tr("ObjectId,VertexHandle")).split(","),
                          QString(tr("id of the object, handle of a vertex")).split(","));

  emit setSlotDescription("minEdgeLength(int)",tr("Get the minimal edge length of an object"),
                          QStringList(tr("ObjectId")), QStringList(tr("id of the object")));

  emit setSlotDescription("maxEdgeLength(int)",tr("Get the maximal edge length of an object"),
                          QStringList(tr("ObjectId")), QStringList(tr("id of the object")));

  emit setSlotDescription("meanEdgeLength(int)",tr("Get the mean edge length of an object"),
                          QStringList(tr("ObjectId")), QStringList(tr("id of the object")));

}


//------------------------------------------------------------------------------

/** \brief get total number of vertices for a given object
 * 
 * @param _id object id
 * @return number of vertices or -1 if an error occured
 */
int InfoMeshObjectPlugin::vertexCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return mesh->n_vertices();

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
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
int InfoMeshObjectPlugin::edgeCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return mesh->n_edges();

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
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
int InfoMeshObjectPlugin::faceCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return mesh->n_faces();

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
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
int InfoMeshObjectPlugin::boundaryCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return MeshInfo::boundaryCount(mesh);

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
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
int InfoMeshObjectPlugin::componentCount(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    return MeshInfo::componentCount(mesh);

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
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
int InfoMeshObjectPlugin::genus(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }
///TODO this formula only works for closed objects: fix it
    return (1 - (mesh->n_vertices() - mesh->n_edges() + mesh->n_faces() ) / 2);

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
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
Vector InfoMeshObjectPlugin::cog(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return Vector();

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return Vector();
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return Vector();
    }

    return MeshInfo::cog(mesh);

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return Vector();
    }

    return MeshInfo::cog(mesh);
  }
}


//------------------------------------------------------------------------------

/** \brief get minimum point of the (axis aligned) bounding box
 * 
 * @param _id id of an object
 * @return minimum point of the bounding box
 */
Vector InfoMeshObjectPlugin::boundingBoxMin(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return Vector();

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return Vector();
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(mesh, min, max);

    return min;

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(mesh, min, max);

    return min;
  }
}


//------------------------------------------------------------------------------

/** \brief get maximum point of the (axis aligned) bounding box
 * 
 * @param _id id of an object
 * @return maximum point of the bounding box
 */
Vector InfoMeshObjectPlugin::boundingBoxMax(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return Vector();

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return Vector();
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(mesh, min, max);

    return max;

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(mesh, min, max);

    return max;
  }
}


//------------------------------------------------------------------------------

/** \brief get the size of the (axis aligned) bounding box
 * 
 * @param _id id of an object
 * @return size of the bounding box
 */
Vector InfoMeshObjectPlugin::boundingBoxSize(int _id)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return Vector();

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return Vector();
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(mesh, min, max);

    return (max - min);

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return Vector();
    }

    ACG::Vec3d min;
    ACG::Vec3d max;
    MeshInfo::getBoundingBox(mesh, min, max);

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
double InfoMeshObjectPlugin::edgeLength(int _id, int _edgeHandle)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1.0;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1.0;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1.0;
    }

    TriMesh::EdgeHandle eh( _edgeHandle );

    if ( !eh.is_valid() ) {
      emit log(LOGERR,tr("Unable to get edge handle"));
      return -1.0;
    }

    TriMesh::HalfedgeHandle hh = mesh->halfedge_handle( eh, 0 );
    TriMesh::Point p0 = mesh->point( mesh->from_vertex_handle(hh) );
    TriMesh::Point p1 = mesh->point( mesh->to_vertex_handle(hh) );

    return (p0 - p1).norm();

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1.0;
    }

    PolyMesh::EdgeHandle eh( _edgeHandle );

    if ( !eh.is_valid() ) {
      emit log(LOGERR,tr("Unable to get edge handle"));
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
double InfoMeshObjectPlugin::faceArea(int _id, int _faceHandle)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1.0;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1.0;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1.0;
    }

    TriMesh::FaceHandle fh( _faceHandle );

    if ( !fh.is_valid() ) {
      emit log(LOGERR,tr("Unable to get face handle"));
      return -1.0;
    }

    TriMesh::FaceVertexIter fv_it = mesh->fv_iter(fh);

    TriMesh::Point v0 = mesh->point( *fv_it );
    ++fv_it;
    TriMesh::Point v1 = mesh->point( *fv_it );
    ++fv_it;
    TriMesh::Point v2 = mesh->point( *fv_it );

    return ACG::Geometry::triangleArea( v0, v1, v2 );

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1.0;
    }

    PolyMesh::FaceHandle fh( _faceHandle );

    if ( !fh.is_valid() ) {
      emit log(LOGERR,tr("Unable to get face handle"));
      return -1.0;
    }

    PolyMesh::FaceVertexIter fv_it;

    std::vector< PolyMesh::Point > vertices;

    for (fv_it = mesh->fv_iter(fh); fv_it.is_valid(); ++fv_it)
      vertices.push_back( mesh->point( *fv_it ) );

///TODO implement polygonArea
  emit log(LOGERR,tr("Not implemented yet"));
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
double InfoMeshObjectPlugin::aspectRatio(int _id, int _faceHandle)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1.0;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1.0;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1.0;
    }

    TriMesh::FaceHandle fh( _faceHandle );

    if ( !fh.is_valid() ) {
      emit log(LOGERR,tr("Unable to get face handle"));
      return -1.0;
    }

    TriMesh::FaceVertexIter fv_it = mesh->fv_iter(fh);

    TriMesh::Point v0 = mesh->point( *fv_it );
    ++fv_it;
    TriMesh::Point v1 = mesh->point( *fv_it );
    ++fv_it;
    TriMesh::Point v2 = mesh->point( *fv_it );

    return ACG::Geometry::aspectRatio( v0, v1, v2 );

  } else {

    emit log(LOGERR,tr("Aspect ratio can only be calculated for triangle meshes"));
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
int InfoMeshObjectPlugin::vertexValence  (int _id, int _vertexHandle)
{

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_id,object) )
    return -1;

  if ( object == 0){
    emit log(LOGERR, tr("Unable to get object"));
    return -1;
  }

  if ( object->dataType(DATA_TRIANGLE_MESH) ) {
    TriMesh* mesh = PluginFunctions::triMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    TriMesh::VertexHandle vh( _vertexHandle );

    if ( !vh.is_valid() ) {
      emit log(LOGERR,tr("Unable to get vertex handle"));
      return -1;
    }

    //check valence
    int valence = 0;
    TriMesh::VertexVertexIter vv_it;
  
    for (vv_it=mesh->vv_iter( vh ); vv_it.is_valid(); ++vv_it)
      valence++;

    return valence;

  } else {
    PolyMesh* mesh = PluginFunctions::polyMesh(object);

    if ( mesh == 0 ) {
      emit log(LOGERR, tr("Unable to get mesh"));
      return -1;
    }

    PolyMesh::VertexHandle vh( _vertexHandle );

    if ( !vh.is_valid() ) {
      emit log(LOGERR,tr("Unable to get vertex handle"));
      return -1;
    }

    //check valence
    int valence = 0;
    PolyMesh::VertexVertexIter vv_it;
  
    for (vv_it=mesh->vv_iter( vh ); vv_it.is_valid(); ++vv_it)
      valence++;

    return valence;
  }
}

//------------------------------------------------------------------------------

/** \brief get minimal edge length for a given object
 * 
 * @param _id object id
 * @return minimal edge length or -1 if an error occured
 */
double InfoMeshObjectPlugin::minEdgeLength(int _id)
{
  double min, max, mean;

  if (getEdgeLengths (_id, min, max, mean))
    return min;
  else
    return -1;
}

//------------------------------------------------------------------------------

/** \brief get maximal edge length for a given object
 * 
 * @param _id object id
 * @return maximal edge length or -1 if an error occured
 */
double InfoMeshObjectPlugin::maxEdgeLength(int _id)
{
  double min, max, mean;

  if (getEdgeLengths (_id, min, max, mean))
    return max;
  else
    return -1;
}

//------------------------------------------------------------------------------

/** \brief get the mean edge length for a given object
 * 
 * @param _id object id
 * @return mean edge length or -1 if an error occured
 */
double InfoMeshObjectPlugin::meanEdgeLength(int _id)
{
  double min, max, mean;

  if (getEdgeLengths (_id, min, max, mean))
    return mean;
  else
    return -1;
}
