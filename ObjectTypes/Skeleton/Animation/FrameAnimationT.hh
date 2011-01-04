#ifndef FRAMEANIMATIONT_HH
#define FRAMEANIMATIONT_HH

#include "AnimationT.hh"

template<typename PointT>
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
      */
    //@{
    inline Pose *pose(unsigned int _iFrame);
    inline unsigned int frameCount();

    void setFrameCount(unsigned int _frames);
    //@}

    /**
      * @name Synchronization
      * Use these methods to keep the poses in sync with the number (and indices) of the joints.
      */
    //@{
    void insertJointAt(unsigned int _index);
    void removeJointAt(unsigned int _index);
    //@}

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