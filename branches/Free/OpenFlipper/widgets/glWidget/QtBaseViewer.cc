/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
#include <ACG/GL/gl.hh>

#include <iostream>
#include <string>
#include <assert.h>

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
#include <QFileDialog>
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
#include <QGLFramebufferObjectFormat>
#include <QPainter>
#include <QPaintEngine>

#include <QImageWriter>

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

//== NAMESPACES ===============================================================



//== IMPLEMENTATION ==========================================================

static const char          VIEW_MAGIC[] =
  "ACG::QtWidgets::QGLViewerWidget encoded view";

//== IMPLEMENTATION ==========================================================


glViewer::glViewer( QGraphicsScene* _scene,
                    QGLWidget* _glWidget,
                    Viewer::ViewerProperties& _properties,
                    QGraphicsWidget* _parent) :
  QGraphicsWidget(_parent),
  glareaGrabbed_(false),
  projectionUpdateLocked_(false),
  blending_(true),
  glScene_(_scene),
  glWidget_(_glWidget),
  cursorPainter_(0),
  cursorPositionValid_(false),
  pProgram_(0),
  philipsStereoInitialized_(false),
  colorTextureBufferWidth_(0),
  colorTextureBufferHeight_(0),
  depthStencilTextureBufferWidth_(0),
  depthStencilTextureBufferHeight_(0),
  pickCache_(0),
  updatePickCache_(true),
  pickCacheSupported_(true),
  clickEvent_(QEvent::MouseButtonPress, QPoint (), Qt::NoButton, Qt::NoButton, Qt::NoModifier),
  properties_(_properties),
  glstate_(0),
  initialized_(false)
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

  // stereo
  stereo_ = false;

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

  allowRotation_ = true;


  connect( &properties_,SIGNAL(updated()), this, SLOT( slotPropertiesUpdated() ) );


  //check for updated properties once
  slotPropertiesUpdated();

  setAcceptDrops(true);

  setHome();

  // initialize custom anaglyph stereo
  agTexWidth_ = 0;
  agTexHeight_ = 0;
  agTexture_[0] = 0;
  agTexture_[1] = 0;
  agProgram_ = 0;
  customAnaglyphSupported_ = false;

  clickTimer_.setSingleShot (true);
  connect (&clickTimer_, SIGNAL(timeout ()), this, SLOT(slotClickTimeout ()));
}


//-----------------------------------------------------------------------------


glViewer::~glViewer()
{
  finiCustomAnaglyphStereo ();
  delete glstate_;
}


//-----------------------------------------------------------------------------


QSize
glViewer::sizeHint() const
{
  return QSize( 600, 600 );
}

//-----------------------------------------------------------------------------


void glViewer::makeCurrent() {
  glWidget_->makeCurrent();
}

void glViewer::swapBuffers() {
  glWidget_->swapBuffers();
}


//-----------------------------------------------------------------------------


void glViewer::sceneGraph(ACG::SceneGraph::BaseNode* _root, const bool _resetTrackBall)
{
  sceneGraphRoot_ = _root;

  if (sceneGraphRoot_ )
  {
    
    // set max number of render pass
    // Single pass action, this info is static during multipass
    ACG::SceneGraph::MultiPassInfoAction info_act;
    ACG::SceneGraph::traverse(sceneGraphRoot_, info_act);
    
    glstate_->set_max_render_passes(info_act.getMaxPasses());
    
    // get scene size
    // Single pass action, as the bounding box is not influenced by multipass traversal
    ACG::SceneGraph::BoundingBoxAction act;
    ACG::SceneGraph::traverse(sceneGraphRoot_, act);

    ACG::Vec3d bbmin = (ACG::Vec3d) act.bbMin();
    ACG::Vec3d bbmax = (ACG::Vec3d) act.bbMax();
    
    

    if ( ( bbmin[0] > bbmax[0] ) ||
         ( bbmin[1] > bbmax[1] ) ||
         ( bbmin[2] > bbmax[2] )   ) {
      
      // Invalid bounding box, try to recover
      setScenePos( ACG::Vec3d( 0.0,0.0,0.0 ) , 1.0, _resetTrackBall );
    
      // Update bounding box to match the scene geometry after recovery
      bbmin = ACG::Vec3d(-1.0,-1.0,-1.0);
      bbmax = ACG::Vec3d( 1.0, 1.0, 1.0);
    } else {
      
      // For very small scenes, we set the scene radius to 0.1
      // otherwise we take the real radius
      if ( ( bbmax - bbmin ).max() < OpenFlipperSettings().value("Core/Gui/glViewer/minimalSceneSize",0.1).toDouble() )  {
        setScenePos( ( bbmin + bbmax )        * 0.5,
                     OpenFlipperSettings().value("Core/Gui/glViewer/minimalSceneSize",0.1).toDouble(),
                     _resetTrackBall);
                   
      } else {
        setScenePos( ( bbmin + bbmax )        * 0.5,
                     ( bbmax - bbmin ).norm() * 0.5,
                     _resetTrackBall); 
      }
                   
    }
                   
    // remember the new bounding box for the state
    glstate_->set_bounding_box(bbmin,bbmax);
    
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

  // In stereo mode we have to use a perspective matrix
  if (stereo_ || projectionMode_ == PERSPECTIVE_PROJECTION)
  {
    double aspect;

    if (isVisible() && glWidth() && glHeight())
      aspect = (double) glWidth() / (double) glHeight();
    else
      aspect = 1.0;

    // Get fovy
    double fovy = OpenFlipperSettings().value("Core/Projection/FOVY", 45.0).toDouble();

    glstate_->perspective(fovy, (GLdouble) aspect,
                          properties_.nearPlane(), properties_.farPlane());
  }
  else
  {
    double aspect;

    if (isVisible() && glWidth() && glHeight())
      aspect = (double) glWidth() / (double) glHeight();
    else
      aspect = 1.0;

    glstate_->ortho( -properties_.orthoWidth(), properties_.orthoWidth(),
                     -properties_.orthoWidth()/aspect, properties_.orthoWidth()/aspect,
                      properties_.nearPlane(), properties_.farPlane() );
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

  // Set far plane
  properties_.farPlane( std::max(0.0002f * properties_.sceneRadius(),  -(c[2] - properties_.sceneRadius() )) );

  // Set near plane
  properties_.nearPlane( std::max(0.0001f * properties_.sceneRadius(),  -(c[2] + properties_.sceneRadius())) );
  
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
      glDisable(GL_NORMALIZE);
      break;

    case NORMALIZE_NORMALS:
      glEnable(GL_NORMALIZE);
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

//    qApp->processEvents();
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

  // Set far plane
  properties_.farPlane( std::max(0.0002f * properties_.sceneRadius(),  -(c[2] - properties_.sceneRadius())) );

  // Set near plane
  properties_.nearPlane( std::max(0.0001f * properties_.sceneRadius(),  -(c[2] + properties_.sceneRadius())) );

  updateProjectionMatrix();

  // store time since last repaint in gl state and restart timer
  glstate_->set_msSinceLastRedraw (redrawTime_.restart ());

  // draw mono or stereo
  makeCurrent();
  if (stereo_) drawScene_stereo();
  else         drawScene_mono();

  
  
  glFinish();
  frame_time_ = timer.elapsed();

  
  // Inside the glWidget rendering, the system should not send extra updates
  properties_.blockSignals(false);

}


//-----------------------------------------------------------------------------


void glViewer::drawScene_mono()
{
  if (sceneGraphRoot_)
  {
    if (! properties_.renderPicking() ) {

      ViewObjectMarker *oM = properties_.objectMarker();
      GLuint refBits = 0;
      QSet<GLuint> references;

      if (oM)
      {
        glClear (GL_STENCIL_BUFFER_BIT);
        glEnable (GL_STENCIL_TEST);
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

      ACG::SceneGraph::DrawAction action( properties_.drawMode(), *glstate_ , false);
      ACG::SceneGraph::traverse_multipass(sceneGraphRoot_, action, *glstate_, properties_.drawMode() );

      if( blending_ )
      {
        ACG::SceneGraph::DrawAction action(properties_.drawMode(), *glstate_, true);
        ACG::SceneGraph::traverse_multipass(sceneGraphRoot_, action, *glstate_, properties_.drawMode());
      }

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

        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_DITHER);

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

          glBlendFunc (sfactor, dfactor);
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
        glDisable (GL_STENCIL_TEST);
      }
    } else {

      // prepare GL state
      makeCurrent();

      glDisable(GL_LIGHTING);
      glDisable(GL_BLEND);
      glClear(GL_DEPTH_BUFFER_BIT);
      glInitNames();
      glPushName((GLuint) 0);

      // do the picking
      glstate_->pick_init (true);
      ACG::SceneGraph::PickAction action(*glstate_, properties_.renderPickingMode(), properties_.drawMode());
      ACG::SceneGraph::traverse_multipass(sceneGraphRoot_, action,*glstate_);

      glEnable(GL_LIGHTING);
      glEnable(GL_BLEND);
    }
    
    // Render depth?
    // If so, we take our depth buffer and draw it as our scene image
    if ( properties_.renderDepthImage() ) {

      // ======================================================================================================
      // Adjust buffer to correct size
      // ======================================================================================================
      updateDepthStencilTextureBuffer();
      
      // ======================================================================================================
      // Get current viewport size
      // ======================================================================================================
      int vp_l, vp_b, vp_w, vp_h;
      glstate_->get_viewport (vp_l, vp_b, vp_w, vp_h);
          
      // ======================================================================================================
      // Bind depth Stencil texture
      // ======================================================================================================
      pDepthStencilTexture_.enable();
      pDepthStencilTexture_.bind();

      // ======================================================================================================
      // Copy depth component of rendered image to texture
      // ======================================================================================================
      glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8_EXT, 0, 0, vp_w , vp_h, 0);
      
      // ======================================================================================================
      // Render plain textured
      // ======================================================================================================
      glDisable(GL_LIGHTING);
      glDisable(GL_COLOR_MATERIAL);
      glDisable(GL_DEPTH_TEST);
      
      // ======================================================================================================
      // Setup orthogonal projection
      // ======================================================================================================
      glstate_->push_projection_matrix();
      glstate_->push_modelview_matrix();
      
      glstate_->reset_projection();
      glstate_->reset_modelview();
      
      glstate_->ortho(0, vp_w, 0, vp_h, 0, 1);
      
      // ======================================================================================================
      // Clear rendering buffer
      // ======================================================================================================
      glClearColor(0.0, 0.0, 0.0, 0.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      // ======================================================================================================
      // Render a simple quad (rest is done by the texture)
      // ======================================================================================================
      glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(0, 0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2i(vp_w, 0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2i(vp_w, vp_h);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2i(0, vp_h);
      glEnd();
      
      // Disable depth stencil buffer
      pDepthStencilTexture_.disable();
      
      // ======================================================================================================
      // Reset projection and modelview
      // ======================================================================================================
      glstate_->pop_projection_matrix();
      glstate_->pop_modelview_matrix();
      
    }
    
    
  }

  if (cursorPainter_ && cursorPainter_->enabled () && cursorPositionValid_)
  {
    glstate_->push_modelview_matrix ();
    // reset view transformation
    glstate_->reset_modelview ();
    // translate cursor position to 0,0
    glstate_->translate (cursorPoint3D_[0], cursorPoint3D_[1], cursorPoint3D_[2]);
    // paint cursor
    cursorPainter_->paintCursor (glstate_);
    glstate_->pop_modelview_matrix ();
  }
}


//-----------------------------------------------------------------------------


void
glViewer::drawScene_stereo()
{
  if (OpenFlipper::Options::stereoMode () == OpenFlipper::Options::OpenGL && OpenFlipper::Options::glStereo ())
  {
    drawScene_glStereo ();
    return;
  }
  else if (OpenFlipper::Options::stereoMode () == OpenFlipper::Options::AnaglyphCustom && customAnaglyphSupported_)
  {
    drawScene_customAnaglyphStereo ();

    // if somthing went wrong, fallback to normal anaglyph
    if (customAnaglyphSupported_)
      return;
  } else if (OpenFlipper::Options::stereoMode () == OpenFlipper::Options::Philips )
  {
    drawScenePhilipsStereo ();

    return;

  }



  drawScene_anaglyphStereo ();
}


//-----------------------------------------------------------------------------


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

  sceneGraph(PluginFunctions::getSceneGraphRootNode(), true);

  properties_.unLockUpdate();
  updateProjectionMatrix();
  updateGL();

  emit viewChanged();

}


//-----------------------------------------------------------------------------


void glViewer::flyTo(const QPoint& _pos, bool _move_back)
{
  makeCurrent();

  unsigned int nodeIdx, targetIdx;
  ACG::Vec3d hitPoint;

  if (pick( ACG::SceneGraph::PICK_ANYTHING, _pos, nodeIdx, targetIdx, &hitPoint))
  {
    if (projectionMode_ == PERSPECTIVE_PROJECTION)
    {
      ACG::Vec3d eye(glstate_->eye());
      ACG::Vec3d t = hitPoint - eye;
      ACG::Vec3d e = eye + t * (_move_back ? -0.5f : 0.5f);
      flyTo(e, hitPoint, 300);
    }
    else
    {

      // Project hitpoint to get depth
      ACG::Vec3d hitPointProjected = glstate_->project(hitPoint);

      // Create projected center point with same depth as hitpoint
      ACG::Vec3d centerPointProjected = hitPointProjected;
      centerPointProjected[0] = glstate_->viewport_width() / 2.0 ;
      centerPointProjected[1] = glstate_->viewport_height() / 2.0 ;

      // unproject center point
      ACG::Vec3d centerPointUnProjected = glstate_->unproject(centerPointProjected);
      
      // translation vector to make hitpoint project to center point (both need same depth)
      ACG::Vec3d t = hitPoint - centerPointUnProjected;

      // Transform to correct translation vector with modelview.
      t = glstate_->modelview().transform_vector(t);
      
      // originalWidth 
      double orthoWidthOriginal = properties_.orthoWidth();

      // Set the double click point as the new trackball center
      // Rotations will use this point as the center.
      properties_.trackballCenter( hitPoint );
      
      // how many frames in _time ms ?
      unsigned int  frames = (unsigned int)(300 / frame_time_);
      if (frames > 1000) frames=1000;
      
      // animate it
      if (frames > 10)
      {
        
        for (unsigned int i=1; i<frames; ++i)
        {
          // zoom back one frame 
          if ( _move_back ) {
            // Move back by factor 2
            properties_.orthoWidth( orthoWidthOriginal * (1.0 + 1.0 / (double)frames * i ) );
          } else
            // Move forward with factor 0.5
            properties_.orthoWidth( orthoWidthOriginal * (1.0 - 0.5 / (double)frames * i ) );
          
          // apply translation
          translate(t * (- 1.0 / (double)frames  ) );
          
          update();
          qApp->processEvents();
        }
        updatePickCache_ = true;
      } else {
        
        // direct translation
        translate(-t);
        
        // set the zoom factor when no animation is performed
        if ( _move_back )
          properties_.orthoWidth( properties_.orthoWidth() * 2.0 );
        else
          properties_.orthoWidth(properties_.orthoWidth() * -0.5);
        
      }

      // Update the projection matrix
      updateProjectionMatrix();

      // Redraw scene
      updateGL();

      emit viewChanged();
    }


  }
}


void glViewer::flyTo(const ACG::Vec3d&  _position,
			 const ACG::Vec3d&  _center,
			 double        _time)
{
  makeCurrent();

  // compute rotation
  ACG::Vec3d c = glstate_->modelview().transform_point(_center);
  ACG::Vec3d p = glstate_->modelview().transform_point(_position);
  ACG::Vec3d view =(p-c).normalize();
  ACG::Vec3d z(0,0,1);
  ACG::Vec3d axis = (z % -view).normalize();
  double angle = acos(std::max(-1.0,
			      std::min(1.0,
				       (z | view)))) / M_PI * 180.0;

  if (angle > 175)
    axis  = ACG::Vec3d(0,1,0);


  // compute translation
  ACG::Vec3d target = glstate_->modelview().transform_point(_center);
  ACG::Vec3d trans ( -target[0],
		-target[1],
		-target[2] - (_position-_center).norm() );



  // how many frames in _time ms ?
  unsigned int  frames = (unsigned int)(_time / frame_time_);
  if (frames > 1000) frames=1000;




  // animate it
  if (frames > 10)
  {
    ACG::Vec3d t = trans / (double)frames;
    double a = angle / (double)frames;

    for (unsigned int i=0; i<frames; ++i)
    {
      translate(t);
      if (fabs(a) > FLT_MIN)
	     rotate(axis, a, _center);

      update();
      qApp->processEvents();
    }
    updatePickCache_ = true;
  }


  // no animation
  else
  {
    translate(trans);
    if (fabs(angle) > FLT_MIN)
      rotate(axis, angle, _center);

    updateGL();
  }


  properties_.trackballCenter( _center );
  properties_.trackballRadius( std::max( properties_.sceneRadius(),( _center - _position ).norm() * 0.9f  ) );
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
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glDisable(GL_DITHER);
  glShadeModel( GL_FLAT );


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

  customAnaglyphSupported_ = ACG::checkExtensionSupported("GL_ARB_fragment_program") &&
                            (ACG::checkExtensionSupported("GL_ARB_texture_rectangle") ||
                             ACG::checkExtensionSupported("GL_EXT_texture_rectangle") ||
                             ACG::checkExtensionSupported("GL_NV_texture_rectangle"));

  initialized_ = true;

  if (sceneGraphRoot_)
  {
    sceneGraph(sceneGraphRoot_, true);
    viewAll ();
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glShadeModel( GL_FLAT );

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();


    normalsMode(      normalsMode_      );

    applyProperties();

    glstate_->setState ();

    glColor4f(1.0,0.0,0.0,1.0);

    if (properties_.renderPicking())
    {
      clear_color = properties_.glState().clear_color();
      properties_.glState().set_clear_color (ACG::Vec4f (0.0, 0.0, 0.0, 1.0));
    }

    // clear (stereo mode clears buffers on its own)
    if (!stereo_)
      glstate_->clearBuffers ();

    properties_.unLockUpdate();

    // draw scene
    drawScene();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopAttrib ();

    if (properties_.renderPicking())
      properties_.glState().set_clear_color (clear_color);
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

void glViewer::encodeView(QString& _view)
{
  // Get current matrices
  const ACG::GLMatrixd m = glstate_->modelview();
  const ACG::GLMatrixd p = glstate_->projection();

  // Add modelview matrix to output
  _view += QString(VIEW_MAGIC) + "\n";
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
  _view += QString::number(glWidth()) + " " +  QString::number(glHeight()) + " " + QString::number(projectionMode_) + " " + QString::number(properties_.orthoWidth()) + "\n";
}


//----------------------------------------------------------------------------


bool glViewer::decodeView(const QString& _view)
{
  if (_view.left(sizeof(VIEW_MAGIC)-1) != QString(VIEW_MAGIC))
    return false;

  // Remove the magic from the string
  QString temp = _view;
  temp.remove(0,sizeof(VIEW_MAGIC));

  //Split it into its components
  QStringList split = temp.split(QRegExp("[\\n\\s]"),QString::SkipEmptyParts);

  ACG::GLMatrixd m, p;
  int            w, h, pMode;

  // Check if the number of components matches the expected size
  if ( split.size() != 36 ) {
    std::cerr << "Unable to paste view ... wrong parameter count!! is" <<  split.size()  << std::endl;
    return false;
  }

  // Parse the components
  bool ok = true;;

  m(0,0) = split[0].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(0,1) = split[1].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(0,2) = split[2].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(0,3) = split[3].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(1,0) = split[4].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(1,1) = split[5].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(1,2) = split[6].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(1,3) = split[7].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(2,0) = split[8].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(2,1) = split[9].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(2,2) = split[10].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(2,3) = split[11].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(3,0) = split[12].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(3,1) = split[13].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(3,2) = split[14].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  m(3,3) = split[15].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(0,0) = split[16].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(0,1) = split[17].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(0,2) = split[18].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(0,3) = split[19].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(1,0) = split[20].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(1,1) = split[21].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(1,2) = split[22].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(1,3) = split[23].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(2,0) = split[24].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(2,1) = split[25].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(2,2) = split[26].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(2,3) = split[27].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(3,0) = split[28].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(3,1) = split[29].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(3,2) = split[30].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  p(3,3) = split[31].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }

  w =  split[32].toInt(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  h =  split[33].toInt(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  pMode =  split[34].toInt(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }
  properties_.orthoWidth( split[35].toDouble(&ok) ); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }

  // Switch to our gl context
  makeCurrent();

  // set projection mode
  if (projectionMode_ != (ProjectionMode)pMode)
    projectionMode((ProjectionMode)pMode);

  // Apply new modelview matrix
  glstate_->set_modelview(m);


  std::cerr << "Todo : Add Checkbox if size should also be pasted" << std::endl;

//   if (w>0 && h>0 &&
//       action_["PasteDropSize"]->isChecked() )
//   {
//     glstate_->set_projection(p);
//     glView_->setFixedSize(w,h);
//     updateGeometry();
//   }

  updateGL();

  return true;
}


//-----------------------------------------------------------------------------


void glViewer::actionCopyView()
{
  QString view; encodeView(view);
  QApplication::clipboard()->setText(view);
}


//-----------------------------------------------------------------------------


void glViewer::actionPasteView()
{
  QString view; view=QApplication::clipboard()->text();
  decodeView(view);
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

  if (cursorPainter_) {
    cursorPainter_->setCursor(Qt::BlankCursor);
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

  if (cursorPainter_) {
    cursorPainter_->setCursor(Qt::ArrowCursor);
    std::cerr << "grabGLArea: Setting cursorPainter cursor to arrow" << std::endl;
  } else {
    setCursor(Qt::ArrowCursor);
    std::cerr << "grabGLArea: Setting qt cursor to arrow" << std::endl;
  }
}


//-----------------------------------------------------------------------------


void glViewer::contextMenuEvent(QGraphicsSceneContextMenuEvent* _e)
{
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
  if (_e->button() != Qt::RightButton )
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
    if (allowRotation_)
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

      lastPoint_hitSphere_ = mapToSphere(lastPoint2D_ = pos, lastPoint3D_);
      isRotating_ = true;
      timer_->stop();

      break;
    }

    case QEvent::MouseButtonDblClick: {
      if (allowRotation_)
          flyTo(_event->pos(), _event->button() == Qt::MidButton);
      break;
    }

    case QEvent::MouseMove: {
      double factor = 1.0;

      if (_event->modifiers() == Qt::ShiftModifier)
          factor = properties_.wheelZoomFactorShift();

      // mouse button should be pressed
      if (_event->buttons() & (Qt::LeftButton | Qt::MidButton)) {
        QPoint newPoint2D = pos;

        double value_x, value_y;
        ACG::Vec3d newPoint3D;
        bool newPoint_hitSphere = mapToSphere(newPoint2D, newPoint3D);

        makeCurrent();

        // Left and middle button are pressed:
        // Translate along image planes normal direction -> zoom
        if ((_event->buttons() & Qt::LeftButton) && (_event->buttons() & Qt::MidButton)) {
          switch (projectionMode()) {
            case PERSPECTIVE_PROJECTION: {
                value_y = properties_.sceneRadius() * ((newPoint2D.y() - lastPoint2D_.y())) * 3.0 / (double) glHeight();
                translate(ACG::Vec3d(0.0, 0.0, value_y * factor));
                updateGL();
                emit viewChanged();
                break;
            }

            case ORTHOGRAPHIC_PROJECTION: {
                value_y = ((newPoint2D.y() - lastPoint2D_.y())) * properties_.orthoWidth() / (double) glHeight();
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
          if ( startDepth_ == 1 ) {
            
            value_x = properties_.sceneRadius() * ((newPoint2D.x() - lastPoint2D_.x())) * 2.0 / (double) glWidth();
            value_y = properties_.sceneRadius() * ((newPoint2D.y() - lastPoint2D_.y())) * 2.0 / (double) glHeight();
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
  double factor = properties_.wheelZoomFactor();

  if (_event->modifiers() == Qt::ShiftModifier)
    factor = properties_.wheelZoomFactorShift();

  if (projectionMode() == PERSPECTIVE_PROJECTION || stereo_)
  {
    double d = -(double)_event->delta() / 120.0 * 0.2 * factor * properties_.trackballRadius() / 3.0;
    translate( ACG::Vec3d(0.0, 0.0, d) );
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
  static int msecs=0, count=0;

  makeCurrent();
  rotate( lastRotationAxis_, lastRotationAngle_ );
  updateGL();

  if (!properties_.updateLocked()) {
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
    glEnable( GL_CULL_FACE );
  else
    glDisable( GL_CULL_FACE );

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
    
    QGLFramebufferObjectFormat format;
    format.setInternalTextureFormat(GL_RGBA);
    format.setTextureTarget(GL_TEXTURE_2D);
    // set the attachments as in the standard rendering
    format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
    // 16 samples per pixel as we want a nice snapshot. If this is not supported
    // it will fall back to the maximal supported number of samples
    format.setSamples(samples);
    QGLFramebufferObject fb( w, h, format);
    
    
    if ( fb.isValid() ){

      fb.bind();
      qApp->processEvents();
      makeCurrent();
      
      // Turn alpha on if demanded
      ACG::Vec4f backColorBak;
      ACG::Vec4f newBack;
      
      // Hide coordsys node if demanded
      if(_hideCoordsys) {
          // Find coordsys node
          ACG::SceneGraph::BaseNode* node = 0;
          node = sceneGraphRoot_->find("Core Coordsys Node");
          if(node != 0) {
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
      paintGL();
      glFinish();
      
      // Reset alpha settings
      if(_alpha)
          properties()->backgroundColor(backColorBak);
      
      if(_hideCoordsys) {
          // Find coordsys node
          ACG::SceneGraph::BaseNode* node = 0;
          node = sceneGraphRoot_->find("Core Coordsys Node");
          if(node != 0) {
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


void glViewer::snapshot( int _width, int _height, bool _alpha, bool _hideCoordsys)
{
   QImage image;
   
   // Capture image
   snapshot(image, _width, _height, _alpha, _hideCoordsys);
   
   QFileInfo fi(properties_.snapshotName());

   QString fname = fi.path() + QDir::separator() +fi.baseName() + "." + QString::number(properties_.snapshotCounter()) + "." + properties_.snapshotFileType().toLower();
   
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
  cursorPainter_ = _cursorPainter;
}

//-----------------------------------------------------------------------------

void glViewer::updateCursorPosition (QPointF _scenePos)
{
  if (!initialized_ || !sceneGraphRoot_ || !isVisible ())
    return;

  unsigned int nodeIdx, targetIdx;
  ACG::Vec3d tmp;

  // ignore cursor if we are outside of our window
  if (!mapRectToScene(boundingRect ()).intersects (cursorPainter_->cursorBoundingBox().translated(_scenePos)))
  {
    cursorPositionValid_ = false;
  }
  // only do real pick in stereo mode
  else if (stereo_ && OpenFlipperSettings().value("Core/Gui/glViewer/stereoMousePick",true).toBool() &&
           pick (ACG::SceneGraph::PICK_ANYTHING, _scenePos.toPoint(), nodeIdx, targetIdx, &tmp))
  {
    // the point we get back will contain the view transformation and we have to revert it
    cursorPoint3D_ = glstate_->modelview ().transform_point (tmp);

    cursorPositionValid_ = true;
  }
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
    tmp = glstate_->unproject (ACG::Vec3d (_scenePos.x(), scene()->height () - _scenePos.y(), zerod[2]));
    cursorPoint3D_ = tmp;
    glstate_->pop_modelview_matrix ();
    cursorPositionValid_ = true;
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

//=============================================================================
//=============================================================================

