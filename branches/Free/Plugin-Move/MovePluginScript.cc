/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include "MovePlugin.hh"

#include "../OpenFlipper/BasePlugin/PluginFunctions.hh"
#ifdef ENABLE_POLYLINE_SUPPORT
#include <ObjectTypes/PolyLine/PolyLine.hh>
#endif

#include <MeshTools/MeshFunctions.hh>


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

  emit setSlotDescription("transform(int,Matrix4x4)",tr("transform object by given matrix."),
                          QString(tr("objectId,Matrix")).split(","), QString(tr("ID of an object, transformation matrix")).split(","));

  emit setSlotDescription("transform(int,idList,Matrix4x4)",tr("transform vertices by given matrix."),
                          QString(tr("objectId,VertexHandles,Matrix")).split(","),
                          QString(tr("ID of an object, List of vertex handles, transformation matrix")).split(","));

  emit setSlotDescription("transformSelection(int,Matrix4x4)",tr("transform current selection of an object by given matrix."),
                          QString(tr("objectId,Matrix")).split(","), QString(tr("ID of an object, transformation matrix")).split(","));

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
      mesh.set_point(v_it,mesh.point(v_it) + _vector );

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));
    PolyMesh::VertexIter v_it  = mesh.vertices_begin();
    PolyMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      mesh.set_point(v_it,mesh.point(v_it) + _vector );

  }
  #ifdef ENABLE_POLYLINE_SUPPORT
    else if ( object->dataType(DATA_POLY_LINE) ) {
      std::cerr << "Todo : translate PolyLine" << std::endl;
    }
  #endif
  #ifdef ENABLE_BSPLINE_CURVE_SUPPORT
    else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
      std::cerr << "Todo : translate BSplineCurve" << std::endl;
    }
  #endif

  emit updatedObject(_objectId);

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
void MovePlugin::translate( int _objectId , idList _vHandles, Vector _vector ){
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
  #ifdef ENABLE_POLYLINE_SUPPORT
    else if ( object->dataType(DATA_POLY_LINE) ) {
      std::cerr << "Todo : translate PolyLine" << std::endl;
    }
  #endif
  #ifdef ENABLE_BSPLINE_CURVE_SUPPORT
    else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
      std::cerr << "Todo : translate BSplineCurve" << std::endl;
    }
  #endif

  emit updatedObject(_objectId);

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
void MovePlugin::translateSelection( int _objectId , Vector _vector) {

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("translate : unable to get object" ));
    return;
  }

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));
    TriMesh::VertexIter v_it  = mesh.vertices_begin();
    TriMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).selected() )
        mesh.set_point(v_it,mesh.point(v_it) + _vector );

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));
    PolyMesh::VertexIter v_it  = mesh.vertices_begin();
    PolyMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).selected() )
        mesh.set_point(v_it,mesh.point(v_it) + _vector );

  }
  #ifdef ENABLE_POLYLINE_SUPPORT
    else if ( object->dataType(DATA_POLY_LINE) ) {
      std::cerr << "Todo : translate PolyLine" << std::endl;
    }
  #endif
  #ifdef ENABLE_BSPLINE_CURVE_SUPPORT
    else if ( object->dataType(DATA_BSPLINE_CURVE) ) {
      std::cerr << "Todo : translate BSplineCurve" << std::endl;
    }
  #endif

  emit updatedObject(_objectId);

  emit scriptInfo( "translate( ObjectId , Vector(" +
                   QString::number( _vector[0] ) + " , " +
                   QString::number( _vector[1] ) + " , " +
                   QString::number( _vector[2] ) + " ) )" );
}


//------------------------------------------------------------------------------

/** \brief tranform an object
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

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));
    TriMesh::VertexIter v_it  = mesh.vertices_begin();
    TriMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it){
      mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
      mesh.set_normal(v_it, _matrix.transform_vector( mesh.normal(v_it) ) );
    }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));
    PolyMesh::VertexIter v_it  = mesh.vertices_begin();
    PolyMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it){
      mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
      mesh.set_normal(v_it, _matrix.transform_vector( mesh.normal(v_it) ) );
    }

  }
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

  emit updatedObject(_objectId);

  QString matString;
  for (int i=0; i < 4; i++)
    for (int j=0; j < 4; j++)
      matString += " , " + QString::number( _matrix(i,j) );

  matString = matString.right( matString.length()-3 );

  emit scriptInfo( "transform( ObjectId , Matrix4x4(" + matString + " ) )" );
}


//------------------------------------------------------------------------------

/** \brief Tranform a set of vertex handles
 *
 * @param _objectId id of an object
 * @param _vHandles list of vertex handles
 * @param _matrix transformation matrix
 */
void MovePlugin::transform( int _objectId , idList _vHandles, Matrix4x4 _matrix ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("transform : unable to get object" ));
    return;
  }

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));
    TriMesh::VertexIter v_it  = mesh.vertices_begin();
    TriMesh::VertexIter v_end = mesh.vertices_end();

    for (uint i=0; i < _vHandles.size(); i++){
      TriMesh::VertexHandle vh( _vHandles[i] );
      mesh.set_point (vh, _matrix.transform_point ( mesh.point(vh) ) );
      mesh.set_normal(vh, _matrix.transform_vector( mesh.normal(vh) ) );
    }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));
    PolyMesh::VertexIter v_it  = mesh.vertices_begin();
    PolyMesh::VertexIter v_end = mesh.vertices_end();

    for (uint i=0; i < _vHandles.size(); i++){
      PolyMesh::VertexHandle vh( _vHandles[i] );
      mesh.set_point (vh, _matrix.transform_point ( mesh.point(vh) ) );
      mesh.set_normal(vh, _matrix.transform_vector( mesh.normal(vh) ) );
    }

  }
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

  emit updatedObject(_objectId);

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
 */
void MovePlugin::transformVertexSelection( int _objectId , Matrix4x4 _matrix ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("transform : unable to get object") );
    return;
  }

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));
    TriMesh::VertexIter v_it  = mesh.vertices_begin();
    TriMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).selected() )
      {
        mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
        mesh.set_normal(v_it, _matrix.transform_vector( mesh.normal(v_it) ) );
      }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));
    PolyMesh::VertexIter v_it  = mesh.vertices_begin();
    PolyMesh::VertexIter v_end = mesh.vertices_end();
    for (; v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).selected() )
      {
        mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
        mesh.set_normal(v_it, _matrix.transform_vector( mesh.normal(v_it) ) );
      }

  }
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

  emit updatedObject(_objectId);

  QString matString;
  for (int i=0; i < 4; i++)
    for (int j=0; j < 4; j++)
      matString += " , " + QString::number( _matrix(i,j) );

  matString = matString.right( matString.length()-3 );

  emit scriptInfo( "transformVertexSelection( ObjectId , Matrix4x4(" + matString + " ) )" );
}


//------------------------------------------------------------------------------

/** \brief transform face selection
 *
 * @param _objectId id of an object
 * @param _matrix transformation matrix
 */
void MovePlugin::transformFaceSelection( int _objectId , Matrix4x4 _matrix ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("transform : unable to get object") );
    return;
  }

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));

    //init tags
    TriMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
        mesh.status(v_it).set_tagged(false);

    TriMesh::FaceIter f_it  = mesh.faces_begin();
    TriMesh::FaceIter f_end = mesh.faces_end();
    for (; f_it!=f_end; ++f_it)
      if ( mesh.status(f_it).selected() )
      {
        for(TriMesh::FVIter fv_it = mesh.fv_iter(f_it); fv_it; ++fv_it)
          mesh.status(fv_it).set_tagged(true);
      }

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).tagged() ){
        mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
        mesh.set_normal(v_it, _matrix.transform_vector( mesh.normal(v_it) ) );
      }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));

    //init tags
    PolyMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
        mesh.status(v_it).set_tagged(false);

    PolyMesh::FaceIter f_it  = mesh.faces_begin();
    PolyMesh::FaceIter f_end = mesh.faces_end();
    for (; f_it!=f_end; ++f_it)
      if ( mesh.status(f_it).selected() )
      {
        for(PolyMesh::FVIter fv_it = mesh.fv_iter(f_it); fv_it; ++fv_it)
          mesh.status(fv_it).set_tagged(true);
      }

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).tagged() ){
        mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
        mesh.set_normal(v_it, _matrix.transform_vector( mesh.normal(v_it) ) );
      }

  }



  emit updatedObject(_objectId);

  QString matString;
  for (int i=0; i < 4; i++)
    for (int j=0; j < 4; j++)
      matString += " , " + QString::number( _matrix(i,j) );

  matString = matString.right( matString.length()-3 );

  emit scriptInfo( "transformFaceSelection( ObjectId , Matrix4x4(" + matString + " ) )" );
}


//------------------------------------------------------------------------------

/** \brief transform edge selection
 *
 * @param _objectId id of an object
 * @param _matrix transformation matrix
 */
void MovePlugin::transformEdgeSelection( int _objectId , Matrix4x4 _matrix ){

  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(_objectId,object) ) {
    emit log(LOGERR,tr("transform : unable to get object" ) );
    return;
  }

  if ( object->dataType( DATA_TRIANGLE_MESH ) ) {

    TriMesh&  mesh  = (*PluginFunctions::triMesh(object));

    //init tags
    TriMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
        mesh.status(v_it).set_tagged(false);

    TriMesh::EdgeIter e_it  = mesh.edges_begin();
    TriMesh::EdgeIter e_end = mesh.edges_end();
    for (; e_it!=e_end; ++e_it)
      if ( mesh.status(e_it).selected() )
      {
        TriMesh::HalfedgeHandle hh = mesh.halfedge_handle( e_it, 0 );

        mesh.status( mesh.from_vertex_handle( hh ) ).set_tagged(true);
        mesh.status( mesh.to_vertex_handle( hh ) ).set_tagged(true);
      }

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).tagged() ){
        mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
        mesh.set_normal(v_it, _matrix.transform_vector( mesh.normal(v_it) ) );
      }

  } else if ( object->dataType( DATA_POLY_MESH ) ) {

    PolyMesh&  mesh  = (*PluginFunctions::polyMesh(object));

    //init tags
    PolyMesh::VertexIter v_it, v_end( mesh.vertices_end() );
    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
        mesh.status(v_it).set_tagged(false);

    PolyMesh::EdgeIter e_it  = mesh.edges_begin();
    PolyMesh::EdgeIter e_end = mesh.edges_end();
    for (; e_it!=e_end; ++e_it)
      if ( mesh.status(e_it).selected() )
      {
        PolyMesh::HalfedgeHandle hh = mesh.halfedge_handle( e_it, 0 );

        mesh.status( mesh.from_vertex_handle( hh ) ).set_tagged(true);
        mesh.status( mesh.to_vertex_handle( hh ) ).set_tagged(true);
      }

    for (v_it=mesh.vertices_begin(); v_it!=v_end; ++v_it)
      if ( mesh.status(v_it).tagged() ){
        mesh.set_point (v_it, _matrix.transform_point ( mesh.point(v_it) ) );
        mesh.set_normal(v_it, _matrix.transform_vector( mesh.normal(v_it) ) );
      }

  }
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

  emit updatedObject(_objectId);

  QString matString;
  for (int i=0; i < 4; i++)
    for (int j=0; j < 4; j++)
      matString += " , " + QString::number( _matrix(i,j) );

  matString = matString.right( matString.length()-3 );

  emit scriptInfo( "transformEdgeSelection( ObjectId , Matrix4x4(" + matString + " ) )" );
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
