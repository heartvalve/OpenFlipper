#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

#include <ACG/GL/gl.hh>

#include "Renderer.hh"

#include <OpenFlipper/common/ViewObjectMarker.hh>

using namespace ACG;


static const char* customAnaglyphProg = {
  "!!ARBfp1.0"
  "TEMP left, right, lmul, rmul;"
  "TEX left, fragment.texcoord[0], texture[0], RECT;"
  "TEX right, fragment.texcoord[0], texture[1], RECT;"
  "DP3 lmul.r, left, program.env[0];"
  "DP3 lmul.g, left, program.env[1];"
  "DP3 lmul.b, left, program.env[2];"
  "DP3 rmul.r, right, program.env[3];"
  "DP3 rmul.g, right, program.env[4];"
  "DP3 rmul.b, right, program.env[5];"
  "ADD result.color, lmul, rmul;"
  "END"
};

// =================================================

Renderer::Renderer()
{
  // initialize custom anaglyph stereo
  agTexWidth_ = 0;
  agTexHeight_ = 0;
  agTexture_[0] = 0;
  agTexture_[1] = 0;
  agProgram_ = 0;
  customAnaglyphSupported_ = false;
}


Renderer::~Renderer()
{
  finiCustomAnaglyphStereo ();
}


void Renderer::initializePlugin()
{
  customAnaglyphSupported_ = ACG::checkExtensionSupported("GL_ARB_fragment_program") &&
                             (ACG::checkExtensionSupported("GL_ARB_texture_rectangle") ||
                              ACG::checkExtensionSupported("GL_EXT_texture_rectangle") ||
                              ACG::checkExtensionSupported("GL_NV_texture_rectangle"));
}


void Renderer::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{

  if ( _properties.stereo())
    drawScene_stereo(_glState,_properties);
  else
    drawScene_mono(_glState,_properties);

}

QString Renderer::checkOpenGL()
{
  // This renderer plugin should run on almost any old style hardware
  return QString("");
}

//-----------------------------------------------------------------------------

void
Renderer::drawScene_stereo(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  if (OpenFlipper::Options::stereoMode () == OpenFlipper::Options::OpenGL && OpenFlipper::Options::glStereo ())
  {
    drawScene_glStereo (_glState,_properties);;
    return;
  }
  else if (OpenFlipper::Options::stereoMode () == OpenFlipper::Options::AnaglyphCustom && customAnaglyphSupported_)
  {
    drawScene_customAnaglyphStereo (_glState,_properties);;

    // if somthing went wrong, fallback to normal anaglyph
    if (customAnaglyphSupported_)
      return;
  }

  drawScene_anaglyphStereo(_glState,_properties);;
}


//-----------------------------------------------------------------------------
void Renderer::drawScene_mono(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  ACG::SceneGraph::BaseNode* root = PluginFunctions::getSceneGraphRootNode();

  if (root) {
    ViewObjectMarker *oM = _properties.objectMarker();
    GLuint refBits = 0;
    QSet<GLuint> references;

    if (oM)
    {
      glClear (GL_STENCIL_BUFFER_BIT);
      ACG::GLState::enable (GL_STENCIL_TEST);
      glStencilOp (GL_KEEP, GL_KEEP, GL_ZERO);
      glStencilFunc (GL_ALWAYS, 0, ~0);

      for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DATA_ALL) ;
          o_it != PluginFunctions::objectsEnd(); ++o_it)
      {
        bool ok;
        GLuint ref;

        ok = oM->stencilRefForObject(*o_it, ref);

        if (ok)
        {
          o_it->stencilRefNode ()->setReference (ref);
          o_it->stencilRefNode ()->show ();
          refBits |= ref;
          references << ref;
        }
        else
          o_it->stencilRefNode ()->hide ();
      }
    }

    ACG::SceneGraph::DrawAction action( _properties.drawMode(), *_glState , false);
    ACG::SceneGraph::traverse_multipass(root, action, *_glState, _properties.drawMode() );

    // Second pass for blending
    ACG::SceneGraph::DrawAction action1(_properties.drawMode(), *_glState, true);
    ACG::SceneGraph::traverse_multipass(root, action1, *_glState, _properties.drawMode());

    if (oM)
    {
      if (oM->type() == ViewObjectMarker::PerBit)
      {
        references.clear ();
        for (unsigned int i = 0; i < sizeof (GLuint) * 8; i++)
          if (refBits & (1 << i))
            references << (1 << i);
      }

      glPushAttrib(GL_ALL_ATTRIB_BITS);

      ACG::GLState::enable(GL_BLEND);
      ACG::GLState::disable(GL_DEPTH_TEST);
      ACG::GLState::disable(GL_LIGHTING);
      ACG::GLState::disable(GL_DITHER);

      int vp_l, vp_b, vp_w, vp_h;
      _glState->get_viewport (vp_l, vp_b, vp_w, vp_h);

      glMatrixMode(GL_PROJECTION);
      glPushMatrix ();
      glLoadIdentity();
      glOrtho(0, vp_w, vp_h, 0, 0, 1.0);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix ();
      glLoadIdentity();

      glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

      foreach (unsigned int ref, references)
      {
        bool ok;
        GLenum sfactor;
        GLenum dfactor;
        ACG::Vec4f color;
        unsigned int mask = ~0;

        if (oM->type() == ViewObjectMarker::PerBit)
        {
          ok = oM->blendForStencilRefBit (ref, sfactor, dfactor, color);
          mask = ref;
        }
        else
          ok = oM->blendForStencilRefNumber (ref, sfactor, dfactor, color);

        if (!ok)
          continue;

        glStencilFunc (GL_EQUAL, ref, mask);

        ACG::GLState::blendFunc (sfactor, dfactor);
        glColor4f (color[0], color [1], color [2], color[3]);

        glBegin (GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(0, vp_h);
        glVertex2i(vp_w, vp_h);
        glVertex2i(vp_w, 0);
        glEnd ();

      }

      glMatrixMode(GL_PROJECTION);
      glPopMatrix ();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix ();

      glPopAttrib ();
      ACG::GLState::disable (GL_STENCIL_TEST);
    }


  }

  if ( _properties.cursorPainter() && _properties.cursorPainter()->enabled () && _properties.cursorPositionValid() )
  {
    _glState->push_modelview_matrix ();
    // reset view transformation
    _glState->reset_modelview ();
    // translate cursor position to 0,0
    _glState->translate (_properties.cursorPoint3D() );
    // paint cursor
    _properties.cursorPainter()->paintCursor (_glState);
    _glState->pop_modelview_matrix ();
  }

}

//-----------------------------------------------------------------------------


void
Renderer::drawScene_glStereo(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  double l, r, t, b, w, h, a, radians, wd2, ndfl, zerop, xrange;

  w = _glState->viewport_width();
  h = _glState->viewport_height();
  a = w / h;

  double fovy = OpenFlipperSettings().value("Core/Projection/FOVY",45.0).toDouble();
  radians = fovy * 0.5 / 180.0 * M_PI;
  wd2     = _properties.nearPlane() * tan(radians);
  zerop   = _properties.nearPlane() + ((_properties.farPlane() - _properties.nearPlane()) * OpenFlipperSettings().value("Core/Stereo/FocalDistance",0.5).toDouble() );
  ndfl    = _properties.nearPlane() / zerop ;
  xrange  = a * wd2 * 2 * zerop / _properties.nearPlane();

  l = -a*wd2;
  r =  a*wd2;
  t =  wd2;
  b = -wd2;

  double offset  = 0.5 * OpenFlipperSettings().value("Core/Stereo/EyeDistance",0.07).toDouble()  * xrange;
  double offset2 = offset * ndfl;

  // left eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l+offset2, r+offset2, b, t, _properties.nearPlane(), _properties.farPlane());
  glTranslatef(+offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  ACG::GLState::drawBuffer(GL_BACK_LEFT);
  _glState->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono(_glState,_properties);


  // right eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l-offset2, r-offset2, b, t, _properties.nearPlane(), _properties.farPlane());
  glTranslatef(-offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  ACG::GLState::drawBuffer(GL_BACK_RIGHT);
  _glState->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono(_glState,_properties);
  ACG::GLState::drawBuffer(GL_BACK);
}



//-----------------------------------------------------------------------------


void
Renderer::drawScene_anaglyphStereo(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
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

  // left eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l+offset2, r+offset2, b, t, _properties.nearPlane(), _properties.farPlane());
  glTranslatef(offset, 0.0, 0.0);

  glMatrixMode(GL_MODELVIEW);
  _glState->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);

  // draw red channel for left eye
  glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);
  drawScene_mono(_glState,_properties);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);


  // right eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l-offset2, r-offset2, b, t, _properties.nearPlane(), _properties.farPlane());
  glTranslatef(-offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glClear(GL_DEPTH_BUFFER_BIT);

  // draw green and blue channel for right eye
  glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE);
  drawScene_mono(_glState,_properties);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

}

//-----------------------------------------------------------------------------

void
Renderer::updateCustomAnaglyphStereo(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  if (!customAnaglyphSupported_)
    return;

  if ( !ACG::checkExtensionSupported("GL_ARB_texture_rectangle") ) {
    std::cerr << "GL_ARB_texture_rectangle not supported! " << std::endl;
    customAnaglyphSupported_ = false;
    return;
  }

  if (!agProgram_)
  {
    GLint errorPos;

    glGenProgramsARB (1, &agProgram_);

    glGetError ();
    glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, agProgram_);
    glProgramStringARB (GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
                        strlen (customAnaglyphProg), customAnaglyphProg);

    glGetIntegerv (GL_PROGRAM_ERROR_POSITION_ARB, &errorPos);
    if (glGetError () != GL_NO_ERROR || errorPos != -1)
    {
      printf("Error loading program %d %s\n",errorPos, glGetString(GL_PROGRAM_ERROR_STRING_ARB));
      glDeleteProgramsARB (1, &agProgram_);
      customAnaglyphSupported_ = false;
      return;
    }
  }

  if (!agTexture_[0])
    glGenTextures (2, agTexture_);

  if (!agTexture_[0])
  {
    finiCustomAnaglyphStereo();
    customAnaglyphSupported_ = false;
    return;
  }

  if (_glState->viewport_width () != agTexWidth_ ||
      _glState->viewport_height () != agTexHeight_)
  {
    ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[0]);
    glTexImage2D (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, _glState->viewport_width (),
                  _glState->viewport_height (), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[1]);
    glTexImage2D (GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, _glState->viewport_width (),
                  _glState->viewport_height (), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);

    agTexWidth_ = _glState->viewport_width ();
    agTexHeight_ = _glState->viewport_height ();
  }
}

//-----------------------------------------------------------------------------

void
Renderer::finiCustomAnaglyphStereo()
{
  if (!customAnaglyphSupported_)
    return;

  if (agProgram_)
    glDeleteProgramsARB (1, &agProgram_);

  if (agTexture_[0])
    glDeleteTextures (2, agTexture_);
}

//-----------------------------------------------------------------------------

void
Renderer::drawScene_customAnaglyphStereo(ACG::GLState* _glState, Viewer::ViewerProperties& _properties)
{
  updateCustomAnaglyphStereo(_glState,_properties);

  if (!customAnaglyphSupported_)
    return;

  double l, r, t, b, w, h, a, radians, wd2, ndfl, zerop, xrange;

  w = _glState->viewport_width();
  h = _glState->viewport_height();
  a = w / h;

  double fovy = OpenFlipperSettings().value("Core/Projection/FOVY",45.0).toDouble();
  radians = fovy * 0.5 / 180.0 * M_PI;
  wd2     = _properties.nearPlane() * tan(radians);
  zerop   = _properties.nearPlane() + ((_properties.farPlane() - _properties.nearPlane()) * OpenFlipperSettings().value("Core/Stereo/FocalDistance",0.5).toDouble() );
  ndfl    = _properties.nearPlane() / zerop ;
  xrange  = a * wd2 * 2 * zerop / _properties.nearPlane();

  l = -a*wd2;
  r =  a*wd2;
  t =  wd2;
  b = -wd2;

  double offset  = 0.5 * OpenFlipperSettings().value("Core/Stereo/EyeDistance",0.07).toDouble() * xrange;
  double offset2 = offset * ndfl;

  int vp_l, vp_b, vp_w, vp_h;
  _glState->get_viewport (vp_l, vp_b, vp_w, vp_h);

  std::vector<float> le = OpenFlipper::Options::anaglyphLeftEyeColorMatrix();
  std::vector<float> re = OpenFlipper::Options::anaglyphRightEyeColorMatrix();

  // left eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l+offset2, r+offset2, b, t, _properties.nearPlane(), _properties.farPlane());
  glTranslatef(offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  _glState->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono(_glState,_properties);

  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[0]);
  glCopyTexSubImage2D (GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, vp_l, vp_b, vp_w, vp_h);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);

  // right eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l-offset2, r-offset2, b, t, _properties.nearPlane(), _properties.farPlane());
  glTranslatef(-offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  _glState->clearBuffers ();
  glClear(GL_DEPTH_BUFFER_BIT);
  drawScene_mono(_glState,_properties);

  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[1]);
  glCopyTexSubImage2D (GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, vp_l, vp_b, vp_w, vp_h);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);

  ACG::GLState::activeTexture (GL_TEXTURE0);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[0]);
  ACG::GLState::enable (GL_TEXTURE_RECTANGLE_ARB);

  ACG::GLState::activeTexture (GL_TEXTURE1);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, agTexture_[1]);
  ACG::GLState::enable (GL_TEXTURE_RECTANGLE_ARB);

  ACG::GLState::enable (GL_FRAGMENT_PROGRAM_ARB);
  glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, agProgram_);

  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 0, le[0], le[3], le[6], 0.0);
  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 1, le[1], le[4], le[7], 0.0);
  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 2, le[2], le[5], le[8], 0.0);

  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 3, re[0], re[3], re[6], 0.0);
  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 4, re[1], re[4], re[7], 0.0);
  glProgramEnvParameter4fARB (GL_FRAGMENT_PROGRAM_ARB, 5, re[2], re[5], re[8], 0.0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, vp_w, vp_h, 0, 0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  ACG::GLState::disable (GL_DEPTH_TEST);

  glBegin (GL_QUADS);
  glMultiTexCoord2f (GL_TEXTURE0, 0, vp_h);
  glMultiTexCoord2f (GL_TEXTURE1, 0, vp_h);
  glVertex2i(0, 0);
  glMultiTexCoord2f (GL_TEXTURE0, 0, 0);
  glMultiTexCoord2f (GL_TEXTURE1, 0, 0);
  glVertex2i(0, vp_h);
  glMultiTexCoord2f (GL_TEXTURE0, vp_w, 0);
  glMultiTexCoord2f (GL_TEXTURE1, vp_w, 0);
  glVertex2i(vp_w, vp_h);
  glMultiTexCoord2f (GL_TEXTURE0, vp_w, vp_h);
  glMultiTexCoord2f (GL_TEXTURE1, vp_w, vp_h);
  glVertex2i(vp_w, 0);
  glEnd ();

  ACG::GLState::enable (GL_DEPTH_TEST);

  glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, 0);
  ACG::GLState::disable (GL_FRAGMENT_PROGRAM_ARB);

  ACG::GLState::activeTexture (GL_TEXTURE1);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);
  ACG::GLState::disable (GL_TEXTURE_RECTANGLE_ARB);

  ACG::GLState::activeTexture (GL_TEXTURE0);
  ACG::GLState::bindTexture (GL_TEXTURE_RECTANGLE_ARB, 0);
  ACG::GLState::disable (GL_TEXTURE_RECTANGLE_ARB);

}

Q_EXPORT_PLUGIN2( classicalrenderer , Renderer );
