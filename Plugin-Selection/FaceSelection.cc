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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectFaces(PluginFunctions::triMesh(object), _faceList);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectFaces(PluginFunctions::polyMesh(object), _faceList);
  else{
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::unselectFaces(PluginFunctions::triMesh(object), _faceList);
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::unselectFaces(PluginFunctions::polyMesh(object), _faceList);
  else{
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectAllFaces(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectAllFaces(PluginFunctions::polyMesh(object));
  else{
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::clearFaceSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::clearFaceSelection(PluginFunctions::polyMesh(object));
  else{
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::invertFaceSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::invertFaceSelection(PluginFunctions::polyMesh(object));
  else{
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::selectBoundaryFaces(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::selectBoundaryFaces(PluginFunctions::polyMesh(object));
  else{
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::shrinkFaceSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::shrinkFaceSelection(PluginFunctions::polyMesh(object));
  else{
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      MeshSelection::growFaceSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      MeshSelection::growFaceSelection(PluginFunctions::polyMesh(object));
  else{
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
  
  if ( object->dataType() == DATA_TRIANGLE_MESH )
      return MeshSelection::getFaceSelection(PluginFunctions::triMesh(object));
  else if ( object->dataType() == DATA_POLY_MESH )
      return MeshSelection::getFaceSelection(PluginFunctions::polyMesh(object));
  else{
      emit log(LOGERR,"getFaceSelection : Unsupported object Type" ); 
      return idList(0);
  }
  

}  
  


