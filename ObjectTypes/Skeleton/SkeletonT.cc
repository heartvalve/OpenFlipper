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

//-----------------------------------------------------------------------------
//
//  CLASS SkeletonT - IMPLEMENTATION
//
//-----------------------------------------------------------------------------

#define SKELETON_C

//== INCLUDES =================================================================

#include <iostream>
#include <algorithm>
#include <utility>

#include "SkeletonT.hh"

#include "Animation/FrameAnimationT.hh"


//-----------------------------------------------------------------------------
// ITERATOR - IMPLEMENTATION 
//-----------------------------------------------------------------------------

/**
 * @brief Default constructor
 *
 * Creates an invalid iterator, not pointing to anything yet.
 */
template<typename PointT>
SkeletonT<PointT>::Iterator::Iterator()
{
  pCurrent_ = 0;
}

//-----------------------------------------------------------------------------

/**
 * @brief Constructor - Creates an iterator for the given (sub-)tree
 *
 * The iterator will traverse all nodes below _root (inclusive _root) in a top-down, "left-to-right" manner.
 */
template<typename PointT>
SkeletonT<PointT>::Iterator::Iterator(Joint *_root)
{
  pCurrent_ = _root;
}

//-----------------------------------------------------------------------------

/**
 * @brief Copy Constructor - Creates a copy of the given iterator
 *
 * This iterator will become a copy of the given iterator. They both point to the same position now, but
 * iterate the tree independently.
 */
template<typename PointT>
SkeletonT<PointT>::Iterator::Iterator(const Iterator &other)
{
  pCurrent_ = other.pCurrent_;
  stJoints_ = other.stJoints_;
}

//-----------------------------------------------------------------------------

/**
 * @brief Destructor
 *
 */
template<typename PointT>
SkeletonT<PointT>::Iterator::~Iterator()
{
}

//-----------------------------------------------------------------------------

/**
 * @brief Assignment Operator
 *
 * Assign values from given iterator to this iterator and also return self
 * 
 */
template<typename PointT>
typename SkeletonT<PointT>::Iterator &SkeletonT<PointT>::Iterator::operator=(const Iterator &other)
{
  pCurrent_ = other.pCurrent_;
  stJoints_ = other.stJoints_;
  return *this;
}

//-----------------------------------------------------------------------------

/**
 * @brief Increase the iterator
 *
 * The iterator will be changed to point to the next child, or whatever joint is next.
 */
template<typename PointT>
typename SkeletonT<PointT>::Iterator &SkeletonT<PointT>::Iterator::operator++()
{
  if(pCurrent_ == 0)
    return *this;

  // try to make this iterator point to the first child
  if(pCurrent_->size() > 0)
  {
    // there are children, so add the current joint to the stack and choose the first child as new current position
    stJoints_.push(pCurrent_);
    pCurrent_ = pCurrent_->child(0);
  }else{

    if ( pCurrent_->isRoot() ){
      //root without children -> return invalid iterator
      pCurrent_ = 0;

    } else {
      // there are no children left, so try to get the next sibling
      Joint *pSibling = nextSibling(stJoints_.top(), pCurrent_);

      while(pSibling == 0 && !stJoints_.empty())
      {
              // there is no sibling, so try the parents sibling and so on
              Joint *pParent = stJoints_.top();
              stJoints_.pop();
              if(!stJoints_.empty())
                      pSibling = nextSibling(stJoints_.top(), pParent);
      }

      // did we fail to find a next node?
      // if so pSibling is 0 now and will invalidate the iterator
      pCurrent_ = pSibling;
    }
  }
  return *this;
}

//-----------------------------------------------------------------------------

/**
 * @brief Compares the iterators
 *
 * The iterators are equal if they point to the same joint, no matter where they began to iterate.
 */
template<typename PointT>
bool SkeletonT<PointT>::Iterator::operator!=(const Iterator &other) const
{
  return pCurrent_ != other.pCurrent_;
}

//-----------------------------------------------------------------------------

/**
 * @brief Compares the iterators
 *
 * The iterators are equal if they point to the same joint, no matter where they began to iterate.
 */
template<typename PointT>
bool SkeletonT<PointT>::Iterator::operator==(const Iterator &other) const
{
  return pCurrent_ == other.pCurrent_;
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns a pointer to the current joint
 */
template<typename PointT>
typename SkeletonT<PointT>::Joint *SkeletonT<PointT>::Iterator::operator*() const
{
  return pCurrent_;
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns a pointer to the current joint
 *
 * This way it is possible to access the joint by calling:
 * @code
 * SkeletonT::Iterator it;
 * ...
 * it->method();
 * @endcode
 */
template<typename PointT>
typename SkeletonT<PointT>::Joint *SkeletonT<PointT>::Iterator::operator->() const
{
  return pCurrent_;
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns false if the iterator is done iterating
 *
 * There are two ways to test if the joint iterator has reached its end. Cast it to bool and test if it is
 * false or compare it to the return value of SkeletonT::end.
 */
template<typename PointT>
SkeletonT<PointT>::Iterator::operator bool() const
{
  return pCurrent_ != 0;
}

//-----------------------------------------------------------------------------

/**
 * @brief Given a parent and one of its child nodes this method finds and returns the next sibling
 *
 * @param _pParent The parent node, take it from the stack
 * @param _pJoint A child node of _pParent, its next sibling is wanted
 * @return Either a pointer to the sibling or 0 if there are no more siblings
 */
template<typename PointT>
typename SkeletonT<PointT>::Joint *SkeletonT<PointT>::Iterator::nextSibling(Joint *_pParent, Joint *_pJoint)
{
  // first find the current node in the parents list of children
  typename Joint::ChildIter it;
  for(it = _pParent->begin(); it != _pParent->end(); ++it)
  {
    if(*it == _pJoint)
    {
      // found it. If there is another child it is the wanted sibling
      ++it;
      if(it == _pParent->end())
        return 0;  // oh no, we ran out of siblings

      return *it;
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------
// AnimationIterator - IMPLEMENTATION
//-----------------------------------------------------------------------------

/**
 * @brief Default constructor
 *
 * Creates an iterator pointing to the beginning of the vector of animations
 */
template<typename PointT>
SkeletonT<PointT>::AnimationIterator::AnimationIterator(std::vector<Animation*>& _animations ) :
  animations_(_animations)
{
  currentIndex_ = 0;

  // Increment, until we are at the end or we found a valid animation which is not deleted ( == 0 )
  while ( currentIndex_ < animations_.size() &&  animations_[currentIndex_] == 0) {
    currentIndex_++;
  }

}

/**
 * @brief Constructor - Creates an iterator for the given animation set starting at a specific position
 *
 */
template<typename PointT>
SkeletonT<PointT>::AnimationIterator::AnimationIterator(std::vector<Animation*>& _animations, unsigned int _animationIndex ) :
animations_(_animations)
{
  currentIndex_ = _animationIndex;

  // Increment, until we are at the end or we found a valid animation which is not deleted ( == 0 )
  while ( currentIndex_ < animations_.size() &&  animations_[currentIndex_] == 0) {
    currentIndex_++;
  }
}

/**
 * @brief Increase the iterator
 *
 * The iterator will be changed to the next animation in the list
 */
template<typename PointT>
typename SkeletonT<PointT>::AnimationIterator& SkeletonT<PointT>::AnimationIterator::operator++() {
  currentIndex_++;

  // Increment, until we are at the end or we found a valid animation which is not deleted ( == 0 )
  while ( currentIndex_ < animations_.size() &&  animations_[currentIndex_] == 0) {
    currentIndex_++;
  }

  return *this;
}

/**
 * @brief Operator =
 *
 */
template<typename PointT>
typename SkeletonT<PointT>::AnimationIterator& SkeletonT<PointT>::AnimationIterator::operator=(const AnimationIterator &other) {
  currentIndex_ = other.currentIndex_;
  animations_ = other.animations_;
  return *this;
}

/**
 * @brief boolean operator
 *
 * Returns true if the iterator is pointing to a valid handle
 */
template<typename PointT>
SkeletonT<PointT>::AnimationIterator::operator bool() const {
  return ( currentIndex_ < animations_.size() );
}

/**
 * @brief Get an animation handle for the current animation
 *
 */
template<typename PointT>
AnimationHandle SkeletonT<PointT>::AnimationIterator::operator*() const {
  return AnimationHandle(currentIndex_);
}

//-----------------------------------------------------------------------------
// SKELETONT - IMPLEMENTATION
//-----------------------------------------------------------------------------

/**
 * @brief Default Constructor
 */
template<typename PointT>
SkeletonT<PointT>::SkeletonT() : referencePose_(this)
{
}

//-----------------------------------------------------------------------------

/**
 * @brief Copy Constructor - Creates a copy of the given skeleton
 *
 * The copy does not inherit properties. You have to copy them yourself if you need them in the clone.
 */
template<typename PointT>
SkeletonT<PointT>::SkeletonT(const SkeletonT<PointT> &_other) :
  Properties(),
  referencePose_(this)
{
  // create a copy of the joints, not yet linked because they refer to each other using pointers
  for(typename std::vector<Joint*>::const_iterator it = _other.joints_.begin(); it != _other.joints_.end(); ++it)
  {
    joints_.push_back(new Joint(**it));
    insert_property_at( (*it)->id() );
  }

  // construct the links
  for(typename std::vector<Joint*>::const_iterator it = _other.joints_.begin(); it != _other.joints_.end(); ++it)
  {
    Joint *pJoint = *it;

    if(pJoint->parent() != 0)
      joint(pJoint->id())->parent_ = joint(pJoint->parent()->id());
    else
      joint(pJoint->id())->parent_ = 0;
    for(typename Joint::ChildIter it_ch = pJoint->begin(); it_ch != pJoint->end(); ++it_ch)
      joint(pJoint->id())->children_.push_back( joint((*it_ch)->id()) );
  }

  names_.insert(_other.names_.begin(), _other.names_.end());

  for(typename std::vector<Animation*>::const_iterator it = _other.animations_.begin(); it != _other.animations_.end(); ++it)
    if (*it) {
        animations_.push_back((**it).copy());
    }

  referencePose_ = _other.referencePose_;
}

//-----------------------------------------------------------------------------

/**
 * @brief Assignment operator - returns a copy of the skeleton
 *
 * The copy does not inherit properties. You have to copy them yourself if you need them in the clone.
 */
template<typename PointT>
SkeletonT<PointT>& SkeletonT<PointT>::operator= (const SkeletonT<PointT>& _other){

  if (this != &_other){ // protect against invalid self-assignment

    // clear the current skeleton
    clear();

    // create a copy of the joints, not yet linked because they refer to each other using pointers
    for(typename std::vector<Joint*>::const_iterator it = _other.joints_.begin(); it != _other.joints_.end(); ++it){
      joints_.push_back(new Joint(**it));
      insert_property_at( (*it)->id() );
    }

    // construct the links
    for(typename std::vector<Joint*>::const_iterator it = _other.joints_.begin(); it != _other.joints_.end(); ++it){
      Joint *pJoint = *it;

      if(pJoint->parent() != 0)
        joint(pJoint->id())->parent_ = joint(pJoint->parent()->id());
      else
        joint(pJoint->id())->parent_ = 0;

      for(typename Joint::ChildIter it_ch = pJoint->begin(); it_ch != pJoint->end(); ++it_ch)
        joint(pJoint->id())->children_.push_back( joint((*it_ch)->id()) );
    }

    names_.insert(_other.names_.begin(), _other.names_.end());

    for(typename std::vector<Animation*>::const_iterator it = _other.animations_.begin(); it != _other.animations_.end(); ++it)
      if (*it)
        animations_.push_back((**it).copy());

    referencePose_ = _other.referencePose_;
  }

  return *this;
}

//-----------------------------------------------------------------------------

/**
 * @brief Destructor
 *
 */
template<typename PointT>
SkeletonT<PointT>::~SkeletonT( )
{
  // clear the joints and animations
  clear();
}

//-----------------------------------------------------------------------------

/**
 * @brief Adds a joint as child of a given parent joint
 *
 * Pass 0 as parent if you want to set a new root joint. This will remove all joints from the skeleton,
 * clean the properties and clear all animation data.
 *
 * @param _pParent The parent joint, 0 if the root is set
 * @param _pJoint The joint that has to be added to the tree
 */
template<typename PointT>
void SkeletonT<PointT>::addJoint(typename SkeletonT<PointT>::Joint *_pParent, typename SkeletonT<PointT>::Joint *_pJoint)
{
  unsigned int newJointID;
  
  if(_pParent == 0)
  {
    clear();
    clean_properties();

    _pJoint->setId(0);
    joints_.push_back(_pJoint);

    newJointID = 0;
  }else{
    _pParent->children_.push_back(_pJoint); // tell the parent about the new child
    _pJoint->setId(joints_.size());         // set its id
    joints_.push_back(_pJoint);             // add it to the skeleton vector

    newJointID = joints_.size() - 1;
  }

  //onAddJoint
  insert_property_at(newJointID);

  referencePose_.insertJointAt(newJointID);
  for(typename std::vector<Animation*>::iterator it = animations_.begin(); it != animations_.end(); ++it)
      if (*it)
          (*it)->insertJointAt(newJointID);

  referencePose_.updateFromGlobal(0, true);
}

//-----------------------------------------------------------------------------

/**
 * @brief Remove the given joint from the tree
 *
 * This method will not only remove the given joint from the vector, it will also reduce the index of all
 * joints with a larger index by one. This is to compensate for their new position in the vector and to
 * make sure the sequence of indices is still continuous.
 *
 * Also all poses and properties are updated.
 */
template<typename PointT>
void SkeletonT<PointT>::removeJoint(typename SkeletonT<PointT>::Joint *_pJoint)
{

  if (joints_.size() == 1){
    std::cerr << "Cannot delete last joint. Delete the skeleton instead." << std::endl;
    return;
  }

  remove_property_at(_pJoint->id());
  referencePose_.removeJointAt(_pJoint->id());

  for(typename std::vector<Animation*>::iterator it = animations_.begin(); it != animations_.end(); ++it)
      if (*it) (*it)->removeJointAt(_pJoint->id());

  // Reattach the deleted joint's children to the joint's parent
  typename SkeletonT<PointT>::Joint::ChildIter c_it = _pJoint->begin();

  if( _pJoint->parent() == 0 ){
    //root removal
    typename SkeletonT<PointT>::Joint* newRoot = *c_it; //first child is new root
    newRoot->parent_ = 0;
    ++c_it;

    for ( ; c_it!=_pJoint->end(); ++c_it) {
      (*c_it)->parent_ = newRoot;
      newRoot->children_.push_back(*c_it);
    }

  } else {

    for ( ; c_it!=_pJoint->end(); ++c_it) {
      (*c_it)->parent_ = _pJoint->parent_;
      _pJoint->parent_->children_.push_back(*c_it);
    }

    if(std::remove(_pJoint->parent_->children_.begin(), _pJoint->parent_->children_.end(), _pJoint) != _pJoint->parent_->children_.end())	// remove the joint from its parent
      _pJoint->parent_->children_.resize(_pJoint->parent_->children_.size() - 1);
  }

  typename std::vector<Joint*>::iterator it = joints_.begin() + _pJoint->id(); // iterator pointing to the element that has to be erased
  it = joints_.erase(it);                                                      // erase the element
  for(; it != joints_.end(); ++it)                                             // for all following elements
    (*it)->setId((*it)->id() - 1);                                             // reduce their index by one (since they have been moved there)

  referencePose_.updateFromGlobal(0, true);
  for (typename std::vector<Animation*>::iterator a_it = animations_.begin(); a_it != animations_.end(); ++a_it) {
    if (*a_it)
      (*a_it)->updateFromGlobal(0);
  }
}

//-----------------------------------------------------------------------------

/**
 * @brief Removes all joints from the skeleton
 *
 * This method also frees the memory occupied by the joints calling \e delete on each of them. Notice that
 * all animations will be deleted as well, since without joint there cannot be animation. Also the properties
 * are cleaned.
 */
template<typename PointT>
inline void SkeletonT<PointT>::clear()
{
  // no joints, so no animation either
  clean_properties();
  clearAnimations();

  // clear the joints
  typename std::vector<Joint*>::iterator it;
  for(it = joints_.begin(); it != joints_.end(); ++it)
    delete *it;

  joints_.clear();
  names_.clear();
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the root joint
 *
 * The root joint has always index 0.
 *
 * @return Returns the root joint or 0 if the tree is empty
 */
template<typename PointT>
inline typename SkeletonT<PointT>::Joint *SkeletonT<PointT>::root()
{
  if(joints_.empty())
    return 0;
  return joints_[0];
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the joint with the given index
 *
 * JointT::id_ is always equal to _index.
 *
 * @param _index The joints index, in the range [0, joints_.size)
 * @return Returns a pointer to the joint or 0 if the index does not exist.
 */
template<typename PointT>
inline typename SkeletonT<PointT>::Joint *SkeletonT<PointT>::joint(const unsigned int& _index)
{
  if(_index >= joints_.size())
    return 0;
  return joints_[_index];
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the parents id of the given node
 *
 * @param _joint A nodes index
 * @return The parent nodes index or -1 if \e _joint is the root node
 */
template<typename PointT>
int SkeletonT<PointT>::parent(unsigned int _joint)
{
  if(joints_[_joint]->parent() == 0)
    return -1;
  return joints_[_joint]->parent()->id();
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the number of children of the given node
 */
template<typename PointT>
unsigned int SkeletonT<PointT>::childCount(unsigned int _joint)
{
  if ( _joint >= joints_.size() ){
    std::cerr << "SkeletonT : childCount() called with non-existing joint " << _joint << std::endl;
    return 0;
  }
  
  return joints_[_joint]->size();
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the child with the given index
 *
 * @param _joint An index identifying a joint
 * @param _child An index identifying a child of that joint
 */
template<typename PointT>
unsigned int SkeletonT<PointT>::child(unsigned int _joint, unsigned int _child)
{
  return joints_[_joint]->child(_child)->id();
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the number of joints
 */
template<typename PointT>
unsigned int SkeletonT<PointT>::jointCount()
{
  return joints_.size();
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns an iterator pointing to the root joint
 *
 * Use the iterator to traverse the skeleton in a top-down, "left-to-right" manner.
 */
template<typename PointT>
typename SkeletonT<PointT>::Iterator SkeletonT<PointT>::begin()
{
  return Iterator(root());
}

//-----------------------------------------------------------------------------

/**
 * @brief Compare an iterator with the return value of this method to test if it is done
 */
template<typename PointT>
typename SkeletonT<PointT>::Iterator SkeletonT<PointT>::end()
{
  // why return Iterator(0)? operator= will compare the current pointer, and 0 is used once the iterator
  // passed the last joint
  return Iterator(0);
}


//-----------------------------------------------------------------------------

/**
 * @brief Returns a pointer to the pose with the given animation handle
 *
 * If the animation handle is invalid the reference pose is returned.
 */
template<typename PointT>
inline typename SkeletonT<PointT>::Pose* SkeletonT<PointT>::pose(const AnimationHandle &_hAni)
{
  if(_hAni.isValid() && _hAni.animationIndex() < animations_.size() && animations_[_hAni.animationIndex()] != NULL)
    return animations_[_hAni.animationIndex()]->pose(_hAni.frame());
  else
    return &referencePose_;
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns a pointer to the reference pose
 *
 * Use this if you need access to the special data members of the reference pose. Notice you can also
 * get the reference pose by passing AnimationHandle() to the SkeletonT::pose method:
 * @code
 *   skeleton.pose(AnimationHandle());
 * @endcode
 */
template<typename PointT>
inline typename SkeletonT<PointT>::Pose* SkeletonT<PointT>::referencePose()
{
  return &referencePose_;
}

//-----------------------------------------------------------------------------

/**
 * @brief Adds a new animation to the list
 *
 * The name is used to identify the animation. The AnimationHandle to the animation
 * becomes invalid as soon as an animation is removed.
 */
template<typename PointT>
AnimationHandle SkeletonT<PointT>::addAnimation(std::string _name, Animation *_animation)
{
  // try to find an unused animation slot first
  typename std::vector<Animation*>::iterator f;
  for(f = animations_.begin(); f != animations_.end(); ++f)
    if(*f == 0)
      break;

  if(f == animations_.end())
  {
    // all in use, append
    names_.insert( std::pair<std::string, unsigned int>(_name, animations_.size()) );
    animations_.push_back(_animation);
  }else{
    // found an empty one, use it
    names_.insert( std::pair<std::string, unsigned int>(_name, f - animations_.begin()) );
    *f = _animation;
  }
  
  if (_animation)
    _animation->setName(_name);

  return AnimationHandle(names_[_name]);
}

//-----------------------------------------------------------------------------

/**
 * @brief Creates a new animation by cloning an existing one
 *
 * All poses for all joints in all frames of the existing animation will be cloned. If the animation
 * handle is invalid a copy of the default pose will be created.
 */
template<typename PointT>
AnimationHandle SkeletonT<PointT>::cloneAnimation(std::string _name, const AnimationHandle &_hAni)
{
  // try to find an unused animation slot first
  typename std::vector<Animation*>::iterator f;
  for(f = animations_.begin(); f != animations_.end(); ++f)
    if(*f == 0)
      break;

  if(f == animations_.end())
  {
    // all in use, append
    names_.insert( std::pair<std::string, unsigned int>(_name, animations_.size()) );
    if(animation(_hAni) != 0)
      animations_.push_back((*animation(_hAni)).copy());
    else
      animations_.push_back(new FrameAnimationT<Point>(referencePose_));
  }else{
    // found an empty one, use it
    names_.insert( std::pair<std::string, unsigned int>(_name, f - animations_.begin()) );
    if(animation(_hAni) != 0)
      *f = (*animation(_hAni)).copy();
    else
      *f = new FrameAnimationT<Point>(referencePose_);
  }

  return AnimationHandle(names_[_name]);
}

//-----------------------------------------------------------------------------

/**
 * @brief Get an AnimationHandle to the animation with the given name
 */
template<typename PointT>
AnimationHandle SkeletonT<PointT>::animationHandle(std::string _name)
{
  std::map<std::string, unsigned int>::iterator f = names_.find(_name);
  if(f == names_.end())
    return AnimationHandle();
  
  return AnimationHandle(f->second);
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns a pointer to the animation to the given name
 */
template<typename PointT>
typename SkeletonT<PointT>::Animation *SkeletonT<PointT>::animation(std::string _name)
{
  std::map<std::string, unsigned int>::iterator f = names_.find(_name);
  if(f == names_.end())
    return 0;

  return animations_[f->second];
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns a pointer to the animation to the given name
 */
template<typename PointT>
typename SkeletonT<PointT>::Animation *SkeletonT<PointT>::animation(const AnimationHandle &_hAni)
{
  if(!_hAni.isValid())
    return 0;
  return animations_[_hAni.animationIndex()];
}

//-----------------------------------------------------------------------------

/**
 * @brief Removes an animation from the list
 */
template<typename PointT>
void SkeletonT<PointT>::removeAnimation(std::string _name)
{
  // get an iterator for the animation
  std::map<std::string, unsigned int>::iterator f = names_.find(_name);
  if(f == names_.end())
    return;

  // delete the animation
  delete animations_[f->second];
  animations_[f->second] = 0;
  // remove the name entry
  names_.erase(f);
}

//-----------------------------------------------------------------------------

/**
 * @brief Removes an animation from the list
 */
template<typename PointT>
void SkeletonT<PointT>::removeAnimation(const AnimationHandle &_hAni)
{
  // delete the animation
  delete animations_[_hAni.animationIndex()];
  animations_[_hAni.animationIndex()] = 0;

  // remove the name entry
  for(typename std::map<std::string, unsigned int>::iterator it = names_.begin(); it != names_.end(); ++it)
  {
    if(it->second == _hAni.animationIndex())
    {
      names_.erase(it);
      break;
    }
  }
}

//-----------------------------------------------------------------------------

/**
 * @brief Removes all animations
 */
template<typename PointT>
void SkeletonT<PointT>::clearAnimations()
{
  names_.clear();

  for(typename std::vector<Animation*>::iterator it = animations_.begin(); it != animations_.end(); ++it)
    delete *it;
  animations_.clear();
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns an iterator pointing to the first animation
 *
 */
template<typename PointT>
typename SkeletonT<PointT>::AnimationIterator SkeletonT<PointT>::animationsBegin()
{
  return AnimationIterator(animations_);
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns an iterator pointing behind the last animation
 */
template<typename PointT>
typename SkeletonT<PointT>::AnimationIterator SkeletonT<PointT>::animationsEnd()
{

  return AnimationIterator(animations_.size());
}


//-----------------------------------------------------------------------------

/**
 * @brief Returns the number of animations stored in this skeleton
 *
 * @see animationName
 */
template<typename PointT>
unsigned int SkeletonT<PointT>::animationCount()
{
  return names_.size();
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the name of the animation with the given index
 *
 * @see animationCount
 */
template<typename PointT>
const std::string &SkeletonT<PointT>::animationName(unsigned int _index)
{
  unsigned int i = 0;
  std::map<std::string, unsigned int>::iterator pos = names_.begin();

  while(pos->second != _index && pos != names_.end())
  {
    ++i;
    ++pos;
  }

  return pos->first;
}

//-----------------------------------------------------------------------------

/**
 * @brief Updates the local matrix for the given joint in all poses
 *
 * @param _idJoint The joints index
 */
template<typename PointT>
void SkeletonT<PointT>::updateFromGlobal(unsigned int _idJoint)
{
  referencePose_.updateFromGlobal(_idJoint);
  for(typename std::vector<Animation*>::iterator it = animations_.begin(); it != animations_.end(); ++it) {
    if (*it)
      (*it)->updateFromGlobal(_idJoint);
  }
}

//-----------------------------------------------------------------------------

/**
 * @brief insert a Joint given its future child joint
 *
 * @param _pChild existing joint, will be the child
 * @param _pInsert joint, which will be inserted. _pChild will be his child and the parent of _pChild will be his parent.
 */
template<typename PointT>
void SkeletonT<PointT>::insertJoint(typename SkeletonT<PointT>::Joint *_pChild, typename SkeletonT<PointT>::Joint *_pInsert)
{
	if (!_pChild || !_pChild->parent() || !_pInsert)
		return;

	Joint* parent = _pChild->parent();

	//update IDs of our joints
	unsigned int childID = _pChild->id();
	for(typename std::vector<Joint*>::iterator it = joints_.begin() + childID; it !=  joints_.end(); ++it)
		(*it)->setId((*it)->id() + 1);

	//insert our new joint into this skeleton
	joints_.insert(joints_.begin() + childID, _pInsert);
	_pInsert->setId(childID);

	//update the parents
	//note: pChild will be automatically erased in parent->children_
	_pInsert->setParent(parent, *this);
	_pChild->setParent(_pInsert, *this);

	insert_property_at(childID);

	referencePose_.insertJointAt(childID);
	for(typename std::vector<Animation*>::iterator it = animations_.begin(); it != animations_.end(); ++it)
		if (*it)
			(*it)->insertJointAt(childID);

	referencePose_.updateFromGlobal(0, true);
}

//-----------------------------------------------------------------------------

