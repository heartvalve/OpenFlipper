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

  if (selectionType_ & VERTEX){
    typename MeshType::VertexIter v_it, v_end=_mesh->vertices_end();

    for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it) {
      _mesh->status(v_it).change_bit(AREA,  _mesh->status(v_it).selected());
      _mesh->status(v_it).set_selected(false);
    }
  } else {

    //reset tagged status
    typename MeshType::VertexIter v_it, v_end( _mesh->vertices_end() );
      for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
        _mesh->status(v_it).set_tagged(false);

    if (selectionType_ & EDGE){
      typename MeshType::EdgeIter e_it, e_end( _mesh->edges_end() );

      for (e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it){
        if ( _mesh->status(e_it).selected() ){

        typename MeshType::HalfedgeHandle h0 = _mesh->halfedge_handle(e_it,0);

          _mesh->status( _mesh->from_vertex_handle(h0) ).set_tagged(true);
          _mesh->status( _mesh->to_vertex_handle(h0)   ).set_tagged(true);
          _mesh->status(e_it).set_selected(false);
        }
      }
    }

    if (selectionType_ & FACE){
      typename MeshType::FaceIter f_it, f_end( _mesh->faces_end() );

      for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it){
        if ( _mesh->status(f_it).selected() ){

          for (typename MeshType::FaceVertexIter fv_it(*_mesh,f_it) ; fv_it; ++fv_it ){

            _mesh->status( fv_it ).set_tagged(true);
            _mesh->status(f_it).set_selected(false);
          }
        }
      }
    }

    //set area bit for tagged handles
    for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it) {
      _mesh->status(v_it).change_bit(AREA,  _mesh->status(v_it).tagged());
      _mesh->status(v_it).set_selected(false);
    }

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

  if (selectionType_ & VERTEX){
    typename MeshType::VertexIter v_it, v_end=_mesh->vertices_end();

    for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it) {
      _mesh->status(v_it).change_bit(HANDLEAREA,  _mesh->status(v_it).selected());
      _mesh->status(v_it).set_selected(false);
    }
  } else {

    //reset tagged status
    typename MeshType::VertexIter v_it, v_end( _mesh->vertices_end() );
      for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
        _mesh->status(v_it).set_tagged(false);

    if (selectionType_ & EDGE){
      typename MeshType::EdgeIter e_it, e_end( _mesh->edges_end() );

      for (e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it){
        if ( _mesh->status(e_it).selected() ){

        typename MeshType::HalfedgeHandle h0 = _mesh->halfedge_handle(e_it,0);

          _mesh->status( _mesh->from_vertex_handle(h0) ).set_tagged(true);
          _mesh->status( _mesh->to_vertex_handle(h0)   ).set_tagged(true);
          _mesh->status(e_it).set_selected(false);
        }
      }
    }

    if (selectionType_ & FACE){
      typename MeshType::FaceIter f_it, f_end( _mesh->faces_end() );

      for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it){
        if ( _mesh->status(f_it).selected() ){

          for (typename MeshType::FaceVertexIter fv_it(*_mesh,f_it) ; fv_it; ++fv_it ){

            _mesh->status( fv_it ).set_tagged(true);
            _mesh->status(f_it).set_selected(false);
          }
        }
      }
    }

    //set area bit for tagged handles
    for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it) {
      _mesh->status(v_it).change_bit(HANDLEAREA,  _mesh->status(v_it).tagged());
      _mesh->status(v_it).set_selected(false);
    }

  }
  update_regions(_mesh);
}

/** \brief Set current selection as feature for a mesh
 *
 * @param _mesh a mesh
 */
template< typename MeshType >
void SelectionPlugin::set_features(MeshType* _mesh)
{

  if (selectionType_ & VERTEX){
    typename MeshType::VertexIter v_it, v_end=_mesh->vertices_end();

    for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
      if ( _mesh->status(v_it).selected() ){
       _mesh->status(v_it).set_feature( _mesh->status(v_it).selected() );
       _mesh->status(v_it).set_selected( false );
      }
  }

  if (selectionType_ & EDGE){
    typename MeshType::EdgeIter e_it, e_end( _mesh->edges_end() );

    for (e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it)
      if ( _mesh->status(e_it).selected() ){
        _mesh->status(e_it).set_feature( _mesh->status(e_it).selected() );
        _mesh->status(e_it).set_selected( false );
      }
  }

  if (selectionType_ & FACE){
    typename MeshType::FaceIter f_it, f_end( _mesh->faces_end() );

    for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
      if ( _mesh->status(f_it).selected() ){
        _mesh->status(f_it).set_feature( _mesh->status(f_it).selected() );
        _mesh->status(f_it).set_selected( false );
      }
  }
}

/** \brief Clear the features of a mesh
 *
 * @param _mesh a mesh
 */
template< typename MeshType >
void SelectionPlugin::clear_features(MeshType* _mesh)
{

  if (selectionType_ & VERTEX){
    typename MeshType::VertexIter v_it, v_end=_mesh->vertices_end();

    for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
      _mesh->status(v_it).set_feature( false );
  }

  if (selectionType_ & EDGE){
    typename MeshType::EdgeIter e_it, e_end( _mesh->edges_end() );

    for (e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it)
      _mesh->status(e_it).set_feature( false );
  }

  if (selectionType_ & FACE){
    typename MeshType::FaceIter f_it, f_end( _mesh->faces_end() );

    for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
      _mesh->status(f_it).set_feature( false );
  }
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


  if (selectionType_ & VERTEX){
    typename MeshT::VertexIter v_it, v_end=_mesh->vertices_end();

    for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
      if ( _mesh->status(v_it).selected() ) {
        _mesh->delete_vertex(v_it);
        changed = true;
      }
  }

  if (selectionType_ & EDGE){
    typename MeshT::EdgeIter e_it, e_end( _mesh->edges_end() );

    for (e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it){
      if ( _mesh->status(e_it).selected() ){
        _mesh->delete_edge(e_it);
        changed = true;
      }
    }
  }

  if (selectionType_ & FACE){
    typename MeshT::FaceIter f_it, f_end( _mesh->faces_end() );

    for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it){
      if ( _mesh->status(f_it).selected() ){
        _mesh->delete_face(f_it);
        changed = true;
      }
    }
  }

  _mesh->garbage_collection();

  return changed;
}


