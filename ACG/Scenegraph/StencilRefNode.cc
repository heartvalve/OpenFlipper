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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




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

void StencilRefNode::enter(GLState& /*_state*/, const DrawModes::DrawMode& /*_drawmode*/)
{
  if(visible()) {
    glPushAttrib (GL_STENCIL_BUFFER_BIT);
    glStencilFunc (GL_ALWAYS, reference_, ~0);
    glStencilOp (GL_KEEP, GL_KEEP, GL_REPLACE);
  }
}

//----------------------------------------------------------------------------

void StencilRefNode::leave(GLState& /*_state*/, const DrawModes::DrawMode& /*_drawmode*/)
{
  if(visible()) {
    glPopAttrib ();
  }
}

//----------------------------------------------------------------------------

void StencilRefNode::enterPick(GLState& /*_state*/ , PickTarget /*_target*/, const DrawModes::DrawMode& /*_drawMode*/ ) {

}

//----------------------------------------------------------------------------

void StencilRefNode::leavePick(GLState& /*_state*/, PickTarget /*_target*/, const DrawModes::DrawMode& /*_drawMode*/ ) {
}

void StencilRefNode::setReference(GLuint _ref)
{
  reference_ = _ref;
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

