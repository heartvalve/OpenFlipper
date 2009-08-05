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

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//***********************************************************************************

// ContextMenu Action triggered
void SelectionPlugin::selectionContextMenu(QAction* _action) {
  QVariant contextObject = _action->data();
  int objectId = contextObject.toInt();
  
  if ( objectId == -1)
    return;
  
  BaseObjectData* object;
  if ( ! PluginFunctions::getObject(objectId,object) )
    return;
  
  if ( _action->text() == "All") 
    selectAll( objectId );
  else if ( _action->text() == "Clear")      
    clearSelection( objectId );
  else if ( _action->text() == "Invert")
    invertSelection( objectId );      
  else if ( _action->text() == "Shrink")
    shrinkSelection( objectId );        
  else if ( _action->text() == "Grow")
    growSelection( objectId );          
  else if ( _action->text() == "Boundary")
    selectBoundary( objectId );

  emit updatedObject(objectId);
}

//***********************************************************************************

void SelectionPlugin::selectAll( int objectID ){

  if (selectionType_ & VERTEX)
    selectAllVertices( objectID );

  if (selectionType_ & EDGE)
    selectAllEdges( objectID );

  if (selectionType_ & FACE)
    selectAllFaces( objectID );
}

//***********************************************************************************

void SelectionPlugin::clearSelection( int objectID ){

  if (selectionType_ & VERTEX)
    clearVertexSelection( objectID );

  if (selectionType_ & EDGE)
    clearEdgeSelection( objectID );

  if (selectionType_ & FACE)
    clearFaceSelection( objectID );
}

//***********************************************************************************

void SelectionPlugin::invertSelection( int objectID ){

  if (selectionType_ & VERTEX)
    invertVertexSelection( objectID );

  if (selectionType_ & EDGE)
    invertEdgeSelection( objectID );

  if (selectionType_ & FACE)
    invertFaceSelection( objectID );
}

//***********************************************************************************

void SelectionPlugin::shrinkSelection( int objectID ){

  if (selectionType_ & VERTEX)
    shrinkVertexSelection( objectID );

//   if (selectionType_ & EDGE)
//     shrinkEdgeSelection( objectID );

  if (selectionType_ & FACE)
    shrinkFaceSelection( objectID );
}

//***********************************************************************************

void SelectionPlugin::growSelection( int objectID ){

  if (selectionType_ & VERTEX)
    growVertexSelection( objectID );

//   if (selectionType_ & EDGE)
//     growEdgeSelection( objectID );

  if (selectionType_ & FACE)
    growFaceSelection( objectID );
}

//***********************************************************************************

void SelectionPlugin::selectBoundary( int objectID ){

  if (selectionType_ & VERTEX)
    selectBoundaryVertices( objectID );

  if (selectionType_ & EDGE)
    selectBoundaryEdges( objectID );

  if (selectionType_ & FACE)
    selectBoundaryFaces( objectID );
}