/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS TranslationManipulatorNode
//
//=============================================================================


#ifndef ACG_TRANSLATIONMANIPULATOR_NODE_HH
#define ACG_TRANSLATIONMANIPULATOR_NODE_HH


//== INCLUDES =================================================================

// GMU
#include "BaseNode.hh"
#include "TransformNode.hh"

// Qt
#include <QEvent>
#include <QMouseEvent>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class TranslationTranslationManipulatorNode TranslationTranslationManipulatorNode.hh <ACG/Scenegraph/TranslationTranslationManipulatorNode.hh>
    Use a manipulator to translate and scale the children of this node.
*/

class ACGDLLEXPORT TranslationManipulatorNode : public TransformNode
{
public:

  enum AutoSizeMode {
    Never,
    Once,
    Always
  };

  enum ManipulatorMode {
    TranslationRotation,
    LocalRotation,
    Resize,
    Place
  };

  /// Default constructor.
  TranslationManipulatorNode( BaseNode* _parent=0,
		   const std::string& _name="<TranslationTranslationManipulatorNode>" );

  /// Destructor.
  ~TranslationManipulatorNode();


  /// class name
  ACG_CLASSNAME(TranslationManipulatorNode);


  //
  // METHODS
  //

  virtual void setIdentity();

  /// set draw_cylinder_
  void set_draw_cylinder(bool _b) { draw_manipulator_ = _b; }

  /// get drawCylinder
  bool draw_manipulator() const { return draw_manipulator_; }

  /// Set direction in world coordinates
  void set_direction(const Vec3d& _directionX, const Vec3d& _directionY);

  /// Get current direction of x-Axis in world coordinates
  Vec3d directionX() const;
  /// Get current direction of y-Axis in world coordinates
  Vec3d directionY() const;
  /// Get current direction of z-Axis in world coordinates
  Vec3d directionZ() const;

  /// set cylindersize  (height + radius)
  void set_size( double _size) { set_manipulator_height_ = _size;
                                 set_manipulator_radius_ = _size/10.0;
                                 setDirty (); }

  /// get cylindersize
  double size() const { return set_manipulator_height_; }

  /// set auto size mode
  void set_autosize (AutoSizeMode _mode) { auto_size_ = _mode; }

  /// get autosize mode
  AutoSizeMode autosize () const { return auto_size_; }

  void rotate (double _angle, Vec3d _axis)
  {
    TransformNode::rotate(_angle, _axis);
  }

  /// draw the cylinder (if enabled)
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);

  /// leave node
  // void leave(GLState& _state, const DrawModes::DrawMode& _drawMode);

  /// picking
  void pick(GLState& _state, PickTarget _target);

  /// get mouse events
  virtual void mouseEvent(GLState& _state, QMouseEvent* _event);

  /// bounding box of node
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);

  /// set current operation mode
  void setMode (ManipulatorMode _mode);

  /// get current mode
  ManipulatorMode getMode () const { return mode_; };


private:

  enum StateUpdates {
    None,
    Over,
    Click
  };

  enum Elements {
    Origin = 0,
    XTop,
    YTop,
    ZTop,
    XAxis,
    YAxis,
    ZAxis,
    XRing,
    YRing,
    ZRing,
    NumElements
  };

  class Element {
    public:
      Element ();

      Vec4f active_target_color_;
      Vec4f active_current_color_;
      Vec4f inactive_target_color_;
      Vec4f inactive_current_color_;

      bool  clicked_;
      bool  over_;
  };

  /// set the current state to follow manipulator transformation
  void update_manipulator_system(GLState& _state);

  /// update the internal rotation matrix ( internal rotation may be changed without modifiing children of this node )
  void update_rotation(GLState& _state);

  bool hitSphere(GLState& _state, const Vec2i& _v2);
  bool hitOuterSphere(GLState& _state, const Vec2i& _v2);

  void drawManipulator (GLState& _state, bool _active);

  void drawCircle(const float innerRadius, const float outerRadius);

  bool mapToCylinder (GLState& _state, const Vec2i& _v2, double& axis_hit, StateUpdates _updateStates = None);
  bool mapToCylinderTop (GLState& _state, const Vec2i& _v2, double& axis_hit, StateUpdates _updateStates = None);

  bool mapToSphere (GLState& _state, const Vec2i& _v2, Vec3d& _v3, StateUpdates _updateStates = None);

  void updateTargetColors ();
  bool updateCurrentColors (GLState& _state);

  double get_screen_length (GLState& _state, Vec3d& _point) const;

  void updateSize (GLState& _state);

  // ELEMENTS
  bool               draw_manipulator_;

  Vec3d              dirX_;
  Vec3d              dirY_;
  Vec3d              dirZ_;

  GLUquadricObj*     axis_;
  GLUquadricObj*     circle_;
  GLUquadricObj*     sphere_;

  double             manipulator_radius_;
  double             manipulator_height_;
  double             set_manipulator_radius_;
  double             set_manipulator_height_;
  int                manipulator_slices_;
  int                manipulator_stacks_;

  bool               any_axis_clicked_;
  bool               any_top_clicked_;
  bool               outer_ring_clicked_;

  bool               any_axis_over_;
  bool               any_top_over_;
  bool               outer_ring_over_;


  Element            element_[NumElements];
  float              resize_current_;

  ManipulatorMode    mode_;
  bool               ignoreTime_;

  Vec2i              oldPoint2D_;
  Vec3d              draggingOrigin3D_;
  bool               dragging_;

  Vec3d              currentScale_;

  GLMatrixd          localTransformation_;

  AutoSizeMode       auto_size_;
  double             auto_size_length_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_TRANSLATIONMANIPULATOR_NODE_HH defined
//=============================================================================

