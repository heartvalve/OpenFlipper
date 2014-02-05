/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
template<class PointT>
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
  virtual ~AnimationT() {}

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
