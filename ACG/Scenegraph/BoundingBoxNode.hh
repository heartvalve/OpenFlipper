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
//  CLASS BoundingBoxNode
//
//=============================================================================


#ifndef ACG_BOUNDINGBOXNODE_HH
#define ACG_BOUNDINGBOXNODE_HH


//== INCLUDES =================================================================

#include "MaterialNode.hh"
#include "DrawModes.hh"
#include <vector>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== CLASS DEFINITION =========================================================



/** \class BoundingBoxNode BoundingBoxNode.hh <ACG/Scenegraph/BoundingBoxNode.hh>

    BoundingBoxNode renders the bounding box of its childe nodes
**/

class ACGDLLEXPORT BoundingBoxNode : public MaterialNode
{
public:

  /// default constructor
  BoundingBoxNode( BaseNode*    _parent=0,
	    std::string  _name="<BoundingBoxNode>" ) :
       MaterialNode(_parent,
		 _name,
		 MaterialNode::BaseColor |
		 MaterialNode::LineWidth)
  {
    drawMode(DrawModes::WIREFRAME);
  }

  /// destructor
  virtual ~BoundingBoxNode();

  /// static name of this class
  ACG_CLASSNAME(BoundingBoxNode);

  /// return available draw modes
  DrawModes::DrawMode availableDrawModes() const;

  /// draw lines and normals
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_LINENODE_HH defined
//=============================================================================

