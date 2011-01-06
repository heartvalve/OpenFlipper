/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
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

#define MULTIINTERPOLATIONANIMATIONT_C

#include "AnimationT.hh"
#include <algorithm>

//-----------------------------------------------------------------------------------------------------

/**
 * @brief Copy constructor
 *
 * @param _other The animation to copy from
 */
template<typename Scalar>
MultiInterpolationAnimationT<Scalar>::MultiInterpolationAnimationT(const MultiInterpolationAnimationT<Scalar> &_other) :
        InterpolationAnimationT<Scalar>(NULL, NULL, NULL),
        interpolationAnimations_(_other.interpolationAnimations_)
{
        
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
AnimationT<Scalar>* MultiInterpolationAnimationT<Scalar>::copy() {
  return new MultiInterpolationAnimationT<Scalar>(*this);
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
bool MultiInterpolationAnimationT<Scalar>::getMinInput(Scalar& _result) {
  if (interpolationAnimations_.size() == 0)
    return false;
  else
    interpolationAnimations_[0]->getMinInput(_result);
  
  for (uint i=0;i<interpolationAnimations_.size();++i) {
    Scalar currentInput;
    interpolationAnimations_[i]->getMinInput(currentInput);
    
    if (currentInput < _result)
      _result = currentInput;
  }
  
  return true;
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
bool MultiInterpolationAnimationT<Scalar>::getMaxInput(Scalar& _result) {
  if (interpolationAnimations_.size() == 0)
    return false;
  else
    interpolationAnimations_[0]->getMaxInput(_result);
  
  for (uint i=0;i<interpolationAnimations_.size();++i) {
    Scalar currentInput;
    interpolationAnimations_[i]->getMaxInput(currentInput);
    
    if (currentInput > _result)
      _result = currentInput;;
  }
  
  return true;
}

//-----------------------------------------------------------------------------------------------------

/**
 * @brief Returns the number of frames that this animation can playback
 * Note that this is not simply the sum of all animations' frame counts, as they can (and most likely will) overlap.
 */
template<typename Scalar>
unsigned long MultiInterpolationAnimationT<Scalar>::getFrameCount()
{
  Scalar minInput=0, maxInput=0;
  if (getMinInput(minInput) && getMaxInput(maxInput)) {
    return ((maxInput - minInput) * FPS);
  }
  
  return 0;
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
PoseT<Scalar> * MultiInterpolationAnimationT<Scalar>::getPose(unsigned long _iFrame) {
  //Use the reference pose of the first (in terms of the input value, i.e. the time in most cases)
 
  if (interpolationAnimations_.size() == 0)
    return NULL;
 
  Scalar minValue=0; uint minInterpolationAnimationIndex = 0;
  for (uint i=0; i<interpolationAnimations_.size(); ++i) {
    Scalar currentValue;
    interpolationAnimations_[i]->getMinInput(currentValue);
    Scalar minValueTmp = std::min(minValue, currentValue);
    minInterpolationAnimationIndex = (minValueTmp < minValue) ? i : minInterpolationAnimationIndex;
  }

  return getPose(_iFrame, interpolationAnimations_[minValue]->getReference());
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
PoseT<Scalar> * MultiInterpolationAnimationT<Scalar>::getPose(unsigned long _iFrame, Pose* _reference) {
  if (_iFrame == 0)
    return _reference;
  
  Pose* newPose = NULL;
  Pose* referenceCopy = new Pose(*_reference);
  
  for (uint i=0; i<interpolationAnimations_.size(); ++i) {
    Scalar minInput, maxInput;
    interpolationAnimations_[i]->getMinInput(minInput); interpolationAnimations_[i]->getMaxInput(maxInput);
    
    unsigned long minFrame = (minInput * FPS);
    unsigned long maxFrame = (maxInput * FPS);
    
    //Check, if the current animation is responsible for displaying this frame
    if (_iFrame < minFrame || _iFrame > maxFrame)
      continue;
    
    if (interpolationAnimations_[i]) {
      if (newPose == NULL)
	newPose = interpolationAnimations_[i]->getPose(_iFrame - minFrame, referenceCopy);
      else
	newPose = interpolationAnimations_[i]->getPose(_iFrame - minFrame, newPose);
    }
  }
  
  delete referenceCopy;
  
  if (newPose == NULL)
    newPose = _reference;
  
  return newPose;
}

//-----------------------------------------------------------------------------------------------------

template<typename Scalar>
InterpolationAnimationT<Scalar>* MultiInterpolationAnimationT<Scalar>::getAnimation(unsigned int _index ) {
  if ( _index < interpolationAnimations_.size() )
    return interpolationAnimations_[ _index ];
  else 
    return 0;
}

//-----------------------------------------------------------------------------------------------------

