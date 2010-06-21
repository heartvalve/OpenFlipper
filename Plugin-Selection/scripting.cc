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
 *   $Revision: 9595 $                                                       *
 *   $Author: moebius $                                                      *
 *   $Date: 2010-06-17 12:48:23 +0200 (Do, 17. Jun 2010) $                   *
 *                                                                           *
\*===========================================================================*/



#include "SelectionPlugin.hh"

#include <iostream>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

// ==================================================================

void SelectionPlugin::setSlotDescriptions() {

    emit setSlotDescription("setSelectionPrimitiveType(QString)",tr("Set selection primitive type. Possible types are VERTEX, EDGE, FACE, OBJECT, KNOT."),
        QStringList(tr("type")),
        QStringList(tr("The primitive type to be selected. Possible types are VERTEX, EDGE, FACE, OBJECT, KNOT.")));
                          
    emit setSlotDescription("setSelectionMetaphor(QString)",
        tr("Set selection pick mode. \
        Possible modes are \
        TOGGLE_SELECTION, PAINT_SPHERE_SELECTION, CLOSEST_BOUNDARY_SELECTION, LASSO_SELECTION, \
        VOLUME_SELECTION, CONNECTED_COMPONENT_SELECTION, FLOOD_FILL_SELECTION, CREATEMESH."),
        QStringList(tr("metaphor")),
        QStringList(tr("The selection metaphor. Choose between \
        TOGGLE_SELECTION, PAINT_SPHERE_SELECTION, CLOSEST_BOUNDARY_SELECTION, LASSO_SELECTION, \
        VOLUME_SELECTION, CONNECTED_COMPONENT_SELECTION, FLOOD_FILL_SELECTION, CREATEMESH.")));

}

// ==================================================================

void SelectionPlugin::setSelectionPrimitiveType(QString _type) {
  if ( _type == "VERTEX" )
    selectionType_ = VERTEX;
  else if ( _type == "EDGE" )
    selectionType_ = EDGE;
  else if ( _type == "FACE" )
    selectionType_ = FACE;
  else if ( _type == "OBJECT" )
    selectionType_ = OBJECT; 
  else if ( _type == "KNOT" )
    selectionType_ = KNOT;
  
  // Update GUI elements
  updateGUI();
}

void SelectionPlugin::setSelectionMetaphor(QString _metaphor) {
  
  // Is object selection active?
  // Since for object selection only toggle and lasso are available
  bool objectSelection = selectionType_ & OBJECT;
    
  // Set selection metaphor
  if(_metaphor == "TOGGLE_SELECTION")                   { if(!objectSelection) PluginFunctions::pickMode( TOGGLE_SELECTION ); }
  else if(_metaphor == "PAINT_SPHERE_SELECTION")        { PluginFunctions::pickMode( PAINT_SPHERE_SELECTION ); }
  else if(_metaphor == "CLOSEST_BOUNDARY_SELECTION")    { PluginFunctions::pickMode( CLOSEST_BOUNDARY_SELECTION ); }
  else if(_metaphor == "LASSO_SELECTION")               { PluginFunctions::pickMode( LASSO_SELECTION ); }
  else if(_metaphor == "VOLUME_LASSO_SELECTION")        { if(!objectSelection) PluginFunctions::pickMode( VOLUME_LASSO_SELECTION ); }
  else if(_metaphor == "CONNECTED_COMPONENT_SELECTION") { PluginFunctions::pickMode( CONNECTED_COMPONENT_SELECTION ); }
  else if(_metaphor == "FLOOD_FILL_SELECTION")          { if(!objectSelection) PluginFunctions::pickMode( FLOOD_FILL_SELECTION ); }
  else if(_metaphor == "CREATEMESH")                    { if(!objectSelection) PluginFunctions::pickMode( CREATEMESH ); }
  /* Surface lasso action -> polyline */
  /* else if(_metaphor == "") {} */
  
  // Update GUI elements
  updateGUI();
}
