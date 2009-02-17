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
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


//=========================================================
//==== Face selections
//=========================================================

void SelectionPlugin::selectFaces( int objectId , idList _faceList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"selectFaces : unable to get object" ); 
    return;
  }
  
  if ( _faceList.size() == 0 )
    return;
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::selectFaces(PluginFunctions::triMesh(object), _faceList);
      break;
    case DATA_POLY_MESH :
      MeshSelection::selectFaces(PluginFunctions::polyMesh(object), _faceList);
      break;
    default:
      emit log(LOGERR,"selectFaces : Unsupported object Type" ); 
      return;
  }
  
  QString selection = "selectFaces( ObjectId , [ " + QString::number(_faceList[0]);
  
  for ( uint i = 1 ; i < _faceList.size(); ++i) {
    selection +=  " , " + QString::number(_faceList[i]);
  }
  
  selection += " ] )";
  
  object->update();
  emit scriptInfo( selection );
}

//=========================================================

void SelectionPlugin::unselectFaces( int objectId , idList _faceList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"unselectFaces : unable to get object" ); 
    return;
  }
  
  if ( _faceList.size() == 0 )
    return;
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::unselectFaces(PluginFunctions::triMesh(object), _faceList);
      break;
    case DATA_POLY_MESH :
      MeshSelection::unselectFaces(PluginFunctions::polyMesh(object), _faceList);
      break;
    default:
      emit log(LOGERR,"unselectFaces : Unsupported object Type" ); 
      return;
  }
  
  QString selection = "unselectFaces( ObjectId , [ " + QString::number(_faceList[0]);
  
  for ( uint i = 1 ; i < _faceList.size(); ++i) {
    selection +=  " , " + QString::number(_faceList[i]);
  }
  
  selection += " ] )";
  
  object->update();
  emit scriptInfo( selection );
}

//=========================================================

void SelectionPlugin::selectAllFaces( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"selectAllFaces : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::selectAllFaces(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      MeshSelection::selectAllFaces(PluginFunctions::polyMesh(object));
      break;
    default:
      emit log(LOGERR,"selectAllFaces : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "selectAllFaces( ObjectId )" );
}

//=========================================================

void SelectionPlugin::clearFaceSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"clearFaceSelection : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::clearFaceSelection(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      MeshSelection::clearFaceSelection(PluginFunctions::polyMesh(object));
      break;
    default:
      emit log(LOGERR,"clearFaceSelection : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "clearFaceSelection( ObjectId )" );
}

//=========================================================

void SelectionPlugin::invertFaceSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"invertFaceSelection : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::invertFaceSelection(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      MeshSelection::invertFaceSelection(PluginFunctions::polyMesh(object));
      break;
    default:
      emit log(LOGERR,"invertFaceSelection : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "invertFaceSelection( ObjectId )" );
}

//=========================================================

void SelectionPlugin::selectBoundaryFaces( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"selectBoundaryFaces : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::selectBoundaryFaces(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      MeshSelection::selectBoundaryFaces(PluginFunctions::polyMesh(object));
      break;
    default:
      emit log(LOGERR,"selectBoundaryFaces : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "selectBoundaryFaces( ObjectId )" );
}


//=========================================================

void SelectionPlugin::shrinkFaceSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"shrinkFaceSelection : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::shrinkFaceSelection(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      MeshSelection::shrinkFaceSelection(PluginFunctions::polyMesh(object));
      break;
    default:
      emit log(LOGERR,"shrinkFaceSelection : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "shrinkFaceSelection( ObjectId )" );
}

//=========================================================

void SelectionPlugin::growFaceSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"growFaceSelection : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::growFaceSelection(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      MeshSelection::growFaceSelection(PluginFunctions::polyMesh(object));
      break;
    default:
      emit log(LOGERR,"growFaceSelection : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "growFaceSelection( ObjectId )" );
}
  
//=========================================================

idList SelectionPlugin::getFaceSelection( int objectId )
{
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"getFaceSelection : unable to get object" ); 
    return idList(0);
  }
  
  emit scriptInfo( "getFaceSelection( ObjectId )" );
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      return MeshSelection::getFaceSelection(PluginFunctions::triMesh(object));
      break;
    case DATA_POLY_MESH :
      return MeshSelection::getFaceSelection(PluginFunctions::polyMesh(object));
      break;
    default:
      emit log(LOGERR,"getFaceSelection : Unsupported object Type" ); 
      return idList(0);
  }
  

}  
  


