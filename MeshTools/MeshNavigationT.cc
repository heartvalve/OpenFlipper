/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  IMPLEMENTATION
//
//=============================================================================

#define MESHNAVIGATIONT_C

//== INCLUDES =================================================================

#include "MeshNavigationT.hh"

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Geometry/MathDefs.hh>

#include <queue>
//== NAMESPACES ===============================================================

namespace MeshNavigation {

//== IMPLEMENTATION ==========================================================

template < typename MeshT >
inline
typename MeshT::HalfedgeHandle
opposite_halfedge(MeshT& _mesh , typename MeshT::HalfedgeHandle _he) {
   typename MeshT::VertexOHalfedgeIter opp_he( _mesh , _he);
   ++opp_he;
   ++opp_he;
   ++opp_he;
   return (opp_he.handle());
}

template < typename MeshT >
inline
typename MeshT::VertexHandle
findClosestBoundary(MeshT* _mesh , typename MeshT::VertexHandle _vh){

  //add visited property
  OpenMesh::VPropHandleT< bool > visited;
  _mesh->add_property(visited,"Visited Property" );
  
  //init visited property
  typename MeshT::VertexIter v_it, v_end = _mesh->vertices_end();
  for( v_it = _mesh->vertices_begin(); v_it != v_end; ++v_it )
    _mesh->property( visited, *v_it ) = false;

  std::queue< typename MeshT::VertexHandle > queue;
  queue.push( _vh );

  while(!queue.empty()){
    typename MeshT::VertexHandle vh = queue.front();
    queue.pop();
    if (_mesh->property(visited, vh)) continue;

    for (typename MeshT::VertexOHalfedgeIter voh_it(*_mesh,vh); voh_it.is_valid(); ++voh_it){

      if ( _mesh->is_boundary(*voh_it) ){
        _mesh->remove_property(visited);
        return _mesh->to_vertex_handle(*voh_it);
      }else{
        queue.push( _mesh->to_vertex_handle(*voh_it) );
      }
    }
    _mesh->property(visited, vh) = true;
  }

  _mesh->remove_property(visited);
  return typename MeshT::VertexHandle(-1);
}


//=============================================================================
} // MeshNavigation Namespace 
//=============================================================================
