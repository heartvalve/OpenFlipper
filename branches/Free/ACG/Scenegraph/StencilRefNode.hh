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
//  CLASS StencilRefNode
//
//=============================================================================


#ifndef ACG_STENCILREF_NODE_HH
#define ACG_STENCILREF_NODE_HH


//== INCLUDES =================================================================


#include "BaseNode.hh"


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class StencilRefNode StencilRefNode.hh <ACG/Scenegraph/StencilRefNode.hh>
    Sets the stencil buffer reference for its child nodes
**/

class ACGDLLEXPORT StencilRefNode : public BaseNode
{
public:

  /// Default constructor.
  StencilRefNode (BaseNode*           _parent = 0,
	          const std::string&  _name = "<StencilRefNode>");

  /// Put this node between _parent and _child
  StencilRefNode (BaseNode*           _parent,
                  BaseNode*           _child,
                  const std::string&  _name="<unknown>");

  /// set class name
  ACG_CLASSNAME(StencilRefNode);

  /// set stencil reference number
  void setReference (GLuint _ref);

  /// set stencil reference for all z-pass pixels
  void enter(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /// restores original stencil behavior
  void leave(GLState& _state, const DrawModes::DrawMode& _drawmode);

  /** \brief Do nothing in picking
   */
  void enterPick(GLState& _state, PickTarget _target, const DrawModes::DrawMode& _drawMode );

  /** \brief Do nothing in picking
   */
  void leavePick(GLState& _state, PickTarget _target, const DrawModes::DrawMode& _drawMode );

private:

  GLuint reference_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_ENVMAP_NODE_HH defined
//=============================================================================

