#define MESHREPAIRPLUGINT_CC

#include "MeshRepairPlugin.hh"


//-----------------------------------------------------------------------------

template<typename MeshT>
inline unsigned MeshRepairPlugin::n_verticesPerFace()
{
  // Reserve for a quad mesh, might be to small!
  return 4;
}

//-----------------------------------------------------------------------------

template<>
inline unsigned MeshRepairPlugin::n_verticesPerFace<TriMesh>()
{
  return 3;
}

//-----------------------------------------------------------------------------

template<typename MeshT>
void MeshRepairPlugin::flipOrientationSelected(MeshT *_mesh)
{
  typename MeshT::FaceIter f_it, f_end = _mesh->faces_end();

  std::vector< typename MeshT::VertexHandle > handles;
  std::vector< uint > valence;

  handles.reserve( _mesh->n_faces() * n_verticesPerFace<MeshT>() );
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

template<typename MeshT>
bool MeshRepairPlugin::sort_less_pair_second(const std::pair<typename MeshT::VertexHandle,double> &lhs,const std::pair<typename MeshT::VertexHandle,double> &rhs)
{
  return lhs.second < rhs.second;
}

//-----------------------------------------------------------------------------

template<typename MeshT>
void MeshRepairPlugin::snapBoundary(MeshT *_mesh, double _eps)
{
  std::vector<typename MeshT::VertexHandle> v_boundary;

  //collect all boundary vertices
  for (typename MeshT::VertexIter v_iter = _mesh->vertices_begin(); v_iter != _mesh->vertices_end(); ++v_iter)
    if (_mesh->is_boundary(v_iter) && _mesh->status(v_iter).selected())
      v_boundary.push_back(v_iter);

  //two maps
  //vertexDistMap saves the vertex and his distanceas a pair
  //vertexVertexMap saves the vertices of a vertex in range and the distances
  std::vector< std::pair<typename MeshT::VertexHandle,double> > vertexDistMap;
  std::map<typename MeshT::VertexHandle,std::vector<std::pair<typename MeshT::VertexHandle,double> > > vertexVertexMap;

  //get all boundary vertices in range and save them into the maps
  for (typename std::vector< typename MeshT::VertexHandle >::iterator oIter = v_boundary.begin(); oIter != v_boundary.end(); ++oIter)
  {
    typename MeshT::Point pos = _mesh->point(*oIter);
    if (!_mesh->status(*oIter).deleted())
    {
      std::vector< std::pair<typename MeshT::VertexHandle,double> > verticesInRange;

      //collect all vertices in range
      for (typename std::vector<typename MeshT::VertexHandle>::iterator cIter = v_boundary.begin(); cIter != v_boundary.end(); ++cIter)
      {
        if ( !_mesh->status(*cIter).deleted() && cIter != oIter)
        {
          double dist = (pos - _mesh->point(*cIter)).length();

          if ( dist <= _eps )
            verticesInRange.push_back(std::make_pair(*cIter,dist));
        }
      }

      //sort them, so nearest vertex is on position 0 (if exist)
      if (!verticesInRange.empty())
      {
        std::sort(verticesInRange.begin(),verticesInRange.end(),sort_less_pair_second<MeshT>);
        vertexDistMap.push_back(std::make_pair(*oIter,verticesInRange[0].second));
        vertexVertexMap[*oIter] = verticesInRange;
      }
    }
  }

  bool finished = false;
  while(!finished)
  {
    finished = true;

    double min = _eps;
    typename MeshT::VertexHandle v_old;//will be replaced by v_new
    typename MeshT::VertexHandle v_new;
    typename std::vector<std::pair<typename MeshT::VertexHandle,double> >::iterator v_oldIter = vertexDistMap.end();
    typename std::vector<std::pair<typename MeshT::VertexHandle,double> >::iterator v_newIter;

    //find next min pair
    for (typename std::vector<std::pair<typename MeshT::VertexHandle,double> >::iterator vd_iter = vertexDistMap.begin(); vd_iter != vertexDistMap.end(); ++vd_iter)
    {
      typename MeshT::VertexHandle v_1 = vd_iter->first;
      if (v_1.is_valid() && !_mesh->status(v_1).deleted() && vertexVertexMap.find(v_1) != vertexVertexMap.end())
      {
        typename MeshT::VertexHandle v_2;
        std::vector<std::pair<typename MeshT::VertexHandle,double> >& verticesInRange = vertexVertexMap[v_1];

        bool validPair = false;

        for (typename std::vector<std::pair<typename MeshT::VertexHandle,double> >::iterator iter = verticesInRange.begin(); iter != verticesInRange.end(); ++iter)
        {
          //check if v_2 shares a face with v_1
          //if so, it is not usable
          v_2 = iter->first;

          for(typename MeshT::VertexFaceIter vf_iter = _mesh->vf_begin(v_1); vf_iter && v_2.is_valid(); ++vf_iter)
            for (typename MeshT::FaceVertexIter fv_iter = _mesh->fv_begin(vf_iter.handle()); fv_iter && v_2.is_valid(); ++fv_iter)
              if (fv_iter.handle() == v_2)
                v_2 = typename MeshT::VertexHandle();

          validPair = v_2.is_valid() && !_mesh->status(v_2).deleted() && _mesh->is_boundary(v_2);

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
    //merge, if not finished (pair found)
    if (!finished)
    {
      //remove the vertex since we will proceed with it
      vertexVertexMap[v_old].erase(v_newIter);
      //save all faces, because faces will be added/deleted
      std::vector<typename MeshT::FaceHandle> faces;
      for (typename MeshT::VertexFaceIter vf_iter = _mesh->vf_begin(v_old); vf_iter; ++vf_iter)
        if (!_mesh->status(vf_iter).deleted())
          faces.push_back(vf_iter);

      //replace v_old with v_new by creating new faces with v_new instead of v_old if possible
      for (typename std::vector<typename MeshT::FaceHandle>::iterator f_iter = faces.begin(); f_iter !=faces.end(); ++f_iter)
      {
        typename MeshT::FaceHandle fHandle = *f_iter;
        if (!fHandle.is_valid() || _mesh->status(fHandle).deleted())
          continue;

        //get face vertices
        std::vector<typename MeshT::VertexHandle> f_vertices;
        for(typename MeshT::FaceVertexIter fv_iter = _mesh->fv_begin(fHandle); fv_iter; ++fv_iter)
          f_vertices.push_back( fv_iter.handle() );

        _mesh->delete_face(fHandle,false);

        //try to add new face
        std::vector<typename MeshT::VertexHandle> newFace_vertices(f_vertices);
        std::replace(newFace_vertices.begin(),newFace_vertices.end(),v_old,v_new);
        typename MeshT::FaceHandle faceH = _mesh->add_face(newFace_vertices);

        if (!faceH.is_valid())
        {
          //failed, try reverse direction
          std::reverse(newFace_vertices.begin(),newFace_vertices.end());
          faceH = _mesh->add_face(newFace_vertices);
          if (!faceH.is_valid())
          {
            //failed, so add the old one
            _mesh->add_face(f_vertices);
          }
        }
      }
    }
    vertexDistMap.erase(v_oldIter);

    //todo: delete vertex before proceed. Now, they will be deleted at the end resulting worse snap
  }
  _mesh->delete_isolated_vertices();

  _mesh->garbage_collection();

}

//-----------------------------------------------------------------------------
