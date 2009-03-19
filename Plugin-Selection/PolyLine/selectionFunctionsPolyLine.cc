

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

