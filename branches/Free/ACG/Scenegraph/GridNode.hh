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
//   $Revision: 3468 $
//   $Author: moebius $
//   $Date: 2008-10-17 14:58:52 +0200 (Fr, 17. Okt 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS GridNode
//
//=============================================================================


#ifndef ACG_GRIDNODE_HH
#define ACG_GRIDNODE_HH


//== INCLUDES =================================================================


#include "MaterialNode.hh"
#include <vector>


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================



/** \class GridNode GridNode.hh <ACG/.../GridNode.hh>

    Scenegraph Node.

    A more elaborate description follows.
**/
class ACGDLLEXPORT GridNode : public MaterialNode
{
public:

  /// Default constructor
  GridNode(BaseNode*  _parent=0,
	     const std::string&  _name="<GridNode>" );

  /// Destructor
  ~GridNode() {}


  /// implement className()
  ACG_CLASSNAME(GridNode);
  /// return available draw modes
  unsigned int availableDrawModes() const;
  /// update bounding box
  void boundingBox(Vec3f& _bbMin, Vec3f& _bbMax);
  /// drawing the primitive
  void draw(GLState& _state, unsigned int _drawMode);


private:

  int horizontalLines_;
  int verticalLines_;

  bool ok_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_GRIDNODE_HH defined
//=============================================================================

