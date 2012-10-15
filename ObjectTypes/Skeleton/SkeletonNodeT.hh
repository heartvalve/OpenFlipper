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


#ifndef ACG_SKELETONNODET_HH
#define ACG_SKELETONNODET_HH


//== INCLUDES =================================================================

#include <ACG/Scenegraph/MaterialNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/GL/GLPrimitives.hh>
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

template <class SkeletonType>
class SkeletonNodeT : public BaseNode
{
public:
  // static name of this class
  ACG_CLASSNAME(SkeletonNodeT);

  typedef typename SkeletonType::Point            Point;
  typedef JointT<typename SkeletonType::Point>    Joint;
  typedef typename SkeletonType::Pose              Pose;
  typedef typename SkeletonType::Matrix          Matrix;

public:
  
  /// Constructor
  SkeletonNodeT(SkeletonType &_skeleton, BaseNode *_parent=0, std::string _name="<SkeletonNode>");

  /// Destructor
  ~SkeletonNodeT();

public:
  /// Returns a pointer to the skeleton
  SkeletonType& skeleton();

  /// Returns available draw modes
  DrawModes::DrawMode availableDrawModes() const;
  /// Returns the bounding box of this node
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);
  /// Renders the nodes contents using the given render state and draw mode
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);
  /// Renders the node in picking mode, restricted to the node components given by \e _target
  void pick(GLState& _state, PickTarget _target);

  /// Adds renderobjects to renderer for flexible shader pipeline
  void getRenderObjects(IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode);

  /// Set the pose which should be rendered
  void setActivePose(const AnimationHandle &_hAni);
  /// Get the pose that is used for rendering
  AnimationHandle activePose();

  /// Toggle visibility of coordinate frames for all joints
  void showCoordFrames(bool _bVisible = true);

  bool coordFramesVisible();

	/// returns the size of the rendered coordinate system
	double frameSize();
  
private:
  /// Pick method for vertices
  void pick_vertices( GLState& _state );
  /// Pick method for edges
  void pick_edges(GLState& _state);

  /// Helper function to draw the bones
  void draw_bone(GLState &_state, DrawModes::DrawMode _drawMode, const Point& _parent, const Point& _axis);
 
  /// Helper function to create a renderobject for bones
  void addBoneToRenderer(IRenderer* _renderer, RenderObject& _base, const Point& _parent, const Point& _axis);


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
  SkeletonType& skeleton_;
  /// The active pose, this one is going to be rendered
  AnimationHandle hAni_;
  
  /// size for the coord-frame
  double fFrameSize_;

  int slices_;
  int stacks_;

  ACG::GLSphere* sphere_;
  ACG::GLCone* cone_;
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

