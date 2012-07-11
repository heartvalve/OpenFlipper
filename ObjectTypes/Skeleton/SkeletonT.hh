/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

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
    std::stack<Joint*> stJoints_;
  };

  /**
    * @brief Iterator class for the animations attached to a skeleton
    *
    * This iterator can be used to iterate over all animations that are attached to the skeleton
    */
  class AnimationIterator {
  public:
    AnimationIterator(std::vector<Animation*>& _animations );
    AnimationIterator(std::vector<Animation*>& _animations, unsigned int _animationIndex );

  public:
    AnimationIterator &operator++();
    AnimationHandle operator*() const;
    AnimationIterator &operator=(const AnimationIterator &other);
    operator bool() const;

  private:
    unsigned int currentIndex_;

    std::vector<Animation*>& animations_;
  };

public:
  /// Default constructor
  SkeletonT();
  /// Copy constructor
  SkeletonT(const SkeletonT<PointT>& _other);
  /// Assignment operator
  SkeletonT& operator= (const SkeletonT<PointT>& _other);

  /// Destructor
  ~SkeletonT();

public:
  /**
    * @name Modifying the tree structure
    * Use these methods to edit the skeleton tree.
    */
  ///@{
  void addJoint(typename SkeletonT<PointT>::Joint *_pParent, typename SkeletonT<PointT>::Joint *_pJoint);
  void insertJoint(typename SkeletonT<PointT>::Joint *_pChild, typename SkeletonT<PointT>::Joint *_pInsert);
  void removeJoint(typename SkeletonT<PointT>::Joint *_pJoint);
  inline void clear();
  ///@}

  /** \anchor JointAccess
    * @name   Basic Joint Access
    *
    * Use these methods to access joints in the skeleton.
    */
  ///@{
  inline Joint *root();
  inline Joint *joint(const unsigned int &_index);
  int parent(unsigned int _joint);
  unsigned int childCount(unsigned int _joint);
  unsigned int child(unsigned int _joint, unsigned int _child);
  unsigned int jointCount();
  
  /// Iterator over joints of the skeletal tree in TOP-DOWN order (from root to leafs)
  Iterator begin();
  Iterator end();
  ///@}
 
  /** \anchor AnimationAccess
    * @name   Animation
    * Use these methods to equip the skeleton with animation data.
    */
  ///@{

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

  void replaceAnimationName(const std::string& _strOld, const std::string& _strNew) {
      std::map<std::string,unsigned int>::iterator f = names_.find(_strOld);
      if(f != names_.end()) {
          unsigned int c = f->second;
          names_.erase(f);
          names_[_strNew] = c;
      }
  }

  /// Iterator over the animations
  AnimationIterator animationsBegin();
  AnimationIterator animationsEnd();

  unsigned int animationCount();
  const std::string &animationName(unsigned int _index);
  ///@}


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

