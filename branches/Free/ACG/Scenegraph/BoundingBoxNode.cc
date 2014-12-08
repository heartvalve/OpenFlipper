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
//  CLASS BoundingBoxNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "BoundingBoxNode.hh"
#include "SceneGraph.hh"
#include "../GL/gl.hh"
#include "../GL/GLPrimitives.hh"


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================

DrawModes::DrawMode
BoundingBoxNode::
availableDrawModes() const
{
  return DrawModes::WIREFRAME;
}

//----------------------------------------------------------------------------

BoundingBoxNode::BoundingBoxNode( BaseNode* _parent, std::string _name ) :
MaterialNode(_parent,
  _name,
  MaterialNode::BaseColor |
  MaterialNode::LineWidth),
  box_(0)
{
  drawMode(DrawModes::WIREFRAME);


  box_ = new GLLineBox();
}

//----------------------------------------------------------------------------
BoundingBoxNode::~BoundingBoxNode() {
  delete box_;
}

//----------------------------------------------------------------------------

void BoundingBoxNode::computeAABB( Vec3d* _outMin, Vec3d* _outMax )
{
  ACG::SceneGraph::BoundingBoxAction act;
  ACG::SceneGraph::traverse(this, act);

  if (_outMin)
    *_outMin = (ACG::Vec3d) act.bbMin();

  if (_outMax)
    *_outMax = (ACG::Vec3d) act.bbMax();
}

//----------------------------------------------------------------------------

void
BoundingBoxNode::
draw(GLState&  _state  , const DrawModes::DrawMode& _drawMode)
{
  if (_drawMode & DrawModes::WIREFRAME)
  {
    ACG::Vec3d bbmin;
    ACG::Vec3d bbmax;
    computeAABB(&bbmin, &bbmax);

    ACG::Vec3d bbcenter = (bbmin + bbmax) * 0.5;
    ACG::Vec3d bbsize = bbmax - bbmin;


    glPushAttrib (GL_ENABLE_BIT);

    ACG::GLState::disable(GL_LIGHTING);

    _state.push_modelview_matrix();

    _state.translate(bbcenter);
    _state.scale(bbsize[0], bbsize[1], bbsize[2]);

    glColor4f(0.0f,1.0f,0.0f,1.0f);

    box_->draw();

    _state.pop_modelview_matrix();
    glPopAttrib ();
  }
}

//----------------------------------------------------------------------------

void BoundingBoxNode::getRenderObjects(IRenderer* _renderer, GLState& _state , const DrawModes::DrawMode& _drawMode , const ACG::SceneGraph::Material* _mat)
{
  int dmlayerId = _drawMode.getLayerIndexByPrimitive(DrawModes::PRIMITIVE_WIREFRAME);

  if (dmlayerId >= 0)
  {
    ACG::Vec3d bbmin;
    ACG::Vec3d bbmax;
    computeAABB(&bbmin, &bbmax);

    ACG::Vec3d bbcenter = (bbmin + bbmax) * 0.5;
    ACG::Vec3d bbsize = bbmax - bbmin;

    // create renderobject
    RenderObject ro;
    ro.initFromState(&_state);
    ro.depthTest = true;
    ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;
    ro.emissive = Vec3f(0.0f, 1.0f, 0.0f);

    ro.modelview.translate(bbcenter);
    ro.modelview.scale(bbsize[0], bbsize[1], bbsize[2]);

    box_->addToRenderer_primitive(_renderer, &ro);
  }
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
