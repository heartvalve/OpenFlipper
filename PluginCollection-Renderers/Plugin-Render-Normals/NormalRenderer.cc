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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include "NormalRenderer.hh"

#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ACG/GL/ShaderCache.hh>
#undef QT_NO_OPENGL
#include <QGLFormat>
#define QT_NO_OPENGL


// =================================================

class NormalFragmentModifier : public ACG::ShaderModifier
{
public:

  void modifyVertexIO( ACG::ShaderGenerator* _shader )
  {
    _shader->addOutput("vec3 fragNormal");
  }

  void modifyVertexEndCode( QStringList* _code )
  {
    _code->push_back("#ifdef SG_NORMALS");
    _code->push_back("fragNormal = vec3(inNormal);");
    _code->push_back("#endif ");
  }

  void modifyFragmentIO(ACG::ShaderGenerator* _shader)
  {
    _shader->addInput("vec3 fragNormal");
  }

  void modifyFragmentEndCode(QStringList* _code)
  {
    _code->push_back("#ifdef SG_NORMALS");
    _code->push_back("outFragment = vec4(fragNormal.x/2.0+0.5,fragNormal.y/2.0+0.5,fragNormal.z/2.0+0.5,1.0);");
    _code->push_back("#endif ");
  }

  static NormalFragmentModifier instance;
};


NormalFragmentModifier NormalFragmentModifier::instance;

// =================================================

NormalRenderer::NormalRenderer()
{
  ACG::ShaderProgGenerator::registerModifier(&NormalFragmentModifier::instance);
}


NormalRenderer::~NormalRenderer()
{
}


void NormalRenderer::initializePlugin()
{
  ACG::ShaderProgGenerator::setShaderDir(OpenFlipper::Options::shaderDirStr());
}

QString NormalRenderer::renderObjectsInfo(bool _outputShaderInfo) {
  std::vector<ACG::ShaderModifier*> modifiers;
  modifiers.push_back(&NormalFragmentModifier::instance);
  return dumpCurrentRenderObjectsToString(&sortedObjects_[0], _outputShaderInfo, &modifiers);
}

void NormalRenderer::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  // collect renderobjects + prepare OpenGL state
  prepareRenderingPipeline(_glState, _properties.drawMode(), PluginFunctions::getSceneGraphRootNode());


  // clear back buffer
  ACG::Vec4f clearColor = _properties.backgroundColor();
  glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  // render every object
  for (int i = 0; i < getNumRenderObjects(); ++i) {

    // Take original shader and modify the output to take only the normal as the color
    GLSL::Program* prog = ACG::ShaderCache::getInstance()->getProgram(&sortedObjects_[i]->shaderDesc, NormalFragmentModifier::instance);
    renderObject(sortedObjects_[i],prog);
  }

  // restore common opengl state
  // log window remains hidden otherwise
  finishRenderingPipeline();

//  dumpRenderObjectsToFile("../../dump_ro.txt", &sortedObjects_[0]);
}

QString NormalRenderer::checkOpenGL()
{
  // Get version and check
  QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
  if ( !flags.testFlag(QGLFormat::OpenGL_Version_3_2) )
    return QString("Insufficient OpenGL Version! OpenGL 3.2 or higher required");

  // Check extensions
  QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));
  QString missing("");
  if ( !glExtensions.contains("GL_ARB_vertex_buffer_object") )
    missing += "GL_ARB_vertex_buffer_object extension missing\n";

#ifndef __APPLE__
  if ( !glExtensions.contains("GL_ARB_vertex_program") )
    missing += "GL_ARB_vertex_program extension missing\n";
#endif

  return missing;
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( normalrenderer , NormalRenderer );
#endif


