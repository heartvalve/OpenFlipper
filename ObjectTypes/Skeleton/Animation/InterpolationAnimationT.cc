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

#define INTERPOLATIONANIMATIONT_C

#include "AnimationT.hh"

#include <vector>
#include <assert.h>
#include <math.h>

using namespace std;

//-----------------------------------------------------------------------------------------------------

/**
 * @brief Copy constructor
 *
 * This animation will copy all frames from the given animation. After the call returns they are completely
 * independent.
 *
 * @param _other The animation to copy from
 */
template<class PointT>
InterpolationAnimationT<PointT>::InterpolationAnimationT(const InterpolationAnimationT<PointT> &_other) :
        AnimationT<PointT>(_other.name_),
        skeleton_(_other.skeleton_),
        matrixManipulator_(_other.matrixManipulator_),
        frames_(0)
{
        
}

//-----------------------------------------------------------------------------------------------------

/**
 * @brief Creates a new empty animation
 *
 * @param _skeleton           The skeleton that will hold this animation
 * @param _matrixManipulator  Interpolator that translates the interpolation into matrix values
 */
template<class PointT>
InterpolationAnimationT<PointT>::InterpolationAnimationT(Skeleton* _skeleton, MatrixManipulator *_matrixManipulator) :
        skeleton_(_skeleton),
        matrixManipulator_(_matrixManipulator),
        frames_(0)
{

}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
InterpolationAnimationT<PointT>::~InterpolationAnimationT()
{
  clearPoseCache();
}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
AnimationT<PointT>* InterpolationAnimationT<PointT>::copy() {
  return new InterpolationAnimationT<PointT>(*this);
}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
PoseT<PointT>* InterpolationAnimationT<PointT>::pose(unsigned int _iFrame)
{
  return pose(_iFrame, skeleton_->referencePose());
}

//-----------------------------------------------------------------------------------------------------

/**
 * @brief Returns a pointer to the pose calculated for the given frame
 *
 * @param _iFrame    The frame number for which the pose should be calculated.
 *                   This is always from 0..frames even if the animation starts with an input value other than 0.
 * @param _reference Reference pose
 */
template<class PointT>
PoseT<PointT>* InterpolationAnimationT<PointT>::pose(unsigned int _iFrame, Pose* _reference)
{
//          std::cerr << "Frame " << _iFrame << ": ";
  
        if (interpolatedPoses_.find(_iFrame) != interpolatedPoses_.end()) {
//           std::cerr << "(from cache)" << std::endl;
          return (interpolatedPoses_[_iFrame]);
        } else {
        
          if (_iFrame == 0) {
            interpolatedPoses_.insert( make_pair(0, new Pose(*_reference)) );
//             std::cerr << "Insert reference to posecache. &_reference: " << _reference << ", &cacheref: " << getPose(_iFrame, _reference) << std::endl;
            return pose(_iFrame, _reference);
          } else {
            //Find the correct interpolator
            Interpolator* interpolator = NULL;
            unsigned long min = 0, max = 0;
            
            uint i;
            for (i=0; i<interpolators_.size(); ++i) {
              min = (i==0 ? 0.0 : calcAbsoluteMaxForInterpolator(i-1) + 1);
              max = calcAbsoluteMaxForInterpolator(i);
              if (_iFrame >= min && _iFrame <= max) {
                interpolator = interpolators_[i];
                break;
              }
            }
            
            if (interpolator == NULL) {
//               std::cerr << "No appropriate interpolator found for this frame!" << std::endl;
              return _reference;
            }
            
//             std::cerr << "Using interpolator " << i << " - ";
            
            //Create a new pose that is a copy of the reference and apply the interpolated transformations to it
            Pose *generatedPose = new Pose(*_reference);
            
            for (uint i=0; i<influencedJoints_.size(); ++i) {
              ACG::GLMatrixT<Scalar> transformation(generatedPose->globalMatrix(influencedJoints_[i]).raw());
              //The frames for each interpolator are stored from 0..max, i.e. in "interpolator local time space".
              // So, they have to be mapped to the global space here.
              TargetType precalcVal = precalculations_[interpolator][_iFrame - min];
              
              matrixManipulator_->doManipulation(transformation, precalcVal);
              generatedPose->setGlobalMatrix(influencedJoints_[i], transformation, false);
            }
            
//             std::cerr << std::endl;
            
            interpolatedPoses_.insert(std::pair<unsigned long, Pose*>(_iFrame, generatedPose));
            return (interpolatedPoses_.find(_iFrame)->second);
          }
          
          
      }
}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
unsigned int InterpolationAnimationT<PointT>::frameCount()
{
  return frames_;
}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
void InterpolationAnimationT<PointT>::insertJointAt(unsigned int _index)
{
  //NOOP
}

//-----------------------------------------------------------------------------------------------------

/**
 * @brief Called by the skeleton as a joint is deleted
 *
 * The call is dispatched to all poses stored in this animation. See BasePoseT::remove_at for more information.
 *
 * @param _index The index of the joint that is being deleted.
 */
template<class PointT>
void InterpolationAnimationT<PointT>::removeJointAt(unsigned int _index)
{
  //NOOP
}

//-----------------------------------------------------------------------------------------------------

/**
 * @brief Updates the local matrix using the global matrix
 *
 * Called when a joints parent is changed.
 * Note: Does nothing at the moment
 *
 * @param _index The joints index
 */
template<class PointT>
void InterpolationAnimationT<PointT>::updateFromGlobal(unsigned int _index)
{
  //NOOP
}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
void InterpolationAnimationT<PointT>::addInterpolator(InterpolationT<double> *_interpolator) {
  if (_interpolator == NULL)
    return;
  
  interpolators_.push_back(_interpolator);
//   std::cerr << "Precalc for interpolator " << interpolators_.size()-1 << ":" << std::endl;
  
  std::vector < TargetType > valueVector;
  
  //Precalc values for this interpolator
  uint i=0;
  for (i=_interpolator->getMinInput()*FPS;i<=(_interpolator->getMaxInput()) * FPS;++i) {
    TargetType precalcValue;
    double input = ((double)i) / ((double)FPS);
    precalcValue = _interpolator->getValue(input);
    valueVector.push_back(precalcValue);
    
//      std::cerr << "Frame " << i << "(t=" << input << "): " << precalcValue[0] << std::endl;
  }
  
//   std::cerr << std::endl;
  
  precalculations_.insert( std::pair< Interpolator*, std::vector < TargetType > >(_interpolator, valueVector) );
  
  frames_ = max<long unsigned int>(frames_, i+1);
}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
InterpolationT<typename PointT::value_type>*
InterpolationAnimationT<PointT>::interpolator(unsigned int _index)
{
  if ( _index < interpolators_.size() )
    return interpolators_[ _index ];
  else 
    return 0;
}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
unsigned int InterpolationAnimationT<PointT>::interpolatorCount()
{
  return interpolators_.size();
}

//-----------------------------------------------------------------------------------------------------

/**
* \brief Calculates the last frame that interpolator _index is responsible for
*/
template<class PointT>
unsigned int InterpolationAnimationT<PointT>::calcAbsoluteMaxForInterpolator(uint _index) {
  assert (_index < interpolators_.size());
  
  if (_index == 0) {
    return precalculations_[interpolators_[_index]].size() - 1;
  } else {
    return precalculations_[interpolators_[_index]].size() + calcAbsoluteMaxForInterpolator(_index - 1);
  }
}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
bool InterpolationAnimationT<PointT>::getMinInput(Scalar& _result) {
  if (interpolators_.size() == 0)
    return false;
  else
    _result = interpolators_[0]->getMinInput();
  
  for (uint i=0;i<interpolators_.size();++i) {
    if (interpolators_[i]->getMinInput() < _result)
      _result = interpolators_[i]->getMinInput();
  }
  
  return true;
}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
bool InterpolationAnimationT<PointT>::getMaxInput(Scalar& _result) {
  if (interpolators_.size() == 0)
    return false;
  else
    _result = interpolators_[0]->getMaxInput();
  
  for (uint i=0;i<interpolators_.size();++i) {
    if (interpolators_[i]->getMaxInput() > _result)
      _result = interpolators_[i]->getMaxInput();
  }
  
  return true;
}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
bool InterpolationAnimationT<PointT>::isInfluenced(int _joint) {
  for (uint i=0; i<influencedJoints_.size(); ++i) 
    if ( influencedJoints_[i] == _joint )
      return true;
    
  return false;
}

//-----------------------------------------------------------------------------------------------------

template<class PointT>
std::vector<int>& InterpolationAnimationT<PointT>::influencedJoints() {
  return influencedJoints_;
}

//-----------------------------------------------------------------------------------------------------

