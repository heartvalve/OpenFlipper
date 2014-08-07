/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision: 18128 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2014-02-05 10:20:28 +0100 (Wed, 05 Feb 2014) $                     *
*                                                                            *
\*===========================================================================*/


//=============================================================================
//
//  StatusViewNode: Combined rendering of multiple Status Nodes
//
//=============================================================================

#ifndef STATUSVIEWNODE_HH
#define STATUSVIEWNODE_HH

//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalDefines.hh>

#include <ACG/Scenegraph/StatusNodesT.hh>

#include "StatusNodeMods.hh"


//== CLASS DEFINITION =========================================================

namespace ACG
{
namespace SceneGraph
{

template <class MeshT>
class DLLEXPORTONLY StatusViewNodeT : public ACG::SceneGraph::BaseNode
{
public:
  StatusViewNodeT(
    BaseNode* _parent,
    const std::string& _name,
    ACG::SceneGraph::SelectionNodeT<MeshT>* _statusNode,
    ACG::SceneGraph::StatusNodeT<MeshT, AreaNodeMod<MeshT> >* _areaNode,
    ACG::SceneGraph::StatusNodeT<MeshT, HandleNodeMod<MeshT> >* _handleNode,
    ACG::SceneGraph::StatusNodeT<MeshT, FeatureNodeMod<MeshT> >* _featureNode);

  // BaseNode implementation

  ACG_CLASSNAME(StatusNodeView);


  void draw(GLState& , const DrawModes::DrawMode& );

  void getRenderObjects(IRenderer* _renderer, GLState& _state , const DrawModes::DrawMode& _drawMode , const Material* _mat);

private:

  ACG::SceneGraph::SelectionNodeT<MeshT>*                       statusNode_;
  ACG::SceneGraph::StatusNodeT<MeshT, AreaNodeMod<MeshT> >*     areaNode_;
  ACG::SceneGraph::StatusNodeT<MeshT, HandleNodeMod<MeshT> >*   handleNode_;
  ACG::SceneGraph::StatusNodeT<MeshT, FeatureNodeMod<MeshT> >*  featureNode_;
};


}
}

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(STATUSVIEWNODE_C)
#define STATUS_VIEW_NODE_TEMPLATES
#include "StatusViewNodeT.cc"
#endif
//=============================================================================
#endif // STATUSVIEWNODE_HH
//=============================================================================
/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision: 18128 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2014-02-05 10:20:28 +0100 (Wed, 05 Feb 2014) $                     *
*                                                                            *
\*===========================================================================*/


//=============================================================================
//
//  StatusViewNode: Combined rendering of multiple Status Nodes
//
//=============================================================================

#ifndef STATUSVIEWNODE_HH
#define STATUSVIEWNODE_HH

//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalDefines.hh>

#include <ACG/Scenegraph/StatusNodesT.hh>

#include "StatusNodeMods.hh"


//== CLASS DEFINITION =========================================================

namespace ACG
{
namespace SceneGraph
{

template <class MeshT>
class DLLEXPORTONLY StatusViewNodeT : public ACG::SceneGraph::BaseNode
{
public:
  StatusViewNodeT(
    BaseNode* _parent,
    const std::string& _name,
    ACG::SceneGraph::SelectionNodeT<MeshT>* _statusNode,
    ACG::SceneGraph::StatusNodeT<MeshT, AreaNodeMod<MeshT> >* _areaNode,
    ACG::SceneGraph::StatusNodeT<MeshT, HandleNodeMod<MeshT> >* _handleNode,
    ACG::SceneGraph::StatusNodeT<MeshT, FeatureNodeMod<MeshT> >* _featureNode);

  // BaseNode implementation

  ACG_CLASSNAME(StatusNodeView);


  void draw(GLState& , const DrawModes::DrawMode& );

  void getRenderObjects(IRenderer* _renderer, GLState& _state , const DrawModes::DrawMode& _drawMode , const Material* _mat);

private:

  ACG::SceneGraph::SelectionNodeT<MeshT>*                       statusNode_;
  ACG::SceneGraph::StatusNodeT<MeshT, AreaNodeMod<MeshT> >*     areaNode_;
  ACG::SceneGraph::StatusNodeT<MeshT, HandleNodeMod<MeshT> >*   handleNode_;
  ACG::SceneGraph::StatusNodeT<MeshT, FeatureNodeMod<MeshT> >*  featureNode_;
};


}
}

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(STATUSVIEWNODE_C)
#define STATUS_VIEW_NODE_TEMPLATES
#include "StatusViewNodeT.cc"
#endif
//=============================================================================
#endif // STATUSVIEWNODE_HH
//=============================================================================