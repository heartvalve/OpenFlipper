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

#ifndef FRAMEANIMATIONT_HH
#define FRAMEANIMATIONT_HH

#include "AnimationT.hh"

template<class PointT>
class FrameAnimationT : public AnimationT<PointT>
{
  template<typename>
  friend class SkeletonT;

  public:
    typedef PointT                                Point;
    typedef typename Point::value_type            Scalar;
    typedef SkeletonT<PointT>                     Skeleton;
    typedef PoseT<PointT>                         Pose;

  public:
    FrameAnimationT(const Pose &_pose);
    FrameAnimationT(Skeleton* _skeleton);
    FrameAnimationT(Skeleton* _skeleton, unsigned int _iNumFrames);
    FrameAnimationT(const FrameAnimationT<PointT> &_other);
    virtual ~FrameAnimationT();

    virtual AnimationT<PointT>* copy();

    virtual void updateFromGlobal(unsigned int _index);

  public:
    /**
      * @name Frame access
      * There is one pose per frame.
       * @{ */

    /** \brief Returns a pointer to the pose stored in the given frame
     *
     * @param _iFrame The poses frame number
     */
    inline Pose *pose(unsigned int _iFrame);

    /** \brief Returns the number of frames stored in this pose
     */
    inline unsigned int frameCount();

    /** \brief Set number of frames stored in this pose
     */
    void setFrameCount(unsigned int _frames);
    /** @} */

    /** @name Synchronization
      *
      * Use these methods to keep the poses in sync with the number (and indices) of the joints.
      * @{ */

    /** \brief Called by the skeleton as a new joint is inserted
     *
     * The call is dispatched to all poses stored in this animation. See BasePoseT::insert_at for more information.
     *
     * @param _index The new joint is inserted at this position. Insert new joints at the end by passing SkeletonT::joints_.size as parameter.
     */
    void insertJointAt(unsigned int _index);

    /** \brief Called by the skeleton as a joint is deleted
     *
     * The call is dispatched to all poses stored in this animation. See BasePoseT::remove_at for more information.
     *
     * @param _index The index of the joint that is being deleted.
     */
    void removeJointAt(unsigned int _index);
    /** @} */

  private:
    /// Pointer to associated skeleton
    Skeleton* skeleton_;
    /// Every entry in this vector is a frame of the animation
    std::vector<Pose*> poses_; 

};

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(FRAMEANIMATIONT_C)
#define FRAMEANIMATIONT_TEMPLATES
#include "FrameAnimationT.cc"
#endif
//=============================================================================

#endif //FRAMEANIMATIONT_HH
