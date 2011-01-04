#ifndef ANIMATIONT_HH
#define ANIMATIONT_HH

#include <vector>
#include <map>

#include "../PoseT.hh"
#include "AnimationHandle.hh"

/**
 * @brief Stores a single animation
 *
 * This class is used by a skeleton to store a single animation. An animation is an array of different Poses.
 * Both the reference pose and the skeleton hierarchy are not used by the animation class itself, but are
 * passed to the poses.
 */
template<typename PointT>
class AnimationT
{
  template<typename>
  friend class SkeletonT;

public:
  typedef PointT                                Point;
  typedef typename Point::value_type            Scalar;
  typedef PoseT<PointT>                         Pose;

public:
  AnimationT(std::string _name = "") : name_(_name), fps_(60) {}
  virtual AnimationT* copy() = 0;

public:
  /**
    * @name Frame access
    * There is one pose per frame.
    */
  //@{
  virtual Pose *pose(unsigned int _iFrame) = 0;
  virtual unsigned int frameCount() = 0;
  //@}

  /**
    * @name Synchronization
    * Use these methods to keep the poses in sync with the number (and indices) of the joints.
    */
  //@{
  virtual void insertJointAt(unsigned int _index) = 0;
  virtual void removeJointAt(unsigned int _index) = 0;
  //@}

  virtual void updateFromGlobal(unsigned int _index) = 0;
  virtual void clearPoseCache() {}

  inline std::string name() {
    return name_;
  }

  inline void setName(std::string _name) {
    name_ = _name;
  }

  inline int fps() {
    return fps_;
  }

  inline void setFps(int _fps) {
    fps_ = _fps;
  }

protected:
  std::string name_;
  int fps_;
};


#endif