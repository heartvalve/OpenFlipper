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

