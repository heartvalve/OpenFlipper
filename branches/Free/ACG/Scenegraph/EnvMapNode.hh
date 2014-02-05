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
  DrawModes::DrawMode  availableDrawModes() const;

  /// set texture
  void enter(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /// restores original texture (or no-texture)
  void leave(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /** \brief Do nothing in picking
   */
  void enterPick(GLState& _state, PickTarget _target, const DrawModes::DrawMode& _drawMode );

  /** \brief Do nothing in picking
   */
  void leavePick(GLState& _state, PickTarget _target, const DrawModes::DrawMode& _drawMode );
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_ENVMAP_NODE_HH defined
//=============================================================================

