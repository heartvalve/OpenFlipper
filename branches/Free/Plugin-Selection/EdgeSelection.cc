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
//==== Edge selections
//=========================================================

void SelectionPlugin::selectEdges( int objectId , IdList _edgeList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("selectEdges : unable to get object") ); 
    return;
  }
  
  if ( _edgeList.size() == 0 )
    return;

  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectEdges(PluginFunctions::triMesh(object), _edgeList);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectEdges(PluginFunctions::polyMesh(object), _edgeList);
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::selectEdges(PluginFunctions::tsplineMesh(object), _edgeList);
#endif

#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::selectEdges( PluginFunctions::polyLine(object) , _edgeList );
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::selectEdges( PluginFunctions::splineCurve(object) , _edgeList );
#endif      
  else{
      emit log(LOGERR,tr("selectEdges : Unsupported object Type") ); 
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

void SelectionPlugin::unselectEdges( int objectId , IdList _edgeList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("unselectEdges : unable to get object") ); 
    return;
  }
  
  if ( _edgeList.size() == 0 )
    return;
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::unselectEdges(PluginFunctions::triMesh(object), _edgeList);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::unselectEdges(PluginFunctions::polyMesh(object), _edgeList);
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::unselectEdges(PluginFunctions::tsplineMesh(object), _edgeList);
#endif
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::unselectEdges( PluginFunctions::polyLine(object) , _edgeList );
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::unselectEdges( PluginFunctions::splineCurve(object) , _edgeList );
#endif      
  else{
      emit log(LOGERR,tr("unselectEdges : Unsupported object Type") ); 
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
    emit log(LOGERR,tr("selectAllVertices : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectAllEdges(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectAllEdges(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::selectAllEdges(PluginFunctions::tsplineMesh(object));
#endif
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::selectAllEdges( PluginFunctions::polyLine(object) );
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::selectAllEdges( PluginFunctions::splineCurve(object) );
#endif            
  else{
      emit log(LOGERR,tr("selectAllEdges : Unsupported object Type") ); 
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
    emit log(LOGERR,tr("clearEdgeSelection : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::clearEdgeSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::clearEdgeSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::clearEdgeSelection(PluginFunctions::tsplineMesh(object));
#endif
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::clearEdgeSelection( PluginFunctions::polyLine(object) );
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::clearEdgeSelection( PluginFunctions::splineCurve(object) );
#endif      
  else{
      emit log(LOGERR,tr("clearEdgeSelection : Unsupported object Type") ); 
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
    emit log(LOGERR,tr("invertEdgeSelection : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::invertEdgeSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::invertEdgeSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::invertEdgeSelection(PluginFunctions::tsplineMesh(object));
#endif
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      PolyLineSelection::invertEdgeSelection( PluginFunctions::polyLine(object) );
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::invertEdgeSelection( PluginFunctions::splineCurve(object) );
#endif      
  else{
      emit log(LOGERR,tr("invertEdgeSelection : Unsupported object Type") ); 
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
    emit log(LOGERR,tr("selectBoundaryEdges : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectBoundaryEdges(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectBoundaryEdges(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      MeshSelection::selectBoundaryEdges(PluginFunctions::tsplineMesh(object));
#endif
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      emit log(LOGERR,tr("selectBoundaryEdges : Not implemented on PolyLines") ); 
//       PolyLineSelection::selectAllEdges( PluginFunctions::polyLine(object) );
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      emit log(LOGERR,tr("selectBoundaryEdges : Not implemented on BSplineCurves") ); 
#endif      
  else{
      emit log(LOGERR,tr("selectBoundaryEdges : Unsupported object Type") ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "selectBoundaryEdges( ObjectId )" );
}

//=========================================================

IdList SelectionPlugin::getEdgeSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("getEdgeSelection : unable to get object") ); 
    return IdList(0);
  }
  
  emit scriptInfo( "getEdgeSelection( ObjectId )" );
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      return MeshSelection::getEdgeSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      return MeshSelection::getEdgeSelection(PluginFunctions::polyMesh(object));
#ifdef ENABLE_TSPLINEMESH_SUPPORT
  else if ( object->dataType() == DATA_TSPLINE_MESH )
      return MeshSelection::getEdgeSelection(PluginFunctions::tsplineMesh(object));
#endif
#ifdef ENABLE_POLYLINE_SUPPORT      
  else if ( object->dataType() == DATA_POLY_LINE )
      return PolyLineSelection::getEdgeSelection( PluginFunctions::polyLine(object) );
#endif      
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  else if ( object->dataType() == DATA_BSPLINE_CURVE )
      return BSplineCurveSelection::getEdgeSelection( PluginFunctions::splineCurve(object) );
#endif        
  else{
      emit log(LOGERR,tr("getEdgeSelection : Unsupported object Type") ); 
      return IdList(0);
  }
  
  return IdList(0);
  
}

//=========================================================

/// colorize the edge selection
void SelectionPlugin::colorizeEdgeSelection(int objectId, int r, int g, int b )
{
    BaseObjectData* object;
    if ( ! PluginFunctions::getObject(objectId,object) ) {
        emit log(LOGERR,"colorizeEdgeSelection : unable to get object" );
        return;
    }
    
    if ( object->dataType() == DATA_TRIANGLE_MESH )
        colorizeSelection(PluginFunctions::triMesh(object), EDGE, r, g, b);
    else if ( object->dataType() == DATA_POLY_MESH )
        colorizeSelection(PluginFunctions::polyMesh(object), EDGE, r, g, b);
#ifdef ENABLE_TSPLINEMESH_SUPPORT
    else if ( object->dataType() == DATA_TSPLINE_MESH )
        colorizeSelection(PluginFunctions::tsplineMesh(object), EDGE, r, g, b);
#endif
    else {
        emit log(LOGERR,"colorizeEdgeSelection : Unsupported object Type" );
        return;
    }
    
    emit scriptInfo( "colorizeEdgeSelection( ObjectId, "
    + QString::number(r) + ", " + QString::number(g) + ", " + QString::number(b) + " )" );
    
    emit updatedObject(objectId);
}

  


