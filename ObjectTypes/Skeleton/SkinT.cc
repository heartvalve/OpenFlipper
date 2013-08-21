#ifdef USE_OPENMP
#include <omp.h>
#endif
#define SKINT_C

//-----------------------------------------------------------------------------

template<typename MeshT>
SkinT<MeshT>::SkinT(SkeletonT<PointT> *_skeleton, MeshT *_mesh, int _objectID) :
  BaseSkin(_objectID),
  skeleton_(_skeleton),
  mesh_(_mesh),
  objectId_(_objectID),
  lastmethod_(M_LBS),
  weightsComputed_(false)

{
}

//-----------------------------------------------------------------------------

template<typename MeshT>
SkinT<MeshT>::~SkinT()
{

}

//-----------------------------------------------------------------------------

template<typename MeshT>
typename SkinT<MeshT>::Skeleton* SkinT<MeshT>::skeleton()
{
  return skeleton_;
}

//-----------------------------------------------------------------------------

/**
 * @brief Attach the given mesh as skin to this skeleton
 *
 * The mesh will be equipped with two properties. One holding the original vertex coordinates, a second
 * holding the per vertex weights for the joints.
 * The bone weights are derived from the segment information stored with the bones and in a vertex based
 * mesh property called "Face Segment".
 */
template<typename MeshT>
void SkinT<MeshT>::attachSkin()
{
  // create the skins properties
  OpenMesh::VPropHandleT<DefaultPose> propDefaultPose;
  OpenMesh::VPropHandleT<SkinWeights> propWeights;

  //make sure properties are there
  if( !mesh_->get_property_handle(propDefaultPose, DEFAULTPOSE_PROP) )
    mesh_->add_property(propDefaultPose, DEFAULTPOSE_PROP);

  if (! mesh_->get_property_handle(propWeights, SKIN_WEIGHTS_PROP))
    mesh_->add_property(propWeights, SKIN_WEIGHTS_PROP);

  // backup the default pose
  for(typename MeshT::VertexIter it = mesh_->vertices_begin(); it != mesh_->vertices_end(); ++it)
    mesh_->property(propDefaultPose, *it) = DefaultPose(mesh_->point(*it), mesh_->normal(*it));
}

//-----------------------------------------------------------------------------

template<typename MeshT>
void SkinT<MeshT>::deformSkin()
{
  deformSkin(lastAnimationHandle_, lastmethod_);
}

//-----------------------------------------------------------------------------

/**
 * @brief Deforms the given skin to the pose defined by the skeleton
 *
 * This method will only work properly if the mesh has been prepared to be a skin by a call of
 * SkeletonT::PrepareSkin.
 *
 * Two algorithms are implemented:
 * - Linear Blend Skinning (LBS) or Skeletal Space Deformation (SSD)
 * - Spherical Blend Skinning (SBS), Ladislav Kavan and Jiri Zara: Spherical blend skinning, a real-time deformation of articulated models
 *
 * @par Linear Blend Skinning
 * Every vertex will be transformed by every joint with a non-null weight. First
 * of all the vertex is transformed into the coordinate system of the joint, then it is transformed back
 * to the global coordinate system of the pose given by the active frame in the given animation.
 *
 * @par
 * Since the mesh is given in the default pose the default pose joints are used to calculate the vertices
 * position relative to the joint. Then they are transformed back into global coordinates using the joints
 * in the current pose.
 *
 * @par
 * The global matrices of the default pose and animation are not updated by this method, make sure to
 * update them before deforming the skin or the results will be unexpected.
 *
 * \f[ v_{pose} = M_{pose} \cdot M^{-1}_{default} \cdot v_{default} \f]
 *
 *
 * @param _hAni The animation frame to be used as target
 * @param _method deformation method to be used @see BaseSkin.hh
 */
template<typename MeshT>
void SkinT<MeshT>::deformSkin(const AnimationHandle &_hAni, Method _method)
{
  // Do not transform skin if skin weights were not computed before
  // as the mesh entirely disappears since all vertices collapse to origin
  if(!weightsComputed_) {
      return;
  }

  lastAnimationHandle_ = _hAni;
  lastmethod_          = _method;

  // first get the properties
  OpenMesh::VPropHandleT<DefaultPose> propDefaultPose;
  OpenMesh::VPropHandleT<SkinWeights> propWeights;

  if(!mesh_->get_property_handle(propDefaultPose, DEFAULTPOSE_PROP) ||
     !mesh_->get_property_handle(propWeights, SKIN_WEIGHTS_PROP))
    return; // missing properties

  Pose* pose = skeleton_->pose(_hAni);

  #ifndef USE_OPENMP
    int verticesWithoutWeights = 0;
  #endif
  
  // for every vertex
  typename MeshT::VertexIter it;
#ifdef USE_OPENMP
  std::vector< OpenMesh::VertexHandle > vhandles; vhandles.clear(); vhandles.reserve(mesh_->n_vertices());
  for(it = mesh_->vertices_begin(); it != mesh_->vertices_end(); ++it){vhandles.push_back(*it);}
  int vhcount = (int) vhandles.size();
  #pragma omp parallel for
  for (int vhindex = 0; vhindex < vhcount; vhindex++)
#else
  for(it = mesh_->vertices_begin(); it != mesh_->vertices_end(); ++it)
#endif
  {
    #ifdef USE_OPENMP
      const OpenMesh::VertexHandle currentVertexH = vhandles[vhindex];
    #else
      const OpenMesh::VertexHandle currentVertexH = it.handle();
    #endif

    // based on its position in the default pose
    OpenMesh::Vec3d default_point  = mesh_->property(propDefaultPose, currentVertexH).point,
                    default_normal = mesh_->property(propDefaultPose, currentVertexH).normal;

    OpenMesh::Vec3d point(0, 0, 0), normal(0, 0, 0);	// the new position and normal

    if( _method == M_LBS ) {

      // Linear blend skinning
      SkinWeights &weights = mesh_->property(propWeights, currentVertexH);

      #ifndef USE_OPENMP
      if (weights.size() == 0)
      {
        verticesWithoutWeights++;
      }
      #endif

      SkinWeights::iterator it_w;
      for(it_w = weights.begin(); it_w != weights.end(); ++it_w)
      {
        const Matrix &unified = pose->unifiedMatrix(it_w->first);

        point  += it_w->second * unified.transform_point(default_point);
        normal += it_w->second * unified.transform_vector(default_normal);
      }

    }else if( _method == M_DBS ) {
      // Dual quaternion blend skinning

      std::vector<double>         weightVector;
      std::vector<DualQuaternion> dualQuaternions;

      SkinWeights &weights = mesh_->property(propWeights, currentVertexH);
      SkinWeights::iterator it_w;

      for(it_w = weights.begin(); it_w != weights.end(); ++it_w){
        weightVector.push_back(    it_w->second );
        dualQuaternions.push_back( pose->unifiedDualQuaternion(it_w->first) );
      }

      DualQuaternion dq = DualQuaternion::interpolate(weightVector, dualQuaternions);

      point  = dq.transform_point(default_point);
      normal = dq.transform_vector(default_normal);

    } else {
      std::cerr << "ERROR: Unknown skinning method!" << std::endl;
    }

    mesh_->set_point(currentVertexH, point);
    mesh_->set_normal(currentVertexH, normal);
  }

  #ifndef USE_OPENMP
    if ( verticesWithoutWeights > 0 )
      std::cerr << "Deform skin: " << verticesWithoutWeights << " vertices without skin weights." << std::endl;
  #endif

  mesh_->update_face_normals();
}

//-----------------------------------------------------------------------------

/**
 * @brief The given mesh will be reset to its default pose and all skin properties are removed
 */
template<typename MeshT>
void SkinT<MeshT>::releaseSkin()
{
  // create the skins properties
  OpenMesh::VPropHandleT<DefaultPose> propDefaultPose;

  // try to restore the default pose
  if(mesh_->get_property_handle(propDefaultPose, DEFAULTPOSE_PROP))
  {
    typename MeshT::VertexIter it;
    for(it = mesh_->vertices_begin(); it != mesh_->vertices_end(); ++it)
    {
      mesh_->set_point(*it, mesh_->property(propDefaultPose, *it).point);
      mesh_->set_normal(*it, mesh_->property(propDefaultPose, *it).normal);
    }
    mesh_->remove_property(propDefaultPose);
  }

  // Remove the skin weights
  OpenMesh::VPropHandleT<SkinWeights> propSkinWeights;
  mesh_->remove_property(propSkinWeights);
}

//-----------------------------------------------------------------------------

