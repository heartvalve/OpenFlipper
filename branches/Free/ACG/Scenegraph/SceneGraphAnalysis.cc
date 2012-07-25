/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include "SceneGraphAnalysis.hh"

namespace ACG {
namespace SceneGraph {


void analyzeSceneGraph(ACG::SceneGraph::BaseNode* _root,
                       unsigned int&              _maxPasses,
                       ACG::Vec3d&                _bbmin,
                       ACG::Vec3d&                _bbmax)
{
  // Get max number of render pass
  // Single pass action, this info is static during multipass
  ACG::SceneGraph::MultiPassInfoAction info_act;
  ACG::SceneGraph::traverse(_root, info_act);

  _maxPasses = info_act.getMaxPasses();

  // get scene size
  // Single pass action, as the bounding box is not influenced by multipass traversal
  ACG::SceneGraph::BoundingBoxAction act;
  ACG::SceneGraph::traverse(_root, act);

  _bbmin = (ACG::Vec3d) act.bbMin();
  _bbmax = (ACG::Vec3d) act.bbMax();
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
