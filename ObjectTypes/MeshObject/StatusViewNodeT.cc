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

#ifndef STATUSVIEWNODE_C
#define STATUSVIEWNODE_C

//== INCLUDES =================================================================

#include "StatusViewNodeT.hh"
#include <ACG/Scenegraph/DrawModes.hh>

namespace ACG
{
namespace SceneGraph
{


//== IMPLEMENTATION ==========================================================

template<class MeshT>
StatusViewNodeT<MeshT>::StatusViewNodeT(
  BaseNode* _parent,
  const std::string& _name,
  ACG::SceneGraph::SelectionNodeT<MeshT>* _statusNode,
  ACG::SceneGraph::StatusNodeT<MeshT, AreaNodeMod<MeshT> >* _areaNode,
  ACG::SceneGraph::StatusNodeT<MeshT, HandleNodeMod<MeshT> >* _handleNode,
  ACG::SceneGraph::StatusNodeT<MeshT, FeatureNodeMod<MeshT> >* _featureNode)
  : BaseNode(_parent, _name), 
  statusNode_(_statusNode), 
  areaNode_(_areaNode),
  handleNode_(_handleNode),
  featureNode_(_featureNode),
  statusNodeVis_(false),
  areaNodeVis_(false),
  handleNodeVis_(false),
  featureNodeVis_(false)
{
}

//----------------------------------------------------------------------------

template <class MeshT>
void StatusViewNodeT<MeshT>::draw( GLState& _state, const DrawModes::DrawMode& _drawMode )
{
  // extract all layers from drawmode

  DrawModes::DrawMode singleLayers[4]; // polygon, edge, halfedge, point

  // remove default property layer
  for (int i = 0; i < 4; ++i)
    singleLayers[i].removeLayer(0u);

  for (unsigned int i = 0; i < _drawMode.getNumLayers(); ++i)
  {
    const DrawModes::DrawModeProperties* props = _drawMode.getLayer(i);

    switch (props->primitive())
    {
    case DrawModes::PRIMITIVE_POLYGON: singleLayers[0].setDrawModeProperties(props); break;
    case DrawModes::PRIMITIVE_EDGE: singleLayers[1].setDrawModeProperties(props); break;
    case DrawModes::PRIMITIVE_HALFEDGE: singleLayers[2].setDrawModeProperties(props); break;
    case DrawModes::PRIMITIVE_POINT: singleLayers[3].setDrawModeProperties(props); break;
    default: break;
    }
  }

  // rendering order: faces -> edges -> halfedges -> points
  //   area -> handle -> feature -> selections
  for (int i = 0; i < 4; ++i)
  {

    if (singleLayers[i].getNumLayers() == 0)
    {
      DrawModes::DrawModeProperties defaultProps;

      switch (i)
      {
      case 0: defaultProps.primitive(DrawModes::PRIMITIVE_POLYGON); break;
      case 1: defaultProps.primitive(DrawModes::PRIMITIVE_EDGE); break;
      case 2: defaultProps.primitive(DrawModes::PRIMITIVE_HALFEDGE); break;
      case 3: defaultProps.primitive(DrawModes::PRIMITIVE_POINT); break;
      default: break;
      }
      
      singleLayers[i].addLayer(&defaultProps);
    }

    // call enter() and leave() to setup the correct material

    if (areaNodeVis_)
    {
      areaNode_->enter(_state, singleLayers[i]);
      areaNode_->draw(_state, singleLayers[i]);
      areaNode_->leave(_state, singleLayers[i]);
    }

    if (handleNodeVis_)
    {
      handleNode_->enter(_state, singleLayers[i]);
      handleNode_->draw(_state, singleLayers[i]);
      handleNode_->leave(_state, singleLayers[i]);
    }

    if (featureNodeVis_)
    {
      featureNode_->enter(_state, singleLayers[i]);
      featureNode_->draw(_state, singleLayers[i]);
      featureNode_->leave(_state, singleLayers[i]);
    }

    if (statusNodeVis_)
    {
      statusNode_->enter(_state, singleLayers[i]);
      statusNode_->draw(_state, singleLayers[i]);
      statusNode_->leave(_state, singleLayers[i]);
    }
  }

}

//----------------------------------------------------------------------------

template <class MeshT>
void StatusViewNodeT<MeshT>::getRenderObjects(IRenderer* _renderer, GLState& _state , const DrawModes::DrawMode& _drawMode , const Material* _mat)
{
  // rendering order only depends on priority in render objects

  // call enter() and leave() to setup the correct material
  if (statusNodeVis_)
  {
    statusNode_->getRenderObjects(_renderer, _state, _drawMode, _mat);
  }

  if (areaNodeVis_)
  {
    areaNode_->getRenderObjects(_renderer, _state, _drawMode, _mat);
  }

  if (handleNodeVis_)
  {
    handleNode_->getRenderObjects(_renderer, _state, _drawMode, _mat);
  }

  if (featureNodeVis_)
  {
    featureNode_->getRenderObjects(_renderer, _state, _drawMode, _mat);
  }
}


//----------------------------------------------------------------------------

template <class MeshT>
void StatusViewNodeT<MeshT>::enter( GLState& _state, const DrawModes::DrawMode& _drawmode )
{
  statusNodeVis_ = statusNode_ && statusNode_->visible();
  areaNodeVis_ = areaNode_ && areaNode_->visible();
  handleNodeVis_ = handleNode_ && handleNode_->visible();
  featureNodeVis_ = featureNode_ && featureNode_->visible();

  // these nodes are manually rendered in this StatusViewNode
  //  hide nodes to prevent drawing them again in the same draw traversal

  if (statusNodeVis_)
    statusNode_->hide();
  if (areaNodeVis_)
    areaNode_->hide();
  if (handleNodeVis_)
    handleNode_->hide();
  if (featureNodeVis_)
    featureNode_->hide();
}

//----------------------------------------------------------------------------

template <class MeshT>
void StatusViewNodeT<MeshT>::leave( GLState& _state, const DrawModes::DrawMode& _drawmode )
{
  if (statusNodeVis_)
    statusNode_->show();
  if (areaNodeVis_)
    areaNode_->show();
  if (handleNodeVis_)
    handleNode_->show();
  if (featureNodeVis_)
    featureNode_->show();
}


}
}

#endif // STATUSVIEWNODE_C
