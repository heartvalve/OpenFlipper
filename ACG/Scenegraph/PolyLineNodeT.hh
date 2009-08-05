/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
// CLASS PolyLineNodeT
//
// Author:  David Bommes <bommes@cs.rwth-aachen.de>
//
//=============================================================================


#ifndef ACG_POLYLINENODET_HH
#define ACG_POLYLINENODET_HH


//== INCLUDES =================================================================

#include <ACG/Scenegraph/MaterialNode.hh>
#include <ACG/Scenegraph/DrawModes.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================




/** \class PolyLineNodeT PolyLineNodeT.hh <ACG/.../PolyLineNodeT.hh>

    Brief Description.

    A more elaborate description follows.
*/

template <class PolyLine>
class PolyLineNodeT : public MaterialNode
{
public:

  // typedefs for easy access
  typedef typename PolyLine::Point Point;

  /// Constructor
  PolyLineNodeT(PolyLine& _pl,
		BaseNode*    _parent=0,
		std::string  _name="<PolyLineNode>" ) :
    MaterialNode(_parent,
		 _name,
		 MaterialNode::BaseColor |
		 MaterialNode::LineWidth |
		 MaterialNode::PointSize |
		 MaterialNode::RoundPoints),
    polyline_(_pl)
  {
    drawMode(DrawModes::WIREFRAME | DrawModes::POINTS);
    set_point_size(8.0);
    set_line_width(3.0);
    set_round_points(true);
    set_random_color();
  }

  /// Destructor
  ~PolyLineNodeT() {}

  void set_random_color();

  PolyLine& polyline() { return polyline_; }

  /// static name of this class
  ACG_CLASSNAME(PolyLineNodeT);

  /// return available draw modes
  unsigned int availableDrawModes() const;

  /// update bounding box
  void boundingBox(Vec3f& _bbMin, Vec3f& _bbMax);

  /// draw lines and normals
  void draw(GLState& /*_state*/, unsigned int _drawMode);

  /// picking
  void pick(GLState& _state, PickTarget _target);


private:

  void pick_vertices( GLState& _state );
  void pick_edges( GLState& _state, unsigned int _offset);


  void draw_cylinder( const Point& _p0, const Point& _axis, double _r, GLState& _state);


  /// Copy constructor (not used)
  PolyLineNodeT(const PolyLineNodeT& _rhs);

  /// Assignment operator (not used)
  PolyLineNodeT& operator=(const PolyLineNodeT& _rhs);

private:

  PolyLine& polyline_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_POLYLINENODET_C)
#define ACG_POLYLINENODET_TEMPLATES
#include "PolyLineNodeT.cc"
#endif
//=============================================================================
#endif // ACG_POLYLINENODET_HH defined
//=============================================================================

