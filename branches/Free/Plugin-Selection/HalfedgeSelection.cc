/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 9595 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2010-06-17 12:48:23 +0200 (Thu, 17 Jun 2010) $                   *
 *                                                                           *
\*===========================================================================*/




#include "SelectionPlugin.hh"

#include <iostream>

#include <MeshTools/MeshSelectionT.hh>

// #ifdef ENABLE_POLYLINE_SUPPORT         
// #include "PolyLine/polyLineSelectionT.hh"
// #endif     

// #ifdef ENABLE_BSPLINECURVE_SUPPORT         
// #include "BSplineCurve/bSplineCurveSelectionT.hh"
// #endif      

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>




//=========================================================
//==== HalfEdge selections
//=========================================================

void SelectionPlugin::selectHalfedges( int objectId , IdList _hedgeList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("selectHalfedges : unable to get object") ); 
    return;
  }
  
  if ( _hedgeList.size() == 0 )
    return;

  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectHalfedges(PluginFunctions::triMesh(object), _hedgeList);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectHalfedges(PluginFunctions::polyMesh(object), _hedgeList);
  else{
      emit log(LOGERR,tr("selectHalfEdges : Unsupported object Type") ); 
      return;
  }
  
  QString selection = "selectHalfedges( ObjectId , [ " + QString::number(_hedgeList[0]);
  
  for ( uint i = 1 ; i < _hedgeList.size(); ++i) {
    selection +=  " , " + QString::number(_hedgeList[i]);
  }
  
  selection += " ] )";
  
  emit updatedObject(object->id(), UPDATE_SELECTION_HALFEDGES );
  emit scriptInfo( selection );
}

//=========================================================

void SelectionPlugin::unselectHalfedges( int objectId , IdList _hedgeList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("unselectHalfedges : unable to get object") ); 
    return;
  }
  
  if ( _hedgeList.size() == 0 )
    return;
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::unselectHalfedges(PluginFunctions::triMesh(object), _hedgeList);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::unselectHalfedges(PluginFunctions::polyMesh(object), _hedgeList);
  else{
      emit log(LOGERR,tr("unselectHalfedges : Unsupported object Type") ); 
      return;
  }
  
  QString selection = "unselectHalfedges( ObjectId , [ " + QString::number(_hedgeList[0]);
  
  for ( uint i = 1 ; i < _hedgeList.size(); ++i) {
    selection +=  " , " + QString::number(_hedgeList[i]);
  }
  
  selection += " ] )";
  
  emit updatedObject(object->id(), UPDATE_SELECTION_HALFEDGES);
  emit scriptInfo( selection );
}

//=========================================================

void SelectionPlugin::selectAllHalfedges( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("selectAllVertices : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectAllHalfedges(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectAllHalfedges(PluginFunctions::polyMesh(object));
  else{
      emit log(LOGERR,tr("selectAllHalfedges : Unsupported object Type") ); 
      return;
  }
  
  emit updatedObject(object->id(), UPDATE_SELECTION_HALFEDGES);
  emit scriptInfo( "selectAllHalfedges( ObjectId )" );
}

//=========================================================

void SelectionPlugin::clearHalfedgeSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("clearHalfedgeSelection : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::clearHalfedgeSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::clearHalfedgeSelection(PluginFunctions::polyMesh(object));
  else{
      emit log(LOGERR,tr("clearHalfedgeSelection : Unsupported object Type") ); 
      return;
  }
  
  emit updatedObject(object->id(), UPDATE_SELECTION_HALFEDGES);
  emit scriptInfo( "clearHalfedgeSelection( ObjectId )" );
}

//=========================================================

void SelectionPlugin::invertHalfedgeSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("invertHalfedgeSelection : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::invertHalfedgeSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::invertHalfedgeSelection(PluginFunctions::polyMesh(object));
  else{
      emit log(LOGERR,tr("invertHalfedgeSelection : Unsupported object Type") ); 
      return;
  }
  
  emit updatedObject(object->id(), UPDATE_SELECTION_HALFEDGES);
  emit scriptInfo( "invertHalfedgeSelection( ObjectId )" );
}

//=========================================================

void SelectionPlugin::selectBoundaryHalfedges( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("selectBoundaryHalfedges : unable to get object") ); 
    return;
  }
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectBoundaryHalfedges(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectBoundaryHalfedges(PluginFunctions::polyMesh(object));
  else{
      emit log(LOGERR,tr("selectBoundaryHalfedges : Unsupported object Type") ); 
      return;
  }
  
  emit updatedObject(object->id(), UPDATE_SELECTION_HALFEDGES);
  emit scriptInfo( "selectBoundaryEdges( ObjectId )" );
}

//=========================================================

IdList SelectionPlugin::getHalfedgeSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,tr("getHalfedgeSelection : unable to get object") ); 
    return IdList(0);
  }
  
  emit scriptInfo( "getHalfedgeSelection( ObjectId )" );
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      return MeshSelection::getHalfedgeSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      return MeshSelection::getHalfedgeSelection(PluginFunctions::polyMesh(object));
  else{
      emit log(LOGERR,tr("getHalfedgeSelection : Unsupported object Type") ); 
      return IdList(0);
  }
  
  return IdList(0);
  
}

//=========================================================

/// colorize the edge selection
void SelectionPlugin::colorizeHalfedgeSelection(int objectId, int r, int g, int b )
{
    BaseObjectData* object;
    if ( ! PluginFunctions::getObject(objectId,object) ) {
        emit log(LOGERR,"colorizeHalfedgeSelection : unable to get object" );
        return;
    }
    
    if ( object->dataType() == DATA_TRIANGLE_MESH )
        colorizeSelection(PluginFunctions::triMesh(object), HALFEDGE, r, g, b);
    else if ( object->dataType() == DATA_POLY_MESH )
        colorizeSelection(PluginFunctions::polyMesh(object), HALFEDGE, r, g, b);
    else {
        emit log(LOGERR,"colorizeHalfedgeSelection : Unsupported object Type" );
        return;
    }
    
    emit scriptInfo( "colorizeHalfedgeSelection( ObjectId, "
    + QString::number(r) + ", " + QString::number(g) + ", " + QString::number(b) + " )" );
    
    emit updatedObject(object->id(), UPDATE_COLOR);
}

  


