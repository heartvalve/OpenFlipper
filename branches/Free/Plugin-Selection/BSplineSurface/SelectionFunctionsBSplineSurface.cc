//=============================================================================
//
//  SelectionFunctions BSplineSurface
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  $Date$
//
//=============================================================================


#include <Plugin-Selection/SelectionPlugin.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//-----------------------------------------------------------------------------

#ifdef ENABLE_BSPLINESURFACE_SUPPORT

//-----------------------------------------------------------------------------

void
SelectionPlugin::
toggleBSplineSurfaceSelection(QMouseEvent* _event)
{
  if ( _event->type() != QEvent::MouseButtonPress )
    return;

  if (selectionType_ & VERTEX)
  {
    unsigned int node_idx, target_idx;
    ACG::Vec3d   hit_point;

    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(),node_idx, target_idx, &hit_point)) 
    {
      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) )
      {
        BSplineSurfaceObject* bsso = PluginFunctions::bsplineSurfaceObject( object );

        // toggle selection
        if( bsso->splineSurface()->controlpoint_selections_available() )
        {
          unsigned int max = bsso->splineSurface()->n_control_points_m() * bsso->splineSurface()->n_control_points_n();

          if( target_idx < max)
          {
            int idx_m = target_idx / bsso->splineSurface()->n_control_points_n();
            int idx_n = target_idx % bsso->splineSurface()->n_control_points_n();
            // TODO try global idx access

            if( bsso->splineSurface()->controlpoint_selection(idx_m, idx_n) == 0)
              bsso->splineSurface()->controlpoint_selection(idx_m, idx_n) = true;
            else
              bsso->splineSurface()->controlpoint_selection(idx_m, idx_n) = 0;
          }
        }
        emit updateView();
      }
    }
  }
}

//-----------------------------------------------------------------------------

#endif
