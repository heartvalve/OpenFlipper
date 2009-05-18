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
//   $Revision: 4826 $
//   $Author: dekkers $
//   $Date: 2009-02-12 20:50:56 +0100 (Do, 12. Feb 2009) $
//
//=============================================================================




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
  unsigned int availableDrawModes() const;

  /// draw lines and normals
  void draw(GLState& _state, unsigned int _drawMode);
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_LINENODE_HH defined
//=============================================================================

