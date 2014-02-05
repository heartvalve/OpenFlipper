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
//  CLASS EnvMapNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "EnvMapNode.hh"
#include "DrawModes.hh"


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


EnvMapNode::EnvMapNode( BaseNode*            _parent,
                        const std::string&   _name ,
                        bool                _texture_repeat,
                        GLint               _texture_filter)
  : TextureNode(_parent, _name,_texture_repeat,_texture_filter)
{}


//----------------------------------------------------------------------------


DrawModes::DrawMode EnvMapNode::availableDrawModes() const
{
  return ACG::SceneGraph::DrawModes::SOLID_ENV_MAPPED;
}


//----------------------------------------------------------------------------


void EnvMapNode::enter(GLState& _state, const DrawModes::DrawMode& _drawmode)
{
  if (_drawmode==DrawModes::SOLID_ENV_MAPPED)
  {
    TextureNode::enter(_state, _drawmode);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    ACG::GLState::enable(GL_TEXTURE_GEN_S);
    ACG::GLState::enable(GL_TEXTURE_GEN_T);
    ACG::GLState::enable(GL_TEXTURE_2D);
  } else
    TextureNode::enter( _state, _drawmode);
}


//----------------------------------------------------------------------------


void EnvMapNode::leave(GLState& _state, const DrawModes::DrawMode& _drawmode)
{
  if (_drawmode==DrawModes::SOLID_ENV_MAPPED)
  {
    TextureNode::leave(_state, _drawmode);
    ACG::GLState::disable(GL_TEXTURE_GEN_S);
    ACG::GLState::disable(GL_TEXTURE_GEN_T);
    ACG::GLState::disable(GL_TEXTURE_2D);
  } else
    TextureNode::leave( _state, _drawmode);
}

void EnvMapNode::enterPick(GLState& /*_state*/ , PickTarget /*_target*/, const DrawModes::DrawMode& /*_drawMode*/ ) {

}

void EnvMapNode::leavePick(GLState& /*_state*/, PickTarget /*_target*/, const DrawModes::DrawMode& /*_drawMode*/ ) {
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
