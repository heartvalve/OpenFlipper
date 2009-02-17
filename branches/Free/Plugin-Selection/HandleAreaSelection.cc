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
//==== Modeling Area selections
//=========================================================

void SelectionPlugin::selectHandleVertices( int objectId , idList _vertexList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"selectHandleVertices : unable to get object" ); 
    return;
  }
  
  if ( _vertexList.size() == 0 )
    return;
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::setArea(PluginFunctions::triMesh(object) , _vertexList , HANDLEAREA, true);
      update_regions( PluginFunctions::triMesh(object) );
      break;
    case DATA_POLY_MESH :
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , _vertexList , HANDLEAREA, true);
      update_regions( PluginFunctions::polyMesh(object) );
      break;
    default:
      emit log(LOGERR,"selectHandleVertices : Unsupported object Type" ); 
      return;
  }
  
  QString selection = "selectHandleVertices( ObjectId , [ " + QString::number(_vertexList[0]);
  
  for ( uint i = 1 ; i < _vertexList.size(); ++i) {
    selection +=  " , " + QString::number(_vertexList[i]);
  }
  
  selection += " ] )";
  
  object->update();
  emit scriptInfo( selection );
}

//=========================================================

void SelectionPlugin::unselectHandleVertices( int objectId , idList _vertexList ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"unselectHandleVertices : unable to get object" ); 
    return;
  }
  
  if ( _vertexList.size() == 0 )
    return;
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::setArea(PluginFunctions::triMesh(object) , _vertexList , HANDLEAREA, false);
      break;
    case DATA_POLY_MESH :
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , _vertexList , HANDLEAREA, false);
      break;
    default:
      emit log(LOGERR,"unselectHandleVertices : Unsupported object Type" ); 
      return;
  }
  
  QString selection = "unselectHandleVertices( ObjectId , [ " + QString::number(_vertexList[0]);
  
  for ( uint i = 1 ; i < _vertexList.size(); ++i) {
    selection +=  " , " + QString::number(_vertexList[i]);
  }
  
  selection += " ] )";
  
  object->update();
  emit scriptInfo( selection );
}

//=========================================================

void SelectionPlugin::clearHandleVertices( int objectId ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"clearHandleVertices : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::setArea(PluginFunctions::triMesh(object) , HANDLEAREA, false);
      break;
    case DATA_POLY_MESH :
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , HANDLEAREA, false);
      break;
    default:
      emit log(LOGERR,"clearHandleVertices : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "clearHandleVertices( ObjectId )" );
}

//=========================================================

void SelectionPlugin::setAllHandleVertices( int objectId  ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"setAllHandleVertices : unable to get object" ); 
    return;
  }
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      MeshSelection::setArea(PluginFunctions::triMesh(object) , HANDLEAREA, true);
      break;
    case DATA_POLY_MESH :
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , HANDLEAREA, true);
      break;
    default:
      emit log(LOGERR,"setAllHandleVertices : Unsupported object Type" ); 
      return;
  }
  
  object->update();
  emit scriptInfo( "setAllHandleVertices( ObjectId )" );
}

//=========================================================

idList SelectionPlugin::getHandleVertices( int objectId  ) {
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) ) {
    emit log(LOGERR,"getHandleVertices : unable to get object" ); 
    return idList(0);
  }
  
  emit scriptInfo( "getHandleVertices( ObjectId )" );  
  
  switch ( object->dataType( ) ) {
    case DATA_TRIANGLE_MESH : 
      return MeshSelection::getArea(PluginFunctions::triMesh(object) , HANDLEAREA);
      break;
    case DATA_POLY_MESH :
      return MeshSelection::getArea(PluginFunctions::polyMesh(object) , HANDLEAREA);
      break;
    default:
      emit log(LOGERR,"getHandleVertices : Unsupported object Type" ); 
      return idList(0);;
  }
  
  return idList(0);
}

