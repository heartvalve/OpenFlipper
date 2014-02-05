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
//  CLASS TrackballNode
//
//=============================================================================


#ifndef ACG_TRACKBALL_NODE_HH
#define ACG_TRACKBALL_NODE_HH


//== INCLUDES =================================================================

// GMU
#include "BaseNode.hh"
#include "TransformNode.hh"

#include <vector>

// Qt
#include <QEvent>
#include <QMouseEvent>

#ifdef max
#  undef max
#endif

#ifdef min
#  undef min
#endif



//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================

	      
/** \class TrackballNode TrackballNode.hh <ACG/Scenegraph/TrackballNode.hh>

    Use a virtual trackball to rotate, translate and scale the children
    of this node. If you want to do translations use the ManipulatorNode.

    \see ACG::SceneGraph::ManipulatorNode
**/
  
class ACGDLLEXPORT TrackballNode : public TransformNode
{
public:
   
  
  /// Default constructor.
  TrackballNode( BaseNode* _parent=0,
		 const std::string& _name="<TrackballNode>" )
    : TransformNode(_parent, _name),
      drawTrackball_(false),
      drawAxes_(false),
      radius_(1.0),
      xAxis_(1.0, 0.0, 0.0),
      yAxis_(0.0, 1.0, 0.0),
      zAxis_(0.0, 0.0, 1.0)
  {}

  
  /// Destructor.
  ~TrackballNode() {}

    
  /// Class name macro
  ACG_CLASSNAME(TrackballNode);

  
  //
  // METHODS
  //


  /// Set trackball radius
  void set_radius(double _r) { radius_ = _r; }
  /// Get trackball radius
  double radius() const { return radius_; }

  
  /// override TransformNode::setIdentity() (update radius_)
  virtual void setIdentity() {
    Vec3d vec (1.0, 1.0, 1.0);
    
    vec = scale ().transform_point(vec);
    radius_ *= vec.max ();
    TransformNode::setIdentity();
  }

  
  /// Turn on/off drawing of the trackball
  void set_draw_trackball(bool _b) { drawTrackball_ = _b; }
  /// Is trackball-drawing on?
  bool draw_trackball() const { return drawTrackball_; }


  /// Turn drawing the axes on/off
  void set_draw_axes(bool _b) { drawAxes_ = _b; }
  /// Is axes-drawing on?
  bool draw_axes() const { return drawAxes_; }
  
  
  /// Draw the trackball + axes (if enabled)
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);


  /// get mouse events
  virtual void mouseEvent(GLState& _state, QMouseEvent* _event);
  
  
private:

  /// Map 2D-screen-coords to trackball
  bool mapToSphere(GLState& _state, const Vec2i& _v2, Vec3d& _v3);

  
  // ELEMENTS
  bool                                 drawTrackball_,
                                       drawAxes_;
  double                               radius_;
  Vec3d                                xAxis_, yAxis_, zAxis_;
  Vec2i                                oldPoint2D_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_TRACKBALL_NODE_HH defined
//=============================================================================

