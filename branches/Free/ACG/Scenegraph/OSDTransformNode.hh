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
 *   $Revision$                                                            *
 *   $Author$                                                              *
 *   $Date$                                                                *
 *                                                                           *
\*===========================================================================*/


//================================================================
//
//  CLASS OSDTransformNode
//
//================================================================


#ifndef ACG_OSDTRANSFORMNODE_HH
#define ACG_OSDTRANSFORMNODE_HH


//== INCLUDES ====================================================


#include "BaseNode.hh"
#include "DrawModes.hh"


//== NAMESPACES ==================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION ============================================


/** \class OSDTransformNode OSDTransformNode.hh <ACG/Scenegraph/OSDTransformNode.hh>

	OSDTransformNode sets up a transformation used to render on screen display geometry.

**/

class ACGDLLEXPORT OSDTransformNode : public BaseNode
{
public:

	/// default constructor
	inline OSDTransformNode( BaseNode *_parent = 0, const std::string &_name = "<OSDTransformNode>" ) : BaseNode( _parent, _name ), center_( 0.0, 0.0 ) { }

	/// destructor
	inline ~OSDTransformNode() { }

	/// static name of this class
	ACG_CLASSNAME( OSDTransformNode );

	/// set up transformation used for drawing
	inline void enter( GLState &_state, const DrawModes::DrawMode &_drawMode ) { enterOSD( _state, _drawMode ); }

	/// restore transformation used for drawing
	inline void leave( GLState &_state, const DrawModes::DrawMode &_drawMode ) { leaveOSD( _state, _drawMode ); }

	/// set up transformation used for picking
	inline void enterPick( GLState &_state, PickTarget _target, const DrawModes::DrawMode &_drawMode ) { enterOSD( _state, _drawMode ); }

	/// restore transformation used for picking
	inline void leavePick( GLState &_state, PickTarget _target, const DrawModes::DrawMode &_drawMode ) { leaveOSD( _state, _drawMode ); }

	/// set center of OSD
	inline void setCenter( double _x, double _y ) { center_[0] = _x;    center_[1] = _y;    }
	inline void setCenter( ACG::Vec2d _c )        { center_[0] = _c[0]; center_[1] = _c[1]; }

	/// get center of OSD
	inline ACG::Vec2d getCenter() const { return center_; }

private:

	void enterOSD( GLState &_state, const DrawModes::DrawMode &_drawMode );
	void leaveOSD( GLState &_state, const DrawModes::DrawMode &_drawMode );

	ACG::Vec2d center_;
};


//================================================================


} // namespace SceneGraph
} // namespace ACG


//================================================================


#endif // ACG_OSDTRANSFORMNODE_HH
