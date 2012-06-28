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

#ifndef INTERPOLATIONANIMATIONT_HH
#define INTERPOLATIONANIMATIONT_HH

#include <vector>
#include <map>

#include "../PoseT.hh"
#include "InterpolationT.hh"
#include "InterpolationMatrixManipulatorT.hh"

template<class PointT>
class InterpolationAnimationT : public AnimationT<PointT>
{ 
  template<typename>
  friend class SkeletonT;

  public:
    typedef PointT                                          Point;
    typedef typename Point::value_type                      Scalar;
    typedef PoseT<PointT>                                   Pose;
    typedef InterpolationMatrixManipulatorT<Scalar>         MatrixManipulator;
    typedef InterpolationT<Scalar>                          Interpolator;
    typedef std::vector<Scalar>                             TargetType;
  
  public:
    InterpolationAnimationT(const InterpolationAnimationT<PointT> &_other);
    InterpolationAnimationT(Skeleton* _skeleton, MatrixManipulator *_matrixManipulator);
    virtual ~InterpolationAnimationT();
    
    virtual AnimationT<PointT>* copy();
    
    virtual void updateFromGlobal(unsigned int _index);
    
    virtual bool getMinInput(Scalar& _result);
    virtual bool getMaxInput(Scalar& _result);

  protected:

    Skeleton* skeleton_;

    std::vector<int> influencedJoints_;
    std::vector< Interpolator* > interpolators_;
    //Hier muss es einen Mapper geben, der weiß, wie er auf die Matrix aus der Pose die Werte, die der Interpolator
    //erzeugt, anwendet.
    std::map < Interpolator*, std::vector < TargetType > > precalculations_;
    MatrixManipulator* matrixManipulator_;
    unsigned int frames_;

    std::map < unsigned int, Pose* > interpolatedPoses_;

    unsigned int calcAbsoluteMaxForInterpolator(uint _index);
  
  public:
    static const int FPS = 60;

  public:

    //===========================================================================
    /** @name Frame access
     *
     * There is one pose per frame.
     * @{ */
    //===========================================================================

    virtual Pose* pose(unsigned int _iFrame);
    virtual Pose* pose(unsigned int _iFrame, Pose* _reference);

    /**
     * @brief Returns the number of frames stored in this pose
     */
    inline unsigned int frameCount();

    /** @} */


    //===========================================================================
    /** @name Synchronization
     *
     * Use these methods to keep the poses in sync with the number (and indices) of the joints.
     * @{ */
    //===========================================================================

    /** \brief Called by the skeleton as a new joint is inserted
     *
     * The call is dispatched to all poses stored in the animation. See BaseNode::insert_at for more information.
     *
     * @param _index The new joint is inserted at this position. Insert new joints at the end by passing SkeletonT::joints_.size as parameter.
     */
    virtual void insertJointAt(unsigned int _index);
    virtual void removeJointAt(unsigned int _index);

    /** @} */
    

    //===========================================================================
    /** @name Interpolators access
     *
     * We use only interpolators with time as input.
     * @{ */
    //===========================================================================
    void          addInterpolator(InterpolationT<double> *_interpolator);
    Interpolator* interpolator(unsigned int _index);
    unsigned int  interpolatorCount();
    /** @} */

    //===========================================================================
    /** @name InfluencedJoints access
     *
     * @{ */
    //===========================================================================

    void addInfluencedJoint(int _joint)
    {
      influencedJoints_.push_back(_joint);
    }

    bool isInfluenced(int _joint);

    std::vector<int>& influencedJoints();

    /** @} */

    //===========================================================================
    /** @name MatrixManipulator access
     *
     * @{ */
    //===========================================================================

    MatrixManipulator* matrixManipulator() { return matrixManipulator_; }

    /** @} */

    Pose* getReference() { return pose(0); }

    virtual void clearPoseCache()
    {
      if (interpolatedPoses_.size() == 1 && interpolatedPoses_.find(0) != interpolatedPoses_.end())
        return;

      if (interpolatedPoses_.find(0) != interpolatedPoses_.end()) {
        //  Pose* frame0 = (interpolatedPoses_.find(0)->second);

        if (interpolatedPoses_.size() > 1) {
          typename std::map<unsigned int, Pose*>::iterator ip_it = interpolatedPoses_.begin();
          ++ip_it;
          for (; ip_it != interpolatedPoses_.end(); ++ip_it) {
            delete ip_it->second;
            interpolatedPoses_.erase(ip_it);
          }
        }

        // interpolatedPoses_.insert(std::pair<unsigned int, Pose>(0, frame0));
      } else {
        interpolatedPoses_.clear();
      }
    }


};

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(INTERPOLATIONANIMATIONT_C)
#define INTERPOLATIONANIMATIONT_TEMPLATES
#include "InterpolationAnimationT.cc"
#endif
//=============================================================================

#endif //INTERPOLATIONANIMATIONT_HH
