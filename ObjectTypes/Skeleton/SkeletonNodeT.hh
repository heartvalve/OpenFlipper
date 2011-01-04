//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 1734 $
//   $Author: moebius $
//   $Date: 2008-05-13 08:28:54 +0200 (Tue, 13 May 2008) $
//
//=============================================================================


#ifndef ACG_SKELETONNODET_HH
#define ACG_SKELETONNODET_HH


//== INCLUDES =================================================================

#include <ACG/Scenegraph/MaterialNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include "JointT.hh"

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================




/** \class SkeletonNodeT SkeletonNodeT.hh <ACG/.../SkeletonNodeT.hh>

    Brief Description.

    A more elaborate description follows.
*/

template <class Skeleton>
class SkeletonNodeT : public BaseNode
{
public:
  // static name of this class
  ACG_CLASSNAME(SkeletonNodeT);

  typedef typename Skeleton::Point            Point;
  typedef JointT<typename Skeleton::Point>    Joint;
  typedef typename Skeleton::Pose              Pose;
  typedef typename Skeleton::Matrix          Matrix;

public:
  
  /// Constructor
  SkeletonNodeT(Skeleton &_skeleton, BaseNode *_parent=0, std::string _name="<SkeletonNode>");

  /// Destructor
  ~SkeletonNodeT();

public:
  /// Returns a pointer to the skeleton
  Skeleton& skeleton();

  /// Returns available draw modes
  DrawModes::DrawMode availableDrawModes() const;
  /// Returns the bounding box of this node
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);
  /// Renders the nodes contents using the given render state and draw mode
  void draw(GLState& _state, DrawModes::DrawMode _drawMode);
  /// Renders the node in picking mode, restricted to the node components given by \e _target
  void pick(GLState& _state, PickTarget _target);

  /// Set the pose which should be rendered
  void setActivePose(const AnimationHandle &_hAni);
  /// Get the pose that is used for rendering
  AnimationHandle activePose();

  /// Toggle visibility of coordinate frames for all joints
  void showCoordFrames(bool _bVisible = true);

  bool coordFramesVisible();
  
private:
  /// Pick method for vertices
  void pick_vertices( GLState& _state );
  /// Pick method for edges
  void pick_edges(GLState& _state);

  /// Helper function to draw the bones
  void draw_bone(GLState &_state, DrawModes::DrawMode _drawMode, const Point& _parent, const Point& _axis);
 
  /// Normalizes a coordinate frame defined by the given matrix
  void NormalizeCoordinateFrame(Matrix &_mat);

  /// get a joint color suitable to the baseColor
  void getJointColor( const Vec4f& _baseColor, Vec4f& _result );
  
  /// Convert HSV color to RGB
  void HSVtoRGB(const Vec4f& _HSV, Vec4f& _RGB);
  /// Convert RGB color to HSV
  void RGBtoHSV(const Vec4f& _RGB, Vec4f& _HSV);
  
private:
  /// Coordinate frames are visible if this is true, not visible if false
  bool bCoordFramesVisible_;
  /// The skeleton nodes skeleton
  Skeleton& skeleton_;
  /// The active pose, this one is going to be rendered
  AnimationHandle hAni_;
  
  ///quadric for drawing cylinders
  bool quadricInitialized_;
  GLUquadricObj* quadric_;
  
  /// size for the coord-frame
  double fFrameSize_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_SKELETONNODET_C)
#define ACG_SKELETONNODET_TEMPLATES
#include "SkeletonNodeT.cc"
#endif
//=============================================================================
#endif // ACG_SKELETONNODET_HH defined
//=============================================================================

