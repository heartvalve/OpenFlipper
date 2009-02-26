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
//  CLASS SeparatorNode
//
//=============================================================================


#ifndef ACG_SEPARATORNODE_HH
#define ACG_SEPARATORNODE_HH


//== INCLUDES =================================================================


#include "BaseNode.hh"



//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class SeparatorNode SeparatorNode.hh <ACG/Scenegraph/SeparatorNode.hh>

    A separator node is "invisible" and only used to administrate its
    children.
**/

class ACGDLLEXPORT SeparatorNode : public BaseNode
{
public:

  /// Constructor.
  SeparatorNode( BaseNode* _parent=0,
		 const std::string& _name="<separator>" )
    : BaseNode(_parent, _name)
  {}


  /// Destructor.
  virtual ~SeparatorNode() {}


  ACG_CLASSNAME(SeparatorNode);


private:

};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_SEPARATORNODE_HH defined
//=============================================================================

