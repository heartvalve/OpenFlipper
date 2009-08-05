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


#include <Plugin-Selection/SelectionPlugin.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//-----------------------------------------------------------------------------

#ifdef ENABLE_POLYLINE_SUPPORT

#include <ObjectTypes/PolyLine/PolyLine.hh>

//-----------------------------------------------------------------------------

void SelectionPlugin::togglePolyLineSelection(QMouseEvent* _event)
{
  if ( _event->type() != QEvent::MouseButtonPress )
    return;

  unsigned int node_idx, target_idx;
  ACG::Vec3d   hit_point;

  // toggle vertex selection
  if (selectionType_ & VERTEX){

    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) )
      {
        PolyLineObject* plo = PluginFunctions::polyLineObject( object );

        if ( !plo )
          return;

        if( plo->line()->vertex_selections_available() ) {
          if( target_idx < plo->line()->n_vertices()) {
            if( plo->line()->vertex_selection( target_idx) == 0)
              plo->line()->vertex_selection( target_idx ) = 1;
            else
              plo->line()->vertex_selection( target_idx ) = 0;
          }
        } else
          plo->line()->vertex_selection( target_idx ) = 0;
      }
    }
  }

  // toggle edge selection
  if (selectionType_ & EDGE){

    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_EDGE, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) )
      {
        PolyLineObject* plo = PluginFunctions::polyLineObject( object );

        if ( !plo )
          return;

        if( plo->line()->edge_selections_available() ){
          if( target_idx < plo->line()->n_edges()) {
            if( plo->line()->edge_selection( target_idx) == 0)
              plo->line()->edge_selection( target_idx ) = 1;
            else
              plo->line()->edge_selection( target_idx ) = 0;
          }
        } else
          plo->line()->edge_selection( target_idx ) = 0;
      }
    }

  }

  emit updateView();
}

#endif

