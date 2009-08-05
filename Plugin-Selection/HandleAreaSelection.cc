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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH ){
      MeshSelection::setArea(PluginFunctions::triMesh(object) , _vertexList , HANDLEAREA, true);
      update_regions( PluginFunctions::triMesh(object) );
  } else if ( object->dataType() == DATA_POLY_MESH ){
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , _vertexList , HANDLEAREA, true);
      update_regions( PluginFunctions::polyMesh(object) );
  } else {
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::setArea(PluginFunctions::triMesh(object) , _vertexList , HANDLEAREA, false);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , _vertexList , HANDLEAREA, false);
  else {
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::setArea(PluginFunctions::triMesh(object) , HANDLEAREA, false);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , HANDLEAREA, false);
  else {
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::setArea(PluginFunctions::triMesh(object) , HANDLEAREA, true);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::setArea(PluginFunctions::polyMesh(object) , HANDLEAREA, true);
  else {
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      return MeshSelection::getArea(PluginFunctions::triMesh(object) , HANDLEAREA);
  else if ( object->dataType() == DATA_POLY_MESH )
      return MeshSelection::getArea(PluginFunctions::polyMesh(object) , HANDLEAREA);
  else {
      emit log(LOGERR,"getHandleVertices : Unsupported object Type" ); 
      return idList(0);;
  }
  
  return idList(0);
}

