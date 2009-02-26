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
//  CLASS SwitchNode
//
//=============================================================================

#ifndef ACG_SWITCHNODE_HH
#define ACG_SWITCHNODE_HH

//=============================================================================

#include "SeparatorNode.hh"

//=============================================================================

namespace ACG {
namespace SceneGraph {

//=============================================================================


class ACGDLLEXPORT SwitchNode : public SeparatorNode
{
public:

  /// Constructor.
  SwitchNode( BaseNode* _parent=0,
	      const std::string& _name="<SwitchNode>" )
    : SeparatorNode(_parent, _name)
  {}


  /// Destructor.
  virtual ~SwitchNode() {}


  ACG_CLASSNAME(SwitchNode);


private:

};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_SWITCHNODE_HH defined
//=============================================================================

