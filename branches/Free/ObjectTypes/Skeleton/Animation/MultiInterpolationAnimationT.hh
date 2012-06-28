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

#ifndef MULTIINTERPOLATIONANIMATIONT_HH
#define MULTIINTERPOLATIONANIMATIONT_HH

#include "InterpolationAnimationT.hh"

template<class PointT>
class MultiInterpolationAnimationT : public InterpolationAnimationT<PointT>
{
  template<typename>
  friend class SkeletonT;

  public:
    typedef PointT                                          Point;
    typedef typename Point::value_type                      Scalar;
    typedef PoseT<PointT>                                   Pose;                           ///< Typedef for the pose template
    typedef InterpolationMatrixManipulatorT<Scalar>         MatrixManipulator;
    typedef InterpolationT<Scalar>                          Interpolator;
    typedef std::vector<Scalar>                             TargetType;
    typedef std::vector< InterpolationAnimationT<ACG::Vec3d>* > InterpolationAnimations;

  public:
    MultiInterpolationAnimationT(const MultiInterpolationAnimationT<PointT> &_other);
    MultiInterpolationAnimationT() : InterpolationAnimationT<PointT>(NULL, NULL), interpolationAnimations_() {}
    MultiInterpolationAnimationT(const InterpolationAnimations& _interpolationAnimations) : interpolationAnimations_(_interpolationAnimations) {}
    
    virtual AnimationT<PointT>* copy();
    
    virtual void updateFromGlobal(unsigned int _index) {/*NOOP*/};
    
    virtual bool getMinInput(Scalar& _result);
    virtual bool getMaxInput(Scalar& _result);

  private:
    InterpolationAnimations interpolationAnimations_;

  public:
    using InterpolationAnimationT<PointT>::FPS;

  public:

    //===========================================================================
    /** @name Frame access
     *
     * There is one pose per frame.
     *
     * @{ */
    //===========================================================================

    /** \brief get a pose
     *
     * @param _iFrame    Frame
     * @return Requested pose
     */
    virtual Pose *pose(unsigned int _iFrame);

    /** \brief get a pose
     *
     * @param _iFrame    Frame
     * @param _reference Reference pose
     * @return Requested pose
     */
    virtual Pose *pose(unsigned int _iFrame, Pose* _reference);
    virtual unsigned int frameCount();
    /** @} */

    //===========================================================================
    /** @name Synchronization
     *
     * Use these methods to keep the poses in sync with the number (and indices) of the joints.
     * @{ */
    //===========================================================================
    void insertJointAt(unsigned int _index) {};
    void removeJointAt(unsigned int _index) {};
    /** @} */
    
    //===========================================================================
    /** @name Animations access
     *
     * @{ */
    //===========================================================================
    void addInterpolationAnimation(InterpolationAnimationT<PointT> *_animation) {
      interpolationAnimations_.push_back(_animation);
    }
    unsigned int animationCount() {
      return interpolationAnimations_.size();
    }
    
    InterpolationAnimationT<PointT>* animation(unsigned int _index );
    /** @} */
    
    virtual void clearPoseCache() {
      for (uint i=0; i < interpolationAnimations_.size(); ++i) {
        interpolationAnimations_[i]->clearPoseCache();
      }
    }
};

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(MULTIINTERPOLATIONANIMATIONT_C)
#define MULTIINTERPOLATIONANIMATIONT_TEMPLATES
#include "MultiInterpolationAnimationT.cc"
#endif
//=============================================================================

#endif //MULTIINTERPOLATIONANIMATIONT_HH
