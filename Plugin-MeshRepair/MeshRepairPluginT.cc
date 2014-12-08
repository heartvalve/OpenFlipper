#define MESHREPAIRPLUGINT_CC

#include "MeshRepairPlugin.hh"

//-----------------------------------------------------------------------------

template<typename MeshT>
void MeshRepairPlugin::flipOrientation(MeshT& _mesh)
{


  std::vector< unsigned int >                 valence;
  std::vector< typename MeshT::VertexHandle > vhandles;



  bool selected = false;

  // Check if a face is selected
  for (typename MeshT::FaceIter f_it = _mesh.faces_begin(); f_it != _mesh.faces_end() ; ++f_it)
    if ( _mesh.status(*f_it).selected() ) {
      selected = true;
      break;
    }

  // Two ways to go
  // if something is selected, we have to duplicate some vertices in order to get a manifold mesh
  if ( selected ) {

    // Tag all vertices adjacent to selected faces
    for (typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it != _mesh.vertices_end(); ++v_it) {

      bool tagged = false;

      for (typename MeshT::VertexFaceIter vf_it = _mesh.vf_iter(*v_it); !tagged && vf_it.is_valid(); ++vf_it)
        if (_mesh.status(*vf_it).selected())
          tagged = true;

      _mesh.status(*v_it).set_tagged(tagged);
    }

    // Remember the vertex mapping for the duplication
    OpenMesh::VPropHandleT< typename MeshT::VHandle>  vmap;
    _mesh.add_property(vmap);

    // duplicate vertices that are incident to tagged and un-tagged faces
    for (typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it !=  _mesh.vertices_end(); ++v_it) {

      typename MeshT::VertexHandle vh = *v_it;

      if (_mesh.status(*v_it).tagged() )
        for (typename MeshT::VertexFaceIter vf_it = _mesh.vf_iter(*v_it); vf_it.is_valid(); ++vf_it)

          if (!_mesh.status(*vf_it).tagged()) {

            typename MeshT::Point tmpPoint = _mesh.point(*v_it);

            // Duplicate the vertex
            vh = _mesh.add_vertex( tmpPoint );

            // The next vertex should be tagged
            _mesh.status(vh).set_tagged(true);

            // The original vertex is already duplicated, so we don't need to do it again!
            _mesh.status(*v_it).set_tagged(false);
            break;
          }

      _mesh.property(vmap, *v_it) = vh;
    }

    // Delete the old faces and collect their information
    for (typename MeshT::FaceIter f_it = _mesh.faces_begin(); f_it != _mesh.faces_end(); ++f_it) {

      if (_mesh.status(*f_it).selected()) {

        // Collect vertex handles
        typename MeshT::FaceVertexIter fv_it = _mesh.fv_iter(*f_it);

        valence.push_back(_mesh.valence(*f_it));
        while (fv_it.is_valid()) {
          vhandles.push_back(*fv_it);
          ++fv_it;
        }

        // delete the corresponding face
        _mesh.delete_face(*f_it, false);
      }

    }

    // Rebuild the faces in the opposite order
    std::size_t pos = 0;
    for (std::size_t i = 0; i < valence.size(); i++) {

      std::vector<typename MeshT::VertexHandle> faceVertices;

      pos += valence[i];

      // add valence vertices in the inverse order
      for (unsigned int j = 1; j <= valence[i]; ++j)
        faceVertices.push_back(_mesh.property(vmap,vhandles[pos - j]) );

      _mesh.add_face(faceVertices);
    }

    _mesh.remove_property(vmap);

  } else {

    std::vector < typename MeshT::Point > points;

    // store vertices
    points.reserve(_mesh.n_vertices() );
    for (typename MeshT::VertexIter v_it = _mesh.vertices_begin(); v_it != _mesh.vertices_end(); ++v_it)
      points.push_back(_mesh.point(*v_it));

    // Better approximation of vector size for storing the handles
    const unsigned int n_VerticesPerFace = _mesh.is_trimesh() ? 3 : 4;

    // Remember vertex handles for each face and its valence
    std::vector< typename MeshT::VertexHandle > vhandles;
    vhandles.reserve( _mesh.n_faces() * n_VerticesPerFace);
    for (typename MeshT::FaceIter f_it = _mesh.faces_begin(); f_it != _mesh.faces_end(); ++f_it) {

      valence.push_back( _mesh.valence(*f_it) );

      for (typename MeshT::FaceVertexIter fv_it = _mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
        vhandles.push_back(*fv_it);
    }

    // Remove all entities from the mesh
    _mesh.clean();

    // Restore vertices
    for (unsigned int i = 0; i < points.size(); ++i)
      _mesh.add_vertex(points[i]);


    // Add back the faces
    std::size_t pos = 0;
    for (std::size_t i = 0; i < valence.size(); i++) {

      std::vector<typename MeshT::VertexHandle> faceVertices;

      pos += valence[i];

      // add valence vertices in the inverse order
      for (unsigned int j = 1; j <= valence[i]; ++j)
        faceVertices.push_back(vhandles[pos - j]);

      _mesh.add_face(faceVertices);
    }

  }


  _mesh.garbage_collection();
  _mesh.update_normals();
}

//-----------------------------------------------------------------------------

