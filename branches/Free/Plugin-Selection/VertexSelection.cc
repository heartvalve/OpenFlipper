//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
// 
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




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

void SelectionPlugin::selectVertices( int objectId , idList _vertexList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"selectVertices : unable to get object" ); 
    return;
  }
  
  if ( _vertexList.size() == 0 )
    return;
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectVertices(PluginFunctions::triMesh(object), _vertexList);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectVertices(PluginFunctions::polyMesh(object), _vertexList);
#ifdef ENABLE_POLYLINE_SUPPORT
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::selectVertices( PluginFunctions::polyLine(object) , _vertexList );
#endif   
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::selectVertices( PluginFunctions::splineCurve(object) , _vertexList );
#endif            
  else {
      emit log(LOGERR,"selectAllVertices : Unsupported object Type" ); 
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

void SelectionPlugin::unselectVertices( int objectId , idList _vertexList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"unselectVertices : unable to get object" ); 
    return;
  }
  
  if ( _vertexList.size() == 0 )
    return;
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::unselectVertices(PluginFunctions::triMesh(object), _vertexList);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::unselectVertices(PluginFunctions::polyMesh(object), _vertexList);
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::unselectVertices( PluginFunctions::polyLine(object) , _vertexList );
#endif        
#ifdef ENABLE_BSPLINECURVE_SUPPORT      
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::unselectVertices( PluginFunctions::splineCurve(object) , _vertexList );
#endif            
  else {
      emit log(LOGERR,"unselectVertices : Unsupported object Type" ); 
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
    emit log(LOGERR,"selectAllVertices : unable to get object" ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectAllVertices(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectAllVertices(PluginFunctions::polyMesh(object));
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::selectAllVertices( PluginFunctions::polyLine(object) );
#endif           
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::selectAllVertices( PluginFunctions::splineCurve(object) );
#endif            
  else {
      emit log(LOGERR,"selectAllVertices : Unsupported object Type" ); 
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
    emit log(LOGERR,"clearVertexSelection : unable to get object" ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::clearVertexSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::clearVertexSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::clearVertexSelection( PluginFunctions::polyLine(object) );
#endif           
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::clearVertexSelection( PluginFunctions::splineCurve(object) );
#endif            
  else {
      emit log(LOGERR,"clearVertexSelection : Unsupported object Type" ); 
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
    emit log(LOGERR,"invertVertexSelection : unable to get object" ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::invertVertexSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::invertVertexSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::invertVertexSelection( PluginFunctions::polyLine(object) );
#endif           
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::invertVertexSelection( PluginFunctions::splineCurve(object) );
#endif                 
  else {
      emit log(LOGERR,"invertVertexSelection : Unsupported object Type" ); 
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
    emit log(LOGERR,"selectBoundaryVertices : unable to get object" ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectBoundaryVertices(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectBoundaryVertices(PluginFunctions::polyMesh(object));
  else {
      emit log(LOGERR,"selectBoundaryVertices : Unsupported object Type" ); 
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
    emit log(LOGERR,"selectClosestBoundaryVertices : unable to get object" ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      closestBoundarySelection(PluginFunctions::triMesh(object), VertexId , VERTEX );
  else if ( object->dataType() == DATA_POLY_MESH )
      closestBoundarySelection(PluginFunctions::polyMesh(object), VertexId, VERTEX );
  else {
      emit log(LOGERR,"selectClosestBoundaryVertices : Unsupported object Type" ); 
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
    emit log(LOGERR,"shrinkVertexSelection : unable to get object" ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::shrinkVertexSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::shrinkVertexSelection(PluginFunctions::polyMesh(object));
  else {
      emit log(LOGERR,"shrinkVertexSelection : Unsupported object Type" ); 
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
    emit log(LOGERR,"growVertexSelection : unable to get object" ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::growVertexSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::growVertexSelection(PluginFunctions::polyMesh(object));
  else {
      emit log(LOGERR,"growVertexSelection : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "growVertexSelection( ObjectId )" );
}

//=========================================================

idList SelectionPlugin::getVertexSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"getVertexSelection : unable to get object" ); 
    return idList(0);
  }
  
  emit scriptInfo( "getVertexSelection( ObjectId )" );
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      return MeshSelection::getVertexSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      return MeshSelection::getVertexSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      return PolyLineSelection::getVertexSelection( PluginFunctions::polyLine(object) );
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      return BSplineCurveSelection::getVertexSelection(PluginFunctions::splineCurve(object));
#endif            
  else {
      emit log(LOGERR,"getVertexSelection : Unsupported object Type" ); 
      return idList(0);
  }
  
  return idList(0);

}

//=========================================================
  
void SelectionPlugin::deleteSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"deleteSelection : unable to get object" ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      deleteSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      deleteSelection(PluginFunctions::polyMesh(object));
  else {
      emit log(LOGERR,"deleteSelection : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "deleteSelection( ObjectId )" );
}

  


