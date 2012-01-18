#ifndef SKINT_HH
#define SKINT_HH

#include <ObjectTypes/Skeleton/BaseSkin.hh>

/**
 * @brief General skin class, used to bind skeleton and mesh and deform the mesh
 *
 * The mesh is given to the constructor. To prepare the mesh for deformation, call the SkinT::AttachSkin method.
 */
template<typename MeshT>
class SkinT : public BaseSkin
{
public:
  typedef typename MeshT::Point                 PointT;
  typedef PointT                                Point;
  typedef typename PointT::value_type           Scalar;
  typedef SkeletonT<PointT>                     Skeleton;
  typedef JointT<PointT>                        Joint;
  typedef PoseT<PointT>                         Pose;
  typedef typename ACG::Matrix4x4T<Scalar>      Matrix;
  typedef typename ACG::QuaternionT<Scalar>     Quaternion;
  typedef typename ACG::DualQuaternionT<Scalar> DualQuaternion;

public:
  /** \brief constructor
   *
   * @param _skeleton  Pointer to the skeleton which will be skinned
   * @param _mesh      Pointer to the mesh that is the skin
   * @param _objectID  ObjectId of the Skeleton
   */
  SkinT(SkeletonT<PointT> *_skeleton, MeshT *_mesh, int _objectID);
  virtual ~SkinT();

public:
  /**
    * @name Skinning
    * These methods are related to using a mesh as skin with this skeleton.
    */
  //@{
  void attachSkin();
  void deformSkin();
  void deformSkin(const AnimationHandle &_hAni, Method _method = M_LBS);
  void releaseSkin();
  //@}

  Skeleton* skeleton();

  /// Set whether weights are computed
  void weightsComputed(bool _flag) { weightsComputed_ = _flag; }

  /// Check whether weights were computed
  bool weightsComputed() const { return weightsComputed_; }

private:
  Skeleton*       skeleton_;
  MeshT*          mesh_;
  int             objectId_;

  AnimationHandle lastAnimationHandle_;
  Method          lastmethod_;

  // Flag that indicates whether skin weights have been computed
  bool weightsComputed_;
};

typedef SkinT< TriMesh >   TriMeshSkin;
typedef SkinT< PolyMesh > PolyMeshSkin;

//=============================================================================
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(SKINT_C)
#define SKINT_TEMPLATES
#include "SkinT.cc"
#endif
//=============================================================================
#endif
//=============================================================================


