/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS SceneGraph - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================


#include <ACG/Scenegraph/SceneGraph.hh>
#include <ACG/GL/ColorTranslator.hh>


//== NAMESPACES ============================================================== 

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 

/** \brief Find a node in the scene graph
 *
 *
 * Traverse scenegraph starting at _root, looking for a node whose id
 * is _node_idx . Returns 0 if node wasn't found.
 *
 * @param _root     The root node where the traversal starts (not necessary the root node of the scenegraph)
 * @param _node_idx The node index this function should look for
 * @return 0 if the node was not found, otherwise a pointer to the node.
**/
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
  
  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::disable(GL_DITHER);
  ACG::GLState::shadeModel(GL_FLAT);

  state_.pick_push_name ((GLuint) _node->id());

  _node->pick(state_, pickTarget_);

  state_.pick_pop_name ();
  return true;
}

bool PickAction::operator()(BaseNode* _node, GLState& /*_state*/) {
  return operator()(_node);
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
