
/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenMesh.                                           *
 *                                                                           *
 *  OpenMesh is free software: you can redistribute it and/or modify         *
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
 *  OpenMesh is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenMesh.  If not,                                    *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
 \*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Date$                   *
 *                                                                           *
 \*===========================================================================*/

/** \file NonManifoldVertexFixingT.cc
 */

//=============================================================================
//
//  CLASS MeshFixing - IMPLEMENTATION
//
//=============================================================================

#define NONMANIFOLDVERTEXFIXING_CC

//== INCLUDES =================================================================

#include "NonManifoldVertexFixingT.hh"

//== NAMESPACE ===============================================================


//== IMPLEMENTATION ==========================================================


template<class MeshT>
NonManifoldVertexFixingT<MeshT>::NonManifoldVertexFixingT(MeshT& _mesh ) :
mesh_(_mesh)
{
}


template<class MeshT>
void NonManifoldVertexFixingT<MeshT>::fix()
{

  OpenMesh::FPropHandleT< size_t > component;
  if ( !mesh_.get_property_handle(component,"component") )
    mesh_.add_property(component, "component");

  for (typename MeshT::VertexIter v_iter = mesh_.vertices_begin(); v_iter != mesh_.vertices_end(); ++v_iter)
  {
    // unmark all faces
    for (typename MeshT::VertexFaceIter vf_iter = mesh_.vf_begin(*v_iter); vf_iter.is_valid(); ++vf_iter)
      mesh_.property(component,*vf_iter) = 0;

    size_t componentCount = 1;


    //search and isolate new components
    //shared vertices will be duplicated
    for (typename MeshT::VertexFaceIter vf_iter = mesh_.vf_begin(*v_iter); vf_iter.is_valid(); ++vf_iter)
    {
      //get the first face in the component
      std::vector<typename MeshT::FaceHandle> checkNeighbour;
      if(mesh_.property(component,*vf_iter) == 0)
      {
        mesh_.property(component,*vf_iter) = componentCount;
        checkNeighbour.push_back(*vf_iter);
      }

      // if a reference face was found, a new component exists
      // and a new vertex is required (except for the first component)
      typename MeshT::VertexHandle v_new;
      if (componentCount > 1 && !checkNeighbour.empty())
      {
        typename MeshT::Point p = mesh_.point(*v_iter);
        v_new = mesh_.add_vertex(p);
      }

      // check all adjacent faces of our reference
      while(!checkNeighbour.empty())
      {
        typename MeshT::FaceHandle face = checkNeighbour.back();
        checkNeighbour.pop_back();

        std::vector<typename MeshT::VertexHandle> f_vertices;
        // get all neighbor faces of face
        for (typename MeshT::FaceVertexIter fv_iter = mesh_.fv_begin(face); fv_iter.is_valid(); ++fv_iter)
        {
          f_vertices.push_back(*fv_iter);
          if (*fv_iter != *v_iter)
          {
            //find the next neighbor face over edge v_iter and fv_iter
            typename MeshT::FaceHandle nf;
            for (typename MeshT::VertexFaceIter nf_iter = mesh_.vf_begin(*v_iter); nf_iter.is_valid() && !nf.is_valid(); ++nf_iter)
            {
              if (*nf_iter != face)
                for (typename MeshT::FaceVertexIter nfv_iter = mesh_.fv_begin(*nf_iter); nfv_iter.is_valid() && !nf.is_valid(); ++nfv_iter)
                  if (*nfv_iter == *fv_iter)
                    nf = *nf_iter;
            }

            //if such a face was found, it is in the same component as the reference face
            if (nf.is_valid() && !mesh_.property(component,nf))
            {
              mesh_.property(component,nf) = componentCount;
              checkNeighbour.push_back(nf);
            }
          }
        }

        //if one face wasn't found in the component = 1 run, then it is a new component, due to split
        if (componentCount > 1 && v_new.is_valid())
        {
          std::replace(f_vertices.begin(),f_vertices.end(),*v_iter,v_new);

          mesh_.delete_face(face,false);
          mesh_.add_face(f_vertices);

        }
      }

      // all faces which belong to v_iter and inside same component found
      // the next face will be in a new component
      ++componentCount;
    }
  }

  mesh_.remove_property(component);
  mesh_.garbage_collection();

}
