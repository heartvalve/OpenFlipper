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
//   $Revision: 2345 $
//   $Author: moebius $
//   $Date: 2008-07-28 21:21:41 +0200 (Mon, 28 Jul 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS QtManipulatorNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "QtManipulatorNode.hh"

#include <QMouseEvent>
#include <QEvent>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================

QtManipulatorNode::
QtManipulatorNode( BaseNode* _parent, const std::string& _name )
  : ManipulatorNode(_parent, _name),
    identifier_(-1)
{
}

void
QtManipulatorNode::mouseEvent(GLState& _state, QMouseEvent* _event)
{
  ManipulatorNode::mouseEvent( _state, _event );
  emit manipulatorMoved( this , _event);
}

QtManipulatorNode::~QtManipulatorNode() {
}



//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
