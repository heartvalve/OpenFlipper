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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



#include "SelectionPlugin.hh"

#include <iostream>

#include <MeshTools/MeshSelectionT.hh>

#ifdef ENABLE_POLYLINE_SUPPORT         
#include "PolyLine/polyLineSelectionT.hh"
#endif     

#ifdef ENABLE_BSPLINECURVE_SUPPORT         
#include "BSplineCurve/bSplineCurveSelectionT.hh"
#endif      

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//=========================================================
//==== Vertex selections
//=========================================================

void SelectionPlugin::selectVertices( int objectId , IdList _vertexList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("selectVertices : unable to get object") ); 
    return;
  }
  
  if ( _vertexList.size() == 0 )
    return;
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectVertices(PluginFunctions::triMesh(object), _vertexList);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectVertices(PluginFunctions::polyMesh(object), _vertexList);
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::selectVertices(PluginFunctions::tsplineMesh(object), _vertexList);
#endif
#ifdef ENABLE_POLYLINE_SUPPORT
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::selectVertices( PluginFunctions::polyLine(object) , _vertexList );
#endif   
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::selectVertices( PluginFunctions::splineCurve(object) , _vertexList );
#endif            
  else {
      emit log(LOGERR,tr("selectAllVertices : Unsupported object Type") ); 
      return;
  }
  
  QString selection = "selectVertices( ObjectId , [ " + QString::number(_vertexList[0]);
  
  for ( uint i = 1 ; i < _vertexList.size(); ++i) {
    selection +=  " , " + QString::number(_vertexList[i]);
  }
  
  selection += " ] )";
  
  object->update();
  emit scriptInfo( selection );
}

//=========================================================

void SelectionPlugin::unselectVertices( int objectId , IdList _vertexList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("unselectVertices : unable to get object") ); 
    return;
  }
  
  if ( _vertexList.size() == 0 )
    return;
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::unselectVertices(PluginFunctions::triMesh(object), _vertexList);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::unselectVertices(PluginFunctions::polyMesh(object), _vertexList);
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::unselectVertices(PluginFunctions::tsplineMesh(object), _vertexList);
#endif
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::unselectVertices( PluginFunctions::polyLine(object) , _vertexList );
#endif        
#ifdef ENABLE_BSPLINECURVE_SUPPORT      
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::unselectVertices( PluginFunctions::splineCurve(object) , _vertexList );
#endif            
  else {
      emit log(LOGERR,tr("unselectVertices : Unsupported object Type") ); 
      return;
  }
  
  QString selection = "unselectVertices( ObjectId , [ " + QString::number(_vertexList[0]);
  
  for ( uint i = 1 ; i < _vertexList.size(); ++i) {
    selection +=  " , " + QString::number(_vertexList[i]);
  }
  
  selection += " ] )";
  
  object->update();
  emit scriptInfo( selection );
}


//=========================================================


void SelectionPlugin::selectAllVertices( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("selectAllVertices : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectAllVertices(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectAllVertices(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::selectAllVertices(PluginFunctions::tsplineMesh(object));
#endif
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::selectAllVertices( PluginFunctions::polyLine(object) );
#endif           
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::selectAllVertices( PluginFunctions::splineCurve(object) );
#endif            
  else {
      emit log(LOGERR,tr("selectAllVertices : Unsupported object Type") ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "selectAllVertices( ObjectId )" );
}

//=========================================================

void SelectionPlugin::clearVertexSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("clearVertexSelection : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::clearVertexSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::clearVertexSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::clearVertexSelection(PluginFunctions::tsplineMesh(object));
#endif
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::clearVertexSelection( PluginFunctions::polyLine(object) );
#endif           
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::clearVertexSelection( PluginFunctions::splineCurve(object) );
#endif            
  else {
      emit log(LOGERR,tr("clearVertexSelection : Unsupported object Type") ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "clearVertexSelection( ObjectId )" );
}

//=========================================================

void SelectionPlugin::invertVertexSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("invertVertexSelection : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::invertVertexSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::invertVertexSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::invertVertexSelection(PluginFunctions::tsplineMesh(object));
#endif
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::invertVertexSelection( PluginFunctions::polyLine(object) );
#endif           
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::invertVertexSelection( PluginFunctions::splineCurve(object) );
#endif                 
  else {
      emit log(LOGERR,tr("invertVertexSelection : Unsupported object Type") ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "invertVertexSelection( ObjectId )" );
}

//=========================================================

void SelectionPlugin::selectBoundaryVertices( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("selectBoundaryVertices : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectBoundaryVertices(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectBoundaryVertices(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::selectBoundaryVertices(PluginFunctions::tsplineMesh(object));
#endif
  else {
      emit log(LOGERR,tr("selectBoundaryVertices : Unsupported object Type") ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "selectBoundaryVertices( ObjectId )" );
}

//=========================================================

void SelectionPlugin::selectClosestBoundaryVertices( int objectId, int VertexId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("selectClosestBoundaryVertices : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      closestBoundarySelection(PluginFunctions::triMesh(object), VertexId , VERTEX );
  else if ( object->dataType() == DATA_POLY_MESH )
      closestBoundarySelection(PluginFunctions::polyMesh(object), VertexId, VERTEX );
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      closestBoundarySelection(PluginFunctions::tsplineMesh(object), VertexId, VERTEX );
#endif
  else {
      emit log(LOGERR,tr("selectClosestBoundaryVertices : Unsupported object Type") ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "selectClosestBoundaryVertices( ObjectId , VertexId )" );
}

//=========================================================

void SelectionPlugin::shrinkVertexSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("shrinkVertexSelection : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::shrinkVertexSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::shrinkVertexSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::shrinkVertexSelection(PluginFunctions::tsplineMesh(object));
#endif
  else {
      emit log(LOGERR,tr("shrinkVertexSelection : Unsupported object Type") ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "shrinkVertexSelection( ObjectId )" );
}

//=========================================================

void SelectionPlugin::growVertexSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("growVertexSelection : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::growVertexSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::growVertexSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::growVertexSelection(PluginFunctions::tsplineMesh(object));
#endif
  else {
      emit log(LOGERR,tr("growVertexSelection : Unsupported object Type") ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "growVertexSelection( ObjectId )" );
}

//=========================================================

IdList SelectionPlugin::getVertexSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("getVertexSelection : unable to get object") ); 
    return IdList(0);
  }
  
  emit scriptInfo( "getVertexSelection( ObjectId )" );
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      return MeshSelection::getVertexSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      return MeshSelection::getVertexSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      return MeshSelection::getVertexSelection(PluginFunctions::tsplineMesh(object));
#endif
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      return PolyLineSelection::getVertexSelection( PluginFunctions::polyLine(object) );
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      return BSplineCurveSelection::getVertexSelection(PluginFunctions::splineCurve(object));
#endif            
  else {
      emit log(LOGERR,tr("getVertexSelection : Unsupported object Type") ); 
      return IdList(0);
  }
  
  return IdList(0);

}

//=========================================================
  
void SelectionPlugin::deleteSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("deleteSelection : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      deleteSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      deleteSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      deleteSelection(PluginFunctions::tsplineMesh(object));
#endif
  else {
      emit log(LOGERR,tr("deleteSelection : Unsupported object Type") ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "deleteSelection( ObjectId )" );
}

//=========================================================

/// colorize the vertex selection
void SelectionPlugin::colorizeVertexSelection(int objectId, int r, int g, int b )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"colorizeVertexSelection : unable to get object" );
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      colorizeSelection(PluginFunctions::triMesh(object), VERTEX, r, g, b);
  else if ( object->dataType() == DATA_POLY_MESH )
      colorizeSelection(PluginFunctions::polyMesh(object), VERTEX, r, g, b);
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      colorizeSelection(PluginFunctions::tsplineMesh(object), VERTEX, r, g, b);
#endif
  else {
      emit log(LOGERR,"colorizeVertexSelection : Unsupported object Type" );
      return;
  }
  
  emit scriptInfo( "colorizeVertexSelection( ObjectId, "
                  + QString::number(r) + ", " + QString::number(g) + ", " + QString::number(b) + " )" );

  emit updatedObject(objectId);
}
  

