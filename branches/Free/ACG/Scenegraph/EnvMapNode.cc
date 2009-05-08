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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




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


unsigned int EnvMapNode::availableDrawModes() const
{
  return ACG::SceneGraph::DrawModes::SOLID_ENV_MAPPED;
}


//----------------------------------------------------------------------------


void EnvMapNode::enter(GLState& _state, unsigned int _drawmode)
{
  if (_drawmode==DrawModes::SOLID_ENV_MAPPED)
  {
    TextureNode::enter(_state, _drawmode);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_2D);
  }
}


//----------------------------------------------------------------------------


void EnvMapNode::leave(GLState& _state, unsigned int _drawmode)
{
  if (_drawmode==DrawModes::SOLID_ENV_MAPPED)
  {
    TextureNode::leave(_state, _drawmode);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
  }
}

void EnvMapNode::enterPick(GLState& /*_state*/ ,
                           PickTarget /*_target*/,
                           unsigned int /*_drawMode*/ ) {

}

void EnvMapNode::leavePick(GLState& /*_state*/,
                           PickTarget /*_target*/,
                           unsigned int /*_drawMode*/ ) {
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
