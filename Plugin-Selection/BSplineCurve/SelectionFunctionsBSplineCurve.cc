
#include <Plugin-Selection/SelectionPlugin.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINECURVE_SUPPORT    

//-----------------------------------------------------------------------------

void SelectionPlugin::toggleBSplineCurveSelection(QMouseEvent* _event)
{
  if ( _event->type() != QEvent::MouseButtonPress )
    return;

  if (selectionType_ & VERTEX){

    unsigned int node_idx, target_idx;
    ACG::Vec3d   hit_point;
  
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;
  
      if ( PluginFunctions::getPickedObject(node_idx, object) )
      {
        BSplineCurveObject* bsco = PluginFunctions::bsplineCurveObject( object );
  
        // toggle selection
        if( bsco->splineCurve()->vertex_selections_available() )
          if( target_idx < bsco->splineCurve()->n_control_points())
            if( bsco->splineCurve()->vertex_selection( target_idx) == 0)
              bsco->splineCurve()->vertex_selection( target_idx ) = 1;
        else
          bsco->splineCurve()->vertex_selection( target_idx ) = 0;
  
        emit updateView();
      }
    }

  }
}

#endif
