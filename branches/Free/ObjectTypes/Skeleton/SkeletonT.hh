#ifndef SKELETONT_HH
#define SKELETONT_HH


//== INCLUDES =================================================================
#include <map>
#include <vector>
#include <iostream>
#include "JointT.hh"
#include "Properties.hh"
#include "Animation/AnimationT.hh"
#include "PoseT.hh"

#include <stack>

template <class PointT>
class SkeletonT : public Properties
{
  template<typename>
  friend class JointT;

public:
  typedef PointT                                  Point;
  typedef typename Point::value_type             Scalar;
  typedef JointT<Point>                           Joint;
  typedef PoseT<PointT>                            Pose;
  typedef AnimationT<PointT>                  Animation;
  typedef typename ACG::Matrix4x4T<Scalar>       Matrix;

public:

  /**
    * @brief Iterator class for the skeleton
    *
    * This iterator can be used to iterate over all joints in a top-down (root to leaf), left-to-right order.
    * It is possible to iterate over subtrees by constructing an iterator passing the root of the subtree
    * as parameter. Cast the iterator to boolean to test if it iterated all joints.
    */
  class Iterator
  {
  public:
    Iterator();
    Iterator(Joint *_root);
    Iterator(const Iterator &other);
    ~Iterator();
    Iterator &operator=(const Iterator &other);

  public:
    Iterator &operator++();
    bool operator!=(const Iterator &other) const;
    bool operator==(const Iterator &other) const;
    Joint *operator*() const;
    Joint *operator->() const;
    operator bool() const;

  private:
    Joint *nextSibling(Joint *_pParent, Joint *_pJoint);

  private:
    // Holds the current position in the tree
    Joint *pCurrent_;
    // The stack of joints, marking a path back to the root joint
    stack<Joint*> stJoints_;
  };


public:
  /// Default constructor
  SkeletonT();
  /// Copy constructor
  SkeletonT(const SkeletonT<PointT> &_other);
  /// Destructor
  ~SkeletonT();

public:
  /**
    * @name Modifying the tree structure
    * Use these methods to edit the skeleton tree.
    */
  //@{
  void addJoint(typename SkeletonT<PointT>::Joint *_pParent, typename SkeletonT<PointT>::Joint *_pJoint);
  void removeJoint(typename SkeletonT<PointT>::Joint *_pJoint);
  inline void clear();
  //@}

  /**
    * @name Basic Joint Access
    * Use these methods to access joints in the skeleton.
    */
  //@{
  inline Joint *root();
  inline Joint *joint(const unsigned int &_index);
  int parent(unsigned int _joint);
  unsigned int childCount(unsigned int _joint);
  unsigned int child(unsigned int _joint, unsigned int _child);
  unsigned int jointCount();
  
  /// Iterator over joints of the skeletal tree in TOP-DOWN order (from root to leafs)
  Iterator begin();
  Iterator end();
  //@}
 

  /**
    * @name Animation
    * Use these methods to equip the skeleton with animation data.
    */
  //@{

  inline Pose *pose(const AnimationHandle &_hAni);
  inline Pose *referencePose();

  AnimationHandle addAnimation(std::string _name, Animation* _animation);
  AnimationHandle cloneAnimation(std::string _name, const AnimationHandle &_hAni);
  AnimationHandle animationHandle(std::string _name);
  Animation *animation(std::string _name);
  Animation *animation(const AnimationHandle &_hAni);
  void removeAnimation(std::string _name);
  void removeAnimation(const AnimationHandle &_hAni);
  void clearAnimations();

  unsigned int animationCount();
  const std::string &animationName(unsigned int _index);
  //@}


protected:
  /// update the structure when parent changes for a joint
  void updateFromGlobal(unsigned int _idJoint);

protected:
  
  /// Joints of the skeleton
  std::vector<Joint*> joints_;

  /// Binds a name to each animation
  std::map<std::string, unsigned int> names_;
  /// Animations defined on the skeleton
  std::vector<Animation*> animations_;

  /// The skeletons reference pose
  Pose referencePose_;
};

//=============================================================================
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(SKELETON_C)
#define SKELETONT_TEMPLATES
#include "SkeletonT.cc"
#endif
//=============================================================================
#endif // SKELETONT_HH defined
//=============================================================================

