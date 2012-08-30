#define MESHREPAIRPLUGINT_CC

#include "MeshRepairPlugin.hh"


template<typename MeshT>
inline unsigned MeshRepairPlugin::n_verticesPerFace()
{
  // Reserve for a quad mesh, might be to small!
  return 4;
}

template<>
inline unsigned MeshRepairPlugin::n_verticesPerFace<TriMesh>()
{
  return 3;
}


template<typename MeshT>
void MeshRepairPlugin::flipOrientationSelected(MeshT *_mesh)
{
  typename MeshT::FaceIter f_it, f_end = _mesh->faces_end();

  std::vector< TriMesh::VertexHandle > handles;
  std::vector< uint > valence;

  handles.reserve( _mesh->n_faces() * n_verticesPerFace<MeshT>() );
  valence.reserve( _mesh->n_faces() );


  for (f_it = _mesh->faces_begin(); f_it != f_end ; ++f_it) {

    if (_mesh->status(f_it).selected())
    {
      // Collect vertex handles
      TriMesh::FaceVertexIter fv_it = _mesh->fv_iter(f_it);


      valence.push_back( _mesh->valence(f_it) );
      while( fv_it) {
        handles.push_back( fv_it.handle() );
        ++fv_it;
      }

      // delete the corresponding face
      _mesh->delete_face(f_it, false);
    }
  }

  // clean the mesh
  _mesh->garbage_collection();

  // read the faces
  uint pos = 0;
  for (uint i=0; i < valence.size(); i++) {

    std::vector< TriMesh::VertexHandle > faceVertices;

    pos += valence[i];

    // add valence vertices in the inverse order
    for (uint j = 1 ; j <= valence[i] ; ++j )
    {
      TriMesh::VertexHandle handle = handles[pos - j];

      //if vertex is not isolated, it has a face and we have to add a new vertex to prevent holes
      //between the old flipped and not flipped faces
      if (!_mesh->is_isolated(handle))
        handle = _mesh->add_vertex(_mesh->point(handle));

      faceVertices.push_back(handle);
    }

    _mesh->add_face(faceVertices);
  }

  _mesh->update_normals();
}
