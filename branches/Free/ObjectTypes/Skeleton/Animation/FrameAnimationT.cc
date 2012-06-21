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

#define FRAMEANIMATIONT_C

#include <vector>
#include <assert.h>

using namespace std;

//-----------------------------------------------------------------------------

/**
 * @brief Constructor - Creates a new animation consisting of a single pose
 *
 * The animation will hold a single frame, made up by a copy of the given pose. After this call returns the
 * pose given by \e _pose is no longer needed and independent from this instance.
 *
 * You can use this operation to clone the reference pose as new instance, then modify the pose.
 *
 * @param _pose This pose will make up the only frame in this new animation
 */
template<class PointT>
FrameAnimationT<PointT>::FrameAnimationT(const PoseT<PointT> &_pose) :
  skeleton_(_pose.skeleton_)
{
  poses_.push_back(new Pose(_pose));
}

//-----------------------------------------------------------------------------

/** \brief Constructor - Creates a new empty animation
 *
 * @param _skeleton The skeleton that will hold this animation
 */
template<class PointT>
FrameAnimationT<PointT>::FrameAnimationT(Skeleton* _skeleton) :
  skeleton_(_skeleton)
{
}

//-----------------------------------------------------------------------------

/**
 * @brief Constructor - Creates a new animation with the given number of frames
 *
 * The poses in the given number of frames will all hold identity matrices for all joints. Make sure you write
 * data to the poses before you use it.
 *
 * @param _skeleton    The skeleton that will hold this animation
 * @param _iNumFrames  The number of frames for this animation
 */
template<class PointT>
FrameAnimationT<PointT>::FrameAnimationT(Skeleton* _skeleton, unsigned int _iNumFrames) :
  skeleton_(_skeleton)
{
  for(unsigned int i = 0; i < _iNumFrames; ++i)
    poses_.push_back(new Pose(_skeleton));
}

//-----------------------------------------------------------------------------

/**
 * @brief Copy constructor
 *
 * This animation will copy all frames from the given animation. After the call returns they are completely
 * independent.
 *
 * @param _other The animation to copy from
 */
template<class PointT>
FrameAnimationT<PointT>::FrameAnimationT(const FrameAnimationT<PointT> &_other) :
  AnimationT<PointT>(),
  skeleton_(_other.skeleton_)
{
  for(typename vector<Pose*>::const_iterator it = _other.poses_.begin(); it != _other.poses_.end(); ++it)
    poses_.push_back(new Pose(**it));
}

//-----------------------------------------------------------------------------

/**
 * @brief Destructor
 */
template<class PointT>
FrameAnimationT<PointT>::~FrameAnimationT()
{
  for(typename vector<Pose*>::iterator it = poses_.begin(); it != poses_.end(); ++it)
    delete *it;
  poses_.clear();
}

//-----------------------------------------------------------------------------

/**
 * @brief Copy Function
 */
template<class PointT>
AnimationT<PointT>* FrameAnimationT<PointT>::copy() {
  return new FrameAnimationT<PointT>(*this);
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns a pointer to the pose stored in the given frame
 *
 * @param _iFrame The poses frame number
 */
template<class PointT>
typename FrameAnimationT<PointT>::Pose *FrameAnimationT<PointT>::pose(unsigned int _iFrame)
{
  assert(_iFrame < poses_.size());

  return poses_[_iFrame];
}

//-----------------------------------------------------------------------------

/**
 * @brief Returns the number of frames stored in this pose
 */
template<class PointT>
unsigned int FrameAnimationT<PointT>::frameCount()
{
  return poses_.size();
}

//-----------------------------------------------------------------------------

template<class PointT>
void FrameAnimationT<PointT>::setFrameCount(unsigned int _frames)
{
  //delete poses
  while ( _frames < poses_.size() ){
    Pose* pose = poses_.back();
    poses_.pop_back();
    delete pose;
  }
  
  //add poses
  while ( _frames > poses_.size() )
    poses_.push_back(new Pose(skeleton_));
}

//-----------------------------------------------------------------------------

/**
 * \brief Called by the skeleton as a new joint is inserted
 *
 * The call is dispatched to all poses stored in this animation. See BasePoseT::insert_at for more information.
 *
 * @param _index The new joint is inserted at this position. Insert new joints at the end by passing
 *                               SkeletonT::joints_.size as parameter.
 */
template<class PointT>
void FrameAnimationT<PointT>::insertJointAt(unsigned int _index)
{
  for(typename vector<Pose*>::iterator it = poses_.begin(); it != poses_.end(); ++it)
    (*it)->insertJointAt(_index);
}

//-----------------------------------------------------------------------------

/**
 * \brief Called by the skeleton as a joint is deleted
 *
 * The call is dispatched to all poses stored in this animation. See BasePoseT::remove_at for more information.
 *
 * @param _index The index of the joint that is being deleted.
 */
template<class PointT>
void FrameAnimationT<PointT>::removeJointAt(unsigned int _index)
{
  for(typename vector<Pose*>::iterator it = poses_.begin(); it != poses_.end(); ++it)
    (*it)->removeJointAt(_index);
}

//-----------------------------------------------------------------------------

/**
 * \brief Updates the local matrix using the global matrix
 *
 * Called when a joints parent is changed.
 *
 * @param _index The joints index
 */
template<class PointT>
void FrameAnimationT<PointT>::updateFromGlobal(unsigned int _index)
{
  for(typename vector<Pose*>::iterator it = poses_.begin(); it != poses_.end(); ++it)
    (*it)->updateFromGlobal(_index);
}

//-----------------------------------------------------------------------------
