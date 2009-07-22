//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2009 by Computer Graphics Group, RWTH Aachen
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
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================




//=============================================================================
//
//  CLASS StencilRefNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "StencilRefNode.hh"


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


StencilRefNode::StencilRefNode (BaseNode*          _parent,
                                const std::string& _name)
  : BaseNode(_parent, _name),
  reference_ (0)
{}

//----------------------------------------------------------------------------

StencilRefNode::StencilRefNode (BaseNode*          _parent,
                                BaseNode*          _child,
                                const std::string& _name)
  : BaseNode(_parent, _child, _name),
  reference_ (0)
{}

//----------------------------------------------------------------------------

void StencilRefNode::enter(GLState& _state, unsigned int _drawmode)
{
  glPushAttrib (GL_STENCIL_BUFFER_BIT);
  glStencilFunc (GL_ALWAYS, reference_, ~0);
  glStencilOp (GL_KEEP, GL_KEEP, GL_REPLACE);
}

//----------------------------------------------------------------------------

void StencilRefNode::leave(GLState& _state, unsigned int _drawmode)
{
  glPopAttrib ();
}

//----------------------------------------------------------------------------

void StencilRefNode::enterPick(GLState& /*_state*/ ,
                           PickTarget /*_target*/,
                           unsigned int /*_drawMode*/ ) {

}

//----------------------------------------------------------------------------

void StencilRefNode::leavePick(GLState& /*_state*/,
                           PickTarget /*_target*/,
                           unsigned int /*_drawMode*/ ) {
}

void StencilRefNode::setReference(GLuint _ref)
{
  reference_ = _ref;
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

