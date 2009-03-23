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
//  CLASS SceneGraph - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================


#include "SceneGraph.hh"
#include "../GL/ColorTranslator.hh"


//== NAMESPACES ============================================================== 

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 


BaseNode* find_node(BaseNode* _root, unsigned int _node_idx) 
{
  FindNodeAction action(_node_idx);
  traverse(_root, action);
  return action.node_ptr();
}


//----------------------------------------------------------------------------


bool PickAction::operator()(BaseNode* _node)
{
  // If picking is disabled for the given Node, return here
  // As this is not a failure return true;
  if ( !_node->pickingEnabled() )
    return true;
  
  glDisable(GL_LIGHTING);
  glDisable(GL_DITHER);
  glShadeModel(GL_FLAT);

  state_.pick_push_name ((GLuint) _node->id());

  _node->pick(state_, pickTarget_);

  state_.pick_pop_name ();
  return true;
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
