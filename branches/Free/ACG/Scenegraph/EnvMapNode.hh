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
//  CLASS EnvMapNode
//
//=============================================================================


#ifndef ACG_ENVMAP_NODE_HH
#define ACG_ENVMAP_NODE_HH


//== INCLUDES =================================================================


#include "TextureNode.hh"


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class EnvMapNode EnvMapNode.hh <ACG/Scenegraph/EnvMapNode.hh>
    Similar to TextureNode, but sets up a spherical environment map.
**/

class ACGDLLEXPORT EnvMapNode : public TextureNode
{
public:

  /// Default constructor. Applies all properties.
  EnvMapNode( BaseNode*           _parent = 0,
	      const std::string&  _name = "<EnvMapNode>",
              bool                _texture_repeat = true,
              GLint               _texture_filter = GL_LINEAR );

  /// set class name
  ACG_CLASSNAME(EnvMapNode);

  /// add env mapping
  unsigned int availableDrawModes() const;

  /// set texture
  void enter(GLState& _state, unsigned int _drawmode);

  /// restores original texture (or no-texture)
  void leave(GLState& _state, unsigned int _drawmode);

  /** \brief Do nothing in picking
   */
  void enterPick(GLState& _state, PickTarget _target, unsigned int _drawMode );

  /** \brief Do nothing in picking
   */
  void leavePick(GLState& _state, PickTarget _target, unsigned int _drawMode );
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_ENVMAP_NODE_HH defined
//=============================================================================

