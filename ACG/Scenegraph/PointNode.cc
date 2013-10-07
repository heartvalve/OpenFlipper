/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS PointNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "PointNode.hh"
#include "../Utils/StopWatch.hh"
#include "../GL/gl.hh"
#include <ACG/GL/IRenderer.hh>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 


void
PointNode::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  ConstPointIter p_it=points_.begin(), p_end=points_.end();
  for (; p_it!=p_end; ++p_it) {
    _bbMin.minimize(*p_it); 
    _bbMax.maximize(*p_it);
  }
}


//----------------------------------------------------------------------------

  
DrawModes::DrawMode
PointNode::
availableDrawModes() const
{
  return ( DrawModes::POINTS | 
	   DrawModes::POINTS_SHADED | 
	   DrawModes::POINTS_COLORED );
}


//----------------------------------------------------------------------------


void
PointNode::
draw(GLState& /* _state */ , const DrawModes::DrawMode& _drawMode)
{
  if (points_.empty())
    return;
  
  // points
  if (_drawMode & DrawModes::POINTS)
  {
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
    ACG::GLState::vertexPointer(&points_[0]);
    glDrawArrays(GL_POINTS, 0, int(points_.size()));
  }


  // points and normals
  if (_drawMode & DrawModes::POINTS_SHADED)
  {
    if (points_.size() == normals_.size())
    {
      ACG::GLState::enable(GL_LIGHTING);
      ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
      ACG::GLState::vertexPointer(&points_[0]);
      ACG::GLState::enableClientState(GL_NORMAL_ARRAY);
      ACG::GLState::normalPointer(&normals_[0]);
      glDrawArrays(GL_POINTS, 0, int(points_.size()));
    }
  }


  // points and colors
  if (_drawMode & DrawModes::POINTS_COLORED)
  {
    if (points_.size() == colors_.size())
    {
      ACG::GLState::disable(GL_LIGHTING);
      ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
      ACG::GLState::vertexPointer(&points_[0]);
      ACG::GLState::enableClientState(GL_COLOR_ARRAY);
      ACG::GLState::colorPointer(&colors_[0]);
      glDrawArrays(GL_POINTS, 0, int(points_.size()));
    } else
      std::cerr << "Mismatch size!" << std::endl;
  }


  // disable arrays
  ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
  ACG::GLState::disableClientState(GL_NORMAL_ARRAY);
  ACG::GLState::disableClientState(GL_COLOR_ARRAY);
}

void 
PointNode::
getRenderObjects( IRenderer* _renderer, GLState& _state , const DrawModes::DrawMode& _drawMode , const Material* _mat )
{
  if (points_.empty())
    return;

  // setup RenderObject
  RenderObject ro;
  ro.debugName = "PointNode";


  // define vertex format
  // buffer address may change so do this every frame

  vertexDecl_.clear();
  vertexDecl_.addElement(GL_DOUBLE, 3, VERTEX_USAGE_POSITION, &points_[0]);
  if (!normals_.empty())
    vertexDecl_.addElement(GL_DOUBLE, 3, VERTEX_USAGE_NORMAL, &normals_[0]);
  if (!colors_.empty())
    vertexDecl_.addElement(GL_FLOAT, 4, VERTEX_USAGE_COLOR, &colors_[0]);

  vertexDecl_.setVertexStride(0);

  ro.vertexDecl = &vertexDecl_;

  for (unsigned int i = 0; i < _drawMode.getNumLayers(); ++i)
  {
    const DrawModes::DrawModeProperties* props = _drawMode.getLayer(i);

    if (props->primitive() == DrawModes::PRIMITIVE_POINT)
    {
      // reset renderobject
      ro.initFromState(&_state);
      ro.setMaterial(_mat);
      ro.setupShaderGenFromDrawmode(props);

      ro.priority = 0;
      ro.depthTest = true;
      ro.depthWrite = true;
      ro.depthFunc = GL_LESS;

      // use pointsize shader
      QString geomTemplate = ShaderProgGenerator::getShaderDir();
      geomTemplate += "PointSize/geometry.tpl";

      QString fragTemplate = ShaderProgGenerator::getShaderDir();
      fragTemplate += "PointSize/fragment.tpl";

      ro.shaderDesc.geometryTemplateFile = geomTemplate;
      ro.shaderDesc.fragmentTemplateFile = fragTemplate;

      // shader uniforms
      ro.setUniform("screenSize", Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));
      ro.setUniform("pointSize", _mat->pointSize());

      ro.glDrawArrays(GL_POINTS, 0, (GLsizei)points_.size());
      _renderer->addRenderObject(&ro);
    }
  }
}


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
