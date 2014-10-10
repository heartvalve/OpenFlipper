
#include "Renderer.hh"


#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ACG/GL/ShaderCache.hh>

#undef QT_NO_OPENGL
#include <QGLFormat>

#include <QMenu>

// =================================================

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}


void Renderer::initializePlugin()
{
  ACG::ShaderProgGenerator::setShaderDir(OpenFlipper::Options::shaderDirStr());
}



void Renderer::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  // collect renderobjects + prepare OpenGL state
  prepareRenderingPipeline(_glState, _properties.drawMode(), PluginFunctions::getSceneGraphRootNode());

  // render every object
  for (int i = 0; i < getNumRenderObjects(); ++i)
    renderObject( getRenderObject(i) );

  // restore common opengl state
  // log window remains hidden otherwise
  finishRenderingPipeline();

//  dumpRenderObjectsToFile("../../dump_ro.txt", &sortedObjects_[0]);
}


QString Renderer::checkOpenGL()
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
  Q_EXPORT_PLUGIN2( shaderrenderer , Renderer );
#endif


