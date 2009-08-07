/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
  /// don't pick me
  void pick(GLState& _state, PickTarget _target);
public:

  /// GridSize
  float gridSize();

  void gridSize(float _size);

private:

  /// initial number of baseLines
  int horizontalLines_;
  int verticalLines_;
  int maxRefinement_;

  /// dimensions of the grid
  float gridSize_;

  /// bounding box
  Vec3f bb_min_;
  Vec3f bb_max_;

  /// colors for the grid
  Vec3f baseLineColor_;
  Vec3f midLineColor_;

  bool ok_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_GRIDNODE_HH defined
//=============================================================================

