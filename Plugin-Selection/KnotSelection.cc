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

#ifdef ENABLE_BSPLINECURVE_SUPPORT         
#include "BSplineCurve/bSplineCurveSelectionT.hh"
#endif      

// #ifdef ENABLE_BSPLINESURFACE_SUPPORT         
// #include "BSplineSurface/bSplineSurfaceSelectionT.hh"
// #endif      

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//=========================================================
//==== Knot selections
//=========================================================

void 
SelectionPlugin::
selectKnots( int objectId , IdList _knotList ) 
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) 
  {
    emit log(LOGERR,tr("selectKnots : unable to get object") ); 
    return;
  }
  
  if ( _knotList.size() == 0 )
    return;

#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::selectKnots( PluginFunctions::splineCurve(object) , _knotList );
#endif  
#ifdef ENABLE_BSPLINESURFACE_SUPPORT   
//   else if ( object->dataType() == DATA_BSPLINE_SURFACE )
//       BSplineSurfaceSelection::selectKnots( PluginFunctions::splineSurface(object) , _knotList );
#endif            
  else {
      emit log(LOGERR,tr("selectKnots : Unsupported object Type") ); 
      return;
  }
  
  QString selection = "selectKnots( ObjectId , [ " + QString::number(_knotList[0]);
  
  for ( unsigned int i = 1 ; i < _knotList.size(); ++i) {
    selection +=  " , " + QString::number(_knotList[i]);
  }
  
  selection += " ] )";
  
//   emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
  emit updatedObject(object->id(), UPDATE_SELECTION_KNOTS);
  emit scriptInfo( selection );
}

//=========================================================

void 
SelectionPlugin::
unselectKnots( int objectId , IdList _knotList ) 
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) 
  {
    emit log(LOGERR,tr("unselectKnots : unable to get object") ); 
    return;
  }
  
  if ( _knotList.size() == 0 )
    return;
  

#ifdef ENABLE_BSPLINECURVE_SUPPORT      
  if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::unselectKnots( PluginFunctions::splineCurve(object) , _knotList );
#endif
#ifdef ENABLE_BSPLINESURFACE_SUPPORT      
//   else if ( object->dataType() == DATA_BSPLINE_SURFACE )
//       BSplineSurfaceSelection::unselectKnots( PluginFunctions::splineSurface(object) , _knotList );
#endif            
  else {
      emit log(LOGERR,tr("unselectKnots : Unsupported object Type") ); 
      return;
  }
  
  QString selection = "unselectKnots( ObjectId , [ " + QString::number(_knotList[0]);
  
  for ( uint i = 1 ; i < _knotList.size(); ++i) {
    selection +=  " , " + QString::number(_knotList[i]);
  }
  
  selection += " ] )";
  
//   emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
  emit updatedObject(object->id(), UPDATE_SELECTION_KNOTS);
  emit scriptInfo( selection );
}


//=========================================================


void 
SelectionPlugin::
selectAllKnots( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) 
  {
    emit log(LOGERR,tr("selectAllKnots : unable to get object") ); 
    return;
  }
  
  
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::selectAllKnots( PluginFunctions::splineCurve(object) );
#endif
#ifdef ENABLE_BSPLINESURFACE_SUPPORT   
//   else if ( object->dataType() == DATA_BSPLINE_SURFACE )
//       BSplineSurfaceSelection::selectAllKnots( PluginFunctions::splineSurface(object) );
#endif            
  else {
      emit log(LOGERR,tr("selectAllKnots : Unsupported object Type") ); 
      return;
  }
  
  emit updatedObject(object->id(), UPDATE_SELECTION_KNOTS);
//   emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
  emit scriptInfo( "selectAllKnots( ObjectId )" );
}

//=========================================================

void
SelectionPlugin::
clearKnotSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) 
  {
    emit log(LOGERR,tr("clearKnotSelection : unable to get object") ); 
    return;
  }
    
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::clearKnotSelection( PluginFunctions::splineCurve(object) );
#endif
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
//   else if ( object->dataType() == DATA_BSPLINE_SURFACE )
//       BSplineSurfaceSelection::clearKnotSelection( PluginFunctions::splineSurface(object) );
#endif            
  else {
      emit log(LOGERR,tr("clearKnotSelection : Unsupported object Type") ); 
      return;
  }
  
//   emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
  emit updatedObject(object->id(), UPDATE_SELECTION_KNOTS);
  emit scriptInfo( "clearKnotSelection( ObjectId )" );
}

//=========================================================

void
SelectionPlugin::
invertKnotSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) 
  {
    emit log(LOGERR,tr("invertKnotSelection : unable to get object") ); 
    return;
  }
  
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  if ( object->dataType() == DATA_BSPLINE_CURVE )
      BSplineCurveSelection::invertKnotSelection( PluginFunctions::splineCurve(object) );
#endif  
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
//   else if ( object->dataType() == DATA_BSPLINE_SURFACE )
//       BSplineSurfaceSelection::invertKnotSelection( PluginFunctions::splineSurface(object) );
#endif                 

  else {
      emit log(LOGERR,tr("invertKnotSelection : Unsupported object Type") ); 
      return;
  }
  
  emit updatedObject(object->id(), UPDATE_SELECTION_KNOTS);
  emit scriptInfo( "invertKnotSelection( ObjectId )" );
}

//=========================================================
/*
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
  
  emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
  emit scriptInfo( "selectBoundaryVertices( ObjectId )" );
}
*/
//=========================================================
/*
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
  
  emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
  emit scriptInfo( "selectClosestBoundaryVertices( ObjectId , VertexId )" );
}
*/
//=========================================================
/*
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
  
  emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
  emit scriptInfo( "shrinkVertexSelection( ObjectId )" );
}
*/
//=========================================================
/*
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
  
  emit updatedObject(object->id(), UPDATE_SELECTION_VERTICES);
  emit scriptInfo( "growVertexSelection( ObjectId )" );
}
*/
//=========================================================

IdList 
SelectionPlugin::
getKnotSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) 
  {
    emit log(LOGERR,tr("getKnotSelection : unable to get object") ); 
    return IdList(0);
  }
  
  emit scriptInfo( "getKnotSelection( ObjectId )" );
  
#ifdef ENABLE_BSPLINECURVE_SUPPORT   
  if ( object->dataType() == DATA_BSPLINE_CURVE )
      return BSplineCurveSelection::getKnotSelection(PluginFunctions::splineCurve(object));
#endif
#ifdef ENABLE_BSPLINESURFACE_SUPPORT   
//   else if ( object->dataType() == DATA_BSPLINE_SURFACE )
//       return BSplineSurfaceSelection::getKnotSelection(PluginFunctions::splineSurface(object));
#endif            
  else {
      emit log(LOGERR,tr("getKnotSelection : Unsupported object Type") ); 
      return IdList(0);
  }
  
  return IdList(0);

}

//=========================================================
