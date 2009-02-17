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
//==== Edge selections
//=========================================================

void SelectionPlugin::selectEdges( int objectId , idList _edgeList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"selectEdges : unable to get object" ); 
    return;
  }
  
  if ( _edgeList.size() == 0 )
    return;
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::selectEdges(PluginFunctions::triMesh(object), _edgeList);
      break;
    case DATA_POLY_MESH :
      MeshSelection::selectEdges(PluginFunctions::polyMesh(object), _edgeList);
      break;
#ifdef ENABLE_POLYLINE_SUPPORT      
    case DATA_POLY_LINE :
      PolyLineSelection::selectEdges( PluginFunctions::polyLine(object) , _edgeList );
      break;
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
    case DATA_BSPLINE_CURVE :
      BSplineCurveSelection::selectEdges( PluginFunctions::splineCurve(object) , _edgeList );
      break;
#endif      
    default:
      emit log(LOGERR,"selectEdges : Unsupported object Type" ); 
      return;
  }
  
  QString selection = "selectEdges( ObjectId , [ " + QString::number(_edgeList[0]);
  
  for ( uint i = 1 ; i < _edgeList.size(); ++i) {
    selection +=  " , " + QString::number(_edgeList[i]);
  }
  
  selection += " ] )";
  
  object->update();
  emit scriptInfo( selection );
}

//=========================================================

void SelectionPlugin::unselectEdges( int objectId , idList _edgeList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"unselectEdges : unable to get object" ); 
    return;
  }
  
  if ( _edgeList.size() == 0 )
    return;
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::unselectEdges(PluginFunctions::triMesh(object), _edgeList);
      break;
    case DATA_POLY_MESH :
      MeshSelection::unselectEdges(PluginFunctions::polyMesh(object), _edgeList);
      break;
#ifdef ENABLE_POLYLINE_SUPPORT      
    case DATA_POLY_LINE :
      PolyLineSelection::unselectEdges( PluginFunctions::polyLine(object) , _edgeList );
      break;
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
    case DATA_BSPLINE_CURVE :
      BSplineCurveSelection::unselectEdges( PluginFunctions::splineCurve(object) , _edgeList );
      break;
#endif      
    default:
      emit log(LOGERR,"unselectEdges : Unsupported object Type" ); 
      return;
  }
  
  QString selection = "unselectVertices( ObjectId , [ " + QString::number(_edgeList[0]);
  
  for ( uint i = 1 ; i < _edgeList.size(); ++i) {
    selection +=  " , " + QString::number(_edgeList[i]);
  }
  
  selection += " ] )";
  
  object->update();
  emit scriptInfo( selection );
}

//=========================================================

void SelectionPlugin::selectAllEdges( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"selectAllVertices : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::selectAllEdges(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      MeshSelection::selectAllEdges(PluginFunctions::polyMesh(object));
      break;
#ifdef ENABLE_POLYLINE_SUPPORT      
    case DATA_POLY_LINE :
      PolyLineSelection::selectAllEdges( PluginFunctions::polyLine(object) );
      break;
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
    case DATA_BSPLINE_CURVE :
      BSplineCurveSelection::selectAllEdges( PluginFunctions::splineCurve(object) );
      break;    
#endif            
    default:
      emit log(LOGERR,"selectAllEdges : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "selectAllEdges( ObjectId )" );
}

//=========================================================

void SelectionPlugin::clearEdgeSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"clearEdgeSelection : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::clearEdgeSelection(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      MeshSelection::clearEdgeSelection(PluginFunctions::polyMesh(object));
      break;
#ifdef ENABLE_POLYLINE_SUPPORT      
    case DATA_POLY_LINE :
      PolyLineSelection::clearEdgeSelection( PluginFunctions::polyLine(object) );
      break;
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
    case DATA_BSPLINE_CURVE :
      BSplineCurveSelection::clearEdgeSelection( PluginFunctions::splineCurve(object) );
      break;
#endif      
    default:
      emit log(LOGERR,"clearEdgeSelection : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "clearEdgeSelection( ObjectId )" );
}

//=========================================================

void SelectionPlugin::invertEdgeSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"invertEdgeSelection : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::invertEdgeSelection(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      MeshSelection::invertEdgeSelection(PluginFunctions::polyMesh(object));
      break;
#ifdef ENABLE_POLYLINE_SUPPORT      
    case DATA_POLY_LINE :
      PolyLineSelection::invertEdgeSelection( PluginFunctions::polyLine(object) );
      break;
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
    case DATA_BSPLINE_CURVE :
      BSplineCurveSelection::invertEdgeSelection( PluginFunctions::splineCurve(object) );
      break;
#endif      
    default:
      emit log(LOGERR,"invertEdgeSelection : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "invertEdgeSelection( ObjectId )" );
}

//=========================================================

void SelectionPlugin::selectBoundaryEdges( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"selectBoundaryEdges : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::selectBoundaryEdges(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      MeshSelection::selectBoundaryEdges(PluginFunctions::polyMesh(object));
      break;
#ifdef ENABLE_POLYLINE_SUPPORT      
    case DATA_POLY_LINE :
      emit log(LOGERR,"selectBoundaryEdges : Not implemented on PolyLines" ); 
//       PolyLineSelection::selectAllEdges( PluginFunctions::polyLine(object) );
      break;
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
    case DATA_BSPLINE_CURVE :
      emit log(LOGERR,"selectBoundaryEdges : Not implemented on BSplineCurves" ); 
      break;  
#endif      
    default:
      emit log(LOGERR,"selectBoundaryEdges : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "selectBoundaryEdges( ObjectId )" );
}

//=========================================================

idList SelectionPlugin::getEdgeSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"getEdgeSelection : unable to get object" ); 
    return idList(0);
  }
  
  emit scriptInfo( "getEdgeSelection( ObjectId )" );
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      return MeshSelection::getEdgeSelection(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      return MeshSelection::getEdgeSelection(PluginFunctions::polyMesh(object));
      break;
#ifdef ENABLE_POLYLINE_SUPPORT      
    case DATA_POLY_LINE :
      return PolyLineSelection::getEdgeSelection( PluginFunctions::polyLine(object) );
      break;
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
    case DATA_BSPLINE_CURVE :
      return BSplineCurveSelection::getEdgeSelection( PluginFunctions::splineCurve(object) );
      break;    
#endif        
    default:
      emit log(LOGERR,"getEdgeSelection : Unsupported object Type" ); 
      return idList(0);
  }
  
  return idList(0);
  
}

  
  
  


