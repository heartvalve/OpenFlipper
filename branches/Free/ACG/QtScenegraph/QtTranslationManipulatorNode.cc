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
//  CLASS QtTranslationManipulatorNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "QtTranslationManipulatorNode.hh"

#include <QMouseEvent>
#include <QEvent>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================

QtTranslationManipulatorNode::
QtTranslationManipulatorNode( BaseNode* _parent, const std::string& _name )
  : TranslationManipulatorNode(_parent, _name),
    identifier_(-1)
{
}

void
QtTranslationManipulatorNode::mouseEvent(GLState& _state, QMouseEvent* _event)
{
  TranslationManipulatorNode::mouseEvent( _state, _event );
  emit manipulatorMoved( this , _event);
}

QtTranslationManipulatorNode::~QtTranslationManipulatorNode() {
}



//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
