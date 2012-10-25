#define MESHREPAIRPLUGINT_CC

#include "MeshRepairPlugin.hh"

//-----------------------------------------------------------------------------

template<typename MeshT>
void MeshRepairPlugin::flipOrientationSelected(MeshT *_mesh)
{
  typename MeshT::FaceIter f_it, f_end = _mesh->faces_end();

  std::vector< typename MeshT::VertexHandle > handles;
  std::vector< uint > valence;

  const unsigned int n_VerticesPerFace = _mesh->is_trimesh() ? 3 : 4;

  handles.reserve( _mesh->n_faces() * n_VerticesPerFace );
  valence.reserve( _mesh->n_faces() );

  OpenMesh::FPropHandleT< bool > orientation; //inidcates the orientation. true = new orientation. false = old orientation
  if ( !_mesh->get_property_handle(orientation,"orientation") )
      _mesh->add_property(orientation, "orientation");

  for (f_it = _mesh->faces_begin(); f_it != f_end ; ++f_it) {

    if (_mesh->status(f_it).selected())
    {
      // Collect vertex handles
      typename MeshT::FaceVertexIter fv_it = _mesh->fv_iter(f_it);


      valence.push_back( _mesh->valence(f_it) );
      while( fv_it) {
        handles.push_back( fv_it.handle() );
        ++fv_it;
      }

      // delete the corresponding face
      _mesh->delete_face(f_it, false);
    }
    else
    {
      _mesh->property(orientation,f_it) = false;
    }
  }

  // clean the mesh
  _mesh->garbage_collection();

  // read the faces
  std::size_t pos = 0;

  for (std::size_t i=0; i < valence.size(); i++) {

    std::vector< typename MeshT::VertexHandle > faceVertices;

    pos += valence[i];

    // add valence vertices in the inverse order
    for (unsigned int j = 1 ; j <= valence[i] ; ++j )
    {
      typename MeshT::VertexHandle handle = handles[pos - j];

      //check, if orientation problems exists. If so, add a new vertex avoiding them
        if(_mesh->vf_begin(handle) && !_mesh->property(orientation,_mesh->vf_begin(handle)))
          handle = _mesh->add_vertex(_mesh->point(handle));

      faceVertices.push_back(handle);
    }

    typename MeshT::FaceHandle fh = _mesh->add_face(faceVertices);
    _mesh->property(orientation,fh) = true;

  }

  _mesh->remove_property(orientation);
  _mesh->update_normals();
}

//-----------------------------------------------------------------------------

