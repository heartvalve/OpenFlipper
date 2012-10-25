
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

/** \file MeshFixingT.cc
 */

//=============================================================================
//
//  CLASS MeshFixing - IMPLEMENTATION
//
//=============================================================================

#define OPENMESH_MESHFIXING_CC

//== INCLUDES =================================================================

#include "MeshFixingT.hh"

//== NAMESPACE ===============================================================


//== IMPLEMENTATION ==========================================================


CompareVectors::CompareVectors(double _epsilon) :
        epsilon_(_epsilon * _epsilon)
{
}

bool CompareVectors::operator()(const ACG::Vec3d& _v0, const ACG::Vec3d& _v1) const
{
  if ((_v0 - _v1).sqrnorm() <= epsilon_)
    return false;
  else
    return _v0 < _v1;
}

template<class MeshT>
MeshFixing<MeshT>::MeshFixing(MeshT& _mesh, double _epsilon ) :
mesh_(_mesh),
vmap_(CompareVectors(_epsilon))
{
}


template<class MeshT>
int MeshFixing<MeshT>::add_vertex(const ACG::Vec3d& _p) {
  vertices_.push_back( Vertex(_p) );
  return vertices_.size()-1;
}

template<class MeshT>
void MeshFixing<MeshT>::add_face(const ACG::Vec3d& _p0, const ACG::Vec3d& _p1, const ACG::Vec3d& _p2) {
  ACG::Vec3d     p[3] = {_p0, _p1, _p2 };
  int            v[3];
  MapIter        it;


  // map points to vertices
  for (unsigned int i=0; i<3; ++i) {

    // Try to find the vertex in the map
    it = vmap_.find(p[i]);

    // Add the vertex to the map, if it is new
    // (Out of the epsilon range! )
    if ( it == vmap_.end() ) {
      v[i] = add_vertex( p[i] );
      vmap_[p[i]] = v[i];
    } else
      v[i] = it->second;
  }


  // Don't add degenerate faces, as we could already skip them here
  if (v[0]==v[1] || v[0]==v[2] || v[1]==v[2])
    return;


  // add face
  faces_.push_back( Face(v[0], v[1], v[2]) );

  unsigned int newFace = faces_.size()- 1;
  for (unsigned int i = 0; i < 3; ++i)
    vertices_[v[i]].faces.push_back( newFace );

}

/** \brief Fix the mesh topology
 *
 * Separates all components that are incident to the vertices by adding new vertices.
 * Required to always get a manifold configuration
 *
 */
template<class MeshT>
void MeshFixing<MeshT>::fix_topology()
{
  int component;
  int nf;

  const unsigned int numberOfVertices = vertices_.size();

  // Iterate over all vertices
  for (unsigned int v = 0; v < numberOfVertices; ++v) {

    // Get reference to the vector of associated faces at the current vertex
    const std::vector<unsigned int>& vfaces = vertices_[v].faces;
    std::vector<unsigned int>::const_iterator f_it, f_end(vfaces.end());
    std::vector<unsigned int> todo;

    todo.reserve(vfaces.size());

    // Reset components to which each face connected to this vertex belongs
    for (f_it = vfaces.begin(); f_it != f_end; ++f_it)
      faces_[*f_it].component = -1;

    // Find the components
    for (component = 0;; ++component) {

      // Find one face, that is not yet flagged with a component
      for (f_it = vfaces.begin(); f_it != f_end; ++f_it) {

        if (faces_[*f_it].component == -1) {

          // Add component as seed to the list
          todo.push_back(*f_it);

          // Mark the current face with the right component
          faces_[*f_it].component = component;

          // Found, so don't continue
          break;
        }

      }

      // No components found anymore
      if (f_it == f_end)
        break;

      // Grow from the seed face until no more connected faces where found
      while (!todo.empty()) {

        // Get next element from the todo list
        const unsigned int currentFace = todo.back();
        todo.pop_back();

        // Go over all vertices of the current face
        for (unsigned int i = 0; i < 3; ++i) {

          // If the vertex of the face is not equal to the current iteration vertex
          if (faces_[currentFace].v[i] != v) {

            // Found neighbor?
            const int neighbourFace = neighbor(currentFace, v, faces_[currentFace].v[i]);
            if ( neighbourFace != -1) {

              // Tag it with the right component and push it to the list of todos
              if (faces_[neighbourFace].component == -1) {
                faces_[neighbourFace].component = component;
                todo.push_back(neighbourFace);
              }

            }
          }
        }
      }
    }

    // Now we separate the components at the current vertex
    for (int c = 1; c < component; ++c) {

      // Duplicate the point to achieve the separation
      const ACG::Vec3d p = vertices_[v].p;
      unsigned int newVertex = add_vertex(p);

      // Possible relocation of vertices_ so we need to update the references!
      std::vector<unsigned int>& vfaces = vertices_[v].faces;
      std::vector<unsigned int>::iterator f_it, f_end(vfaces.end());

      // Update the face indices of the current component
      bool finished = false;
      while (!finished) {
        finished = true;

        for (f_it = vfaces.begin(), f_end = vfaces.end(); f_it != f_end; ++f_it) {

          // If the face belongs to the current component

          if (faces_[*f_it].component == c) {

            // Iterate over its vertices
            for (unsigned int i = 0; i < 3; ++i) {

              // Relink to the new vertex by updating the face and pushing it in as new
              if (faces_[*f_it].v[i] == v) {
                faces_[*f_it].v[i] = newVertex;
                vertices_[newVertex].faces.push_back(*f_it);
                finished = false;
              }

            }

            // Erase the old one
            vfaces.erase(f_it);
            break;
          }
        }
      }
    }

  }
}

//-----------------------------------------------------------------------------


/** Get the neighbor
 *
 * @param _f   Current face
 * @param _v0  Center vertex
 * @param _v1  Test vertex
 * @return
 */
template<class MeshT>
int MeshFixing<MeshT>::neighbor(unsigned int _f, unsigned int _v0, unsigned int _v1)
{
  // Get the list of faces at vertex _v0
  const std::vector<unsigned int>& v0faces = vertices_[_v0].faces;
  std::vector<unsigned int>::const_iterator f_it, f_end = v0faces.end();


  int neighborFace = -1;

  // Iterate over all associated faces at the current vertex
  for (f_it = v0faces.begin(); f_it != f_end; ++f_it) {

    // if the face associated at this vertex is not equal to the face we are analyzing
    if (*f_it != _f) {

      // Iterate over all vertices of this face
      for (unsigned int j = 0; j < 3; ++j)

        // If this face is adjacent to the vertex we are analyzing
        if (faces_[*f_it].v[j] == _v1) {

          if (neighborFace != -1)
            return -1;
          else
            neighborFace = *f_it;

        }
    }
  }

  return neighborFace;
}

/** Tries to ensure that all faces in one component have the same orientation.
 *
 */
template<class MeshT>
void MeshFixing<MeshT>::fix_orientation()
{
  // We need faces to work on!
  if (faces_.empty()) return;

  int   fh = 0;

  std::vector<int>  todo;
  todo.reserve( (int) sqrt( (double) faces_.size()) );

  const unsigned int faceCount = faces_.size();

  // Reset components of all faces
  for (unsigned int i = 0; i < faceCount; ++i)
    faces_[i].component = -1;


  // Iterate over all components
  for (int component = 0;; ++component) {

    unsigned int start = 0 ;

    // Find one seed triangle in this component
    for (; start < faceCount; ++start) {

      // Found an untagged face == new component?
      if (faces_[start].component == -1) {
        fh = start;
        todo.push_back(fh);
        faces_[fh].component = component;
        break;
      }

    }

    // No more Components!
    if (start == faceCount)
      break;

    // Flood fill the current component, while fixing the orientation
    while (!todo.empty()) {

      // Get the next face to work on
      fh = todo.back();
      todo.pop_back();

      // Find neighbors of the current face
      for (unsigned int i = 0; i < 3; ++i) {
        unsigned int i0 = faces_[fh].v[i];
        unsigned int i1 = faces_[fh].v[(i + 1) % 3];

        int neighborFace = neighbor(fh, i0, i1);
        if (neighborFace != -1 && faces_[neighborFace].component == -1) {

          int nextVertex = 0;
          for (nextVertex = 0; nextVertex < 3; ++nextVertex)
            if (faces_[neighborFace].v[nextVertex] == i1)
              break;

          // If the orientation is incorrect, we have to flip it!
          if (faces_[neighborFace].v[(nextVertex + 2) % 3] == i0)
            std::swap(faces_[neighborFace].v[1], faces_[neighborFace].v[2]);

          faces_[neighborFace].component = component;
          todo.push_back(neighborFace);
        }
      }

    }
  }
}

template<class MeshT>
bool MeshFixing<MeshT>::fix()
{

  // Maximal number of steps:
  // _mesh.n_faces() +                           // add face
  // (_fix_topology ? _mesh.n_vertices() : 0) +  // non-manifold
  // (_fix_orientation ? _mesh.n_faces() : 0) +  // orientation
  // _mesh.n_faces(),                            // copy output


  typename MeshT::FaceVertexIter fv_it;

  std::cerr << "Setup data structure" << std::endl;

  // add faces
  for (typename MeshT::FaceIter f_it = mesh_.faces_begin(); f_it != mesh_.faces_end(); ++f_it)
  {

    const ACG::Vec3d p0 = mesh_.point( fv_it = mesh_.fv_iter(f_it) );
    const ACG::Vec3d p1 = mesh_.point(++fv_it);
    const ACG::Vec3d p2 = mesh_.point(++fv_it);

    add_face(p0, p1, p2);

  }



  std::cerr << "Fix topology" << std::endl;
  //fix_topology();

  std::cerr << "Fix orientation" << std::endl;

  fix_orientation();

  std::cerr << "Copy back to mesh" << std::endl;

  // Remove all primitives but keep the properties
  mesh_.clean();

  // Copy back to the target mesh
  unsigned int vertexCount(vertices_.size());
  unsigned int faceCount(faces_.size());


  std::vector<typename MeshT::VertexHandle> vh;
  typename MeshT::VertexHandle v0, v1, v2;
  typename MeshT::FaceHandle fh;
  bool ok = true;


  // Add all vertices back to the mesh
  vh.resize(vertexCount);
  for (unsigned int i = 0; i < vertexCount; ++i)
    vh[i] = mesh_.add_vertex((typename TriMesh::Point) vertices_[i].p);

  // Add all faces back to mesh
  for (unsigned int i = 0; i < faceCount; ++i) {

    // try to add face
    if (!(fh = mesh_.add_face(vh[faces_[i].v[0]], vh[faces_[i].v[1]], vh[faces_[i].v[2]])).is_valid()) {
      // fails -> add isolated face
      v0 = mesh_.add_vertex((typename MeshT::Point) vertices_[faces_[i].v[0]].p);
      v1 = mesh_.add_vertex((typename MeshT::Point) vertices_[faces_[i].v[1]].p);
      v2 = mesh_.add_vertex((typename MeshT::Point) vertices_[faces_[i].v[2]].p);
      fh = mesh_.add_face(v0, v1, v2);
      ok = false;
    }

  }

  return ok;
}
