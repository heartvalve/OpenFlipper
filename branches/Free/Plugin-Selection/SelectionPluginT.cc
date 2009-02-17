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





#define SELECTIONPLUGINT_C

#include <QtGui>

#include "SelectionPlugin.hh"

#include <iostream>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include "OpenFlipper/INIFile/INIFile.hh"

//-----------------------------------------------------------------------------

/** \brief Update face selection to correspond to the vertex selection
 *
 * @param _mesh a mesh
 */
template< typename MeshType >
void SelectionPlugin::update_regions(MeshType* _mesh)
{
  // set face status
  typename MeshType::FaceIter                 f_it, f_end=_mesh->faces_end();
  typename MeshType::FaceVertexIter           fv_it;
  typename MeshType::VertexHandle             v0, v1, v2;
  bool                                        a, h;

  for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it) {
    fv_it = _mesh->fv_iter(f_it);
    v0  = fv_it; v1  = ++fv_it; v2  = ++fv_it;

    a = (_mesh->status(v0).is_bit_set(AREA) || _mesh->status(v1).is_bit_set(AREA) || _mesh->status(v2).is_bit_set(AREA));
    h = (_mesh->status(v0).is_bit_set(HANDLEAREA) &&
         _mesh->status(v1).is_bit_set(HANDLEAREA) &&
         _mesh->status(v2).is_bit_set(HANDLEAREA));

    if (!(a || h))
      if (_mesh->status(v0).is_bit_set(HANDLEAREA) ||
          _mesh->status(v1).is_bit_set(HANDLEAREA) ||
          _mesh->status(v2).is_bit_set(HANDLEAREA))
        h = true;

    _mesh->status(f_it).change_bit(AREA,   a);
    _mesh->status(f_it).change_bit(HANDLEAREA, h);
  }
}


//-----------------------------------------------------------------------------

/** \brief Set current selection as area-region for a mesh
 *
 * @param _mesh a mesh
 */
template< typename MeshType >
void SelectionPlugin::set_area(  MeshType* _mesh)
{

  typename MeshType::VertexIter v_it, v_end=_mesh->vertices_end();

  for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it) {
    _mesh->status(v_it).change_bit(AREA,  _mesh->status(v_it).selected());
    _mesh->status(v_it).set_selected(false);
  }

  update_regions(_mesh);
}


//-----------------------------------------------------------------------------

/** \brief Set current selection as handle-region for a mesh
 *
 * @param _mesh a mesh
 */
template< typename MeshType >
void SelectionPlugin::set_handle(MeshType* _mesh)
{
   typename MeshType::VertexIter v_it, v_end=_mesh->vertices_end();

   for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it) {
      if (_mesh->status(v_it).selected()) {
         _mesh->status(v_it).change_bit(HANDLEAREA,  true);
         _mesh->status(v_it).set_selected(false);
      }
   }

   update_regions(_mesh);
}


//-----------------------------------------------------------------------------

/** \brief Clear the handle-region of a mesh
 *
 * @param _mesh a mesh
 */
template< typename MeshType >
void SelectionPlugin::clear_handle(MeshType* _mesh)
{
  typename MeshType::VertexIter v_it, v_end=_mesh->vertices_end();

  for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
     _mesh->status(v_it).change_bit(HANDLEAREA,  false);

  update_regions(_mesh);
}


//-----------------------------------------------------------------------------

/** \brief Clear the area-region of a mesh
 *
 * @param _mesh a mesh
 */
template< typename MeshType >
void SelectionPlugin::clear_area(MeshType* _mesh)
{
  typename MeshType::VertexIter v_it, v_end=_mesh->vertices_end();

  for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
     _mesh->status(v_it).change_bit(AREA,  false);

  update_regions(_mesh);
}


//-----------------------------------------------------------------------------

/** \brief Delete all selected elements of a mesh
 *
 * @param _mesh a mesh
 * @return returns true if the mesh was changed (i.e there was something selected that was deleted)
 */
template< typename MeshT >
bool SelectionPlugin::deleteSelection(MeshT* _mesh) {
  bool changed = false;

  // delete all selected faces
  for ( typename MeshT::FaceIter f_it = _mesh->faces_begin() ; f_it != _mesh->faces_begin() ; ++f_it)
    if ( _mesh->status(f_it).selected() ) {
        _mesh->delete_face(f_it);
        changed = true;
    }

  for ( typename MeshT::VertexIter v_it = _mesh->vertices_begin() ; v_it != _mesh->vertices_end() ; ++v_it)
    if ( _mesh->status(v_it).selected() ) {
        _mesh->delete_vertex(v_it);
        changed = true;
    }

  _mesh->garbage_collection();

  return changed;
}


