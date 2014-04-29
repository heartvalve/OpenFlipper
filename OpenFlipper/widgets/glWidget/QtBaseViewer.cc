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




//=============================================================================
//
//  CLASS glViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtBaseViewer.hh"
#include "QtGLViewerLayout.hh"
#include "CursorPainter.hh"

#include <ACG/QtWidgets/QtWheel.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/Scenegraph/CoordsysNode.hh>
#include <ACG/Scenegraph/SceneGraphAnalysis.hh>
#include <ACG/GL/gl.hh>
#include <ACG/GL/GLError.hh>
#include <ACG/GL/FBO.hh>
#include <ACG/GL/IRenderer.hh>

#include <iostream>
#include <string>
#include <cassert>

#include <QMimeData>
#include <QToolButton>
#include <QFrame>

#include <QClipboard>
#include <QApplication>
#include <QSplitter>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QImage>
#include <QColorDialog>
#include <QToolTip>
#include <QTextStream>
#include <QDateTime>
#include <QTimer>

#include <QDesktopWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QGridLayout>
#include <QContextMenuEvent>
#include <QWheelEvent>
#include <QDropEvent>
#include <QPixmap>
#include <QMenu>
#include <QVariant>
#include <QButtonGroup>
#include <QToolBar>

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <QGraphicsProxyWidget>
#include <QPainter>
#include <QPaintEngine>

#include <QImageWriter>

#if QT_VERSION < 0x050000
  #include <QGLFramebufferObject>
#else // QT_VERSION > 0x050000
  #undef QT_NO_OPENGL
  #include <QOpenGLFramebufferObject>
  #define QT_NO_OPENGL
#endif //QT_VERSION < 0x050000

#ifdef max
#  undef max
#endif

#ifdef min
#  undef min
#endif

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/ViewObjectMarker.hh>

#include <OpenFlipper/common/GlobalOptions.hh>

#include <QGLFramebufferObject>

#include <OpenFlipper/common/RendererInfo.hh>
#include <OpenFlipper/BasePlugin/PostProcessorInterface.hh>

//== NAMESPACES ===============================================================



//== IMPLEMENTATION ==========================================================

static const char          COPY_PASTE_VIEW_START_STRING[] =
  "ACG::QtWidgets::QGLViewerWidget encoded view";

//== IMPLEMENTATION ==========================================================


glViewer::glViewer( QGraphicsScene* _scene,
                    QGLWidget* _glWidget,
                    Viewer::ViewerProperties& _properties,
                    QGraphicsWidget* _parent) :
  QGraphicsWidget(_parent),
  glareaGrabbed_(false),
  projectionUpdateLocked_(false),
  glScene_(_scene),
  glWidget_(_glWidget),
  pickCache_(0),
  updatePickCache_(true),
  pickCacheSupported_(true),
  constrainedRotationAxis_(std::numeric_limits<double>::quiet_NaN(), 0, 0),
  clickEvent_(QEvent::MouseButtonPress, QPoint (), Qt::NoButton, Qt::NoButton, Qt::NoModifier),
  properties_(_properties),
  glstate_(0),
  initialized_(false),
  flyAnimationPerspective_(0),
  flyAnimationOrthogonal_(0),
  flyAngle_(0.0),
  currentAnimationPos_(0.0),
  flyMoveBack_(false)
{

  // widget stuff
  createWidgets();

  // bind GL context to GL state class
  glstate_ = new ACG::GLState();
  properties_.setglState( glstate_ );

  // state
  isRotating_       = false;
  lookAround_       = false;

  sceneGraphRoot_   = 0;

  normalsMode_      = NORMALIZE_NORMALS;
  projectionMode_   = PERSPECTIVE_PROJECTION;
  navigationMode_   = NORMAL_NAVIGATION;

  trackMouse_ = false;

  // Note: we start locked (initialization of updateLocked_)
  // will be unlocked in initializeGL()

  QSizePolicy sp = sizePolicy();
  sp.setHorizontalPolicy( QSizePolicy::Expanding );
  sp.setVerticalPolicy( QSizePolicy::Expanding );
  sp.setHorizontalStretch( 1 );
  sp.setVerticalStretch( 1 );
  setSizePolicy( sp );

  redrawTime_.start ();

  // timer for animation
  timer_ = new QTimer( this );
  connect( timer_, SIGNAL(timeout()), this, SLOT( slotAnimation()) );

  fovyModifier_ = 1.0;

  allowRotation_ = true;


  connect( &properties_,SIGNAL(updated()), this, SLOT( slotPropertiesUpdated() ) );


  //check for updated properties once
  slotPropertiesUpdated();

  setAcceptDrops(true);

  setHome();

  clickTimer_.setSingleShot (true);
  connect (&clickTimer_, SIGNAL(timeout ()), this, SLOT(slotClickTimeout ()));
}


//-----------------------------------------------------------------------------


glViewer::~glViewer()
{
  delete glstate_;
}


//-----------------------------------------------------------------------------


//QSizeF
//glViewer::sizeHint(Qt::SizeHint which, const QSizeF & constraint) const
//{
//  return QSizeF( 600, 600 );
//}

//-----------------------------------------------------------------------------


void glViewer::makeCurrent() {
  glWidget_->makeCurrent();
}

void glViewer::swapBuffers() {
  glWidget_->swapBuffers();
}


//-----------------------------------------------------------------------------

void glViewer::sceneGraph(ACG::SceneGraph::BaseNode* _root,
                          unsigned int               _maxPasses,
                          ACG::Vec3d                 _bbmin,
                          ACG::Vec3d                 _bbmax,
                          const bool _resetTrackBall)
{
  sceneGraphRoot_ = _root;

  if (sceneGraphRoot_ )
  {

    // set max number of render pass
    glstate_->set_max_render_passes(_maxPasses);
    
    if ( ( _bbmin[0] > _bbmax[0] ) ||
         ( _bbmin[1] > _bbmax[1] ) ||
         ( _bbmin[2] > _bbmax[2] )   ) {
      
      // Invalid bounding box, try to recover
      setScenePos( properties_.sceneCenter() , properties_.sceneRadius(), _resetTrackBall );
    
      // Update bounding box to match the scene geometry after recovery
      _bbmin = ACG::Vec3d(-1.0,-1.0,-1.0);
      _bbmax = ACG::Vec3d( 1.0, 1.0, 1.0);
    } else {
      
      // For very small scenes, we set the scene radius to 0.1
      // otherwise we take the real radius
      if ( ( _bbmax - _bbmin ).max() < OpenFlipperSettings().value("Core/Gui/glViewer/minimalSceneSize",0.1).toDouble() )  {
        setScenePos( ( _bbmin + _bbmax )        * 0.5,
                     OpenFlipperSettings().value("Core/Gui/glViewer/minimalSceneSize",0.1).toDouble(),
                     _resetTrackBall);
                   
      } else {
        setScenePos( ( _bbmin + _bbmax )        * 0.5,
                     ( _bbmax - _bbmin ).norm() * 0.5,
                     _resetTrackBall); 
      }
                   
    }
                   
    // remember the new bounding box for the state
    glstate_->set_bounding_box(_bbmin,_bbmax);
    
  }
  

  updateGL();

  emit(signalSceneGraphChanged(sceneGraphRoot_));
}


//-----------------------------------------------------------------------------


void glViewer::trackMouse(bool _track)
{
  trackMouse_ = _track;
}


//-----------------------------------------------------------------------------

void glViewer::setCoordSysProjection(ProjectionMode _mode) {
  // Find coordsys node
  ACG::SceneGraph::BaseNode* node = 0;
  node = PluginFunctions::getSceneGraphRootNode()->find("Core Coordsys Node");

  // set the projection mode for the coordinate system node
  if (node != 0) {
    ACG::SceneGraph::CoordsysNode* cnode = dynamic_cast<ACG::SceneGraph::CoordsysNode*> (node);
    if (_mode ==  ORTHOGRAPHIC_PROJECTION) {
      cnode->setProjectionMode(ACG::SceneGraph::CoordsysNode::ORTHOGRAPHIC_PROJECTION);
    } else {
      cnode->setProjectionMode(ACG::SceneGraph::CoordsysNode::PERSPECTIVE_PROJECTION);
    }
  }
}


//-----------------------------------------------------------------------------


void glViewer::perspectiveProjection()
{
  projectionMode(PERSPECTIVE_PROJECTION);
  updateGL();
}


void glViewer::orthographicProjection()
{
  projectionMode(ORTHOGRAPHIC_PROJECTION);
  updateGL();
}


void glViewer::toggleProjectionMode()
{
  if (projectionMode_ == ORTHOGRAPHIC_PROJECTION)
    projectionMode(PERSPECTIVE_PROJECTION);
  else
    projectionMode(ORTHOGRAPHIC_PROJECTION);

  updateGL();
}


void glViewer::projectionMode(ProjectionMode _p)
{
  if ((projectionMode_ = _p) == ORTHOGRAPHIC_PROJECTION)
    emit projectionModeChanged( true );
  else
    emit projectionModeChanged( false );

  updateProjectionMatrix();

  setCoordSysProjection(_p);

  emit viewChanged();
}

void glViewer::toggleNavigationMode()
{
  if (navigationMode_ == NORMAL_NAVIGATION)
    navigationMode(FIRSTPERSON_NAVIGATION);
  else
    navigationMode(NORMAL_NAVIGATION);
}


void glViewer::navigationMode(NavigationMode _n)
{
  if ((navigationMode_ = _n) == NORMAL_NAVIGATION)
    emit navigationModeChanged( true );
  else
    emit navigationModeChanged( false );
}

void glViewer::setFOVY(double _fovy) {
    
  if(_fovy <= 0.0 || _fovy >= 180) {
    std::cerr << "Error: Minimum or maximum fovy angle exceeded!" << std::endl;
    return;
  }
  
  OpenFlipperSettings().setValue("Core/Projection/FOVY", _fovy);
  updateProjectionMatrix();
}

void glViewer::updateProjectionMatrix()
{
  if( projectionUpdateLocked_ )
    return;

  makeCurrent();

  glstate_->reset_projection();

  const double aspect = this->aspect_ratio();
  // In stereo mode we have to use a perspective matrix
  if ( projectionMode_ == PERSPECTIVE_PROJECTION)
  {

    // Get fovy
    const double fovy = this->field_of_view_vertical();

    glstate_->perspective(fovy, (GLdouble) aspect,
                          near_plane(), far_plane());
  }
  else
  {

    glstate_->ortho( -ortho_width(), ortho_width(),
                     -ortho_width() / aspect, ortho_width() / aspect,
                     near_plane(), far_plane() );
  }

}


//-----------------------------------------------------------------------------


void glViewer::setScenePos(const ACG::Vec3d& _center, double _radius, const bool _resetTrackBall)
{
  if(_resetTrackBall) {
    properties_.trackballCenter(_center);
  }

  properties_.sceneCenter(_center);

  properties_.sceneRadius(_radius);
  properties_.trackballRadius(_radius);

  ACG::Vec3d c = glstate_->modelview().transform_point(properties_.sceneCenter());

  double nearPlane = std::max(0.0001f * properties_.sceneRadius(),  -(c[2] + properties_.sceneRadius()));
  double farPlane  = std::max(0.0002f * properties_.sceneRadius(),  -(c[2] - properties_.sceneRadius()));

   // Safety check, if near < 0
   if ( nearPlane <= 0.0 ) {
     std::cerr << "Error in BaseViewer drawScene, nearplane <= 0.0" << std::endl;
     nearPlane = 0.000000000000001;
   }

  // Safety check. Make sure that they are in the right order
  if ( nearPlane > farPlane ) {
    std::cerr << "Error in BaseViewer setScenePos, Nearplane > Farplane" << std::endl;
    std::swap(nearPlane,farPlane);
  }

  properties_.setPlanes(nearPlane,farPlane);
  properties_.orthoWidth(_radius);

  updateProjectionMatrix();
  updateGL();

  emit viewChanged();
}


//----------------------------------------------------------------------------


void glViewer::viewingDirection( const ACG::Vec3d& _dir, const ACG::Vec3d& _up )
{
  // calc eye point for this direction
  ACG::Vec3d eye = properties_.sceneCenter() - _dir * ( 3.0 * properties_.sceneRadius());

  glstate_->reset_modelview();
  glstate_->lookAt((ACG::Vec3d)eye, (ACG::Vec3d)properties_.sceneCenter(), (ACG::Vec3d)_up);

  emit viewChanged();
}

//-----------------------------------------------------------------------------

void glViewer::lookAt(const ACG::Vec3d& _eye, const ACG::Vec3d& _center, const ACG::Vec3d& _up) {
    
    glstate_->reset_modelview();
    glstate_->lookAt(_eye, _center, _up);
    
    properties_.sceneCenter( _center );
    properties_.sceneRadius( (properties_.sceneCenter() - _eye).norm() );
    
    emit viewChanged();
}

//-----------------------------------------------------------------------------

void glViewer::normalsMode(NormalsMode _mode)
{
  makeCurrent();

  switch(normalsMode_ = _mode)
  {
    case DONT_TOUCH_NORMALS:
      ACG::GLState::disable(GL_NORMALIZE);
      break;

    case NORMALIZE_NORMALS:
      ACG::GLState::enable(GL_NORMALIZE);
      break;
  }

  updateGL();
}


//-----------------------------------------------------------------------------


void
glViewer::copyToImage( QImage& _image,
		       unsigned int _l, unsigned int _t,
		       unsigned int _w, unsigned int _h,
			     GLenum /* _buffer */ )
{

//    makeCurrent();

  _image = glWidget_->grabFrameBuffer(true).copy (_l, _t, _w, _h).convertToFormat (QImage::Format_RGB32);
}


//-----------------------------------------------------------------------------


void glViewer::updateGL()
{
  if (!properties_.updateLocked() && isVisible() )
  {
    updatePickCache_ = true;
    update();

    emit viewUpdated();
  }
}



//-----------------------------------------------------------------------------


void glViewer::drawScene()
{
  
  // Inside the glWidget rendering, the system should not send extra updates
  properties_.blockSignals(true);
  
  QTime  timer;
  timer.start();

  // *****************************************************************
  // Adjust clipping planes
  // *****************************************************************
  // Far plane
  ACG::Vec3d c = glstate_->modelview().transform_point(properties_.sceneCenter());

  double nearPlane = std::max(0.0001f * properties_.sceneRadius(),  -(c[2] + properties_.sceneRadius()));
  double farPlane  = std::max(0.0002f * properties_.sceneRadius(),  -(c[2] - properties_.sceneRadius()));

  // Safety check, if near < 0
  if ( nearPlane <= 0.0 ) {
    std::cerr << "Error in BaseViewer drawScene, nearplane < 0" << std::endl;
    nearPlane = 0.000000000000001;
  }

  // Safety check. Make sure that they are in the right order
  if ( nearPlane > farPlane ) {
    std::cerr << "Error in BaseViewer drawScene, Nearplane > Farplane" << std::endl;
    std::swap(nearPlane,farPlane);
  }

  properties_.setPlanes(nearPlane,farPlane);

  updateProjectionMatrix();

  // store time since last repaint in gl state and restart timer
  glstate_->set_msSinceLastRedraw (redrawTime_.restart ());

  makeCurrent();

  // draw mono or stereo
  // If stereo mode is selected, we have to provide multiple ways of rendering.

  // 1. Default internal pipeline : Directly to output buffer (no stereo support!)
  // 2. Non default : Non-stereo Directly to output buffer (no stereo support!)
  //
  //

//  if (OpenFlipper::Options::stereoMode () == OpenFlipper::Options::OpenGL && OpenFlipper::Options::glStereo ())
//  {
//    // Stereo      : Hardware support (OpenGL stereo : left and right buffer with offset eyes)
//
//    return;
//  }
//  else if (OpenFlipper::Options::stereoMode () == OpenFlipper::Options::AnaglyphCustom && customAnaglyphSupported_)
//  {
//    //Stereo      : No Hardware support (Left and right frame buffer with offset eyes)
//  } else {
//
//  }

  // Clear back buffer here:
  // Render plugins do not have to worry about using scissor test for clearing their viewports later on.
  glClearColor(properties_.backgroundColor()[0], properties_.backgroundColor()[1], properties_.backgroundColor()[2], 1.0f);
  GLint curViewport[4];
  glGetIntegerv(GL_VIEWPORT, curViewport);
  glScissor(curViewport[0], curViewport[1], curViewport[2], curViewport[3]);
  glEnable(GL_SCISSOR_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_SCISSOR_TEST);


  // Check if we use build in default renderer
  if ( renderManager().activeId( properties_.viewerId() ) == 0 ) {
    drawScene_mono();
  } else {
    RenderInterface* renderPlugin = renderManager().active( properties_.viewerId() )->plugin;

    // eventually set viewer id in IRenderer
    ACG::IRenderer* shaderRenderPlugin = dynamic_cast<ACG::IRenderer*>(renderPlugin);

    if (shaderRenderPlugin)
      shaderRenderPlugin->setViewerID( properties_.viewerId() );

    renderPlugin->render(glstate_,properties_);
  }
  checkGLError();


  // =================================================================================
  // Post-Processing pipeline


  const int numPostProcessors = postProcessorManager().numActive(properties_.viewerId());

  if (numPostProcessors)
  {
    GLuint backbufferFbo = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&backbufferFbo);


    updatePostProcessingBufs(glstate_->viewport_width(),glstate_->viewport_height());

    readBackBuffer(glstate_);

    // 1st post processing source: active fbo
    int postProcSrc = 1;
    PostProcessorInput postProcInput;

    postProcInput.colorTex_ = readBackFbo_.getAttachment(GL_COLOR_ATTACHMENT0);
    postProcInput.depthTex_ = readBackFbo_.getAttachment(GL_DEPTH_ATTACHMENT);
    postProcInput.width     = readBackFbo_.width();
    postProcInput.height    = readBackFbo_.height();

    // execute post processing chain with 2 FBOs
    for (int i = 0; i < numPostProcessors; ++i)  {

      int postProcTarget = 1 - postProcSrc;

      GLuint targetFBO = postProcessFBO_[postProcTarget].getFboID();

      // write to back buffer in last step
      if (i + 1 == numPostProcessors)
        targetFBO = backbufferFbo;

      // apply post processor
      PostProcessorInfo* proc = postProcessorManager().active( properties_.viewerId(), i );
      if (proc && proc->plugin)
        proc->plugin->postProcess(glstate_, postProcInput, targetFBO);


      // swap target/source fbo
      postProcSrc = postProcTarget;

      postProcInput.colorTex_ = postProcessFBO_[postProcSrc].getAttachment(GL_COLOR_ATTACHMENT0);
    }
  }
  

  // =================================================================================

  // unbind vbo for qt log window
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

//  fbo.release();
//
//  QRect blitRect(0,0,glstate_->viewport_width(),glstate_->viewport_height());
//
//  //
//  //QTime time;
//  //time.restart();
//  QGLFramebufferObject::blitFramebuffer( 0 , blitRect, &fbo, blitRect , GL_COLOR_BUFFER_BIT );
//  //std::cerr << "Elapsed for blit: " << time.elapsed() << std::endl;

  glFinish();

  frame_time_ = timer.elapsed();

  
  // Inside the glWidget rendering, the system should not send extra updates
  properties_.blockSignals(false);

}


//-----------------------------------------------------------------------------


void glViewer::drawScene_mono()
{
  if (sceneGraphRoot_) {
    ViewObjectMarker *oM = properties_.objectMarker();
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

    // First pass
    ACG::SceneGraph::DrawAction pass1( properties_.drawMode(), *glstate_ , false);
    ACG::SceneGraph::traverse_multipass(sceneGraphRoot_, pass1, *glstate_, properties_.drawMode() );

    // Second Pass for Blending operations
    ACG::SceneGraph::DrawAction pass2(properties_.drawMode(), *glstate_, true);
    ACG::SceneGraph::traverse_multipass(sceneGraphRoot_, pass2, *glstate_, properties_.drawMode());

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
      glstate_->get_viewport (vp_l, vp_b, vp_w, vp_h);

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

  if (properties_.cursorPainter() && properties_.cursorPainter()->enabled () && properties_.cursorPositionValid() )
  {
    glstate_->push_modelview_matrix ();
    // reset view transformation
    glstate_->reset_modelview ();
    // translate cursor position to 0,0
    glstate_->translate ( properties_.cursorPoint3D() );
    // paint cursor
    properties_.cursorPainter()->paintCursor (glstate_);
    glstate_->pop_modelview_matrix ();
  }

}

void glViewer::setHome()
{
  home_modelview_          = glstate_->modelview();
  home_inverse_modelview_  = glstate_->inverse_modelview();
  homeOrthoWidth_          = properties_.orthoWidth();
  home_center_             = properties_.trackballCenter();
  home_radius_             = properties_.trackballRadius();
}


void glViewer::home()
{
  makeCurrent();
  glstate_->set_modelview(home_modelview_, home_inverse_modelview_);
  properties_.orthoWidth( homeOrthoWidth_ );
  properties_.trackballCenter( home_center_ );
  properties_.trackballRadius(home_radius_);
  updateProjectionMatrix();
  updateGL();

  emit viewChanged();

}

//-----------------------------------------------------------------------------

void glViewer::viewAll()
{
  makeCurrent();

  // update scene graph (get new bounding box and set projection right, including near and far plane)
  properties_.lockUpdate();

  // move center (in camera coords) to origin and translate in -z dir
  translate(-(glstate_->modelview().transform_point(properties_.sceneCenter()))
            - ACG::Vec3d(0.0, 0.0, 3.0 * properties_.sceneRadius()));

  properties_.orthoWidth( 1.1 * properties_.sceneRadius() );
  double aspect = (double) glWidth() / (double) glHeight();
  if (aspect > 1.0)
    properties_.orthoWidth( aspect * properties_.orthoWidth() ) ;

  unsigned int maxPases = 1;
  ACG::Vec3d bbmin,bbmax;
  ACG::SceneGraph::analyzeSceneGraph(PluginFunctions::getSceneGraphRootNode(),maxPases,bbmin,bbmax);

  sceneGraph ( PluginFunctions::getSceneGraphRootNode(), maxPases,bbmin,bbmax,true);

  properties_.unLockUpdate();
  updateProjectionMatrix();
  updateGL();

  emit viewChanged();

}


//-----------------------------------------------------------------------------


void glViewer::setView(const ACG::GLMatrixd& _modelview,
			                  const ACG::GLMatrixd& _inverse_modelview)
{
  makeCurrent();
  glstate_->set_modelview(_modelview, _inverse_modelview);
  updateGL();

  emit viewChanged();
}


//-----------------------------------------------------------------------------


void glViewer::initializeGL()
{

  // lock update
  properties_.lockUpdate();

  // init GL state
  glstate_->initialize();

  // OpenGL state
  ACG::GLState::enable(GL_DEPTH_TEST);
  ACG::GLState::enable(GL_LIGHTING);
  ACG::GLState::disable(GL_DITHER);
  ACG::GLState::shadeModel( GL_FLAT );


  projectionMode(   projectionMode_   );
  normalsMode(      normalsMode_      );

  // Update all settings which would require a redraw
  applyProperties();

  // modelview
  glstate_->translate(0.0, 0.0, -3.0);
  setHome();


  // pixel transfer
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ROW_LENGTH, 0);
  glPixelStorei(GL_PACK_SKIP_ROWS, 0);
  glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);


  // unlock update (we started locked)
  properties_.unLockUpdate();

  initialized_ = true;

  if (sceneGraphRoot_)
  {
    unsigned int maxPases = 1;
    ACG::Vec3d bbmin,bbmax;
    ACG::SceneGraph::analyzeSceneGraph(sceneGraphRoot_,maxPases,bbmin,bbmax);

    sceneGraph ( sceneGraphRoot_, maxPases,bbmin,bbmax,true);

    viewAll();
  }
}


//-----------------------------------------------------------------------------


void glViewer::paintGL()
{
  if (!initialized_)
    initializeGL ();

  if (!properties_.updateLocked())
  {
    ACG::Vec4f clear_color;

    properties_.lockUpdate();

    glPushAttrib (GL_ALL_ATTRIB_BITS);

    ACG::GLState::enable(GL_DEPTH_TEST);
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::disable(GL_DITHER);
    ACG::GLState::shadeModel( GL_FLAT );

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();


    normalsMode(      normalsMode_      );

    applyProperties();

    glstate_->setState();

    glColor4f(1.0,0.0,0.0,1.0);

    glstate_->clearBuffers ();

    properties_.unLockUpdate();

    // draw scene
    drawScene();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopAttrib ();
  }

}


//-----------------------------------------------------------------------------


void glViewer::resizeEvent(QGraphicsSceneResizeEvent *)
{
  updateProjectionMatrix();
  glstate_->viewport(scenePos().x(),
                     scene()->height () - scenePos().y() - size().height (),
                     size().width (), size().height (),
                     scene()->width (), scene()->height ());
  update();

  emit viewChanged();
}

void glViewer::moveEvent (QGraphicsSceneMoveEvent *)
{
  glstate_->viewport(scenePos().x(),
                     scene()->height () - scenePos().y() - size().height (),
                     size().width (), size().height (),
                     scene()->width (), scene()->height ());
  update();

  emit viewChanged();
}

//-----------------------------------------------------------------------------

void glViewer::encodeView(QString& _view, const QSize& _windowSize /*= QSize()*/, const int _splitterWidth /*=-1*/)
{
  // Get current matrices
  const ACG::GLMatrixd m = glstate_->modelview();
  const ACG::GLMatrixd p = glstate_->projection();

  // Add modelview matrix to output
  _view += QString(COPY_PASTE_VIEW_START_STRING) + "\n";
  _view += QString::number(m(0,0)) + " " + QString::number(m(0,1)) + " " + QString::number(m(0,2)) + " " + QString::number(m(0,3)) + "\n";
  _view += QString::number(m(1,0)) + " " + QString::number(m(1,1)) + " " + QString::number(m(1,2)) + " " + QString::number(m(1,3)) + "\n";
  _view += QString::number(m(2,0)) + " " + QString::number(m(2,1)) + " " + QString::number(m(2,2)) + " " + QString::number(m(2,3)) + "\n";
  _view += QString::number(m(3,0)) + " " + QString::number(m(3,1)) + " " + QString::number(m(3,2)) + " " + QString::number(m(3,3)) + "\n";

  // Add projection matrix to output
  _view += QString::number(p(0,0)) + " " + QString::number(p(0,1)) + " " + QString::number(p(0,2)) + " " + QString::number(p(0,3)) + "\n";
  _view += QString::number(p(1,0)) + " " + QString::number(p(1,1)) + " " + QString::number(p(1,2)) + " " + QString::number(p(1,3)) + "\n";
  _view += QString::number(p(2,0)) + " " + QString::number(p(2,1)) + " " + QString::number(p(2,2)) + " " + QString::number(p(2,3)) + "\n";
  _view += QString::number(p(3,0)) + " " + QString::number(p(3,1)) + " " + QString::number(p(3,2)) + " " + QString::number(p(3,3)) + "\n";

  // add gl width/height, current projection Mode and the ortho mode width to output
  _view += QString::number(_windowSize.width()) + " " +  QString::number(_windowSize.height()) + " "  + QString::number(_splitterWidth)+ " " + QString::number(projectionMode_) + " " + QString::number(properties_.orthoWidth()) + "\n";
}


//----------------------------------------------------------------------------


bool glViewer::decodeView(const QString& _view, QSize *_windowSize /*= NULL*/, int* _splitterWidth /*= NULL*/)
{
  if (_view.left(sizeof(COPY_PASTE_VIEW_START_STRING)-1) != QString(COPY_PASTE_VIEW_START_STRING))
  {
    std::cerr << "No View was copied." << std::endl;
    return false;
  }

  // Remove the magic from the string
  QString temp = _view;
  temp.remove(0,sizeof(COPY_PASTE_VIEW_START_STRING));

  //Split it into its components
  QStringList split = temp.split(QRegExp("[\\n\\s]"),QString::SkipEmptyParts);

  ACG::GLMatrixd m, p;
  int            pMode;

  // New version
  if ( split.size() == 37 ) {

    //*********************************************************
    // Parse the components
    // first, get the projection and the modelview matrices
    //*********************************************************
    for (std::size_t i = 0; i < 4; ++i)
    {
      for (std::size_t j = 0; j < 4; ++j)
      {
        m(i,j) = split[i*4 + j].toDouble();
        p(i,j) = split[i*4 + j +16].toDouble();
      }
    }

    //*********************************************************
    //parse the window size if requested
    //*********************************************************
    if (_windowSize)
    {
      //restore the old window size
      int w =  split[32].toInt();
      int h =  split[33].toInt();
      *_windowSize = QSize(w,h);
    }

    //*********************************************************
    //parse the splitter width for the toolboxes if requested
    //*********************************************************
    if (_splitterWidth) {
      *_splitterWidth = split[34].toInt();
    }

    //*********************************************************
    // Projection mode and orthogonal width
    //*********************************************************
    pMode =  split[35].toInt();
    properties_.orthoWidth( split[36].toDouble() );

  } else if ( split.size() == 36 )  { // Old Version

    //*********************************************************
    // Parse the components
    // first, get the projection and the modelview matrices
    //*********************************************************
    for (std::size_t i = 0; i < 4; ++i)
    {
      for (std::size_t j = 0; j < 4; ++j)
      {
        m(i,j) = split[i*4 + j].toDouble();
        p(i,j) = split[i*4 + j +16].toDouble();
      }
    }

    //*********************************************************
    //parse the window size if requested
    //*********************************************************
    if (_windowSize)
    {
      //restore the old window size
      int w =  split[32].toInt();
      int h =  split[33].toInt();
      *_windowSize = QSize(w,h);
    }


    //*********************************************************
    // Return -1 to inform, that the value is unknown
    //*********************************************************
    if (_splitterWidth) {
      *_splitterWidth = -1;
    }

    //*********************************************************
    // Projection mode and orthogonal width
    //*********************************************************
    pMode =  split[34].toInt();
    properties_.orthoWidth( split[35].toDouble() );

  } else { // Garbage ?!
    std::cerr << "Unable to paste view ... wrong parameter count!! is" <<  split.size()  << std::endl;
    return false;
  }

  // Switch to our gl context
  makeCurrent();

  // set projection mode
  if (projectionMode_ != (ProjectionMode)pMode)
    projectionMode((ProjectionMode)pMode);

  // Apply new modelview matrix
  glstate_->set_modelview(m);

  updateGL();

  return true;
}


//-----------------------------------------------------------------------------


void glViewer::actionCopyView(const QSize &_windowSize /*= QSize(-1,-1)*/, const int _splitterWidth /*= -1*/)
{
  QString view;
  encodeView(view,_windowSize,_splitterWidth);
  QApplication::clipboard()->setText(view);
}


//-----------------------------------------------------------------------------


void glViewer::actionPasteView(QSize * _windowSize /*= NULL*/, int *_splitterWidth /*= NULL*/)
{
  QString view;
  view = QApplication::clipboard()->text();
  decodeView(view,_windowSize,_splitterWidth);
}

void glViewer::actionSetView(QString view) {
    decodeView(view, 0, 0);
}

//-----------------------------------------------------------------------------

void
glViewer::createWidgets()
{
  // Construct GL context & widget

  wheelZ_=new ACG::QtWidgets::QtWheel( 0,"wheel-z",ACG::QtWidgets::QtWheel::Vertical);
  wheelZ_->setMinimumSize(wheelZ_->sizeHint());
  wheelZ_->setMaximumSize(wheelZ_->sizeHint());
  connect(wheelZ_,SIGNAL(angleChangedBy(double)),
	  this,SLOT(slotWheelZ(double)));
  wheelZ_->setToolTip( tr("Translate along <b>z-axis</b>."));
  wheelZ_->setWhatsThis( tr("Translate along <b>z-axis</b>."));

  wheelY_=new ACG::QtWidgets::QtWheel( 0,"wheel-y",ACG::QtWidgets::QtWheel::Horizontal);
  wheelY_->setMinimumSize(wheelY_->sizeHint());
  wheelY_->setMaximumSize(wheelY_->sizeHint());
  connect(wheelY_,SIGNAL(angleChangedBy(double)),
          this,SLOT(slotWheelY(double)));
  wheelY_->setToolTip(tr("Rotate around <b>y-axis</b>."));
  wheelY_->setWhatsThis( tr("Rotate around <b>y-axis</b>."));

  wheelX_=new ACG::QtWidgets::QtWheel( 0,"wheel-x" ,ACG::QtWidgets::QtWheel::Vertical);
  wheelX_->setMinimumSize(wheelX_->sizeHint());
  wheelX_->setMaximumSize(wheelX_->sizeHint());
  connect(wheelX_,SIGNAL(angleChangedBy(double)),
          this,SLOT(slotWheelX(double)));
  wheelX_->setToolTip(tr("Rotate around <b>x-axis</b>."));
  wheelX_->setWhatsThis( tr("Rotate around <b>x-axis</b>."));

  // Hide or show wheels (depending on ini option)
  if( ! OpenFlipperSettings().value("Core/Gui/glViewer/showControlWheels").toBool() ) 
    slotHideWheels();

  QGraphicsWidget *wheelX = glScene_->addWidget (wheelX_);
  QGraphicsWidget *wheelY = glScene_->addWidget (wheelY_);
  QGraphicsWidget *wheelZ = glScene_->addWidget (wheelZ_);

  wheelX_->setWindowOpacity (0.5);
  wheelY_->setWindowOpacity (0.5);
  wheelZ_->setWindowOpacity (0.5);

  wheelX->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  wheelY->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  wheelZ->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  glBaseLayout_ = new QtGLViewerLayout;
  glBaseLayout_->addWheelX(wheelX);
  glBaseLayout_->addWheelY(wheelY);
  glBaseLayout_->addWheelZ(wheelZ);

  connect(wheelX_,SIGNAL(hideWheel()),this,SLOT(slotHideWheels()));
  connect(wheelY_,SIGNAL(hideWheel()),this,SLOT(slotHideWheels()));
  connect(wheelZ_,SIGNAL(hideWheel()),this,SLOT(slotHideWheels()));

  setLayout(glBaseLayout_);
}

//-----------------------------------------------------------------------------


void glViewer::translate(const ACG::Vec3d& _trans)
{
  makeCurrent();
  glstate_->translate(_trans[0], _trans[1], _trans[2], ACG::MULT_FROM_LEFT);

  updateGL();

  emit viewChanged();
}


//-----------------------------------------------------------------------------


void glViewer::initModelviewMatrix()
{
  makeCurrent();
  glstate_->reset_modelview();
}


//-----------------------------------------------------------------------------


void glViewer::rotate(const ACG::Vec3d&  _axis,
                          double        _angle,
                          const ACG::Vec3d&  _center)
{
  makeCurrent();
  ACG::Vec3d t = glstate_->modelview().transform_point(_center);
  glstate_->translate(-t[0], -t[1], -t[2], ACG::MULT_FROM_LEFT);
  glstate_->rotate(_angle, _axis[0], _axis[1], _axis[2], ACG::MULT_FROM_LEFT);
  glstate_->translate( t[0],  t[1],  t[2], ACG::MULT_FROM_LEFT);

  updateGL();

  emit viewChanged();
}


//-----------------------------------------------------------------------------


unsigned int glViewer::glWidth() const {
  return size().width();
}
unsigned int glViewer::glHeight() const {
  return size().height();
}
QSize glViewer::glSize() const {
  return QSize(size().width(),size().height());
}
QPoint glViewer::glMapFromGlobal( const QPoint& _pos ) const {
  QPoint p (scene()->views().front()->mapFromGlobal(_pos));
  QPointF f (mapFromScene(QPointF(p.x(), p.y ())));
  return QPoint (f.x(), f.y());
}

QPoint glViewer::glMapToGlobal( const QPoint& _pos ) const {
  QPointF f (mapToScene(QPointF(_pos.x(), _pos.y ())));
  QPoint p (f.x(), f.y());
  return scene()->views().front()->mapToGlobal(p);
}

double glViewer::field_of_view_vertical() const {
    return OpenFlipperSettings().value("Core/Projection/FOVY", 45.0).toDouble()
            + fovyModifier_;
}


//-----------------------------------------------------------------------------


void glViewer::slotWheelX(double _dAngle)
{
  rotate(ACG::Vec3d(1,0,0),ACG::QtWidgets::QtWheel::deg(ACG::QtWidgets::QtWheel::clip(_dAngle)));
  updateGL();

  emit viewChanged();
}

void glViewer::slotWheelY(double _dAngle)
{
  rotate(ACG::Vec3d(0,1,0),ACG::QtWidgets::QtWheel::deg(ACG::QtWidgets::QtWheel::clip(_dAngle)));
  updateGL();

  emit viewChanged();
}

void glViewer::slotWheelZ(double _dist)
{
  double dz = _dist * 0.5 / M_PI * properties_.sceneRadius() * 2.0;
  translate(ACG::Vec3d(0,0,dz));
  updateGL();

  emit viewChanged();
}

//-----------------------------------------------------------------------------


void glViewer::grabGLArea()
{
  glareaGrabbed_ = true;

  if (properties_.cursorPainter()) {
    properties_.cursorPainter()->setCursor(Qt::BlankCursor);
    std::cerr << "grabGLArea: Blanking cursorpainter  cursor" << std::endl; 
  } else {
    setCursor(Qt::BlankCursor);
    std::cerr << "grabGLArea: Blanking qt cursor" << std::endl;
  }
  grabMouse();
  grabKeyboard();
}

void glViewer::releaseGLArea()
{
  glareaGrabbed_ = false;

  ungrabMouse();
  ungrabKeyboard();

  if (properties_.cursorPainter()) {
    properties_.cursorPainter()->setCursor(Qt::ArrowCursor);
    std::cerr << "grabGLArea: Setting cursorPainter cursor to arrow" << std::endl;
  } else {
    setCursor(Qt::ArrowCursor);
    std::cerr << "grabGLArea: Setting qt cursor to arrow" << std::endl;
  }
}


//-----------------------------------------------------------------------------


void glViewer::contextMenuEvent(QGraphicsSceneContextMenuEvent* _e)
{
  emit signalMakeActive();
  glScene_->update ();

  QPoint p (_e->pos().x(), _e->pos().y());
  emit signalCustomContextMenuRequested (p);
}


//-----------------------------------------------------------------------------

void glViewer::mousePressEvent(QGraphicsSceneMouseEvent* _e)
{
  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseButtonPress ,p, _e->screenPos(), _e->button(),
                 _e->buttons(), _e->modifiers());
  _e->accept ();

  emit signalMakeActive();
  glScene_->update ();

  // right button pressed => popup menu (ignore here)
  if (allowConstrainedRotation() || _e->button() != Qt::RightButton )
  {
    switch (properties_.actionMode())
    {
      case Viewer::ExamineMode:
        if ((_e->modifiers() & Qt::ControlModifier)) // drag&drop
          emit startDragEvent( &me );
        else
          viewMouseEvent(&me); // examine

        if (clickTimer_.isActive ())
        {
          clickTime_ = QTime ();
          clickTimer_.stop ();
        }
        else
        {
          clickTime_.start ();
          clickEvent_ = me;
        }
        break;

      case Viewer::LightMode:
        emit(signalMouseEventLight(&me));
        break;

      case Viewer::PickingMode: // give event to application
        emit(signalMouseEvent(&me, properties_.pickMode() ));
        emit(signalMouseEvent(&me));
        break;

      case Viewer::QuestionMode: // give event to application
        emit(signalMouseEventIdentify(&me));
        break;
    }
  }
}


//-----------------------------------------------------------------------------


void glViewer::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _e)
{
  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseButtonDblClick ,p, _e->screenPos(), _e->button(),
                 _e->buttons(), _e->modifiers());
  _e->accept ();

  emit signalMakeActive();
  glScene_->update ();

  switch (properties_.actionMode())
  {
    case Viewer::ExamineMode:
      viewMouseEvent(&me);
      emit signalMouseEventClick (&me, true);
      break;

    case Viewer::LightMode:
      emit(signalMouseEventLight(&me));
      break;

    case Viewer::PickingMode: // give event to application
      emit(signalMouseEvent(&me, properties_.pickMode() ));
      emit(signalMouseEvent(&me));
      break;

    case Viewer::QuestionMode: // give event to application
      emit(signalMouseEventIdentify(&me));
      break;
  }
}


//-----------------------------------------------------------------------------


void glViewer::mouseMoveEvent(QGraphicsSceneMouseEvent* _e)
{
  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseMove ,p, _e->screenPos(), _e->button(),
                 _e->buttons(), _e->modifiers());
  _e->accept();

  switch ( properties_.actionMode() )
  {
    case Viewer::ExamineMode:
      viewMouseEvent(&me);
      break;

    case Viewer::LightMode:
      emit(signalMouseEventLight(&me));
      break;

    case Viewer::PickingMode:
      // give event to application
      // deliver mouse moves with no button down, if tracking is enabled,
      if ((_e->buttons() & (Qt::LeftButton | Qt::MidButton | Qt::RightButton))
          || trackMouse_)
      {
        emit(signalMouseEvent(&me, properties_.pickMode() ));
        emit(signalMouseEvent(&me));
      }
      break;

    case Viewer::QuestionMode: // give event to application
      emit(signalMouseEventIdentify(&me));
      break;

    default: // avoid warning
      break;
  }
}

//-----------------------------------------------------------------------------


void glViewer::mouseReleaseEvent(QGraphicsSceneMouseEvent* _e)
{
  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseButtonRelease ,p, _e->screenPos(), _e->button(),
                 _e->buttons(), _e->modifiers());
  _e->accept();

//   if (_event->button() == Qt::RightButton )
//     hidePopupMenus();

  if (_e->button() != Qt::RightButton || (properties_.actionMode() == Viewer::PickingMode) )
  {
    switch ( properties_.actionMode() )
    {
      case Viewer::ExamineMode:
        viewMouseEvent(&me);

        if (!clickTime_.isNull ())
        {
          int elapsed = clickTime_.elapsed ();
          QPoint diff = clickEvent_.pos () - me.pos ();

          if (abs (diff.x ()) <= 1 && abs (diff.y ()) <= 1 && elapsed <= QApplication::doubleClickInterval () / 2)
          {
            clickTimer_.setSingleShot (true);
            clickTimer_.setInterval (QApplication::doubleClickInterval () - elapsed);
            clickTimer_.start ();
          }
          else
          {
            clickTime_ = QTime ();
            clickTimer_.stop ();
          }
        }
        break;

      case Viewer::LightMode:
        emit(signalMouseEventLight(&me));
        break;

      case Viewer::PickingMode: // give event to application
        emit(signalMouseEvent(&me, properties_.pickMode() ));
        emit(signalMouseEvent(&me));
        break;

      case Viewer::QuestionMode: // give event to application
        emit(signalMouseEventIdentify(&me));
        break;

      default: // avoid warning
        break;
    }
  }

  isRotating_ = false;
}


//-----------------------------------------------------------------------------


void glViewer::wheelEvent(QGraphicsSceneWheelEvent* _e)
{
  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QWheelEvent we(p, _e->screenPos(), _e->delta(), _e->buttons(),
                 _e->modifiers(), _e->orientation());
  _e->accept();

  switch ( properties_.actionMode() )
  {
    case Viewer::ExamineMode:
      viewWheelEvent(&we);
      break;

    case Viewer::PickingMode: // give event to application
      emit(signalWheelEvent(&we, properties_.pickMode() ));
      break;

    default: // avoid warning
      break;
  }

  isRotating_ = false;
}

//-----------------------------------------------------------------------------

void glViewer::dragEnterEvent(QGraphicsSceneDragDropEvent* _e)
{
  std::cerr << "dragEnter" << std::endl;

  QPoint p (_e->pos().x(), _e->pos().y());
  QDragEnterEvent de(p, _e->possibleActions(), _e->mimeData(), _e->buttons(),
		     _e->modifiers ());
  emit dragEnterEvent(&de);
  _e->accept();
}

//-----------------------------------------------------------------------------


void glViewer::dropEvent(QGraphicsSceneDragDropEvent* _e)
{
  std::cerr << "drop" << std::endl;

  QPoint p (_e->pos().x(), _e->pos().y());
  QDropEvent de(p, _e->possibleActions(), _e->mimeData(), _e->buttons(),
                _e->modifiers ());
  emit dropEvent(&de);
  _e->accept();
}

//-----------------------------------------------------------------------------


void glViewer::viewMouseEvent(QMouseEvent* _event) {

  if (navigationMode_ == NORMAL_NAVIGATION) {

    handleNormalNavigation(_event);

  } else if (navigationMode_ == FIRSTPERSON_NAVIGATION) {

    handleFirstPersonNavigation(_event);
  }

}

//----------------------------------------------------------------------------

void glViewer::handleFirstPersonNavigation( QMouseEvent* _event) {

  // Ego-shooter navigation mode is selected
  QPointF f(mapFromScene(QPointF(_event->pos().x(), _event->pos().y())));
  QPoint pos(f.x(), f.y());

  switch (_event->type()) {

  case QEvent::MouseButtonPress: {
    lastPoint2D_ = pos;
    lookAround_ = true;
    break;
  }

  case QEvent::MouseButtonDblClick: {
    flyTo(_event->pos(), _event->button() == Qt::MidButton);
    break;
  }

  case QEvent::MouseMove: {

    if(!lookAround_) break;

    // Considering center point of screen as origin
    QPoint newpos = QPoint(pos.x() - glWidth() / 2, glHeight() / 2 - pos.y());
    QPoint oldpos = QPoint(lastPoint2D_.x() - glWidth() / 2, glHeight() / 2 - lastPoint2D_.y());

    double x = 2.0 * newpos.x() / glWidth();
    double y = 2.0 * newpos.y() / glHeight();

    double xo = 2.0 * oldpos.x() / glWidth();
    double yo = 2.0 * oldpos.y() / glHeight();

    double diffx = xo - x;
    double diffy = yo - y;

    ACG::Vec3d yaxis = glstate_->modelview().transform_vector(glstate_->up());
    ACG::Vec3d xaxis = glstate_->modelview().transform_vector(glstate_->right());

    rotate(yaxis, -diffx * 90, glstate_->eye());
    rotate(xaxis, diffy * 90, glstate_->eye());

    lastPoint2D_ = pos;

    updateGL();
    lastMoveTime_.restart();

    emit viewChanged();

    break;
  }

  case QEvent::MouseButtonRelease: {
    lookAround_ = false;
    break;
  }

  default: // avoid warning
    break;
  }
}

//----------------------------------------------------------------------------

void glViewer::handleNormalNavigation( QMouseEvent* _event ) {

  // Normal navigation mode is selected
  QPointF f(mapFromScene(QPointF(_event->pos().x(), _event->pos().y())));
  QPoint pos(f.x(), f.y());

  switch (_event->type()) {

    case QEvent::MouseButtonPress: {
      
      // Get the depth at the current mouse position ( projected )
      // This is used to do the translation in world coordinates
      // As the scene is rendered, we can get the depth directly from the framebuffer.
      GLfloat       depth[1];
      GLint x2d = f.x() - scenePos().x();
      GLint y2d = glHeight() - (f.y() - scenePos().y());
      glReadPixels (x2d, y2d, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, depth); 
      startDepth_ = depth[0];
      
      // shift key -> set rotation center
      if (_event->modifiers() & Qt::ShiftModifier) {
          ACG::Vec3d c;
          if (fast_pick(pos, c)) {
              properties_.trackballCenter( c );
              properties_.trackballRadius( std::max(properties_.sceneRadius(), (c - glstate_->eye()).norm() * 0.9f) );
          }
      }

      lastPoint_hitSphere_ = mapToSphere(pos, lastPoint3D_);
      lastPoint2D_ = pos;
      isRotating_ = true;
      timer_->stop();

      break;
    }

    case QEvent::MouseButtonDblClick: {
      flyTo(_event->pos(), _event->button() == Qt::MidButton);
      break;
    }

    case QEvent::MouseMove: {
      double factor = 1.0;

      if (_event->modifiers() == Qt::ShiftModifier)
          factor = properties_.wheelZoomFactorShift();

      // mouse button should be pressed
      if (_event->buttons() & (Qt::LeftButton | Qt::MidButton | Qt::RightButton)) {
        QPoint newPoint2D = pos;

        ACG::Vec3d newPoint3D;
        bool newPoint_hitSphere = mapToSphere(newPoint2D, newPoint3D);

        makeCurrent();

        // Left and middle button are pressed:
        // Translate along image planes normal direction -> zoom
        if ((_event->buttons() & Qt::LeftButton) && (_event->buttons() & Qt::MidButton)) {
          switch (projectionMode()) {
            case PERSPECTIVE_PROJECTION: {
                double value_y = properties_.sceneRadius() * ((newPoint2D.y() - lastPoint2D_.y())) * 3.0 / (double) glHeight();
                translate(ACG::Vec3d(0.0, 0.0, value_y * factor));
                updateGL();
                emit viewChanged();
                break;
            }

            case ORTHOGRAPHIC_PROJECTION: {
                double value_y = ((newPoint2D.y() - lastPoint2D_.y())) * properties_.orthoWidth() / (double) glHeight();
                properties_.orthoWidth( properties_.orthoWidth() - value_y * factor );
                updateProjectionMatrix();
                updateGL();
                emit viewChanged();
                break;
            }
          }
        }

        // Middle button is pressed or if rotation is locked, left button can also be used
        // translation parallel to image plane
        // If an object was hit when the user started the translation, 
        // the depth to the object is used to calculate the right translation vectors
        // such that the hitpoint stays below the mouse.
        else if ((_event->buttons() & Qt::MidButton) || (!allowRotation_ && (_event->buttons() & Qt::LeftButton))  ) {
          
          ACG::Vec3d translation;
          

          // if start depth is 1, no object was hit when starting translation
          if ( startDepth_ == 1 && projectionMode() != ORTHOGRAPHIC_PROJECTION ) {
            double value_x = properties_.sceneRadius() * ((newPoint2D.x() - lastPoint2D_.x())) * 2.0 / (double) glWidth();
            double value_y = properties_.sceneRadius() * ((newPoint2D.y() - lastPoint2D_.y())) * 2.0 / (double) glHeight();
            translation = ACG::Vec3d(value_x * factor, -value_y * factor, 0.0);
            
          } else {
            // Get the new mouse position in GL coordinates
            GLint x2dEnd = newPoint2D.x() - scenePos().x();
            GLint y2dEnd = glHeight() - (newPoint2D.y() - scenePos().y());
            
            // Get the last mouse position in GL coordinates
            GLint x2dStart = lastPoint2D_.x() - scenePos().x();
            GLint y2dStart = glHeight() - (lastPoint2D_.y() - scenePos().y());
            
            // unproject both to get translation vector
            ACG::Vec3d projectedStart(float(x2dStart),float(y2dStart),startDepth_);
            ACG::Vec3d projectedEnd(float(x2dEnd),float(y2dEnd),startDepth_);
            ACG::Vec3d unprojectedStart = glstate_->unproject(projectedStart);
            ACG::Vec3d unprojectedEnd   = glstate_->unproject(projectedEnd);

            // calculate the difference and transform it with the modelview to get the right translation
            translation = unprojectedEnd - unprojectedStart;
            translation = glstate_->modelview().transform_vector(translation);
          }
          
          translate(translation);

        // left button pressed:
        // rotate the scene if rotation is not locked
        } else if (allowRotation_ && (_event->buttons() & Qt::LeftButton)) {
            ACG::Vec3d axis(1.0, 0.0, 0.0);
            double angle(0.0);

            if (lastPoint_hitSphere_) {

                if ((newPoint_hitSphere = mapToSphere(newPoint2D, newPoint3D))) {
                    axis = lastPoint3D_ % newPoint3D;
                    double cos_angle = (lastPoint3D_ | newPoint3D);
                    if (fabs(cos_angle) < 1.0) {
                        angle = acos(cos_angle) * 180.0 / M_PI * factor;
                        angle *= 2.0; // inventor rotation
                    }
                }

                rotate(axis, angle);

            }

            lastRotationAxis_ = axis;
            lastRotationAngle_ = angle;
        } else if (allowConstrainedRotation() && (_event->buttons() & Qt::RightButton)) {
            //ACG::Vec3d axis(constrainedRotationAxis_);
            //double angle(0.0);

            if (lastPoint_hitSphere_) {

                const QPoint dragVector = newPoint2D - lastPoint2D_;
                const double dragLength = dragVector.y();
                        //ACG::Vec2d(dragVector.x(), dragVector.y()).norm();
                const double angle = -dragLength * factor;

                /*
                {
                    makeCurrent();
                    const ACG::Vec3d center =
                            properties_.trackballCenter();
                    const ACG::Vec3d t =
                            startViewMatrix_.transform_point(center);
                    glstate_->set_modelview(startViewMatrix_);
                    glstate_->translate(-t[0], -t[1], -t[2],
                            ACG::MULT_FROM_LEFT);
                    glstate_->rotate(angle,
                            constrainedRotationAxis_[0],
                            constrainedRotationAxis_[1],
                            constrainedRotationAxis_[2],
                            ACG::MULT_FROM_LEFT);
                    glstate_->translate(t[0], t[1], t[2],
                            ACG::MULT_FROM_LEFT);
                }
                */
                ACG::Vec3d center(glWidth()/2.0, glHeight()/2.0, 0);
                rotate(constrainedRotationAxis_, angle, unproject(center));

                lastRotationAxis_ = constrainedRotationAxis_;
                lastRotationAngle_ = angle;
            }
        }

        lastPoint2D_ = newPoint2D;
        lastPoint3D_ = newPoint3D;
        lastPoint_hitSphere_ = newPoint_hitSphere;

        updateGL();
        lastMoveTime_.restart();
        emit viewChanged();
      }
      break;
    }

    case QEvent::MouseButtonRelease: {
      lastPoint_hitSphere_ = false;

      // continue rotation ?
      if (isRotating_ && (_event->button() == Qt::LeftButton) && (!(_event->buttons() & Qt::MidButton))
              && (lastMoveTime_.elapsed() < 50) && properties_.animation())
        timer_->start(0);
      break;
    }

    default: // avoid warning
      break;
  }
}

//-----------------------------------------------------------------------------

void glViewer::viewWheelEvent( QWheelEvent* _event)
{

  // Default mouse wheel factor
  double factor = properties_.wheelZoomFactor();

  // Shift pressed, so we use the smaller factor
  if (_event->modifiers() == Qt::ShiftModifier)
    factor = properties_.wheelZoomFactorShift();

  // Mouse wheel inversion
  if (properties_.wheelInvert())
    factor *= -1.0;

  if (projectionMode() == PERSPECTIVE_PROJECTION )
  {

    // Control key : Modify field of view. Otherwise translate
    if ( _event->modifiers() &  Qt::ControlModifier ) {

      // Most mouse types work in steps of 15 degrees, in which case the delta value is a
      // multiple of 120; i.e., 120 units * 1/8 = 15 degrees
      double numDegrees = double(_event->delta()) / 8.0;
      double numSteps = numDegrees / 15.0;

      // Update the fovy modifier
      // This modifier will be added to the default fov to get the zoom
      fovyModifier_ += numSteps * factor ;

      // Clamp to minimum
      if ( (OpenFlipperSettings().value("Core/Projection/FOVY", 45.0).toDouble() + fovyModifier_) < 1.0 )
        fovyModifier_ = 1.0 -OpenFlipperSettings().value("Core/Projection/FOVY", 45.0).toDouble();

      // Clamp to maximum
      if ( (OpenFlipperSettings().value("Core/Projection/FOVY", 45.0).toDouble() + fovyModifier_) > 179.0 )
        fovyModifier_ = 179.0-OpenFlipperSettings().value("Core/Projection/FOVY", 45.0).toDouble();
    } else {


      // Old style zoom (bad as it does not modify the projection but the modelview,
      // which kills e.g. Sky Boxes
      double d = -(double)_event->delta() / 120.0 * 0.2 * factor * properties_.trackballRadius() / 3.0;
      translate( ACG::Vec3d(0.0, 0.0, d) );
    }



    updateGL();
  }
  else
  {
    double d = (double)_event->delta() / 120.0 * 0.2 * factor * properties_.orthoWidth();
    properties_.orthoWidth( properties_.orthoWidth() + d );


    updateProjectionMatrix();
    updateGL();
  }

  emit viewChanged();
}


//-----------------------------------------------------------------------------


bool glViewer::mapToSphere(const QPoint& _v2D, ACG::Vec3d& _v3D) const
{
  if ( (_v2D.x() >= 0) && (_v2D.x() < (int)glWidth()) &&
       (_v2D.y() >= 0) && (_v2D.y() < (int)glHeight()) )
  {
    double x   = (double)(_v2D.x() - ((double)glWidth() / 2.0))  / (double)glWidth();
    double y   = (double)(((double)glHeight() / 2.0) - _v2D.y()) / (double)glHeight();
    double sinx         = sin(M_PI * x * 0.5);
    double siny         = sin(M_PI * y * 0.5);
    double sinx2siny2   = sinx * sinx + siny * siny;

    _v3D[0] = sinx;
    _v3D[1] = siny;
    _v3D[2] = sinx2siny2 < 1.0 ? sqrt(1.0 - sinx2siny2) : 0.0;

    return true;
  }
  else return false;
}


//-----------------------------------------------------------------------------


void glViewer::slotAnimation()
{
  makeCurrent();
  rotate( lastRotationAxis_, lastRotationAngle_ );
  updateGL();

  if (!properties_.updateLocked()) {

    static int msecs=0, count=0;

    msecs += frame_time_;
    if (count >= 10 && msecs >= 500) {
      char s[100];
      sprintf( s, "%.3f fps", (1000.0 * count / (float)msecs) );
      emit statusMessage(s,2000);
      count = msecs = 0;
    }
    else
      ++count;
  }
}

void glViewer::applyProperties() {

  glstate_->set_twosided_lighting( properties_.twoSidedLighting() );

  glstate_->set_clear_color( properties_.backgroundColor() );

  glstate_->allow_multisampling( properties_.multisampling() );

  if (properties_.isCCWFront() )
    glFrontFace( GL_CCW );
  else
    glFrontFace( GL_CW );

  if ( properties_.backFaceCulling() )
    ACG::GLState::enable( GL_CULL_FACE );
  else
    ACG::GLState::disable( GL_CULL_FACE );

  // Make sure the right buffer is used in non stereo setup
  makeCurrent();
  ACG::GLState::drawBuffer(ACG::GLState::getFramebufferDraw() ? GL_COLOR_ATTACHMENT0 : GL_BACK);

  // Required for stereo toggling
  updateProjectionMatrix ();
}

void glViewer::slotPropertiesUpdated() {
  makeCurrent();
  applyProperties();
  updateGL();
}

void glViewer::snapshot(QImage& _image, int _width, int _height, bool _alpha, bool _hideCoordsys, int samples) {
    
    int w = 0, h = 0, bak_w = 0, bak_h = 0, left = 0, bottom = 0;
    
    glstate_->push_modelview_matrix();
    glstate_->push_projection_matrix();
    
    // Get viewport data
    glstate_->get_viewport(left, bottom, w, h);
    
    // Test if size is given:
    if(_width != 0 || _height != 0) {
        
        // Adapt dimensions if aspect ratio is demanded
        if(_width == 0) {
            double aspect = (double)w / (double)h;
            _width = (int)((double)_height * aspect);
        }
        if(_height == 0) {
            double aspect = (double)w / (double)h;
            _height = (int)((double)_width / aspect);
        }
        bak_w = w;
        bak_h = h;
        w = _width;
        h = _height;
        
        // Set new viewport
        glstate_->viewport(0, 0, w, h);
    }
    
    QFramebufferObjectFormat format;
    format.setInternalTextureFormat(GL_RGBA);
    format.setTextureTarget(GL_TEXTURE_2D);
    // set the attachments as in the standard rendering
    format.setAttachment(QFramebufferObject::CombinedDepthStencil);
//    format.setAttachment(QFramebufferObject::CombinedDepthStencil);
    // 16 samples per pixel as we want a nice snapshot. If this is not supported
    // it will fall back to the maximal supported number of samples
    format.setSamples(samples);
    QFramebufferObject fb(w,h,format);    

    if ( fb.isValid() ){

      const GLuint prevFbo = ACG::GLState::getFramebufferDraw();

      makeCurrent();
      ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, fb.handle());
      
      // Turn alpha on if demanded
      ACG::Vec4f backColorBak;
      ACG::Vec4f newBack;
      
      bool formerCoordsysState = true;
      // Hide coordsys node if demanded
      if(_hideCoordsys) {
          // Find coordsys node
          ACG::SceneGraph::BaseNode* node = 0;
          node = sceneGraphRoot_->find("Core Coordsys Node");
          if(node != 0) {
              formerCoordsysState = node->visible();
              node->hide();
          } else {
              emit statusMessage(QString(tr("Could not find coordsys node, thus it will appear in the snapshot anyway.")));
          }
      }
      
      backColorBak = properties()->backgroundColor();

      newBack = ACG::Vec4f(backColorBak[0], backColorBak[1], backColorBak[2], (_alpha ? 0.0f : 1.0f));
      properties()->backgroundColor(newBack);

      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

      glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glEnable(GL_MULTISAMPLE);
      paintGL();
      glFinish();

      glDisable(GL_MULTISAMPLE);


      ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER_EXT, prevFbo);
      

      // Reset alpha settings
      if(_alpha)
          properties()->backgroundColor(backColorBak);
      
      if(_hideCoordsys) {
          // Find coordsys node
          ACG::SceneGraph::BaseNode* node = 0;
          node = sceneGraphRoot_->find("Core Coordsys Node");
          if(node != 0 && formerCoordsysState) {
              node->show();
          }
      }


       _image = fb.toImage().copy(0, 0, w, h);
    }
    
    if(_width != 0 || _height != 0) {
        // Reset viewport to former size
        glstate_->viewport(left, bottom, bak_w, bak_h);
    }
    
    glstate_->pop_modelview_matrix();
    glstate_->pop_projection_matrix();
}


void glViewer::snapshot( int _width, int _height, bool _alpha, bool _hideCoordsys, int samples)
{
   QImage image;
   
   // Capture image
   snapshot(image, _width, _height, _alpha, _hideCoordsys, samples);

   /*
    * Meta data
    */
   QString comments = PluginFunctions::collectObjectComments(true, false).join("\n");
   if (!comments.isEmpty())
       image.setText("Mesh Comments", comments);
   QString view;
   encodeView(view);
   image.setText("View", view);

   QFileInfo fi(properties_.snapshotName());

   QString fname = fi.path() + QDir::separator() +fi.baseName() + "." + QString::number(properties_.snapshotCounter()).rightJustified(7,'0') + "." + properties_.snapshotFileType().toLower();
   
   QImageWriter writer(fname);
   writer.setFormat(properties_.snapshotFileType().simplified().toLatin1());
   
   bool rval = writer.canWrite();
   if (rval)
     writer.write(image);
   
   if (rval)
     emit statusMessage (QString(tr("Snapshot: "))+fname,5000);
   else
     emit statusMessage (QString(tr("Could not save snapshot to ")) + fname + QString(tr(" Error: ")) + writer.errorString() );
}

void glViewer::slotHideWheels() {
  if (isVisible())
  {
    wheelX_->hide();
    wheelY_->hide();
    wheelZ_->hide();
  }
  else
  {
    show ();
    wheelX_->hide();
    wheelY_->hide();
    wheelZ_->hide();
    hide ();
  }
}

void glViewer::slotShowWheels() {
  if (isVisible())
  {
    wheelX_->show();
    wheelY_->show();
    wheelZ_->show();
  }
  else
  {
    show ();
    wheelX_->show();
    wheelY_->show();
    wheelZ_->show();
    hide ();
  }
}

bool glViewer::wheelsVisible() {
  // TODO: Return valid values
  return true;
}

//-----------------------------------------------------------------------------

void glViewer::slotClickTimeout()
{
  emit signalMouseEventClick (&clickEvent_, false);
}

//-----------------------------------------------------------------------------

void glViewer::setCursorPainter (CursorPainter *_cursorPainter)
{
  properties_.cursorPainter( _cursorPainter );
}

//-----------------------------------------------------------------------------

void glViewer::updateCursorPosition (QPointF _scenePos)
{
  if (!initialized_ || !sceneGraphRoot_ || !isVisible ())
    return;

  ACG::Vec3d tmp;

  unsigned int nodeIdx   = 0;
  unsigned int targetIdx = 0;


  // ignore cursor if we are outside of our window
  if (!mapRectToScene(boundingRect ()).intersects (properties_.cursorPainter()->cursorBoundingBox().translated(_scenePos)))
  {
    properties_.cursorPositionValid( false );
  }
  // only do real pick in stereo mode
  else if ( properties_.stereo() && OpenFlipperSettings().value("Core/Gui/glViewer/stereoMousePick",true).toBool() &&
            pick (ACG::SceneGraph::PICK_ANYTHING, _scenePos.toPoint(), nodeIdx, targetIdx, &tmp))
   {
    // the point we get back will contain the view transformation and we have to revert it
    properties_.cursorPoint3D(glstate_->modelview().transform_point(tmp));
    properties_.cursorPositionValid(true);
  }
  // only do real pick in stereo mode
  else
  {
    glstate_->push_modelview_matrix ();

    // reset modelview to ignore the view transformation
    glstate_->reset_modelview ();

    // Project the depth value of the stereo mode zero paralax plane.
    // We need to use this depth to to get the cursor exactly on zero paralax plane in stereo mode
    double zerop = properties_.nearPlane() + ((properties_.farPlane() - properties_.nearPlane()) * OpenFlipperSettings().value("Core/Stereo/FocalLength").toDouble() );
    ACG::Vec3d zerod = glstate_->project (ACG::Vec3d (0.0, 0.0, -zerop));

    // unproject the cursor into the scene
    properties_.cursorPoint3D( glstate_->unproject (ACG::Vec3d (_scenePos.x(), scene()->height () - _scenePos.y(), zerod[2])) );

    glstate_->pop_modelview_matrix();

    properties_.cursorPositionValid(true);
  }
}

//-----------------------------------------------------------------------------


void glViewer::moveForward() {
    if(navigationMode_ ==  FIRSTPERSON_NAVIGATION) {

        ACG::Vec3d dir = glstate_->viewing_direction();

        dir *= -0.1;

        glstate_->translate(dir[0], dir[1], dir[2]);

        updateGL();
        lastMoveTime_.restart();

        emit viewChanged();
    }
}

void glViewer::moveBack() {
    if(navigationMode_ ==  FIRSTPERSON_NAVIGATION) {
        ACG::Vec3d dir = glstate_->viewing_direction();

        dir *= 0.1;

        glstate_->translate(dir[0], dir[1], dir[2]);

        updateGL();
        lastMoveTime_.restart();

        emit viewChanged();
    }
}

void glViewer::strafeLeft() {
    if(navigationMode_ ==  FIRSTPERSON_NAVIGATION) {
        ACG::Vec3d dir = glstate_->right();

        dir *= 0.1;

        glstate_->translate(dir[0], dir[1], dir[2]);

        updateGL();
        lastMoveTime_.restart();

        emit viewChanged();
    }
}

void glViewer::strafeRight() {
    if(navigationMode_ ==  FIRSTPERSON_NAVIGATION) {
        ACG::Vec3d dir = glstate_->right();

        dir *= -0.1;

        glstate_->translate(dir[0], dir[1], dir[2]);

        updateGL();
        lastMoveTime_.restart();

        emit viewChanged();
    }
}


void glViewer::readBackBuffer(ACG::GLState* _glstate)
{
  GLint curFbo = 0;
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &curFbo);

  ACG::GLState::bindFramebuffer(GL_READ_FRAMEBUFFER, curFbo);
  ACG::GLState::bindFramebuffer(GL_DRAW_FRAMEBUFFER, readBackFbo_.getFboID());

  glBlitFramebuffer(0, 0, readBackFbo_.width(), readBackFbo_.height(), 
    0, 0, readBackFbo_.width(), readBackFbo_.height(), 
    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  checkGLError();

  ACG::GLState::bindFramebuffer(GL_FRAMEBUFFER, curFbo);
}


void glViewer::updatePostProcessingBufs(int _width, int _height)
{
  ACG::FBO* fbos[] = {postProcessFBO_, postProcessFBO_ + 1, &readBackFbo_};

  for (int i = 0; i < 3; ++i)
  {
    if (!fbos[i]->getFboID())
    {
      fbos[i]->init();

      fbos[i]->attachTexture2D(GL_COLOR_ATTACHMENT0, _width, _height, GL_RGBA, GL_RGBA, GL_CLAMP, GL_NEAREST, GL_NEAREST);
      fbos[i]->attachTexture2DDepth(_width, _height, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL);
    }
    else
      fbos[i]->resize(_width, _height);
  }
}

//=============================================================================
//=============================================================================

