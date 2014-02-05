/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS ManipulatorNode
//
//=============================================================================


#ifndef ACG_MANIPULATOR_NODE_HH
#define ACG_MANIPULATOR_NODE_HH


//== INCLUDES =================================================================

// GMU
#include "BaseNode.hh"
#include "TransformNode.hh"

// Qt
#include <QEvent>
#include <QMouseEvent>

#include <ACG/GL/GLPrimitives.hh>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class ManipulatorNode ManipulatorNode.hh <ACG/Scenegraph/ManipulatorNode.hh>
    Use a manipulator to rotate, translate and scale the children of
    this node.
*/

class ACGDLLEXPORT ManipulatorNode : public TransformNode
{
public:


  /// Default constructor.
  ManipulatorNode( BaseNode* _parent=0,
		   const std::string& _name="<ManipulatorNode>" );

  /// Destructor.
  ~ManipulatorNode();


  /// class name
  ACG_CLASSNAME(ManipulatorNode);


  //
  // METHODS
  //



  virtual void setIdentity();

  /// set draw_cylinder_
  void set_draw_cylinder(bool _b) { draw_cylinder_ = _b; }

  /// get drawCylinder
  bool draw_cylinder() const { return draw_cylinder_; }

  /** Set direction in world coordinates. Will take the current
      rotation into account. **/
  void set_direction(Vec3d& _v);

  /** Get direction in world coordinates. Will take the current
      rotation into account. **/
  Vec3d direction() const;

  /// set cylindersize  (height + radius)
  void set_size( double _size) { cylinder_height_ = _size;
                                 cylinder_radius_ = _size/10.0;}

  /// get cylindersize
  double size() const { return cylinder_height_; }

  /// translate in cylinder direction
  void translate(double _s) {
    Vec3d rel_dir = rotation().transform_vector(direction_);
    TransformNode::translate(_s * rel_dir);
  }

  /// draw the cylinder (if enabled)
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);

  /// picking
  void pick(GLState& _state, PickTarget _target);

  /// get mouse events
  virtual void mouseEvent(GLState& _state, QMouseEvent* _event);

  /** if the manipulator node has been clicked by the mouse, this function
  *   will return true. Use reset_touched to reset this flag
  */
  bool touched() { return touched_; };

  /// Reset the touched flag ( see touched for details )
  void reset_touched() { touched_ = false; };


private:

  void setup_sphere_system(GLState& _state);
  void setup_cylinder_system(GLState& _state);

  bool mapToSphere(GLState& _state, const Vec2i& _v2, Vec3d& _v3);
  bool hitSphere(GLState& _state, const Vec2i& _v2);
  bool mapToCylinder (GLState& _state, const Vec2i& _v2, double& axis_hit);





  // ELEMENTS
  bool               draw_cylinder_;

  Vec3d              direction_;
  GLCylinder*        cylinder_;
  double             cylinder_radius_;
  double             cylinder_height_;
  int                cylinder_slices_;
  int                cylinder_stacks_;


  bool               cylinder_clicked_;
  bool               sphere_clicked_;

  Vec2i              oldPoint2D_;

  /// if the manipulator is cklicked with a mouse, this flag will be set to true
  bool 	             touched_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_MANIPUKATOR_NODE_HH defined
//=============================================================================

