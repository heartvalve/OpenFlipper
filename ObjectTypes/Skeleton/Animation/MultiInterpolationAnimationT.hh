#ifndef MULTIINTERPOLATIONANIMATIONT_HH
#define MULTIINTERPOLATIONANIMATIONT_HH

#include "InterpolationAnimationT.hh"

template<typename PoinT>
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
    typedef std::vector< InterpolationAnimationT<Scalar>* > InterpolationAnimations;

  public:
    MultiInterpolationAnimationT(const MultiInterpolationAnimationT<Scalar> &_other);
    MultiInterpolationAnimationT() : InterpolationAnimationT<Scalar>(NULL, NULL, NULL), interpolationAnimations_() {}
    MultiInterpolationAnimationT(const InterpolationAnimations& _interpolationAnimations) : interpolationAnimations_(_interpolationAnimations) {}
    
    virtual AnimationT<Scalar>* copy();
    
    virtual void UpdateFromGlobal(unsigned int _index) {/*NOOP*/};
    
    virtual bool getMinInput(Scalar& _result);
    virtual bool getMaxInput(Scalar& _result);

  private:
    InterpolationAnimations interpolationAnimations_;

  public:
    using InterpolationAnimationT<Scalar>::FPS;

  public:
    /**
      * @name Frame access
      * There is one pose per frame.
      */
    //@{
    virtual Pose *getPose(unsigned int _iFrame);
    virtual Pose *getPose(unsigned int _iFrame, Pose* _reference);
    virtual unsigned int getFrameCount();
    //@}

    /**
      * @name Synchronization
      * Use these methods to keep the poses in sync with the number (and indices) of the joints.
      */
    //@{
    void insertJointAt(unsigned int _index) {};
    void removeJointAt(unsigned int _index) {};
    //@}
    
    /**
      * @name Animations access
      */
    //@{
    void addInterpolationAnimation(InterpolationAnimationT<Scalar> *_animation) {
      interpolationAnimations_.push_back(_animation);
    }
    unsigned int getNumberOfAnimations() {
      return interpolationAnimations_.size();
    }
    
    InterpolationAnimationT<Scalar>* getAnimation(unsigned int _index );
    
    //@}
    
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