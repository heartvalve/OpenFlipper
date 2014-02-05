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

#include "MovePlugin.hh"

#include "../OpenFlipper/BasePlugin/PluginFunctions.hh"
#ifdef ENABLE_POLYLINE_SUPPORT
#include <ObjectTypes/PolyLine/PolyLine.hh>
#endif
#ifdef ENABLE_TSPLINEMESH_SUPPORT
#include <ObjectTypes/TSplineMesh/TSplineMesh.hh>
#endif
#include <MeshTools/MeshFunctions.hh>

#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
#include <ObjectTypes/HexahedralMesh/HexahedralMesh.hh>
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
#endif

/** \brief Set Descriptions for Scripting Slots
 *
 */
void MovePlugin::setDescriptions(){

  emit setSlotDescription("translate(int,Vector)",tr("Translate object by given vector."),
                          QString(tr("objectId,Vector")).split(","), QString(tr("ID of an object, translation vector")).split(","));

  emit setSlotDescription("translate(int,idList,Vector)",tr("Translate vertices by given vector."),
                          QString(tr("objectId,VertexHandles,Vector")).split(","),
                          QString(tr("ID of an object, List of vertex handles, translation vector")).split(","));

  emit setSlotDescription("translateVertexSelection(int,Vector)",tr("Translate current vertex selection of an object by given vector."),
                          QString(tr("objectId,Vector")).split(","), QString(tr("ID of an object, translation vector")).split(","));

  emit setSlotDescription("translateFaceSelection(int,Vector)",tr("Translate current face selection of an object by given vector."),
                          QString(tr("objectId,Vector")).split(","), QString(tr("ID of an object, translation vector")).split(","));

  emit setSlotDescription("translateEdgeSelection(int,Vector)",tr("Translate current edge selection of an object by given vector."),
                          QString(tr("objectId,Vector")).split(","), QString(tr("ID of an object, translation vector")).split(","));

  emit setSlotDescription("transformHandleRegion(int,Matrix4x4)",tr("Transform handle region using the specified matrix."),
                          QString(tr("objectId,Matrix")).split(","), QString(tr("ID of an object, transformation matrix")).split(","));

  emit setSlotDescription("transform(int,Matrix4x4)",tr("transform object by given matrix."),
                          QString(tr("objectId,Matrix")).split(","), QString(tr("ID of an object, transformation matrix")).split(","));

  emit setSlotDescription("transform(int,IdList,Matrix4x4)",tr("transform vertices by given matrix."),
                          QString(tr("objectId,VertexHandles,Matrix")).split(","),
                          QString(tr("ID of an object, List of vertex handles, transformation matrix")).split(","));

  emit setSlotDescription("transformSelection(int,Matrix4x4)",tr("transform current selection of an object by given matrix."),
                          QString(tr("objectId,Matrix")).split(","), QString(tr("ID of an object, transformation matrix")).split(","));

  emit setSlotDescription("transformCellSelection(int,Matrix4x4)",tr("transform selected cells by given matrix."),
                          QString(tr("objectId,Matrix")).split(","),
                          QString(tr("ID of an object, transformation matrix")).split(","));

  emit setSlotDescription("transformVertexSelection(int,Matrix4x4)",tr("transform selected vertices by given matrix."),
                          QString(tr("objectId,Matrix")).split(","),
                          QString(tr("ID of an object, transformation matrix")).split(","));

  emit setSlotDescription("transformFaceSelection(int,Matrix4x4)",tr("transform selected faces by given matrix."),
                          QString(tr("objectId,Matrix")).split(","),
                          QString(tr("ID of an object, transformation matrix")).split(","));

  emit setSlotDescription("transformEdgeSelection(int,Matrix4x4)",tr("transform selected edges by given matrix."),
                          QString(tr("objectId,Matrix")).split(","),
                          QString(tr("ID of an object, transformation matrix")).split(","));

  emit setSlotDescription("setManipulatorPosition(int,Vector)",tr("Set the position of the manipulator."),
                          QString(tr("objectId,Position")).split(","), QString(tr("ID of an object, 3D point")).split(","));

  emit setSlotDescription("setManipulatorDirection(int,Vector, Vector)",tr("Set the direction of the manipulator."),
                          QString(tr("objectId,Direction, Direction")).split(","), QString(tr("ID of an object, x-direction, y-direction")).split(","));

  emit setSlotDescription("manipulatorPosition(int)",tr("Returns the position of an object's manipulator."),
                          QStringList(tr("objectId")), QStringList(tr("ID of an object")));

  emit setSlotDescription("manipulatorDirectionX(int)",tr("Returns the x-direction of an object's manipulator."),
                          QStringList(tr("objectId")), QStringList(tr("ID of an object")));

  emit setSlotDescription("manipulatorDirectionY(int)",tr("Returns the y-direction of an object's manipulator."),
                          QStringList(tr("objectId")), QStringList(tr("ID of an object")));

  emit setSlotDescription("manipulatorDirectionZ(int)",tr("Returns the z-direction of an object's manipulator."),
                          QStringList(tr("objectId")), QStringList(tr("ID of an object")));


  emit setSlotDescription("objectRenderingMatrixIdentity(int)",tr("Resets  the objects rendering matrix to identity."),
                          QStringList(tr("objectId")), QStringList(tr("ID of an object")));


  emit setSlotDescription("objectRenderingMatrixScale(int,double)",tr("Adds a scaling factor to the Object rendering Matrix in the scenegraph."),
                          QStringList(tr("objectId;Scaling Factor").split(";")), QStringList(tr("ID of an object; Scaling factor").split(";")));

  emit setSlotDescription("objectRenderingMatrixTranslate(int,Vector)",tr("Adds a translation to the Object rendering Matrix in the scenegraph."),
                          QStringList(tr("objectId;translation vector").split(";")), QStringList(tr("ID of an object;Translation vector").split(";")));

  emit setSlotDescription("objectRenderingMatrixRotate(int,Vector,double)",tr("Adds a Rotation to the Object rendering Matrix in the scenegraph."),
                          QStringList(tr("objectId;rotation axis;angle").split(";")), QStringList(tr("ID of an object;Rotation axis;angle").split(";")));

  emit setSlotDescription("getObjectRenderingMatrix(int)",tr("Returns the current object transformation matrix from the scenegraph."),
                            QStringList(tr("objectId").split(";")), QStringList(tr("ID of an object").split(";")));


}


//------------------------------------------------------------------------------

/** \brief Translate an object
 *
 * @param _objectId id of the object
 * @param _vector translation vector
 */
void MovePlugin::translate( int _objectId , Vector _vector) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("translate : unable to get object") );
    return;
  }

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));
    TriMesh::VertexIter v_it  = mesh.vertices_begin();
    TriMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      mesh.set_point(*v_it,mesh.point(*v_it) + _vector );

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));
    PolyMesh::VertexIter v_it  = mesh.vertices_begin();
    PolyMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      mesh.set_point(*v_it,mesh.point(*v_it) + _vector );

  }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType( DATA_TSPLINE_MESH ) ) {

    TSplineMesh&  mesh  = (*PluginFunctions::tsplineMesh(object));
    TSplineMesh::VertexIter v_it  = mesh.vertices_begin();
    TSplineMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      mesh.set_point(v_it,mesh.point(v_it) + _vector );

  }
#endif
#ifdef ENABLE_POLYLINE_SUPPORT
  else if ( object->dataType(DATA_POLY_LINE) ) {
    
    PolyLine& line = (* PluginFunctions::polyLine(object) );
    
    for ( int i = 0 ; i < (int)line.n_vertices(); ++i )
      line.point(i) = line.point(i)  + _vector; 
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
  else if ( object->dataType(DATA_HEXAHEDRAL_MESH) ) {

    HexahedralMesh& mesh = (*PluginFunctions::hexahedralMesh(object));
    OpenVolumeMesh::VertexIter v_it = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it != v_end; ++v_it)
      mesh.set_vertex(*v_it, mesh.vertex(*v_it) + _vector );
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  else if ( object->dataType(DATA_POLYHEDRAL_MESH) ) {

    PolyhedralMesh& mesh = (*PluginFunctions::polyhedralMesh(object));
    OpenVolumeMesh::VertexIter v_it = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it != v_end; ++v_it)
      mesh.set_vertex(*v_it, mesh.vertex(*v_it) + _vector );
  }
#endif
#ifdef ENABLE_BSPLINE_CURVE_SUPPORT
  else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
    std::cerr << "Todo : translate BSplineCurve" << std::endl;
  }
#endif

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  emit scriptInfo( "translate( ObjectId , Vector(" +
                   QString::number( _vector[0] ) + " , " +
                   QString::number( _vector[1] ) + " , " +
                   QString::number( _vector[2] ) + " ) )" );
}


//------------------------------------------------------------------------------

/** \brief translate a set of vertex handles
 *
 * @param _objectId id of an object
 * @param _vHandles list of vertex handles
 * @param _vector translation vector
 */
void MovePlugin::translate( int _objectId , IdList _vHandles, Vector _vector ){
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("translate : unable to get object") );
    return;
  }

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));

    for (uint i=0; i < _vHandles.size(); i++){
      TriMesh::VertexHandle vh( _vHandles[i] );
      mesh.set_point(vh  ,mesh.point( vh ) + _vector );
    }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));

    for (uint i=0; i < _vHandles.size(); i++){
      PolyMesh::VertexHandle vh( _vHandles[i] );
      mesh.set_point(vh  ,mesh.point( vh ) + _vector );
    }
  }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
   else if ( object->dataType( DATA_TSPLINE_MESH ) ) {

    TSplineMesh&  mesh  = (*PluginFunctions::tsplineMesh(object));

    for (uint i=0; i < _vHandles.size(); i++){
      TSplineMesh::VertexHandle vh( _vHandles[i] );
      mesh.set_point(vh  ,mesh.point( vh ) + _vector );
    }
   }
#endif
#ifdef ENABLE_POLYLINE_SUPPORT
  else if ( object->dataType(DATA_POLY_LINE) ) {
    
    PolyLine& line = (* PluginFunctions::polyLine(object) );
    
    const int max = line.n_vertices();
    
    for ( unsigned int i = 0 ; i < _vHandles.size(); ++i ) 
      if ( (_vHandles[i] > 0) && ( _vHandles[i] < max ) )
        line.point( _vHandles[i] ) = line.point( _vHandles[i] )  + _vector; 
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
  else if ( object->dataType(DATA_HEXAHEDRAL_MESH) ) {

    HexahedralMesh& mesh = (*PluginFunctions::hexahedralMesh(object));
    for (unsigned int i = 0; i < _vHandles.size(); ++i) {
      OpenVolumeMesh::VertexHandle v(_vHandles[i]);
      mesh.set_vertex(v, mesh.vertex(v) + _vector );
    }
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  else if ( object->dataType(DATA_POLYHEDRAL_MESH) ) {

    PolyhedralMesh& mesh = (*PluginFunctions::polyhedralMesh(object));
    for (unsigned int i = 0; i < _vHandles.size(); ++i) {
      OpenVolumeMesh::VertexHandle v(_vHandles[i]);
      mesh.set_vertex(v, mesh.vertex(v) + _vector );
    }
  }
#endif
#ifdef ENABLE_BSPLINE_CURVE_SUPPORT
  else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
    std::cerr << "Todo : translate BSplineCurve" << std::endl;
  }
#endif

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  emit scriptInfo( "translate( ObjectId , Vector(" +
                   QString::number( _vector[0] ) + " , " +
                   QString::number( _vector[1] ) + " , " +
                   QString::number( _vector[2] ) + " ) )" );
                   
}


//------------------------------------------------------------------------------

/** \brief translate vertex selection
 *
 * @param _objectId id of an object
 * @param _vector translation vector
 */
void MovePlugin::translateVertexSelection( int _objectId , Vector _vector) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("translate : unable to get object" ));
    return;
  }

  bool noneSelected = true;

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));
    TriMesh::VertexIter v_it  = mesh.vertices_begin();
    TriMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).selected() ) {
        noneSelected = false;
        mesh.set_point(*v_it,mesh.point(*v_it) + _vector );
      }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));
    PolyMesh::VertexIter v_it  = mesh.vertices_begin();
    PolyMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).selected() ) {
        noneSelected = false;
        mesh.set_point(*v_it,mesh.point(*v_it) + _vector );
      }
  }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType( DATA_TSPLINE_MESH ) ) {

    TSplineMesh&  mesh  = (*PluginFunctions::tsplineMesh(object));
    TSplineMesh::VertexIter v_it  = mesh.vertices_begin();
    TSplineMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).selected() ) {
        noneSelected = false;
        mesh.set_point(v_it,mesh.point(v_it) + _vector );
      }
  }
#endif
#ifdef ENABLE_POLYLINE_SUPPORT
  else if ( object->dataType(DATA_POLY_LINE) ) {
    
    PolyLine& line = (* PluginFunctions::polyLine(object) );
    
    for ( int i = 0 ; i < (int)line.n_vertices(); ++i )
      if ( line.vertex_selection(i) ) {
        noneSelected = false;
        line.point(i) = line.point(i)  + _vector; 
      }
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
  else if ( object->dataType(DATA_HEXAHEDRAL_MESH) ) {

    HexahedralMesh& mesh = (*PluginFunctions::hexahedralMesh(object));
    OpenVolumeMesh::StatusAttrib& statusAttrib = ((HexahedralMeshObject*)object)->status();
    OpenVolumeMesh::VertexIter v_it = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it != v_end; ++v_it)
      if (statusAttrib[*v_it].selected()) {
        noneSelected = false;
        mesh.set_vertex(*v_it, mesh.vertex(*v_it) + _vector );
      }
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  else if ( object->dataType(DATA_POLYHEDRAL_MESH) ) {

    PolyhedralMesh& mesh = (*PluginFunctions::polyhedralMesh(object));
    OpenVolumeMesh::StatusAttrib& statusAttrib = ((PolyhedralMeshObject*)object)->status();
    OpenVolumeMesh::VertexIter v_it = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it != v_end; ++v_it)
      if (statusAttrib[*v_it].selected()) {
        noneSelected = false;
        mesh.set_vertex(*v_it, mesh.vertex(*v_it) + _vector );
      }
  }
#endif
#ifdef ENABLE_BSPLINE_CURVE_SUPPORT
  else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
    std::cerr << "Todo : translate BSplineCurve" << std::endl;
  }
#endif

  if (noneSelected)
    return;

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  emit scriptInfo( "translate( ObjectId , Vector(" +
                   QString::number( _vector[0] ) + " , " +
                   QString::number( _vector[1] ) + " , " +
                   QString::number( _vector[2] ) + " ) )" );
}


//------------------------------------------------------------------------------

/** \brief translate face selection
 *
 * @param _objectId id of an object
 * @param _vector translation vector
 */
void MovePlugin::translateFaceSelection( int _objectId , Vector _vector) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("translate : unable to get object" ));
    return;
  }

  bool noneSelected = true;

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));

    // clear tags
    TriMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      mesh.status(*v_it).set_tagged(false);

    TriMesh::FaceIter f_it  = mesh.faces_begin();
    TriMesh::FaceIter f_end = mesh.faces_end();
    for (; f_it!=f_end; ++f_it)
      if ( mesh.status(*f_it).selected() )
      {
        for(TriMesh::FVIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it) {
          noneSelected = false;
          mesh.status(*fv_it).set_tagged(true);
        }
      }

    if (noneSelected)
      return;

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).tagged() )
        mesh.set_point(*v_it,mesh.point(*v_it) + _vector );

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));

    // clear tags
    PolyMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      mesh.status(*v_it).set_tagged(false);

    PolyMesh::FaceIter f_it  = mesh.faces_begin();
    PolyMesh::FaceIter f_end = mesh.faces_end();
    for (; f_it!=f_end; ++f_it)
      if ( mesh.status(*f_it).selected() )
      {
        for(TriMesh::FVIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it) {
          noneSelected = false;
          mesh.status(*fv_it).set_tagged(true);
        }
      }

    if (noneSelected)
      return;

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).tagged() )
        mesh.set_point(*v_it,mesh.point(*v_it) + _vector );
  }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType( DATA_TSPLINE_MESH ) ) {

    TSplineMesh&  mesh  = (*PluginFunctions::tsplineMesh(object));

    // clear tags
    TSplineMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      mesh.status(*v_it).set_tagged(false);

    TSplineMesh::FaceIter f_it  = mesh.faces_begin();
    TSplineMesh::FaceIter f_end = mesh.faces_end();
    for (; f_it!=f_end; ++f_it)
      if ( mesh.status(f_it).selected() )
      {
        for(TriMesh::FVIter fv_it = mesh.fv_iter(f_it); fv_it; ++fv_it) {
          noneSelected = false;
          mesh.status(fv_it).set_tagged(true);
        }
      }

    if (noneSelected)
      return;

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).tagged() )
        mesh.set_point(v_it,mesh.point(v_it) + _vector );
  }
#endif

  #ifdef ENABLE_POLYLINE_SUPPORT
    else if ( object->dataType(DATA_POLY_LINE) ) {

      PolyLine& line = (* PluginFunctions::polyLine(object) );

      for ( int i = 0 ; i < (int)line.n_vertices(); ++i )
        if ( line.vertex_selection(i) ) {
          noneSelected = false;
          line.point(i) = line.point(i)  + _vector;
        }

      if (noneSelected)
        return;
    }
  #endif
  #ifdef ENABLE_BSPLINE_CURVE_SUPPORT
    else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
      std::cerr << "Todo : translate BSplineCurve" << std::endl;
    }
  #endif

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  emit scriptInfo( "translate( ObjectId , Vector(" +
                   QString::number( _vector[0] ) + " , " +
                   QString::number( _vector[1] ) + " , " +
                   QString::number( _vector[2] ) + " ) )" );

}


//------------------------------------------------------------------------------

/** \brief translate edge selection
 *
 * @param _objectId id of an object
 * @param _vector translation vector
 */
void MovePlugin::translateEdgeSelection( int _objectId , Vector _vector) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("translate : unable to get object" ));
    return;
  }

  bool noneSelected = true;

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));

    // clear tags
    TriMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      mesh.status(*v_it).set_tagged(false);

    TriMesh::EdgeIter e_it  = mesh.edges_begin();
    TriMesh::EdgeIter e_end = mesh.edges_end();
    for (; e_it!=e_end; ++e_it)
      if ( mesh.status(*e_it).selected() )
      {
        noneSelected = false;
        TriMesh::HalfedgeHandle hh = mesh.halfedge_handle( *e_it, 0 );

        mesh.status( mesh.from_vertex_handle( hh ) ).set_tagged(true);
        mesh.status( mesh.to_vertex_handle( hh ) ).set_tagged(true);
      }

    if (noneSelected)
      return;

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).tagged() ){
        mesh.set_point(*v_it,mesh.point(*v_it) + _vector );
      }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));

    // clear tags
    PolyMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      mesh.status(*v_it).set_tagged(false);

    PolyMesh::EdgeIter e_it  = mesh.edges_begin();
    PolyMesh::EdgeIter e_end = mesh.edges_end();
    for (; e_it!=e_end; ++e_it)
      if ( mesh.status(*e_it).selected() )
      {
        noneSelected = false;
        PolyMesh::HalfedgeHandle hh = mesh.halfedge_handle( *e_it, 0 );

        mesh.status( mesh.from_vertex_handle( hh ) ).set_tagged(true);
        mesh.status( mesh.to_vertex_handle( hh ) ).set_tagged(true);
      }

    if (noneSelected)
      return;

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).tagged() ){
        mesh.set_point(*v_it,mesh.point(*v_it) + _vector );
      }
  }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType( DATA_TSPLINE_MESH ) ) {

    TSplineMesh&  mesh  = (*PluginFunctions::tsplineMesh(object));

    // clear tags
    TSplineMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      mesh.status(v_it).set_tagged(false);

    TSplineMesh::EdgeIter e_it  = mesh.edges_begin();
    TSplineMesh::EdgeIter e_end = mesh.edges_end();
    for (; e_it!=e_end; ++e_it)
      if ( mesh.status(e_it).selected() )
      {
        noneSelected = false;
        PolyMesh::HalfedgeHandle hh = mesh.halfedge_handle( e_it, 0 );

        mesh.status( mesh.from_vertex_handle( hh ) ).set_tagged(true);
        mesh.status( mesh.to_vertex_handle( hh ) ).set_tagged(true);
      }

    if (noneSelected)
      return;

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).tagged() ){
        mesh.set_point(v_it,mesh.point(v_it) + _vector );
      }
  }
#endif

  #ifdef ENABLE_POLYLINE_SUPPORT
    else if ( object->dataType(DATA_POLY_LINE) ) {

      PolyLine& line = (* PluginFunctions::polyLine(object) );

      for ( int i = 0 ; i < (int)line.n_vertices(); ++i )
        if ( line.vertex_selection(i) ) {
          noneSelected = false;
          line.point(i) = line.point(i)  + _vector;
        }

      if (noneSelected)
        return;
    }
  #endif
  #ifdef ENABLE_BSPLINE_CURVE_SUPPORT
    else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
      std::cerr << "Todo : translate BSplineCurve" << std::endl;
    }
  #endif

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  emit scriptInfo( "translate( ObjectId , Vector(" +
                   QString::number( _vector[0] ) + " , " +
                   QString::number( _vector[1] ) + " , " +
                   QString::number( _vector[2] ) + " ) )" );

}
//------------------------------------------------------------------------------

void MovePlugin::transformHandleRegion(int _objectId, Matrix4x4 _matrix) {

  BaseObjectData* object = NULL;
  if (!PluginFunctions::getObject(_objectId, object)) {
    emit log(LOGERR, tr("transformHandleRegion: Unable to get object!"));
    return;
  }

  if(object->dataType(DATA_TRIANGLE_MESH)) {

    TriMesh& mesh = (*PluginFunctions::triMesh(object));

    MeshFunctions::transformHandleVertices(_matrix, mesh);

  } else if(object->dataType(DATA_POLY_MESH)) {

    PolyMesh& mesh = (*PluginFunctions::polyMesh(object));

    MeshFunctions::transformHandleVertices(_matrix, mesh);
  }

  emit updatedObject(_objectId, UPDATE_GEOMETRY);
}

//------------------------------------------------------------------------------

/** \brief transform an object
 *
 * @param _objectId object id
 * @param _matrix transformation matrix
 */
void MovePlugin::transform( int _objectId , Matrix4x4 _matrix ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("transform : unable to get object" ));
    return;
  }

  Matrix4x4 normalMatrix = _matrix;
  normalMatrix.invert();
  normalMatrix.transpose();

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));
    TriMesh::VertexIter v_it  = mesh.vertices_begin();
    TriMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it){
      mesh.set_point (*v_it, _matrix.transform_point ( mesh.point(*v_it) ) );
      mesh.set_normal(*v_it, normalMatrix.transform_vector( mesh.normal(*v_it) ) );
    }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));
    PolyMesh::VertexIter v_it  = mesh.vertices_begin();
    PolyMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it){
      mesh.set_point (*v_it, _matrix.transform_point ( mesh.point(*v_it) ) );
      mesh.set_normal(*v_it, normalMatrix.transform_vector( mesh.normal(*v_it) ) );
    }
  }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType( DATA_TSPLINE_MESH ) ) {

    TSplineMesh&  mesh  = (*PluginFunctions::tsplineMesh(object));
    TSplineMesh::VertexIter v_it  = mesh.vertices_begin();
    TSplineMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it){
      mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
      mesh.set_normal(v_it, normalMatrix.transform_vector( mesh.normal(v_it) ) );
    }
  }
#endif
#ifdef ENABLE_POLYLINE_SUPPORT
  else if ( object->dataType(DATA_POLY_LINE) ) {
    
    PolyLine& line = (* PluginFunctions::polyLine(object) );
    
    for ( int i = 0 ; i < (int)line.n_vertices(); ++i )
      line.point(i) = _matrix.transform_point( line.point(i) ); 
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
  else if ( object->dataType(DATA_HEXAHEDRAL_MESH) ) {

    HexahedralMesh& mesh = (*PluginFunctions::hexahedralMesh(object));
    OpenVolumeMesh::NormalAttrib<HexahedralMesh>& normalAttrib = ((HexahedralMeshObject*)object)->normals();
    OpenVolumeMesh::VertexIter v_it = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it != v_end; ++v_it) {
      mesh.set_vertex(*v_it, _matrix.transform_point ( mesh.vertex(*v_it) ) );
      normalAttrib[*v_it] = normalMatrix.transform_vector( normalAttrib[*v_it] );
    }
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  else if ( object->dataType(DATA_POLYHEDRAL_MESH) ) {

    PolyhedralMesh& mesh = (*PluginFunctions::polyhedralMesh(object));
    OpenVolumeMesh::NormalAttrib<PolyhedralMesh>& normalAttrib = ((PolyhedralMeshObject*)object)->normals();
    OpenVolumeMesh::VertexIter v_it = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it != v_end; ++v_it) {
      mesh.set_vertex(*v_it, _matrix.transform_point ( mesh.vertex(*v_it) ) );
      normalAttrib[*v_it] = normalMatrix.transform_vector( normalAttrib[*v_it] );
    }
  }
#endif
#ifdef ENABLE_BSPLINE_CURVE_SUPPORT
  else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
    std::cerr << "Todo : transform BSplineCurve" << std::endl;
  }
#endif

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  QString matString;
  for (int i=0; i < 4; i++)
    for (int j=0; j < 4; j++)
      matString += " , " + QString::number( _matrix(i,j) );

  matString = matString.right( matString.length()-3 );

  emit scriptInfo( "transform( ObjectId , Matrix4x4(" + matString + " ) )" );
}


//------------------------------------------------------------------------------

/** \brief Transform a set of vertex handles
 *
 * @param _objectId id of an object
 * @param _vHandles list of vertex handles
 * @param _matrix transformation matrix
 */
void MovePlugin::transform( int _objectId , IdList _vHandles, Matrix4x4 _matrix ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("transform : unable to get object" ));
    return;
  }

  Matrix4x4 normalMatrix = _matrix;
  normalMatrix.invert();
  normalMatrix.transpose();

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));

    for (uint i=0; i < _vHandles.size(); i++){
      TriMesh::VertexHandle vh( _vHandles[i] );
      mesh.set_point (vh, _matrix.transform_point ( mesh.point(vh) ) );
      mesh.set_normal(vh, normalMatrix.transform_vector( mesh.normal(vh) ) );
    }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));

    for (uint i=0; i < _vHandles.size(); i++){
      PolyMesh::VertexHandle vh( _vHandles[i] );
      mesh.set_point (vh, _matrix.transform_point ( mesh.point(vh) ) );
      mesh.set_normal(vh, normalMatrix.transform_vector( mesh.normal(vh) ) );
    }
  }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType( DATA_TSPLINE_MESH ) ) {

    TSplineMesh&  mesh  = (*PluginFunctions::tsplineMesh(object));

    for (uint i=0; i < _vHandles.size(); i++){
      TSplineMesh::VertexHandle vh( _vHandles[i] );
      mesh.set_point (vh, _matrix.transform_point ( mesh.point(vh) ) );
      mesh.set_normal(vh, normalMatrix.transform_vector( mesh.normal(vh) ) );
    }
   }
#endif
#ifdef ENABLE_POLYLINE_SUPPORT
  else if ( object->dataType(DATA_POLY_LINE) ) {
    
    PolyLine& line = (* PluginFunctions::polyLine(object) );
    
    const int max = line.n_vertices();
    
    for ( unsigned int i = 0 ; i < _vHandles.size(); ++i ) 
      if ( (_vHandles[i] > 0) && ( _vHandles[i] < max ) )
        line.point( _vHandles[i] ) = _matrix.transform_point( line.point( _vHandles[i] ) ); 
    
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
  else if ( object->dataType(DATA_HEXAHEDRAL_MESH) ) {

    HexahedralMesh& mesh = (*PluginFunctions::hexahedralMesh(object));
    OpenVolumeMesh::NormalAttrib<HexahedralMesh>& normalAttrib = ((HexahedralMeshObject*)object)->normals();
    for (unsigned int i = 0; i < _vHandles.size(); ++i) {
      OpenVolumeMesh::VertexHandle v(_vHandles[i]);
      mesh.set_vertex(v, _matrix.transform_point ( mesh.vertex(v) ) );
      normalAttrib[v] = normalMatrix.transform_vector( normalAttrib[v] );
    }
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  else if ( object->dataType(DATA_POLYHEDRAL_MESH) ) {

    PolyhedralMesh& mesh = (*PluginFunctions::polyhedralMesh(object));
    OpenVolumeMesh::NormalAttrib<PolyhedralMesh>& normalAttrib = ((PolyhedralMeshObject*)object)->normals();
    for (unsigned int i = 0; i < _vHandles.size(); ++i) {
      OpenVolumeMesh::VertexHandle v(_vHandles[i]);
      mesh.set_vertex(v, _matrix.transform_point ( mesh.vertex(v) ) );
      normalAttrib[v] = normalMatrix.transform_vector( normalAttrib[v] );
    }
  }
#endif
#ifdef ENABLE_BSPLINE_CURVE_SUPPORT
  else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
    std::cerr << "Todo : transform BSplineCurve" << std::endl;
  }
#endif

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  QString matString;
  for (int i=0; i < 4; i++)
    for (int j=0; j < 4; j++)
      matString += " , " + QString::number( _matrix(i,j) );

  matString = matString.right( matString.length()-3 );

  emit scriptInfo( "transform( ObjectId , Matrix4x4(" + matString + " ) )" );
  
}


//------------------------------------------------------------------------------

/** \brief transform vertex selection
 *
 * @param _objectId id of an object
 * @param _matrix transformation matrix
 *
 * @return returns true if selected elements were transformed
 */
bool MovePlugin::transformVertexSelection( int _objectId , Matrix4x4 _matrix ){
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("transform : unable to get object") );
    return false;
  }

  Matrix4x4 normalMatrix = _matrix;
  normalMatrix.invert();
  normalMatrix.transpose();

  bool noneSelected = true;
  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));
    TriMesh::VertexIter v_it  = mesh.vertices_begin();
    TriMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).selected() )
      {
        noneSelected = false;
        mesh.set_point (*v_it, _matrix.transform_point ( mesh.point(*v_it) ) );
        mesh.set_normal(*v_it, normalMatrix.transform_vector( mesh.normal(*v_it) ) );
      }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));
    PolyMesh::VertexIter v_it  = mesh.vertices_begin();
    PolyMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).selected() )
      {
        noneSelected = false;
        mesh.set_point (*v_it, _matrix.transform_point ( mesh.point(*v_it) ) );
        mesh.set_normal(*v_it, normalMatrix.transform_vector( mesh.normal(*v_it) ) );
      }
  }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
   else if ( object->dataType( DATA_TSPLINE_MESH ) ) {

    TSplineMesh&  mesh  = (*PluginFunctions::tsplineMesh(object));
    TSplineMesh::VertexIter v_it  = mesh.vertices_begin();
    TSplineMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).selected() )
      {
        noneSelected = false;
        mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
        mesh.set_normal(v_it, normalMatrix.transform_vector( mesh.normal(v_it) ) );
      }
   }
#endif
#ifdef ENABLE_POLYLINE_SUPPORT
  else if ( object->dataType(DATA_POLY_LINE) ) {
    
    PolyLine& line = (* PluginFunctions::polyLine(object) );
    
    for ( int i = 0 ; i < (int)line.n_vertices(); ++i )
      if ( line.vertex_selection(i) ) {
        noneSelected = false;
        line.point(i) = _matrix.transform_point( line.point(i) );
      }
  }
#endif
#ifdef ENABLE_BSPLINE_CURVE_SUPPORT
  else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
    std::cerr << "Todo : transform BSplineCurve" << std::endl;
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
  else if ( object->dataType(DATA_HEXAHEDRAL_MESH) ) {
    HexahedralMesh& mesh = (*PluginFunctions::hexahedralMesh(object));
    OpenVolumeMesh::NormalAttrib<HexahedralMesh>& normalAttrib = ((HexahedralMeshObject*)object)->normals();
    OpenVolumeMesh::StatusAttrib& statusAttrib = ((HexahedralMeshObject*)object)->status();
    OpenVolumeMesh::VertexIter v_it  = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( statusAttrib[*v_it].selected() )
      {
        noneSelected = false;
        mesh.set_vertex(*v_it, _matrix.transform_point ( mesh.vertex(*v_it) ) );
        normalAttrib[*v_it] = normalMatrix.transform_vector( normalAttrib[*v_it] );
      }
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  else if ( object->dataType(DATA_POLYHEDRAL_MESH) ) {
    PolyhedralMesh& mesh = (*PluginFunctions::polyhedralMesh(object));
    OpenVolumeMesh::NormalAttrib<PolyhedralMesh>& normalAttrib = ((PolyhedralMeshObject*)object)->normals();
    OpenVolumeMesh::StatusAttrib& statusAttrib = ((PolyhedralMeshObject*)object)->status();
    OpenVolumeMesh::VertexIter v_it  = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( statusAttrib[*v_it].selected() )
      {
        noneSelected = false;
        mesh.set_vertex(*v_it, _matrix.transform_point ( mesh.vertex(*v_it) ) );
        normalAttrib[*v_it] = normalMatrix.transform_vector( normalAttrib[*v_it] );
      }
  }
#endif

  if (noneSelected)
    return false;

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  QString matString;
  for (int i=0; i < 4; i++)
    for (int j=0; j < 4; j++)
      matString += " , " + QString::number( _matrix(i,j) );

  matString = matString.right( matString.length()-3 );

  emit scriptInfo( "transformVertexSelection( ObjectId , Matrix4x4(" + matString + " ) )" );

  return true;
}


//------------------------------------------------------------------------------

/** \brief transform face selection
 *
 * @param _objectId id of an object
 * @param _matrix transformation matrix
 *
 * @return returns true if selected elements were transformed
 */
bool MovePlugin::transformFaceSelection( int _objectId , Matrix4x4 _matrix ){
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("transform : unable to get object") );
    return false;
  }

  Matrix4x4 normalMatrix = _matrix;
  normalMatrix.invert();
  normalMatrix.transpose();

  bool noneSelected = true;
  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));

    //init tags
    TriMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
        mesh.status(*v_it).set_tagged(false);

    TriMesh::FaceIter f_it  = mesh.faces_begin();
    TriMesh::FaceIter f_end = mesh.faces_end();
    for (; f_it!=f_end; ++f_it)
      if ( mesh.status(*f_it).selected() )
      {
        noneSelected = false;
        for(TriMesh::FVIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
          mesh.status(*fv_it).set_tagged(true);
      }

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).tagged() ){
        mesh.set_point (*v_it, _matrix.transform_point ( mesh.point(*v_it) ) );
        mesh.set_normal(*v_it, normalMatrix.transform_vector( mesh.normal(*v_it) ) );
      }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));

    //init tags
    PolyMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
        mesh.status(*v_it).set_tagged(false);

    PolyMesh::FaceIter f_it  = mesh.faces_begin();
    PolyMesh::FaceIter f_end = mesh.faces_end();
    for (; f_it!=f_end; ++f_it)
      if ( mesh.status(*f_it).selected() )
      {
        noneSelected = false;
        for(PolyMesh::FVIter fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
          mesh.status(*fv_it).set_tagged(true);
      }

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).tagged() ){
        mesh.set_point (*v_it, _matrix.transform_point ( mesh.point(*v_it) ) );
        mesh.set_normal(*v_it, normalMatrix.transform_vector( mesh.normal(*v_it) ) );
      }
  }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType( DATA_TSPLINE_MESH ) ) {

    TSplineMesh&  mesh  = (*PluginFunctions::tsplineMesh(object));

    //init tags
    TSplineMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
        mesh.status(v_it).set_tagged(false);

    TSplineMesh::FaceIter f_it  = mesh.faces_begin();
    TSplineMesh::FaceIter f_end = mesh.faces_end();
    for (; f_it!=f_end; ++f_it)
      if ( mesh.status(f_it).selected() )
      {
        noneSelected = false;
        for(TSplineMesh::FVIter fv_it = mesh.fv_iter(f_it); fv_it; ++fv_it)
          mesh.status(fv_it).set_tagged(true);
      }

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).tagged() ){
        mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
        mesh.set_normal(v_it, normalMatrix.transform_vector( mesh.normal(v_it) ) );
      }
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
  if ( object->dataType( DATA_HEXAHEDRAL_MESH ) ) {

    HexahedralMesh& mesh = (*PluginFunctions::hexahedralMesh(object));
    OpenVolumeMesh::NormalAttrib<HexahedralMesh>& normalAttrib = ((HexahedralMeshObject*)object)->normals();
    OpenVolumeMesh::StatusAttrib& statusAttrib = ((HexahedralMeshObject*)object)->status();
    OpenVolumeMesh::VertexIter v_begin  = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();

    //init tags
    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
        statusAttrib[*v_it].set_tagged(false);

    OpenVolumeMesh::FaceIter f_it  = mesh.faces_begin();
    OpenVolumeMesh::FaceIter f_end = mesh.faces_end();
    for (; f_it!=f_end; ++f_it)
      if ( statusAttrib[*f_it].selected() )
      {
        noneSelected = false;
        for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mesh.hfv_iter(mesh.halfface_handle(*f_it,0)); hfv_it.valid(); ++hfv_it)
            statusAttrib[*hfv_it].set_tagged(true);
      }

    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
      if ( statusAttrib[*v_it].tagged() )
      {
          mesh.set_vertex(*v_it, _matrix.transform_point ( mesh.vertex(*v_it) ) );
          normalAttrib[*v_it] = normalMatrix.transform_vector( normalAttrib[*v_it] );
      }

  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  else if ( object->dataType( DATA_POLYHEDRAL_MESH ) ) {

    PolyhedralMesh& mesh = (*PluginFunctions::polyhedralMesh(object));
    OpenVolumeMesh::NormalAttrib<PolyhedralMesh>& normalAttrib = ((PolyhedralMeshObject*)object)->normals();
    OpenVolumeMesh::StatusAttrib& statusAttrib = ((PolyhedralMeshObject*)object)->status();
    OpenVolumeMesh::VertexIter v_begin  = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();

    //init tags
    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
        statusAttrib[*v_it].set_tagged(false);

    OpenVolumeMesh::FaceIter f_it  = mesh.faces_begin();
    OpenVolumeMesh::FaceIter f_end = mesh.faces_end();
    for (; f_it!=f_end; ++f_it)
      if ( statusAttrib[*f_it].selected() )
      {
        noneSelected = false;
        for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = mesh.hfv_iter(mesh.halfface_handle(*f_it,0)); hfv_it.valid(); ++hfv_it)
            statusAttrib[*hfv_it].set_tagged(true);
      }

    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
      if ( statusAttrib[*v_it].tagged() )
      {
          mesh.set_vertex(*v_it, _matrix.transform_point ( mesh.vertex(*v_it) ) );
          normalAttrib[*v_it] = normalMatrix.transform_vector( normalAttrib[*v_it] );
      }
  }
#endif

  if (noneSelected)
    return false;

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  QString matString;
  for (int i=0; i < 4; i++)
    for (int j=0; j < 4; j++)
      matString += " , " + QString::number( _matrix(i,j) );

  matString = matString.right( matString.length()-3 );

  emit scriptInfo( "transformFaceSelection( ObjectId , Matrix4x4(" + matString + " ) )" );
  
  return true;
}


//------------------------------------------------------------------------------

/** \brief transform edge selection
 *
 * @param _objectId id of an object
 * @param _matrix transformation matrix
 *
 * @return returns true if selected elements were transformed
 */
bool MovePlugin::transformEdgeSelection( int _objectId , Matrix4x4 _matrix ){
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("transform : unable to get object" ) );
    return false;
  }

  Matrix4x4 normalMatrix = _matrix;
  normalMatrix.invert();
  normalMatrix.transpose();

  bool noneSelected = true;
  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));

    //init tags
    TriMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
        mesh.status(*v_it).set_tagged(false);

    TriMesh::EdgeIter e_it  = mesh.edges_begin();
    TriMesh::EdgeIter e_end = mesh.edges_end();
    for (; e_it!=e_end; ++e_it)
      if ( mesh.status(*e_it).selected() )
      {
        noneSelected = false;
        TriMesh::HalfedgeHandle hh = mesh.halfedge_handle( *e_it, 0 );

        mesh.status( mesh.from_vertex_handle( hh ) ).set_tagged(true);
        mesh.status( mesh.to_vertex_handle( hh ) ).set_tagged(true);
      }

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).tagged() ){
        mesh.set_point (*v_it, _matrix.transform_point ( mesh.point(*v_it) ) );
        mesh.set_normal(*v_it, normalMatrix.transform_vector( mesh.normal(*v_it) ) );
      }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));

    //init tags
    PolyMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
        mesh.status(*v_it).set_tagged(false);

    PolyMesh::EdgeIter e_it  = mesh.edges_begin();
    PolyMesh::EdgeIter e_end = mesh.edges_end();
    for (; e_it!=e_end; ++e_it)
      if ( mesh.status(*e_it).selected() )
      {
        noneSelected = false;
        PolyMesh::HalfedgeHandle hh = mesh.halfedge_handle( *e_it, 0 );

        mesh.status( mesh.from_vertex_handle( hh ) ).set_tagged(true);
        mesh.status( mesh.to_vertex_handle( hh ) ).set_tagged(true);
      }

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(*v_it).tagged() ){
        mesh.set_point (*v_it, _matrix.transform_point ( mesh.point(*v_it) ) );
        mesh.set_normal(*v_it, normalMatrix.transform_vector( mesh.normal(*v_it) ) );
      }
  }
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType( DATA_TSPLINE_MESH ) ) {

    TSplineMesh&  mesh  = (*PluginFunctions::tsplineMesh(object));

    //init tags
    TSplineMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
        mesh.status(v_it).set_tagged(false);

    TSplineMesh::EdgeIter e_it  = mesh.edges_begin();
    TSplineMesh::EdgeIter e_end = mesh.edges_end();
    for (; e_it!=e_end; ++e_it)
      if ( mesh.status(e_it).selected() )
      {
        noneSelected = false;
        TSplineMesh::HalfedgeHandle hh = mesh.halfedge_handle( e_it, 0 );

        mesh.status( mesh.from_vertex_handle( hh ) ).set_tagged(true);
        mesh.status( mesh.to_vertex_handle( hh ) ).set_tagged(true);
      }

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).tagged() ){
        mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
        mesh.set_normal(v_it, normalMatrix.transform_vector( mesh.normal(v_it) ) );
      }
  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
  if ( object->dataType( DATA_HEXAHEDRAL_MESH ) ) {

    HexahedralMesh& mesh = (*PluginFunctions::hexahedralMesh(object));
    OpenVolumeMesh::NormalAttrib<HexahedralMesh>& normalAttrib = ((HexahedralMeshObject*)object)->normals();
    OpenVolumeMesh::StatusAttrib& statusAttrib = ((HexahedralMeshObject*)object)->status();
    OpenVolumeMesh::VertexIter v_begin  = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();

    //init tags
    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
        statusAttrib[*v_it].set_tagged(false);

    OpenVolumeMesh::EdgeIter e_it  = mesh.edges_begin();
    OpenVolumeMesh::EdgeIter e_end = mesh.edges_end();
    for (; e_it!=e_end; ++e_it)
      if ( statusAttrib[*e_it].selected() )
      {
        noneSelected = false;
        OpenVolumeMesh::OpenVolumeMeshEdge e(mesh.edge(*e_it));
        statusAttrib[e.from_vertex()].set_tagged(true);
        statusAttrib[e.to_vertex()].set_tagged(true);
      }

    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
      if ( statusAttrib[*v_it].tagged() )
      {
          mesh.set_vertex(*v_it, _matrix.transform_point ( mesh.vertex(*v_it) ) );
          normalAttrib[*v_it] = normalMatrix.transform_vector( normalAttrib[*v_it] );
      }

  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  if ( object->dataType( DATA_POLYHEDRAL_MESH ) ) {

    PolyhedralMesh& mesh = (*PluginFunctions::polyhedralMesh(object));
    OpenVolumeMesh::NormalAttrib<PolyhedralMesh>& normalAttrib = ((PolyhedralMeshObject*)object)->normals();
    OpenVolumeMesh::StatusAttrib& statusAttrib = ((PolyhedralMeshObject*)object)->status();
    OpenVolumeMesh::VertexIter v_begin  = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();

    //init tags
    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
        statusAttrib[*v_it].set_tagged(false);

    OpenVolumeMesh::EdgeIter e_it  = mesh.edges_begin();
    OpenVolumeMesh::EdgeIter e_end = mesh.edges_end();
    for (; e_it!=e_end; ++e_it)
      if ( statusAttrib[*e_it].selected() )
      {
        noneSelected = false;
        OpenVolumeMesh::OpenVolumeMeshEdge e(mesh.edge(*e_it));
        statusAttrib[e.from_vertex()].set_tagged(true);
        statusAttrib[e.to_vertex()].set_tagged(true);
      }

    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
      if ( statusAttrib[*v_it].tagged() )
      {
          mesh.set_vertex(*v_it, _matrix.transform_point ( mesh.vertex(*v_it) ) );
          normalAttrib[*v_it] = normalMatrix.transform_vector( normalAttrib[*v_it] );
      }

  }
#endif
  
  #ifdef ENABLE_POLYLINE_SUPPORT
    else if ( object->dataType(DATA_POLY_LINE) ) {
      std::cerr << "Todo : transform PolyLine" << std::endl;
    }
  #endif
  #ifdef ENABLE_BSPLINE_CURVE_SUPPORT
    else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
      std::cerr << "Todo : transform BSplineCurve" << std::endl;
    }
  #endif

  if (noneSelected)
    return false;

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  QString matString;
  for (int i=0; i < 4; i++)
    for (int j=0; j < 4; j++)
      matString += " , " + QString::number( _matrix(i,j) );

  matString = matString.right( matString.length()-3 );

  emit scriptInfo( "transformEdgeSelection( ObjectId , Matrix4x4(" + matString + " ) )" );
  
  return true;
}

//------------------------------------------------------------------------------

/** \brief transform cell selection
 *
 * @param _objectId id of an object
 * @param _matrix transformation matrix
 *
 * @return returns true if selected elements were transformed
 */
bool MovePlugin::transformCellSelection( int _objectId , Matrix4x4 _matrix ){
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("transform : unable to get object") );
    return false;
  }

  Matrix4x4 normalMatrix = _matrix;
  normalMatrix.invert();
  normalMatrix.transpose();

  bool noneSelected = true;

#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
  if ( object->dataType( DATA_HEXAHEDRAL_MESH ) ) {

    HexahedralMesh& mesh = (*PluginFunctions::hexahedralMesh(object));
    OpenVolumeMesh::NormalAttrib<HexahedralMesh>& normalAttrib = ((HexahedralMeshObject*)object)->normals();
    OpenVolumeMesh::StatusAttrib& statusAttrib = ((HexahedralMeshObject*)object)->status();
    OpenVolumeMesh::VertexIter v_begin  = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();

    //init tags
    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
        statusAttrib[*v_it].set_tagged(false);

    OpenVolumeMesh::CellIter c_it  = mesh.cells_begin();
    OpenVolumeMesh::CellIter c_end = mesh.cells_end();
    for (; c_it!=c_end; ++c_it)
      if ( statusAttrib[*c_it].selected() )
      {
        noneSelected = false;
        for (OpenVolumeMesh::CellVertexIter cv_it = mesh.cv_iter(*c_it); cv_it.valid(); ++cv_it)
            statusAttrib[*cv_it].set_tagged(true);
      }

    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
      if ( statusAttrib[*v_it].tagged() )
      {
          mesh.set_vertex(*v_it, _matrix.transform_point ( mesh.vertex(*v_it) ) );
          normalAttrib[*v_it] = normalMatrix.transform_vector( normalAttrib[*v_it] );
      }

  }
#endif
#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
  else if ( object->dataType( DATA_POLYHEDRAL_MESH ) ) {

    PolyhedralMesh& mesh = (*PluginFunctions::polyhedralMesh(object));
    OpenVolumeMesh::NormalAttrib<PolyhedralMesh>& normalAttrib = ((PolyhedralMeshObject*)object)->normals();
    OpenVolumeMesh::StatusAttrib& statusAttrib = ((PolyhedralMeshObject*)object)->status();
    OpenVolumeMesh::VertexIter v_begin  = mesh.vertices_begin();
    OpenVolumeMesh::VertexIter v_end = mesh.vertices_end();

    //init tags
    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
        statusAttrib[*v_it].set_tagged(false);

    OpenVolumeMesh::CellIter c_it  = mesh.cells_begin();
    OpenVolumeMesh::CellIter c_end = mesh.cells_end();
    for (; c_it!=c_end; ++c_it)
      if ( statusAttrib[*c_it].selected() )
      {
        noneSelected = false;
        for (OpenVolumeMesh::CellVertexIter cv_it = mesh.cv_iter(*c_it); cv_it.valid(); ++cv_it)
            statusAttrib[*cv_it].set_tagged(true);
      }

    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it!=v_end; ++v_it)
      if ( statusAttrib[*v_it].tagged() )
      {
          mesh.set_vertex(*v_it, _matrix.transform_point ( mesh.vertex(*v_it) ) );
          normalAttrib[*v_it] = normalMatrix.transform_vector( normalAttrib[*v_it] );
      }
  }
#endif

  if (noneSelected)
    return false;

  emit updatedObject(_objectId, UPDATE_GEOMETRY);

  QString matString;
  for (int i=0; i < 4; i++)
    for (int j=0; j < 4; j++)
      matString += " , " + QString::number( _matrix(i,j) );

  matString = matString.right( matString.length()-3 );

  emit scriptInfo( "transformCellSelection( ObjectId , Matrix4x4(" + matString + " ) )" );

  return true;
}


//------------------------------------------------------------------------------

/** \brief set the position of the manipulator
 *
 * @param _objectId id of an object
 * @param _position new position
 */
void MovePlugin::setManipulatorPosition( int _objectId , Vector _position ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("setManipulatorPosition : unable to get object") );
    return;
  }


  manip_size_ = PluginFunctions::sceneRadius() * 0.1;


  object->manipPlaced( true );

  object->manipulatorNode()->loadIdentity();
  object->manipulatorNode()->set_center(_position);
  object->manipulatorNode()->set_draw_cylinder(true);
  object->manipulatorNode()->set_size(manip_size_ * manip_size_modifier_);
  object->manipulatorNode()->show();

  connect(object->manipulatorNode() , SIGNAL(manipulatorMoved(QtTranslationManipulatorNode*,QMouseEvent*)),
          this                      , SLOT(  manipulatorMoved(QtTranslationManipulatorNode*,QMouseEvent*)));

  connect(object->manipulatorNode() , SIGNAL(positionChanged(QtTranslationManipulatorNode*)),
          this                      , SLOT(  ManipulatorPositionChanged(QtTranslationManipulatorNode*)));

  lastActiveManipulator_ = object->id();

  emit updateView();

  emit scriptInfo( "setManipulatorPosition( ObjectId , Vector(" +
                   QString::number( _position[0] ) + " , " +
                   QString::number( _position[1] ) + " , " +
                   QString::number( _position[2] ) + " ) )" );
}


//------------------------------------------------------------------------------

/** \brief Get the position of the manipulator
 *
 * @param _objectId id of an object
 * @return current manipulator position
 */
Vector MovePlugin::manipulatorPosition( int _objectId ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("manipulatorPosition : unable to get object" ));
    return Vector();
  }

  return (Vector) object->manipulatorNode()->center();
}


//------------------------------------------------------------------------------

/** \brief set the direction of the manipulator
 *
 * @param _objectId id of an object
 * @param _directionX vector for the X direction
 * @param _directionY vector for the Y direction
 */
void MovePlugin::setManipulatorDirection( int _objectId , Vector _directionX, Vector _directionY ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("setManipulatorDirection : unable to get object") );
    return;
  }

  if ( !object->manipPlaced() ){
    emit log(LOGERR,tr("setManipulatorDirection : manipulator position has to be set first" ));
    return;
  }

  object->manipulatorNode()->set_direction(_directionX, _directionY);

  emit scriptInfo( "setManipulatorDirection( ObjectId , Vector(" +
                   QString::number( _directionX[0] ) + " , " +
                   QString::number( _directionX[1] ) + " , " +
                   QString::number( _directionX[2] ) + " ), Vector(" +
                   QString::number( _directionY[0] ) + " , " +
                   QString::number( _directionY[1] ) + " , " +
                   QString::number( _directionY[2] ) + " ) )" );
}


//------------------------------------------------------------------------------

/** \brief Get the x-direction of the manipulator
 *
 * @param _objectId id of an object
 * @return x-direction of the manipulator
 */
Vector MovePlugin::manipulatorDirectionX( int _objectId ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("manipulatorDirection : unable to get object" ));
    return Vector();
  }

  return (Vector) object->manipulatorNode()->directionX();
}


//------------------------------------------------------------------------------

/** \brief Get the y-direction of the manipulator
 *
 * @param _objectId id of an object
 * @return y-direction of the manipulator
 */
Vector MovePlugin::manipulatorDirectionY( int _objectId ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("manipulatorDirection : unable to get object" ));
    return Vector();
  }

  return (Vector) object->manipulatorNode()->directionY();
}


//------------------------------------------------------------------------------

/** \brief Get the z-direction of the manipulator
 *
 * @param _objectId id of an object
 * @return z-direction of the manipulator
 */
Vector MovePlugin::manipulatorDirectionZ( int _objectId ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("manipulatorDirection : unable to get object" ));
    return Vector();
  }

  return (Vector) object->manipulatorNode()->directionZ();
}

//------------------------------------------------------------------------------

void MovePlugin::objectRenderingMatrixIdentity(int _objectId) {
  BaseObjectData* object;

  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("objectRenderingMatrixIdentity : unable to get object" ));
    return ;
  }

  object->manipulatorNode()->loadIdentity();

  emit updatedObject(_objectId,UPDATE_VISIBILITY);
}

//------------------------------------------------------------------------------

void MovePlugin::objectRenderingMatrixScale(int _objectId, double _s) {
  BaseObjectData* object;

  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("objectRenderingMatrixScale : unable to get object" ));
    return ;
  }

  object->manipulatorNode()->scale(_s);

  emit updatedObject(_objectId,UPDATE_VISIBILITY);
}

//------------------------------------------------------------------------------

void MovePlugin::objectRenderingMatrixTranslate(int _objectId, Vector _translation) {
  BaseObjectData* object;

  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("objectRenderingMatrixTranslate : unable to get object" ));
    return ;
  }

  object->manipulatorNode()->translate(_translation);

  emit updatedObject(_objectId,UPDATE_VISIBILITY);
}

//------------------------------------------------------------------------------

void MovePlugin::objectRenderingMatrixRotate(int _objectId, Vector _axis, double _angle) {
  BaseObjectData* object;

  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("objectRenderingMatrixRotate : unable to get object" ));
    return ;
  }

  object->manipulatorNode()->rotate(_angle,_axis);

  emit updatedObject(_objectId,UPDATE_VISIBILITY);
}

//------------------------------------------------------------------------------

Matrix4x4 MovePlugin::getObjectRenderingMatrix(int _objectId) {
  BaseObjectData* object;

  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("getObjectRenderingMatrix : unable to get object" ));
    return Matrix4x4();
  }

  return object->manipulatorNode()->matrix();
}

