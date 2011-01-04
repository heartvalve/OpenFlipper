#define POSET_C

#include <assert.h>

//-----------------------------------------------------------------------------

/**
 * @brief Constructor for a new pose
 *
 * The pose will automatically be equipped with the right number of fields for the joints stored in the skeleton.
 * Just fill them with data.
 *
 * @param _hierarchy The skeleton that owns this pose
 * @param _reference A pointer to the reference pose used by the skeleton, used for precalculations
 */
template<typename PointT>
PoseT<PointT>::PoseT(SkeletonT<PointT>* _skeleton) : skeleton_(_skeleton)
{
  assert(_skeleton != 0);

  // add joints until we have the same size as the reference pose
  while( skeleton_->jointCount() > local_.size() )
    insertJointAt( local_.size() );
}

//-----------------------------------------------------------------------------

/**
 * @brief Copy constructor
 *
 * Creates an independent copy of the given pose.
 */
template<typename PointT>
PoseT<PointT>::PoseT(const PoseT<PointT> &_other) : skeleton_(_other.skeleton_)
{
  local_.insert(local_.begin(), _other.local_.begin(), _other.local_.end());
  global_.insert(global_.begin(), _other.global_.begin(), _other.global_.end());
  unified_.insert(unified_.begin(), _other.unified_.begin(), _other.unified_.end());
  unifiedDualQuaternion_.insert(unifiedDualQuaternion_.begin(), _other.unifiedDualQuaternion_.begin(), _other.unifiedDualQuaternion_.end());
}

//-----------------------------------------------------------------------------

/**
 * @brief Destructor
 *
 */
template<typename PointT>
PoseT<PointT>::~PoseT()
{

}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the local matrix for the given joint
 *
 * @param _joint The joints index, same as for SkeletonT::joints_
 * @return The local matrix, relative to the parents coordinate system
 */
template<typename PointT>
inline const typename PoseT<PointT>::Matrix& PoseT<PointT>::localMatrix(unsigned int _joint) const
{
  return local_[_joint];
}

//-----------------------------------------------------------------------------

/**
 * @brief Sets the local coordinate system
 *
 * The change will automatically be propagated to all children. Also the global matrices will be updated.
 *
 * @param _joint The joints index, same as for SkeletonT::joints_
 * @param _local The new local matrix
 */
template<typename PointT>
void PoseT<PointT>::setLocalMatrix(unsigned int _joint, const Matrix &_local, bool _keepLocalChildPositions)
{
  local_[_joint] = _local;

  updateFromLocal(_joint, _keepLocalChildPositions);
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the local translation vector
 *
 * The local translation vector describes the joints position in its parents coordinate system.
 *
 * @param _joint The joints index, same as for SkeletonT::joints_
 * @return The local translation vector (the 4th column in the local matrix)
 */
template<typename PointT>
inline typename PoseT<PointT>::Vector PoseT<PointT>::localTranslation(unsigned int _joint)
{
  Vector ret;
  Matrix &mat = local_[_joint];
  for(int i = 0; i < 3; ++i)
    ret[i] = mat(i, 3);
  return ret;
}

//-----------------------------------------------------------------------------

/**
 * @brief Sets the local translation vector
 *
 * The matrix is otherwise not modified. The change is automatically propagated to all children. Also the
 * global coordinate frames will be updated.
 *
 * @param _joint The joints index, same as for SkeletonT::joints_
 * @param _position The new local translation vector
 */
template<typename PointT>
void PoseT<PointT>::setLocalTranslation(unsigned int _joint, const Vector &_position, bool _keepLocalChildPositions)
{
  Matrix &mat = local_[_joint];
  for(int i = 0; i < 3; ++i)
    mat(i, 3) = _position[i];

  updateFromLocal(_joint, _keepLocalChildPositions);
}

//-----------------------------------------------------------------------------

/**
 * @brief Simply returns the inverse of the local matrix
 */
template<typename PointT>
typename PoseT<PointT>::Matrix PoseT<PointT>::localMatrixInv(unsigned int _joint) const
{
  Matrix ret = local_[_joint];
  ret.invert();
  return ret;
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the global matrix for the given joint
 *
 * @param _joint The joints index, same as for SkeletonT::joints_
 * @return The global matrix, relative to world coordinates
 */
template<typename PointT>
inline const typename PoseT<PointT>::Matrix& PoseT<PointT>::globalMatrix(unsigned int _joint) const
{
  return global_[_joint];
}

//-----------------------------------------------------------------------------

/**
 * @brief Sets the global coordinate system
 *
 * The change will automatically be propagated to all children. Also the local matrices will be updated.
 *
 * @param _joint The joints index, same as for SkeletonT::joints_
 * @param _global The new global matrix
 */
template<typename PointT>
void PoseT<PointT>::setGlobalMatrix(unsigned int _joint, const Matrix &_global, bool _keepGlobalChildPositions)
{
  global_[_joint] = _global;

  updateFromGlobal(_joint, _keepGlobalChildPositions);
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the global translation vector
 *
 * @param _joint The joints index, same as for SkeletonT::joints_
 * @return The global translation vector (the 4th column in the global matrix)
 */
template<typename PointT>
inline typename PoseT<PointT>::Vector PoseT<PointT>::globalTranslation(unsigned int _joint)
{
  Vector ret;
  Matrix &mat = global_[_joint];
  for(int i = 0; i < 3; ++i)
    ret[i] = mat(i, 3);
  return ret;
}

//-----------------------------------------------------------------------------

/**
 * @brief Sets the global translation vector
 *
 * The matrix is otherwise not modified. The change is automatically propagated to all children. Also the
 * local coordinate frames will be updated.
 *
 * @param _joint The joints index, same as for SkeletonT::joints_
 * @param _position The new global translation vector
 * @param _keepGlobalChildPositions Do the children stay at the same position or do they move with their parent joint
 */
template<typename PointT>
void PoseT<PointT>::setGlobalTranslation(unsigned int _joint, const Vector &_position, bool _keepGlobalChildPositions)
{
  Matrix &mat = global_[_joint];
  for(int i = 0; i < 3; ++i)
    mat(i, 3) = _position[i];

  updateFromGlobal(_joint, _keepGlobalChildPositions);
}

//-----------------------------------------------------------------------------

/**
 * @brief Simply returns the inverse of the global matrix
 */
template<typename PointT>
typename PoseT<PointT>::Matrix PoseT<PointT>::globalMatrixInv(unsigned int _joint) const
{
  if (_joint >= global_.size()) {
    std::cerr << "Illegal joint number: " << _joint << std::endl;
    return global_[0];
  }
  Matrix ret = global_[_joint];
  ret.invert();
  return ret;
}

//-----------------------------------------------------------------------------

/**
 * @brief Called by the skeleton/animation as a new joint is inserted
 *
 * To keep the vectors storing the matrices for the joints in sync with the joints a new entry has to be inserted
 * in exactly the same place if a new joint is added to the skeleton. This is done here. Derived classes
 * have to overwrite this method to keep their data members in sync as well. Always call the base class
 * method first.
 *
 * @param _index The new joint is inserted at this position. Insert new joints at the end by passing
 *               SkeletonT::joints_.size as parameter.
 */
template<typename PointT>
void PoseT<PointT>::insertJointAt(unsigned int _index)
{
  Matrix id;
  id.identity();

  DualQuaternion idDQ;
  idDQ.identity();
  
  local_.insert(local_.begin() + _index, id);
  global_.insert(global_.begin() + _index, id);
  unified_.insert(unified_.begin() + _index, id);
  unifiedDualQuaternion_.insert(unifiedDualQuaternion_.begin() + _index, idDQ);
}

//-----------------------------------------------------------------------------

/**
 * @brief Called by the skeleton/animation as a joint is removed
 *
 * To keep the vectors storing the matrices for the joints in sync with the joints exactly the same entry
 * has to be removed as a joint is removed from the skeleton. This is done here. Derived classes
 * have to overwrite this method to keep their data members in sync as well. Always call the base class
 * method first.
 *
 * @param _index The new joint is inserted at this position. Insert new joints at the end by passing
 * 				 SkeletonT::joints_.size as parameter.
 */
template<typename PointT>
void PoseT<PointT>::removeJointAt(unsigned int _index)
{
  local_.erase(local_.begin() + _index);
  global_.erase(global_.begin() + _index);
  unified_.erase(unified_.begin() + _index);
  unifiedDualQuaternion_.erase(unifiedDualQuaternion_.begin() + _index);
}

//-----------------------------------------------------------------------------

/**
 * @brief This method propagates the change in the local coordinate system to the global system and all children
 *
 * @param _joint The updated joints index
 */
template<typename PointT>
void PoseT<PointT>::updateFromLocal(unsigned int _joint, bool _keepChildPositions)
{
  // first update the global coordinate system
  if(skeleton_->parent(_joint) == -1)
    global_[_joint] = local_[_joint];
  else
    global_[_joint] = globalMatrix(skeleton_->parent(_joint)) * localMatrix(_joint);

  // update the unified matrices
  Matrix matRefGlobalInv = skeleton_->referencePose()->globalMatrix(_joint);
  matRefGlobalInv.invert();

  unified_[_joint] = globalMatrix(_joint) * matRefGlobalInv;
  unifiedDualQuaternion_[_joint] = DualQuaternion(unified_[_joint]);

  // update children
  if (_keepChildPositions) {
    // finally update all children as well
    for(unsigned int i = 0; i < skeleton_->childCount(_joint); ++i) {
      updateFromLocal(skeleton_->child(_joint, i));
    }
  } else {
    updateFromGlobal(_joint, true);    //this will adjust the childrens' positions according to the _joint position.
  }
}

//-----------------------------------------------------------------------------

/**
 * @brief This method propagates the change in the global coordinate system to the local system and all children
 *
 * Do not overwrite this method, instead overwrite BasePose::UpdateFromGlobalCallback. Otherwise the recursion
 * will become a problem.
 *
 * @param _joint The updated joints index
 */
template<typename PointT>
void PoseT<PointT>::updateFromGlobal(unsigned int _joint, bool _keepChildPositions)
{
  // first update the local coordinate system
  if(skeleton_->parent(_joint) == -1)
    local_[_joint] = global_[_joint];
  else
    local_[_joint] = globalMatrixInv(skeleton_->parent(_joint)) * globalMatrix(_joint);

  // update the unified matrices
  Matrix matRefGlobalInv = skeleton_->referencePose()->globalMatrix(_joint);
  matRefGlobalInv.invert();

  unified_[_joint] = globalMatrix(_joint) * matRefGlobalInv;
  unifiedDualQuaternion_[_joint] = DualQuaternion(unified_[_joint]);

  // update children  
  if (_keepChildPositions) {
    for(unsigned int i = 0; i < skeleton_->childCount(_joint); ++i) {
      updateFromGlobal(skeleton_->child(_joint, i));
    }
  } else {
    updateFromLocal(_joint, true);    //this will adjust the childrens' positions according to the _joint position.
  }
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the unified matrix
 *
 * The unified matrix stores
 * \f[ M_{unified} = M_{pose} \cdot M^{-1}_{reference} \f]
 * speeding up the calculation of the vertices in the current pose
 * \f[ v_{pose} = M_{pose} \cdot M^{-1}_{reference} \cdot v_{reference} \f]
 * \f[ v_{pose} = M_{unified} \cdot v_{reference} \f]
 *
 * @param _joint The joints index, same as for SkeletonT::joints_
 */
template<typename PointT>
inline const typename PoseT<PointT>::Matrix& PoseT<PointT>::unifiedMatrix(unsigned int _joint)
{
  return unified_[_joint];
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns a quaternion holding the rotational part of the unified matrix
 *
 * This is used by the spherical blend skinning.
 *
 * The rotational part of the unified matrix is stored in the real part of the dual quaternion
 *
 * @param _joint The joints index, same as for SkeletonT::joints_
 * @return The rotational part of the unified matrix
 */
template<typename PointT>
inline const typename PoseT<PointT>::Quaternion& PoseT<PointT>::unifiedRotation(unsigned int _joint)
{
  return unifiedDualQuaternion_[_joint].real();
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns a dual quaternion holding the unified matrix represented as dual quaternion
 *
 * This is used by the Dual Quaternion blend skinning.
 *
 * @param _joint The joints index, same as for SkeletonT::joints_
 * @return The rotational part of the unified matrix
 */
template<typename PointT>
inline const typename PoseT<PointT>::DualQuaternion& PoseT<PointT>::unifiedDualQuaternion(unsigned int _joint)
{
  return unifiedDualQuaternion_[_joint];
}

//-----------------------------------------------------------------------------

