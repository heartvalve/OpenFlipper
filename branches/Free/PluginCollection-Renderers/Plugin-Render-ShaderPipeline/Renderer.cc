
#include "Renderer.hh"


#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#undef QT_NO_OPENGL
#include <QGLFormat>
#define QT_NO_OPENGL


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


  // clear back buffer
  ACG::Vec4f clearColor = _properties.backgroundColor();
  glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  // render every object

  for (int i = 0; i < getNumRenderObjects(); ++i)
    renderObject(sortedObjects_[i]);


  // restore common opengl state
  // log window remains hidden otherwise
  finishRenderingPipeline();

//  dumpRenderObjectsToFile("../../dump_ro.txt", &sortedObjects_[0]);
}


QString Renderer::checkOpenGL()
{
  // Get version and check
  QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
  if ( ! flags.testFlag(QGLFormat::OpenGL_Version_2_0) )
    return QString("Insufficient OpenGL Version! OpenGL 2.0 or higher required");

  return "";
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( shaderrenderer , Renderer );
#endif


