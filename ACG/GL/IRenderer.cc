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

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <QTextStream>

#include <ACG/GL/gl.hh>

#include <ACG/GL/IRenderer.hh>

#include <ACG/GL/VertexDeclaration.hh>
#include <ACG/GL/GLState.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/Scenegraph/MaterialNode.hh>

namespace ACG
{

void RenderObject::initFromState( GLState* _glState )
{
  culling = true;
  blending = false;
  depthTest = true;
  depthWrite = true;
  alphaTest = false;

  colorWriteMask[0] = colorWriteMask[1] = colorWriteMask[2] = colorWriteMask[3] = 1;

  fillMode = GL_FILL;

  depthRange = Vec2f(0.0f, 1.0f);
  depthFunc = GL_LESS;

  blendSrc = GL_ONE;
  blendDest = GL_ZERO;

  alpha = 1.0f;

  if (_glState)
  {
    modelview = _glState->modelview();
    proj = _glState->projection();

    culling =_glState->isStateEnabled(GL_CULL_FACE);
    blending = _glState->isStateEnabled(GL_BLEND);
    depthTest = _glState->isStateEnabled(GL_DEPTH_TEST);

//    shadeModel = _glState->getShadeModel();

    _glState->getBlendFunc(&blendSrc, &blendDest);

    GLclampd zn, zf;
    _glState->getDepthRange(&zn, &zf);
    depthRange = Vec2f(zn, zf);

    depthFunc = _glState->depthFunc();


    alphaTest = _glState->isStateEnabled(GL_ALPHA_TEST);

    for (int i = 0; i < 3; ++i)
    {
      diffuse[i] = _glState->diffuse_color()[i];
      ambient[i] = _glState->ambient_color()[i];
      specular[i] = _glState->specular_color()[i];
      emissive[i] = _glState->base_color()[i];
    }
    shininess = _glState->shininess();
  }
}

void RenderObject::setupShaderGenFromDrawmode( const SceneGraph::DrawModes::DrawModeProperties* _props )
{
  if (_props)
  {
    shaderDesc.vertexColors = _props->colored();
    shaderDesc.textured = _props->textured();
    shaderDesc.numLights = _props->lighting() ? 0 : -1;

    switch (_props->lightStage()) {
      case SceneGraph::DrawModes::LIGHTSTAGE_SMOOTH:
        shaderDesc.shadeMode = SG_SHADE_GOURAUD;
        break;
      case SceneGraph::DrawModes::LIGHTSTAGE_PHONG:
        shaderDesc.shadeMode = SG_SHADE_PHONG;
        break;
      case SceneGraph::DrawModes::LIGHTSTAGE_UNLIT:
        shaderDesc.shadeMode = SG_SHADE_UNLIT;
        break;
      default:
        break;
    }

    if (_props->flatShaded())
      shaderDesc.shadeMode = SG_SHADE_FLAT;

    if (_props->primitive() == SceneGraph::DrawModes::PRIMITIVE_WIREFRAME ||
        _props->primitive() == SceneGraph::DrawModes::PRIMITIVE_HIDDENLINE ||
        _props->primitive() == SceneGraph::DrawModes::PRIMITIVE_EDGE ||
        _props->primitive() == SceneGraph::DrawModes::PRIMITIVE_HALFEDGE)
      shaderDesc.shadeMode = SG_SHADE_UNLIT;
  }
}

void RenderObject::setMaterial( const SceneGraph::Material* _mat )
{
  for (int i = 0; i < 3; ++i)
  {
    diffuse[i] = _mat->diffuseColor()[i];
    ambient[i] = _mat->ambientColor()[i];
    specular[i] = _mat->specularColor()[i];
    emissive[i] = _mat->baseColor()[i];
  }
  shininess = _mat->shininess();
  alpha = _mat->diffuseColor()[3];
}


RenderObject::RenderObject()
{
  memset(this, 0, sizeof(RenderObject));
}


QString RenderObject::toString() const
{
  // several mappings: (int)GLEnum -> string

  const char* primitiveString[] = 
  {
    "GL_POINTS",
    "GL_LINES",
    "GL_LINE_LOOP",
    "GL_LINE_STRIP",
    "GL_TRIANGLES",
    "GL_TRIANGLE_STRIP",
    "GL_TRIANGLE_FAN",
    "GL_QUADS",
    "GL_QUAD_STRIP",
    "GL_POLYGON"
  };

  const char* fillModeString[] = 
  {
    "GL_POINT",
    "GL_LINE",
    "GL_FILL"
  };

  const char* depthFunString[] =
  {
    "GL_NEVER",
    "GL_LESS",
    "GL_EQUAL",
    "GL_LEQUAL",
    "GL_GREATER",
    "GL_NOTEQUAL",
    "GL_GEQUAL",
    "GL_ALWAYS"
  };

  QString result;
  QTextStream resultStrm(&result);

  resultStrm << "name: " << debugName
             << "\ndebugID: " << debugID
             << "\npriority: " << priority

             << "\nprimitiveMode: " << (primitiveMode <= GL_POLYGON ? primitiveString[primitiveMode] : "undefined")

             << "\nfillMode: " << fillModeString[fillMode - GL_POINT]
             << "\nnumIndices: " << numIndices
             << "\nindexOffset: " << indexOffset;


  resultStrm << "\n" << shaderDesc.toString();


  resultStrm << "\nculling: " << culling
    << "\nblending: " << blending
    << "\nalphaTest: " << alphaTest;


  resultStrm << "\ndepthTest: " << depthTest
    << "\ndepthWrite: " << depthWrite
    << "\ndepthFunc: " << depthFunString[depthFunc - GL_NEVER]
    << "\ndepthRange: [" << depthRange[0] << ", " << depthRange[1] << "]"
    << "\ncolorWriteMask: " << colorWriteMask[0] << ", " << colorWriteMask[1] << ", "<< colorWriteMask[2] << ", "<< colorWriteMask[2];


  resultStrm << "\ndiffuse: [" << diffuse[0] << ", " << diffuse[1] << ", " << diffuse[2] << "]";
  resultStrm << "\nambient: [" << ambient[0] << ", " << ambient[1] << ", " << ambient[2] << "]";
  resultStrm << "\nspecular: [" << specular[0] << ", " << specular[1] << ", " << specular[2] << "]";
  resultStrm << "\nemissive: [" << emissive[0] << ", " << emissive[1] << ", " << emissive[2] << "]";

  resultStrm << "\nshininess: " << shininess;
  resultStrm << "\nalpha: " << alpha;

  resultStrm << "\ninternalFlags: " << internalFlags_;

  if (vertexDecl)
    resultStrm << "\n" << vertexDecl->toString();


  return result;
}



} // namespace ACG end

