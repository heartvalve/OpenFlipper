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

#ifndef ANIMATIONHANDLE_HH
#define ANIMATIONHANDLE_HH

/**
 * @brief A handle used to refer to an animation or to a specific frame in an animation
 */
class AnimationHandle
{
public:
  /// Constructs an invalid animation handle (interpreted as handle of the reference pose)
  AnimationHandle() : idAnimation(0), iFrame(0) {};
  /// Constructs a valid handle for the given animation and frame
  AnimationHandle(unsigned int idAnimation, unsigned int iFrame = 0) : idAnimation(idAnimation + 1), iFrame(iFrame) {};
  /// Returns true if the handle is valid
  inline bool isValid() const { return idAnimation > 0; }
  /// Returns the animation index (zero based)
  inline unsigned int animationIndex() const { return idAnimation - 1; }
  
  /// Returns the selected frame (zero based)
  inline unsigned int frame() const { return iFrame; }
  /// Sets the current animation frame (not failsafe)
  inline void setFrame(unsigned int _iFrame) { iFrame = _iFrame; }

  /// Returns to the first frame
  inline void firstFrame() { iFrame = 0; }
  /// Increases the handle to the next frame (not failsafe)
  inline void operator++() { ++iFrame; }
  /// Decreases the handle to the previous frame (not failsafe)
  inline void operator--() { --iFrame; }

  inline bool operator==(const AnimationHandle &rhs) { return (idAnimation == rhs.idAnimation) && (iFrame == rhs.iFrame); }
  inline bool operator!=(const AnimationHandle &rhs) { return (idAnimation != rhs.idAnimation) || (iFrame != rhs.iFrame); }

private:
  /// The one based index of the animation, set to 0 for invalid (or reference pose)
  unsigned int idAnimation;
  /// The frame number
  unsigned int iFrame;
};

#endif // ANIMATIONHANDLE_HH
