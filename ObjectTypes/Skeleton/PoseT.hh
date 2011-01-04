#ifndef POSET_HH
#define POSET_HH

#include "ACG/Math/Matrix4x4T.hh"
#include "ACG/Math/VectorT.hh"

#include "ACG/Math/QuaternionT.hh"
#include "ACG/Math/DualQuaternionT.hh"


template<typename PointT>
class SkeletonT;

/**
 * @brief A general pose, used to store the frames of the animation
 *
 */
template<typename PointT>
class PoseT
{
  template<typename>
  friend class SkeletonT;
  template<typename>
  friend class AnimationT;
  template<typename>
  friend class FrameAnimationT;

protected:
  typedef PointT                                Point;
  typedef typename Point::value_type            Scalar;
  typedef typename ACG::VectorT<Scalar, 3>      Vector;
  typedef typename ACG::Matrix4x4T<Scalar>      Matrix;
  typedef typename ACG::QuaternionT<Scalar>     Quaternion;
  typedef typename ACG::DualQuaternionT<Scalar> DualQuaternion;

public:
  /// Constructor
  PoseT(SkeletonT<Point>* _skeleton);
  /// Copy Constructor
  PoseT(const PoseT<PointT>& _other);
  /// Destructor
  virtual ~PoseT();

  /**
    * @name Pose editing
    * These methods update the other coordinate systems, changing the local coordinates will also change the global and vice versa.
    */
  //@{
  
  /// local matrix manipulation
  /// the local matrix represents a joints orientation/translation in the coordinate frame of the parent joint
  inline const Matrix& localMatrix(unsigned int _joint) const;
  void setLocalMatrix(unsigned int _joint, const Matrix &_local, bool _keepLocalChildPositions=true);
  inline Vector localTranslation(unsigned int _joint);
  void setLocalTranslation(unsigned int _joint, const Vector &_position, bool _keepLocalChildPositions=true);  

  inline Matrix localMatrixInv(unsigned int _joint) const;
  
  /// global matrix manipulation
  /// the global matrix represents a joints orientation/translation in global coordinates
  inline const Matrix& globalMatrix(unsigned int _joint) const;
  void setGlobalMatrix(unsigned int _joint, const Matrix &_global, bool _keepGlobalChildPositions=true);
  inline Vector globalTranslation(unsigned int _joint);
  void setGlobalTranslation(unsigned int _joint, const Vector &_position, bool _keepGlobalChildPositions=true);
  
  virtual Matrix globalMatrixInv(unsigned int _joint) const;
  //@}

  /**
    * @name Synchronization
    * Use these methods to keep the pose in sync with the number (and indices) of the joints.
    */
  //@{
  virtual void insertJointAt(unsigned int _index);
  virtual void removeJointAt(unsigned int _index);
  //@}

protected:
  /**
    * @name Coordinate system update methods
    * These methods propagate the change in one of the coordinate systems into the other. This will
    * keep intact the children nodes' positions per default (by recursively updating all children.).
    * This behavior can be influenced via the _keepChildPositions parameter.
    * 
    */
  //@{
  void updateFromLocal(unsigned int _joint, bool _keepChildPositions=true);
  void updateFromGlobal(unsigned int _joint, bool _keepChildPositions=true);
  //@}
  
public:
  /**
    * @name Precalculated values
    * Use these methods to gain access to the precalculations performed by this derivation.
    */
  //@{
  inline const Matrix&         unifiedMatrix(unsigned int _joint);
  inline const Quaternion&     unifiedRotation(unsigned int _joint);
  inline const DualQuaternion& unifiedDualQuaternion(unsigned int _joint);
  //@}

protected:

  /// a pointer to the skeleton
  SkeletonT<PointT>* skeleton_;
  /// the pose in local coordinates
  std::vector<Matrix> local_;
  /// the pose in global coordinates
  std::vector<Matrix> global_;

  /// the global pose matrix left-multiplied to the inverse global reference matrix: \f$ M_{pose} \cdot M^{-1}_{reference} \f$
  std::vector<Matrix> unified_;
  
  /// JointT::PoseT::unified in DualQuaternion representation
  /// note: the real part of the dual quaternion is the rotation part of the transformation as quaternion
  std::vector<DualQuaternion> unifiedDualQuaternion_;
};

//=============================================================================
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(POSET_C)
#define POSET_TEMPLATES
#include "PoseT.cc"
#endif
//=============================================================================
#endif
//=============================================================================

