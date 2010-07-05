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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


//=============================================================================
//
//  SelectionFunctions BSplineSurface
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================

#ifdef ENABLE_BSPLINESURFACE_SUPPORT

#include <Plugin-Selection/SelectionPlugin.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include "../Knotvector/knotvectorSelectionT.hh"

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
  } // end of if VERTEX selection
  
  else if (selectionType_ & KNOT)
  {
    unsigned int node_idx, target_idx;
    ACG::Vec3d   hit_point;

    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_SPLINE, _event->pos(),node_idx, target_idx, &hit_point)) 
    {
      BaseObjectData* object;
      
      if ( PluginFunctions::getPickedObject(node_idx, object) )
      {
        BSplineSurfaceObject * bsso = PluginFunctions::bsplineSurfaceObject( object );
        BSplineSurface *       bss  = bsso->splineSurface();
          
        // toggle selection
        if (bss->get_knotvector_m_ref()->selections_available() && 
            bss->get_knotvector_n_ref()->selections_available()    )
        {
          // the target index we are getting here refers to the respective texel in the picking-texture
          // hence, we have to compute the actual u,v coordinates from this texel
          // given the uv coord, we finally compute the closes knot in u and v direction, respectively
          
          int numKnots_m = bss->n_knots_m();
          int numKnots_n = bss->n_knots_n();
          int order_m    = bss->degree_m() + 1;
          int order_n    = bss->degree_n() + 1;
            
          Knotvector * knotvec_u = bss->get_knotvector_m_ref();
          Knotvector * knotvec_v = bss->get_knotvector_n_ref();

          double minu = knotvec_u->getKnot(bss->degree_m());
          double minv = knotvec_v->getKnot(bss->degree_n());
          double maxu = knotvec_u->getKnot(numKnots_m - order_m);
          double maxv = knotvec_v->getKnot(numKnots_n - order_n);
            
          double udiff = maxu - minu;
          double vdiff = maxv - minv;

          int texres = bsso->splineSurfaceNode()->pick_texture_res();
          double curu = double (target_idx / texres) / double(texres) * udiff + minu;
          double curv = double (target_idx % texres) / double(texres) * vdiff + minv;
      
          ACG::Vec2d found_params(curu, curv); 
          
          // knots closest to parameters of hitpoint on the surface
          int knotIdx_m = KnotvectorSelection::closestKnot(knotvec_u, curu);
          int knotIdx_n = KnotvectorSelection::closestKnot(knotvec_v, curv);
          
          if( bss->get_knotvector_m_ref()->selection( knotIdx_m ) == 0)
            bss->get_knotvector_m_ref()->selection( knotIdx_m ) = 1;
          else
            bss->get_knotvector_m_ref()->selection( knotIdx_m ) = 0;
        
          if( bss->get_knotvector_n_ref()->selection( knotIdx_n ) == 0)
            bss->get_knotvector_n_ref()->selection( knotIdx_n ) = 1;
          else
            bss->get_knotvector_n_ref()->selection( knotIdx_n ) = 0;
        
        } // end of if selections available
      }
    }
  } // end of if KNOT selection  
  
}

#endif //ENABLE_BSPLINESURFACE_SUPPORT

//-----------------------------------------------------------------------------
