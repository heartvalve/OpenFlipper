
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
 *   $Revision: 685 $                                                         *
 *   $Date: 2012-09-19 18:15:39 +0200 (Mi, 19 Sep 2012) $                   *
 *                                                                           *
 \*===========================================================================*/

/** \file BoundarySnappingT.cc
 */

//=============================================================================
//
//  CLASS MeshFixing - IMPLEMENTATION
//
//=============================================================================

#define BOUNDARYSNAPPING_CC

//== INCLUDES =================================================================

#include "BoundarySnappingT.hh"

//== NAMESPACE ===============================================================


//== IMPLEMENTATION ==========================================================


template<class MeshT>
BoundarySnappingT<MeshT>::BoundarySnappingT(MeshT& _mesh ) :
mesh_(_mesh)
{
}

template<typename MeshT>
bool sort_less_pair_second(const std::pair<typename MeshT::VertexHandle,double> &lhs,const std::pair<typename MeshT::VertexHandle,double> &rhs)
{
  return lhs.second < rhs.second;
}

template<class MeshT>
void BoundarySnappingT<MeshT>::snap(double _epsilon)
{
  std::vector<typename MeshT::VertexHandle> v_boundary;

  // collect all boundary vertices
  for (typename MeshT::VertexIter v_iter = mesh_.vertices_begin(); v_iter != mesh_.vertices_end(); ++v_iter)
    if (mesh_.is_boundary(v_iter) && mesh_.status(v_iter).selected())
      v_boundary.push_back(v_iter);

  //two maps
  // vertexDistMap saves the vertex and his distances as pairs
  // vertexVertexMap saves the vertices of a vertex in range and the distances
  std::vector< std::pair<typename MeshT::VertexHandle,double> > vertexDistMap;
  std::map<typename MeshT::VertexHandle,std::vector<std::pair<typename MeshT::VertexHandle,double> > > vertexVertexMap;

  // get all boundary vertices in range and save them into the maps
  for (typename std::vector< typename MeshT::VertexHandle >::iterator oIter = v_boundary.begin(); oIter != v_boundary.end(); ++oIter)
  {
    typename MeshT::Point pos = mesh_.point(*oIter);
    if (!mesh_.status(*oIter).deleted())
    {
      std::vector< std::pair<typename MeshT::VertexHandle,double> > verticesInRange;

      //collect all vertices in range
      for (typename std::vector<typename MeshT::VertexHandle>::iterator cIter = v_boundary.begin(); cIter != v_boundary.end(); ++cIter)
      {
        if ( !mesh_.status(*cIter).deleted() && cIter != oIter)
        {
          double dist = (pos - mesh_.point(*cIter)).length();

          if ( dist <= _epsilon )
            verticesInRange.push_back(std::make_pair(*cIter,dist));
        }
      }

      // sort them, so nearest vertex is on position 0 (if exist)
      if (!verticesInRange.empty())
      {
        std::sort( verticesInRange.begin(),verticesInRange.end(),&sort_less_pair_second<MeshT> );
        vertexDistMap.push_back(std::make_pair(*oIter,verticesInRange[0].second));
        vertexVertexMap[*oIter] = verticesInRange;
      }
    }
  }

  bool finished = false;
  while(!finished)
  {
    finished = true;

    double min = _epsilon;
    typename MeshT::VertexHandle v_old;//will be replaced by v_new
    typename MeshT::VertexHandle v_new;
    typename std::vector<std::pair<typename MeshT::VertexHandle,double> >::iterator v_oldIter = vertexDistMap.end();
    typename std::vector<std::pair<typename MeshT::VertexHandle,double> >::iterator v_newIter;

    // find next min pair
    for (typename std::vector<std::pair<typename MeshT::VertexHandle,double> >::iterator vd_iter = vertexDistMap.begin(); vd_iter != vertexDistMap.end(); ++vd_iter)
    {
      typename MeshT::VertexHandle v_1 = vd_iter->first;
      if (v_1.is_valid() && !mesh_.status(v_1).deleted() && vertexVertexMap.find(v_1) != vertexVertexMap.end())
      {
        typename MeshT::VertexHandle v_2;
        std::vector<std::pair<typename MeshT::VertexHandle,double> >& verticesInRange = vertexVertexMap[v_1];

        bool validPair = false;

        for (typename std::vector<std::pair<typename MeshT::VertexHandle,double> >::iterator iter = verticesInRange.begin(); iter != verticesInRange.end(); ++iter)
        {
          //check if v_2 shares a face with v_1
          //if so, it is not usable
          v_2 = iter->first;

          for(typename MeshT::VertexFaceIter vf_iter = mesh_.vf_begin(v_1); vf_iter && v_2.is_valid(); ++vf_iter)
            for (typename MeshT::FaceVertexIter fv_iter = mesh_.fv_begin(vf_iter.handle()); fv_iter && v_2.is_valid(); ++fv_iter)
              if (fv_iter.handle() == v_2)
                v_2 = typename MeshT::VertexHandle();

          validPair = v_2.is_valid() && !mesh_.status(v_2).deleted() && mesh_.is_boundary(v_2);

          //if v_2 is valid, save it, or erase it if not, because v_2 will not be valid in the future
          if (validPair && iter->second <= min)
          {
            //new min pair found
            min = iter->second;
            v_old = v_1;
            v_new = v_2;
            finished = false;
            v_oldIter = vd_iter;
            v_newIter = iter;
          }
        }
      }

    }
    // merge, if not finished (pair found)
    if (!finished)
    {
      //remove the vertex since we will proceed with it
      vertexVertexMap[v_old].erase(v_newIter);
      //save all faces, because faces will be added/deleted
      std::vector<typename MeshT::FaceHandle> faces;
      for (typename MeshT::VertexFaceIter vf_iter = mesh_.vf_begin(v_old); vf_iter; ++vf_iter)
        if (!mesh_.status(vf_iter).deleted())
          faces.push_back(vf_iter);

      //replace v_old with v_new by creating new faces with v_new instead of v_old if possible
      for (typename std::vector<typename MeshT::FaceHandle>::iterator f_iter = faces.begin(); f_iter !=faces.end(); ++f_iter)
      {
        typename MeshT::FaceHandle fHandle = *f_iter;
        if (!fHandle.is_valid() || mesh_.status(fHandle).deleted())
          continue;

        //get face vertices
        std::vector<typename MeshT::VertexHandle> f_vertices;
        for(typename MeshT::FaceVertexIter fv_iter = mesh_.fv_begin(fHandle); fv_iter; ++fv_iter)
          f_vertices.push_back( fv_iter.handle() );

        mesh_.delete_face(fHandle,false);

        //try to add new face
        std::vector<typename MeshT::VertexHandle> newFace_vertices(f_vertices);
        std::replace(newFace_vertices.begin(),newFace_vertices.end(),v_old,v_new);
        typename MeshT::FaceHandle faceH = mesh_.add_face(newFace_vertices);

        if (!faceH.is_valid())
        {
          //failed, try reverse direction
          std::reverse(newFace_vertices.begin(),newFace_vertices.end());
          faceH = mesh_.add_face(newFace_vertices);
          if (!faceH.is_valid())
          {
            //failed, so add the old one
            mesh_.add_face(f_vertices);
          }
        }
      }
    }
    vertexDistMap.erase(v_oldIter);

    //todo: delete vertex before proceed. Now, they will be deleted at the end resulting worse snap
  }
  mesh_.delete_isolated_vertices();

  mesh_.garbage_collection();

}
