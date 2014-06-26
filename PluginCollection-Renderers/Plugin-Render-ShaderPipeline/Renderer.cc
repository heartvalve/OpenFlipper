
#include "Renderer.hh"


#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <QGLFormat>


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

  if (!_properties.stereo())
  {
    for (int i = 0; i < getNumRenderObjects(); ++i)
      renderObject(sortedObjects_[i]);
  }
  else
  {
    ACG::GLMatrixd projLeft, projRight;
    computeProjStereo(_glState, _properties, &projLeft, &projRight);

    // draw red channel for left eye

    for (int i = 0; i < getNumRenderObjects(); ++i)
    {
      ACG::GLMatrixd prev = sortedObjects_[i]->proj;

      sortedObjects_[i]->proj = projLeft;
      sortedObjects_[i]->glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
      renderObject(sortedObjects_[i]);
     
      sortedObjects_[i]->proj = prev;
    }

    // draw green and blue channel for right eye

    glClear(GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < getNumRenderObjects(); ++i)
    {
      sortedObjects_[i]->proj = projRight;
      sortedObjects_[i]->glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
      renderObject(sortedObjects_[i]);
    }
  }


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

void Renderer::computeProjStereo( ACG::GLState* _glState, Viewer::ViewerProperties& _properties, ACG::GLMatrixd* _outLeft, ACG::GLMatrixd* _outRight )
{
  double l, r, t, b, w, h, a, radians, wd2, ndfl, zerop, xrange;

  w = _glState->viewport_width();
  h = _glState->viewport_height();
  a = w / h;

  double fovy = OpenFlipperSettings().value("Core/Projection/FOVY", 45.0).toDouble();
  radians = fovy * 0.5 / 180.0 * M_PI;
  wd2     = _properties.nearPlane() * tan(radians);
  zerop   = _properties.nearPlane() + ((_properties.farPlane() - _properties.nearPlane()) * OpenFlipperSettings().value("Core/Stereo/FocalDistance", 0.5).toDouble() );
  ndfl    = _properties.nearPlane() / zerop ;
  xrange  = a * wd2 * 2 * zerop / _properties.nearPlane();

  l = -a*wd2;
  r =  a*wd2;
  t =  wd2;
  b = -wd2;

  double offset  = 0.5 * OpenFlipperSettings().value("Core/Stereo/EyeDistance", 0.07).toDouble() * xrange;
  double offset2 = offset * ndfl;

  if (_outLeft)
  {
    _outLeft->identity();
    _outLeft->frustum(l+offset2, r+offset2, b, t, _properties.nearPlane(), _properties.farPlane());
    _outLeft->translate(offset, 0.0, 0.0);
  }

  if (_outRight)
  {
    _outRight->identity();
    _outRight->frustum(l-offset2, r-offset2, b, t, _properties.nearPlane(), _properties.farPlane());
    _outRight->translate(-offset, 0.0, 0.0);
  }
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( shaderrenderer , Renderer );
#endif


