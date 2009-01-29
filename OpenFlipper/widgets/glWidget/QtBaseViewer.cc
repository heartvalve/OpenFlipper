//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 4430 $
//   $Author: moebius $
//   $Date: 2009-01-23 17:14:32 +0100 (Fr, 23. Jan 2009) $
//
//=============================================================================




//=============================================================================
//
//  CLASS glViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtBaseViewer.hh"
#include "QtGLGraphicsScene.hh"
#include "QtGLGraphicsView.hh"
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
#include <QStatusBar>
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

#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <QGraphicsProxyWidget>

#ifdef max
#  undef max
#endif

#ifdef min
#  undef min
#endif


//== NAMESPACES ===============================================================



//== IMPLEMENTATION ==========================================================

static const char          VIEW_MAGIC[] =
  "ACG::QtWidgets::QGLViewerWidget encoded view";

//== IMPLEMENTATION ==========================================================


glViewer::glViewer( QWidget* _parent,
			    const char* /* _name */ ,
			    QStatusBar *_statusBar,
			    const QGLFormat* _format,
			    const glViewer* _share) :
  QWidget(_parent),
  statusbar_(_statusBar),
  glareaGrabbed_(false),
  projectionUpdateLocked_(false),
  blending_(true),
  pick_mode_name_(""),
  pick_mode_idx_(-1),
  glstate_(0)
{
  // check for OpenGL support
  if ( !QGLFormat::hasOpenGL() )
  {
    std::cerr << "This system has no OpenGL support.\n";
    exit(1);
  }

  // widget stuff
  createWidgets(_format,_statusBar,_share);

  // bind GL context to GL state class
  glstate_ = new ACG::GLState();
  properties_.setglState( glstate_ );

  // state
  orthoWidth_       = 2.0;
  isRotating_       = false;
  near_             = 0.1;
  far_              = 100.0;
  fovy_             = 45.0;

  focalDist_        = 3.0;
  eyeDist_          = 0.01;

  sceneGraphRoot_   = 0;
  curDrawMode_      = ACG::SceneGraph::DrawModes::NONE;
  availDrawModes_   = ACG::SceneGraph::DrawModes::NONE;

  normalsMode_      = DONT_TOUCH_NORMALS;
  projectionMode_   = PERSPECTIVE_PROJECTION;


  light_matrix_.identity();

  trackMouse_ = false;

  // stereo
  stereo_ = false;


  pickMenu_ = 0;
  drawMenu_ = 0;


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

  //default wheel zoom factors
  wZoomFactor_ = 1.0;
  wZoomFactorShift_ = 0.2;

  connect( &properties_,SIGNAL(updated()), this, SLOT( slotPropertiesUpdated() ) );
  connect( &properties_,SIGNAL(actionModeChanged(Viewer::ActionMode)), this, SLOT( updateActionMode(Viewer::ActionMode) ) );

  properties_.setExamineMode();

}


//-----------------------------------------------------------------------------


glViewer::~glViewer()
{
  delete glstate_;
}


//-----------------------------------------------------------------------------


QSize
glViewer::sizeHint() const
{
  return QSize( 600, 600 );
}


//-----------------------------------------------------------------------------


void glViewer::setStatusBar(QStatusBar* _sb)
{
  statusbar_ = _sb;
}

//-----------------------------------------------------------------------------


void glViewer::makeCurrent() {
  glWidget_->makeCurrent();
}

void glViewer::swapBuffers() {
  glWidget_->swapBuffers();
}


//-----------------------------------------------------------------------------


void glViewer::sceneGraph(ACG::SceneGraph::BaseNode* _root)
{
  sceneGraphRoot_ = _root;

  if (sceneGraphRoot_)
  {
    // get draw modes
    ACG::SceneGraph::CollectDrawModesAction action;
    ACG::SceneGraph::traverse(sceneGraphRoot_, action);
    availDrawModes_ = action.drawModes();
    updatePopupMenu();

    // get scene size
    ACG::SceneGraph::BoundingBoxAction act;
    ACG::SceneGraph::traverse(sceneGraphRoot_, act);

    ACG::Vec3d bbmin = (ACG::Vec3d) act.bbMin();
    ACG::Vec3d bbmax = (ACG::Vec3d) act.bbMax();

    if ( ( bbmin[0] > bbmax[0] ) ||
         ( bbmin[1] > bbmax[1] ) ||
         ( bbmin[2] > bbmax[2] )   )
      setScenePos( ACG::Vec3d( 0.0,0.0,0.0 ) , 1.0 );
    else
      setScenePos( ( bbmin + bbmax )        * 0.5,
                   ( bbmax - bbmin ).norm() * 0.5 );
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
}


void glViewer::updateProjectionMatrix()
{
  if( projectionUpdateLocked_ )
    return;

  makeCurrent();

  glstate_->reset_projection();

  switch (projectionMode_)
  {
    case ORTHOGRAPHIC_PROJECTION:
    {
      double aspect = (double) glWidth() / (double) glHeight();
      glstate_->ortho( -orthoWidth_, orthoWidth_,
		       -orthoWidth_/aspect, orthoWidth_/aspect,
		       near_, far_ );
      break;
    }

    case PERSPECTIVE_PROJECTION:
    {
      glstate_->perspective(fovy_,
			    (GLdouble) glWidth() / (GLdouble) glHeight(),
			    near_, far_);
      break;
    }
  }
}


//-----------------------------------------------------------------------------


void glViewer::setScenePos(const ACG::Vec3d& _center, double _radius)
{
  scene_center_ = trackball_center_ = _center;
  scene_radius_ = trackball_radius_ = _radius;

  orthoWidth_ = 2.0   * scene_radius_;
  near_       = 0.001 * scene_radius_;
  far_        = 10.0  * scene_radius_;

  updateProjectionMatrix();
  updateGL();
}


//----------------------------------------------------------------------------


void glViewer::viewingDirection( const ACG::Vec3d& _dir, const ACG::Vec3d& _up )
{
  // calc eye point for this direction
  ACG::Vec3d eye = scene_center_ - _dir*(3.0*scene_radius_);

  glstate_->reset_modelview();
  glstate_->lookAt((ACG::Vec3d)eye, (ACG::Vec3d)scene_center_, (ACG::Vec3d)_up);
}


//-----------------------------------------------------------------------------


void glViewer::updateActionMode(Viewer::ActionMode _am)
{

  trackMouse(false);


  switch ( properties_.actionMode() )
  {
    case Viewer::ExamineMode:
    {
      glView_->setCursor(Qt::PointingHandCursor);
      glBase_->setCursor(Qt::PointingHandCursor);
      break;
    }


    case Viewer::LightMode:
    {
      glView_->setCursor(Qt::PointingHandCursor);
      glBase_->setCursor(Qt::PointingHandCursor);
      break;
    }


    case Viewer::PickingMode:
    {
      glView_->setCursor(Qt::ArrowCursor);
      glBase_->setCursor(Qt::ArrowCursor);
      if (pick_mode_idx_ != -1) {
	     trackMouse(pick_modes_[pick_mode_idx_].tracking);
        glView_->setCursor(pick_modes_[pick_mode_idx_].cursor);
	     glBase_->setCursor(pick_modes_[pick_mode_idx_].cursor);
      }

      break;
    }


    case Viewer::QuestionMode:
    {
      glView_->setCursor(Qt::WhatsThisCursor);
      glBase_->setCursor(Qt::WhatsThisCursor);
      break;
    }
  }

  //emit pickmodeChanged with either the name of the current pickmode or an empty string
  if( properties_.pickingMode() )
    emit(signalPickModeChanged(pick_mode_name_));
  else
    emit(signalPickModeChanged(""));
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
			   unsigned int /* _l */ , unsigned int /* _t */ ,
			   unsigned int /* _w */ , unsigned int /* _h */ ,
			   GLenum /* _buffer */ )
{
  makeCurrent();
  _image = glWidget_->grabFrameBuffer(true);
}


//-----------------------------------------------------------------------------


void glViewer::drawNow()
{
  makeCurrent();
  paintGL();
  swapBuffers();
  glView_->repaint();
}

void glViewer::updateGL()
{
  if (!properties_.updateLocked() && !isHidden() )
  {
    glScene_->update();
  }
}



//-----------------------------------------------------------------------------


void glViewer::drawScene()
{
  QTime  timer;
  timer.start();


  // *****************************************************************
  // Adjust clipping planes
  // *****************************************************************
  // Far plane
  ACG::Vec3d c = glstate_->modelview().transform_point(scene_center_);

  // Set far plane
  far_    = std::max(0.0002f * scene_radius_,  -(c[2] - scene_radius_));

  // Set near plane
  near_   = std::max(0.0001f * scene_radius_,  -(c[2] + scene_radius_));

  // measure distance from scene center ( as projection onto the z-Axis )
//   if ( -c[2] < scene_radius_ ) {
//     std::cerr << "Camera in scene radius" << std::endl;
//
//   }
//
//   std::cerr << "-c[2]   : " << -c[2] << std::endl;
//   std::cerr << "radius  : " << scene_radius_ << std::endl;
//   std::cerr << "z-range : " << far_ - near_ << std::endl;
//   std::cerr << "Near    : " << near_ << std::endl;
//   std::cerr << "Far     : " << far_ << std::endl;
//   near_   = std::max(far_ / 256.0f,  -(c[2] + scene_radius_));

  updateProjectionMatrix();

  // store time since last repaint in gl state and restart timer
  glstate_->set_msSinceLastRedraw (redrawTime_.restart ());

  // draw mono or stereo
  makeCurrent();
  if (stereo_) drawScene_stereo();
  else         drawScene_mono();


  glFinish();
  frame_time_ = timer.elapsed();
}


//-----------------------------------------------------------------------------


void glViewer::drawScene_mono()
{
  if (sceneGraphRoot_)
  {
    if (! properties_.renderPicking() ) {
      ACG::SceneGraph::DrawAction action(curDrawMode_, false);
      ACG::SceneGraph::traverse(sceneGraphRoot_, action, *glstate_, curDrawMode_);

      if( blending_ )
      {
        ACG::SceneGraph::DrawAction action(curDrawMode_, true);
        ACG::SceneGraph::traverse(sceneGraphRoot_, action, *glstate_, curDrawMode_);
      }
    } else {

      // prepare GL state
      makeCurrent();

      glDisable(GL_LIGHTING);
      glClear(GL_DEPTH_BUFFER_BIT);
      glInitNames();
      glPushName((GLuint) 0);

      // do the picking
      ACG::SceneGraph::PickAction action(properties_.renderPickingMode());
      ACG::SceneGraph::traverse(sceneGraphRoot_, action, *glstate_);

      glEnable(GL_LIGHTING);
    }
  }
}


//-----------------------------------------------------------------------------


void
glViewer::drawScene_stereo()
{
  double l, r, t, b, w, h, a, radians, wd2, ndfl;

  w = glWidth();
  h = glHeight();
  a = w / h;

  radians = fovy_ * 0.5 / 180.0 * M_PI;
  wd2     = near_ * tan(radians);
  ndfl    = near_ / focalDist_ * scene_radius_;

  l = -a*wd2;
  r =  a*wd2;
  t =  wd2;
  b = -wd2;

  double offset  = 0.5 * eyeDist_;
  double offset2 = offset * ndfl;



  // left eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l+offset2, r+offset2, b, t, near_, far_);
  glTranslatef(-offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glDrawBuffer(GL_BACK_LEFT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawScene_mono();


  // right eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l-offset2, r-offset2, b, t, near_, far_);
  glTranslatef(offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  glDrawBuffer(GL_BACK_RIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawScene_mono();
  glDrawBuffer(GL_BACK);
}



//-----------------------------------------------------------------------------


void glViewer::setHome()
{
  home_modelview_          = glstate_->modelview();
  home_inverse_modelview_  = glstate_->inverse_modelview();
  homeOrthoWidth_          = orthoWidth_;
  home_center_             = trackball_center_;
  home_radius_             = trackball_radius_;
}


void glViewer::home()
{
  makeCurrent();
  glstate_->set_modelview(home_modelview_, home_inverse_modelview_);
  orthoWidth_ = homeOrthoWidth_;
  trackball_center_ = home_center_;
  trackball_radius_ = home_radius_;
  updateProjectionMatrix();
  updateGL();

}


//-----------------------------------------------------------------------------


void glViewer::viewAll()
{
  makeCurrent();

  // move center (in camera coords) to origin and translate in -z dir
  translate(-(glstate_->modelview().transform_point(scene_center_))
	    - ACG::Vec3d(0.0, 0.0, 3.0*scene_radius_ ));

  orthoWidth_ = 1.1*scene_radius_;
  double aspect = (double) glWidth() / (double) glHeight();
  if (aspect > 1.0) orthoWidth_ *= aspect;
  updateProjectionMatrix();
  updateGL();

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
      // Zoom in or out?
      orthoWidth_ *= _move_back ? 2.0 : 0.5;

      // Set the double click point as the new trackball center
      // Rotations will use this point as the center.
      trackball_center_ = hitPoint;

      /// @todo : Translate view such that hitpoint is in center of viewport

      // Update the projection matrix
      updateProjectionMatrix();

      // Redraw scene
      updateGL();
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

      drawNow();
    }
  }


  // no animation
  else
  {
    translate(trans);
    if (fabs(angle) > FLT_MIN)
      rotate(axis, angle, _center);

    updateGL();
  }


  trackball_center_ = _center;
  trackball_radius_ = std::max(scene_radius_,
			       (_center-_position).norm()*0.9f);
}


//-----------------------------------------------------------------------------


void glViewer::setView(const ACG::GLMatrixd& _modelview,
			                  const ACG::GLMatrixd& _inverse_modelview)
{
  makeCurrent();
  glstate_->set_modelview(_modelview, _inverse_modelview);
  updateGL();
}


//-----------------------------------------------------------------------------


void glViewer::initializeGL()
{
  // we use GLEW to manage extensions
  // initialize it first
  glewInit();


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

  // light sources
  light_matrix_.identity();
  update_lights();


  // scene pos and size
  scene_center_ = trackball_center_ = ACG::Vec3d( 0.0, 0.0, 0.0 );
  scene_radius_ = trackball_radius_ = 1.0;
  orthoWidth_   = 2.0;


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
}


//-----------------------------------------------------------------------------


void glViewer::update_lights()
{
  makeCurrent();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMultMatrixd(light_matrix_.data());

  GLfloat pos[4], col[4];

  col[0] = col[1] = col[2] = 0.7;
  pos[3] = col[3] = 0.0;

#define SET_LIGHT(i,x,y,z) { 			\
    pos[0]=x; pos[1]=y; pos[2]=z;		\
    glLightfv(GL_LIGHT##i, GL_POSITION, pos);	\
    glLightfv(GL_LIGHT##i, GL_DIFFUSE,  col);	\
    glLightfv(GL_LIGHT##i, GL_SPECULAR, col);	\
    glEnable(GL_LIGHT##i);			\
  }

  SET_LIGHT(0,  0.0,  0.0, 1.0);
  SET_LIGHT(1, -1.0,  1.0, 0.7);
  SET_LIGHT(2,  1.0,  1.0, 0.7);

  col[0] = col[1] = col[2] = 0.3; col[3] = 1.0;
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, col);

  glPopMatrix();
}


void glViewer::rotate_lights(ACG::Vec3d& _axis, double _angle)
{
  light_matrix_.rotate(_angle, _axis[0], _axis[1], _axis[2], ACG::MULT_FROM_LEFT);
  update_lights();
}


//-----------------------------------------------------------------------------


void glViewer::paintGL()
{
  static bool initialized = false;
  if (!initialized)
  {
    // we use GLEW to manage extensions
    // initialize it first
    glewInit();

    // lock update
    properties_.lockUpdate();

    // init GL state
    glstate_->initialize();

    // initialize lights
    light_matrix_.identity();

    // scene pos and size
    scene_center_ = trackball_center_ = ACG::Vec3d( 0.0, 0.0, 0.0 );
    scene_radius_ = trackball_radius_ = 1.0;
    orthoWidth_   = 2.0;

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

    initialized = true;
  }

  if (!properties_.updateLocked())
  {
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

    // light sources
    update_lights();

    glstate_->setState ();

    glColor4f(1.0,0.0,0.0,1.0);

    // clear (stereo mode clears buffers on its own)
    if (!stereo_)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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


void glViewer::resizeGL(int _w, int _h)
{
  updateProjectionMatrix();
  glstate_->viewport(0, 0, _w, _h);
  updateGL();
}


//-----------------------------------------------------------------------------


void glViewer::encodeView(QString& _view)
{
  const ACG::GLMatrixd m = glstate_->modelview();
  const ACG::GLMatrixd p = glstate_->projection();

  _view.sprintf("%s\n"
                "%lf %lf %lf %lf\n%lf %lf %lf %lf\n"
                "%lf %lf %lf %lf\n%lf %lf %lf %lf\n"
                "%lf %lf %lf %lf\n%lf %lf %lf %lf\n"
                "%lf %lf %lf %lf\n%lf %lf %lf %lf\n"
                "%d %d %d %lf\n",
                VIEW_MAGIC,
                m(0,0), m(0,1), m(0,2), m(0,3),
                m(1,0), m(1,1), m(1,2), m(1,3),
                m(2,0), m(2,1), m(2,2), m(2,3),
                m(3,0), m(3,1), m(3,2), m(3,3),
                p(0,0), p(0,1), p(0,2), p(0,3),
                p(1,0), p(1,1), p(1,2), p(1,3),
                p(2,0), p(2,1), p(2,2), p(2,3),
                p(3,0), p(3,1), p(3,2), p(3,3),
                glWidth(), glHeight(),
                projectionMode_,
                orthoWidth_ );
}


//----------------------------------------------------------------------------


bool glViewer::decodeView(const QString& _view)
{
  if (_view.left(sizeof(VIEW_MAGIC)-1) != QString(VIEW_MAGIC))
    return false;



  ACG::GLMatrixd m, p;
  int            w, h, pMode;

  sscanf( (_view.toAscii().data())+sizeof(VIEW_MAGIC)-1,
	  "%lf %lf %lf %lf\n%lf %lf %lf %lf\n"
	  "%lf %lf %lf %lf\n%lf %lf %lf %lf\n"
	  "%lf %lf %lf %lf\n%lf %lf %lf %lf\n"
	  "%lf %lf %lf %lf\n%lf %lf %lf %lf\n"
	  "%d %d %d %lf\n",
	  &m(0,0), &m(0,1), &m(0,2), &m(0,3),
	  &m(1,0), &m(1,1), &m(1,2), &m(1,3),
	  &m(2,0), &m(2,1), &m(2,2), &m(2,3),
	  &m(3,0), &m(3,1), &m(3,2), &m(3,3),
	  &p(0,0), &p(0,1), &p(0,2), &p(0,3),
	  &p(1,0), &p(1,1), &p(1,2), &p(1,3),
	  &p(2,0), &p(2,1), &p(2,2), &p(2,3),
	  &p(3,0), &p(3,1), &p(3,2), &p(3,3),
	  &w, &h,
	  &pMode,
	  &orthoWidth_ );

  makeCurrent();

  if (projectionMode_ != (ProjectionMode)pMode)
    projectionMode((ProjectionMode)pMode);

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


void glViewer::actionDrawMenu( QAction * _action )
{
  unsigned int mode( _action->data().toUInt() );

  // combine draw modes
  if (qApp->keyboardModifiers() & Qt::ShiftModifier)
  {
    if (drawMode() & mode)
      drawMode(drawMode() & ~mode);
    else
      drawMode(drawMode() | mode);
  }

  // simply switch draw mode
  else
  {
    // clear all other checked items
    std::vector< QAction * >::iterator aIter, aEnd;

    aEnd = drawMenuActions_.end();
    for( aIter = drawMenuActions_.begin();
	 aIter != aEnd;
	 ++aIter )
    {
      if( (*aIter)->data().toUInt() != mode )
	  (*aIter)->setChecked( false );
    }

    drawMode(mode);
  }

  hidePopupMenus();
  updateGL();
}


//-----------------------------------------------------------------------------

void
glViewer::createWidgets(const QGLFormat* _format,
                            QStatusBar* _sb,
                            const glViewer* _share)
{
  setStatusBar(_sb);
  drawMenu_=0;
  pickMenu_=0;


  // contains splitter and eventually status bar
  // QT3:  Q3VBoxLayout* layout=new Q3VBoxLayout(this,0,0,"toplevel layout");
  QVBoxLayout* layout=new QVBoxLayout(this);
  layout->setSpacing( 0 );
  layout->setMargin( 0 );

  // contains glarea and buttons

  // QT3:  Q3Frame* work=new Q3Frame(this,"box-glarea-buttons");
  QFrame* work=new QFrame(this);

  layout->addWidget(work,1); // gets all stretch


  // Construct GL context & widget
  QGLWidget* share = 0;
  if (_share)  share = _share->glWidget_;

  QGLFormat format;
  format.setAlpha(true);
  if (_format!=0) format = *_format;

  glWidget_ = new QGLWidget(format, 0, share);
  glView_ = new QtGLGraphicsView(this, work);
  glScene_ = new QtGLGraphicsScene (this);

  glView_->setViewport(glWidget_);
  glView_->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  glView_->setScene(glScene_);

  glView_->setContextMenuPolicy( Qt::CustomContextMenu );

  wheelZ_=new ACG::QtWidgets::QtWheel( 0,"wheel-z",ACG::QtWidgets::QtWheel::Vertical);
  wheelZ_->setMinimumSize(wheelZ_->sizeHint());
  wheelZ_->setMaximumSize(wheelZ_->sizeHint());
  connect(wheelZ_,SIGNAL(angleChangedBy(double)),
	  this,SLOT(slotWheelZ(double)));
  wheelZ_->setToolTip( "Translate along <b>z-axis</b>.");
  wheelZ_->setWhatsThis( "Translate along <b>z-axis</b>.");

  wheelY_=new ACG::QtWidgets::QtWheel( 0,"wheel-y",ACG::QtWidgets::QtWheel::Horizontal);
  wheelY_->setMinimumSize(wheelY_->sizeHint());
  wheelY_->setMaximumSize(wheelY_->sizeHint());
  connect(wheelY_,SIGNAL(angleChangedBy(double)),
          this,SLOT(slotWheelY(double)));
  wheelY_->setToolTip("Rotate around <b>y-axis</b>.");
  wheelY_->setWhatsThis( "Rotate around <b>y-axis</b>.");

  wheelX_=new ACG::QtWidgets::QtWheel( 0,"wheel-x",ACG::QtWidgets::QtWheel::Vertical);
  wheelX_->setMinimumSize(wheelX_->sizeHint());
  wheelX_->setMaximumSize(wheelX_->sizeHint());
  connect(wheelX_,SIGNAL(angleChangedBy(double)),
          this,SLOT(slotWheelX(double)));
  wheelX_->setToolTip("Rotate around <b>x-axis</b>.");
  wheelX_->setWhatsThis( "Rotate around <b>x-axis</b>.");


  QGraphicsWidget *wheelX = glScene_->addWidget (wheelX_);
  QGraphicsWidget *wheelY = glScene_->addWidget (wheelY_);
  QGraphicsWidget *wheelZ = glScene_->addWidget (wheelZ_);

  wheelX_->setWindowOpacity (0.5);
  wheelY_->setWindowOpacity (0.5);
  wheelZ_->setWindowOpacity (0.5);

  wheelX->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  wheelY->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
  wheelZ->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  glBaseLayout_ = new QGraphicsGridLayout;
  glBaseLayout_->addItem(wheelX, 1, 0);
  glBaseLayout_->addItem(wheelY, 2, 1);
  glBaseLayout_->addItem(wheelZ, 1, 3);

  glBaseLayout_->setColumnStretchFactor(0,0);
  glBaseLayout_->setColumnStretchFactor(1,0);
  glBaseLayout_->setColumnStretchFactor(2,1);
  glBaseLayout_->setColumnStretchFactor(3,0);

  glBaseLayout_->setRowStretchFactor(0,1);
  glBaseLayout_->setRowStretchFactor(1,0);
  glBaseLayout_->setRowStretchFactor(2,0);

  glBase_ = new QGraphicsWidget;
  glBase_->setLayout(glBaseLayout_);
  glScene_->addItem(glBase_);
  glBase_->setGeometry (glScene_->sceneRect ());
  QRectF r = glScene_->sceneRect ();

  connect ( glScene_, SIGNAL( sceneRectChanged( const QRectF & ) ),
            this, SLOT( sceneRectChanged( const QRectF & ) ) );

  // If popupEnabled_ is set to false the Contextmenu Mode will be set to customContextMenuRequested
  // and this signal will be emitted on right click
  connect( glView_ , SIGNAL( customContextMenuRequested( const QPoint& ) ) ,
           this    , SIGNAL( signalCustomContextMenuRequested( const QPoint& ) ) );

  // is stereo possible ?
  if (format.stereo())
    std::cerr << "Stereo buffer requested: "
	      << (glWidget_->format().stereo() ? "ok\n" : "failed\n");

  glLayout_ = new QGridLayout(work);
  glLayout_->setSpacing( 0 );
  glLayout_->setMargin( 0 );

  glLayout_->addWidget(glView_,    0,0);

  glLayout_->setColumnStretch(0,1);
  glLayout_->setColumnStretch(1,0);

}


//-----------------------------------------------------------------------------


void glViewer::updatePopupMenu()
{
  //
  // Draw mode menu
  //

  if ( ! drawMenu_ )
  {
    drawMenu_ = new QMenu( this );
    connect( drawMenu_, SIGNAL( aboutToHide() ),
	     this, SLOT( hidePopupMenus() ) );

  }

  QActionGroup * drawGroup = new QActionGroup( this );
  drawGroup->setExclusive( false );
  connect( drawGroup, SIGNAL( triggered( QAction * ) ),
	   this, SLOT( actionDrawMenu( QAction * ) ) );



  drawMenuActions_.clear();

  std::vector< unsigned int > draw_mode_id;

  draw_mode_id = ACG::SceneGraph::DrawModes::getDrawModeIDs( availDrawModes_ );

  for ( unsigned int i = 0; i < draw_mode_id.size(); ++i )
  {
    unsigned int id    = draw_mode_id[i];
    std::string  descr = ACG::SceneGraph::DrawModes::description( id );

    QAction * action = new QAction( descr.c_str(), drawGroup );
    action->setData( QVariant( id ) );
    action->setCheckable( true );
    action->setChecked( ACG::SceneGraph::DrawModes::containsId( curDrawMode_, id ) );
    drawMenuActions_.push_back( action );
  }


  drawMenu_->clear();
  drawMenu_->addActions( drawGroup->actions() );


}


//-----------------------------------------------------------------------------


void glViewer::hidePopupMenus()
{
  if ( drawMenu_ )
  {
    drawMenu_->blockSignals(true);
    drawMenu_->hide();
    drawMenu_->blockSignals(false);
  }

  if ( pickMenu_ )
  {
    pickMenu_->blockSignals(true);
    pickMenu_->hide();
    pickMenu_->blockSignals(false);
  }
}


//-----------------------------------------------------------------------------


void glViewer::translate(const ACG::Vec3d& _trans)
{
  makeCurrent();
  glstate_->translate(_trans[0], _trans[1], _trans[2], ACG::MULT_FROM_LEFT);
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
}


//-----------------------------------------------------------------------------


unsigned int glViewer::glWidth() const {
  return glView_->width();
}
unsigned int glViewer::glHeight() const {
  return glView_->height();
}
QSize glViewer::glSize() const {
  return glView_->size();
}
QPoint glViewer::glMapFromGlobal( const QPoint& _pos ) const {
  return glView_->mapFromGlobal(_pos);
}

QPoint glViewer::glMapToGlobal( const QPoint& _pos ) const {
  return glView_->mapToGlobal(_pos);
}


//-----------------------------------------------------------------------------


void
glViewer::slotNodeChanged(ACG::SceneGraph::BaseNode* _node)
{
  emit(signalNodeChanged(_node));
  updateGL();
}


//-----------------------------------------------------------------------------


void glViewer::slotWheelX(double _dAngle)
{
  rotate(ACG::Vec3d(1,0,0),ACG::QtWidgets::QtWheel::deg(ACG::QtWidgets::QtWheel::clip(_dAngle)));
  updateGL();

}

void glViewer::slotWheelY(double _dAngle)
{
  rotate(ACG::Vec3d(0,1,0),ACG::QtWidgets::QtWheel::deg(ACG::QtWidgets::QtWheel::clip(_dAngle)));
  updateGL();

}

void glViewer::slotWheelZ(double _dist)
{
  double dz=_dist*0.5/M_PI*scene_radius_*2.0;
  translate(ACG::Vec3d(0,0,dz));
  updateGL();

}


//-----------------------------------------------------------------------------

void glViewer::sceneRectChanged(const QRectF &rect)
{
  glBase_->setGeometry (rect);
}

//-----------------------------------------------------------------------------


void glViewer::grabGLArea()
{
  glareaGrabbed_ = true;

  glView_->setCursor(Qt::BlankCursor);
  glBase_->setCursor(Qt::BlankCursor);
  glView_->grabMouse();
  glView_->grabKeyboard();
}

void glViewer::releaseGLArea()
{
  glareaGrabbed_ = false;

  glView_->releaseMouse();
  glView_->releaseKeyboard();
  glView_->setCursor(Qt::ArrowCursor);
  glBase_->setCursor(Qt::ArrowCursor);
}


//-----------------------------------------------------------------------------


void glViewer::glContextMenuEvent(QContextMenuEvent* _event)
{

}


//-----------------------------------------------------------------------------


void glViewer::glMousePressEvent(QMouseEvent* _event)
{
  // right button pressed => popup menu (ignore here)
  if (_event->button() != Qt::RightButton )
  {
    switch (properties_.actionMode())
    {
      case Viewer::ExamineMode:
        if ((_event->modifiers() & Qt::ControlModifier)) // drag&drop
          emit startDragEvent( _event );
        else
          viewMouseEvent(_event); // examine
        break;

      case Viewer::LightMode:
	lightMouseEvent(_event);
	break;

      case Viewer::PickingMode: // give event to application
        emit(signalMouseEvent(_event, pick_mode_name_));
        emit(signalMouseEvent(_event));
        break;

      case Viewer::QuestionMode: // give event to application
        emit(signalMouseEventIdentify(_event));
        break;
    }
  }
}


//-----------------------------------------------------------------------------


void glViewer::glMouseDoubleClickEvent(QMouseEvent* _event)
{
  switch (properties_.actionMode())
  {
    case Viewer::ExamineMode:
      viewMouseEvent(_event);
      break;

    case Viewer::LightMode:
      lightMouseEvent(_event);
      break;

    case Viewer::PickingMode: // give event to application
      emit(signalMouseEvent(_event, pick_mode_name_));
      emit(signalMouseEvent(_event));
      break;

    case Viewer::QuestionMode: // give event to application
      emit(signalMouseEventIdentify(_event));
      break;
  }
}


//-----------------------------------------------------------------------------


void glViewer::glMouseMoveEvent(QMouseEvent* _event)
{
  switch ( properties_.actionMode() )
  {
    case Viewer::ExamineMode:
      viewMouseEvent(_event);
      break;

    case Viewer::LightMode:
      lightMouseEvent(_event);
      break;

    case Viewer::PickingMode:
      // give event to application
      // deliver mouse moves with no button down, if tracking is enabled,
      if ((_event->buttons() & (Qt::LeftButton | Qt::MidButton | Qt::RightButton))
          || trackMouse_)
      {
        emit(signalMouseEvent(_event, pick_mode_name_));
        emit(signalMouseEvent(_event));
      }
      break;

    case Viewer::QuestionMode: // give event to application
      emit(signalMouseEventIdentify(_event));
      break;

    default: // avoid warning
      break;
  }
}


//-----------------------------------------------------------------------------


void glViewer::glMouseReleaseEvent(QMouseEvent* _event)
{
//   if (_event->button() == Qt::RightButton )
//     hidePopupMenus();

  if (_event->button() != Qt::RightButton ||
      properties_.pickingMode() )
  {
    switch ( properties_.actionMode() )
    {
      case Viewer::ExamineMode:
        viewMouseEvent(_event);
        break;

      case Viewer::LightMode:
        lightMouseEvent(_event);
        break;

      case Viewer::PickingMode: // give event to application
        emit(signalMouseEvent(_event, pick_mode_name_));
        emit(signalMouseEvent(_event));
        break;

      case Viewer::QuestionMode: // give event to application
        emit(signalMouseEventIdentify(_event));
        break;

      default: // avoid warning
        break;
    }
  }

  isRotating_ = false;
}


//-----------------------------------------------------------------------------


void glViewer::glMouseWheelEvent(QWheelEvent* _event)
{
  switch ( properties_.actionMode() )
  {
    case Viewer::ExamineMode:
      viewWheelEvent(_event);
      break;

    case Viewer::PickingMode: // give event to application
      emit(signalWheelEvent(_event, pick_mode_name_));
      break;

    default: // avoid warning
      break;
  }

  isRotating_ = false;
}

//-----------------------------------------------------------------------------


void glViewer::updatePickMenu()
{
  delete pickMenu_;

  pickMenu_ = new QMenu( 0 );
  connect( pickMenu_, SIGNAL( aboutToHide() ),
	   this, SLOT( hidePopupMenus() ) );

  QActionGroup * ag = new QActionGroup( pickMenu_ );
  ag->setExclusive( true );

  for (unsigned int i=0; i<pick_modes_.size(); ++i) {
    if ( !pick_modes_[i].visible )
      continue;

    if (pick_modes_[i].name == "Separator")
    {
      if ((i > 0) && (i<pick_modes_.size()-1)) // not first, not last
	   pickMenu_->addSeparator();
    }
    else
    {
      QAction * ac = new QAction( pick_modes_[i].name.c_str(), ag );
      ac->setData( QVariant( i ) );
      ac->setCheckable( true );

      if ((int)i == pick_mode_idx_)
        ac->setChecked( true );

      pickMenu_->addAction( ac );
    }
  }

  connect( ag, SIGNAL( triggered( QAction * ) ),
	   this, SLOT( actionPickMenu( QAction * ) ));
}


//-----------------------------------------------------------------------------


void glViewer::actionPickMenu( QAction * _action )
{
  int _id = _action->data().toInt();
  if (_id < (int) pick_modes_.size() )
  {
    pickMode( _id );
  }

  properties_.setPickingMode();

  hidePopupMenus();
}


//-----------------------------------------------------------------------------


void
glViewer::viewMouseEvent(QMouseEvent* _event)
{
  switch (_event->type())
  {
    case QEvent::MouseButtonPress:
    {
      // shift key -> set rotation center
      if (_event->modifiers() & Qt::ShiftModifier)
      {
        ACG::Vec3d c;
        if (fast_pick(_event->pos(), c))
        {
          trackball_center_ = c;
          trackball_radius_ = std::max(scene_radius_, (c-glstate_->eye()).norm()*0.9f);
        }
      }

      lastPoint_hitSphere_ = mapToSphere( lastPoint2D_=_event->pos(),
                 lastPoint3D_ );
      isRotating_ = true;
      timer_->stop();


      break;
    }


    case QEvent::MouseButtonDblClick:
    {
      if (allowRotation_)
        flyTo(_event->pos(), _event->button()==Qt::MidButton);
      break;
    }


    case QEvent::MouseMove:
    {
      double factor  = 1.0;

      if (_event->modifiers() == Qt::ShiftModifier)
        factor = wZoomFactorShift_;

      // mouse button should be pressed
      if (_event->buttons() & (Qt::LeftButton | Qt::MidButton))
      {
        QPoint newPoint2D = _event->pos();

        if ( (newPoint2D.x()<0) || (newPoint2D.x() > (int)glWidth()) ||
             (newPoint2D.y()<0) || (newPoint2D.y() > (int)glHeight()) )
          return;

        double value_x, value_y;
        ACG::Vec3d  newPoint3D;
        bool   newPoint_hitSphere = mapToSphere( newPoint2D, newPoint3D );

        makeCurrent();

        // move in z direction
        if ( (_event->buttons() & Qt::LeftButton) &&
              (_event->buttons() & Qt::MidButton))
        {
          switch (projectionMode())
          {
            case PERSPECTIVE_PROJECTION:
            {
              value_y = scene_radius_ * ((newPoint2D.y() - lastPoint2D_.y())) * 3.0 / (double) glHeight();
              translate( ACG::Vec3d(0.0, 0.0, value_y * factor ) );
              updateGL();
              break;
            }

            case ORTHOGRAPHIC_PROJECTION:
            {
              value_y = ((newPoint2D.y() - lastPoint2D_.y())) * orthoWidth_ / (double) glHeight();
              orthoWidth_ -= value_y  * factor;
              updateProjectionMatrix();
              updateGL();
              break;
            }
          }
        }

        // move in x,y direction
        else if (_event->buttons() & Qt::MidButton)
        {
          value_x = scene_radius_ * ((newPoint2D.x() - lastPoint2D_.x())) * 2.0 / (double) glWidth();
          value_y = scene_radius_ * ((newPoint2D.y() - lastPoint2D_.y())) * 2.0 / (double) glHeight();
          translate( ACG::Vec3d(value_x  * factor , -value_y  * factor , 0.0) );
        }

        // rotate
        else if (allowRotation_ && (_event->buttons() & Qt::LeftButton) )
        {
          ACG::Vec3d axis(1.0,0.0,0.0);
          double angle(0.0);

          if ( lastPoint_hitSphere_ ) {

            if ( ( newPoint_hitSphere = mapToSphere( newPoint2D,
                                newPoint3D ) ) ) {
              axis = lastPoint3D_ % newPoint3D;
              double cos_angle = ( lastPoint3D_ | newPoint3D );
              if ( fabs(cos_angle) < 1.0 ) {
                angle = acos( cos_angle ) * 180.0 / M_PI  * factor ;
                angle *= 2.0; // inventor rotation
              }
            }

            rotate(axis, angle);

          }

          lastRotationAxis_  = axis;
          lastRotationAngle_ = angle;
        }

        lastPoint2D_ = newPoint2D;
        lastPoint3D_ = newPoint3D;
        lastPoint_hitSphere_ = newPoint_hitSphere;

        updateGL();
        lastMoveTime_.restart();
      }
      break;
    }



    case QEvent::MouseButtonRelease:
    {
      lastPoint_hitSphere_ = false;

      // continue rotation ?
      if ( isRotating_ &&
      (_event->button() == Qt::LeftButton) &&
      (!(_event->buttons() & Qt::MidButton)) &&
      (lastMoveTime_.elapsed() < 10) && properties_.animation() )
        timer_->start(0);
      break;
    }

    default: // avoid warning
      break;
  }


}


//-----------------------------------------------------------------------------


void
glViewer::lightMouseEvent(QMouseEvent* _event)
{
  switch (_event->type())
  {
    case QEvent::MouseButtonPress:
    {
      lastPoint_hitSphere_ = mapToSphere( lastPoint2D_=_event->pos(),
                 lastPoint3D_ );
      isRotating_ = true;
      timer_->stop();
      break;
    }


    case QEvent::MouseMove:
    {
      // rotate lights
      if (_event->buttons() & Qt::LeftButton)
      {
   QPoint newPoint2D = _event->pos();

   if ( (newPoint2D.x()<0) || (newPoint2D.x() > (int)glWidth()) ||
        (newPoint2D.y()<0) || (newPoint2D.y() > (int)glHeight()) )
     return;


   ACG::Vec3d  newPoint3D;
   bool   newPoint_hitSphere = mapToSphere( newPoint2D, newPoint3D );

   makeCurrent();

   ACG::Vec3d axis(1.0,0.0,0.0);
   double angle(0.0);

   if ( lastPoint_hitSphere_ )
   {
     if ( ( newPoint_hitSphere = mapToSphere( newPoint2D, newPoint3D ) ) )
     {
       axis = lastPoint3D_ % newPoint3D;
       double cos_angle = ( lastPoint3D_ | newPoint3D );
       if ( fabs(cos_angle) < 1.0 ) {
         angle = acos( cos_angle ) * 180.0 / M_PI;
         angle *= 2.0;
       }
     }
     rotate_lights(axis, angle);
   }

   lastPoint2D_ = newPoint2D;
   lastPoint3D_ = newPoint3D;
   lastPoint_hitSphere_ = newPoint_hitSphere;

   updateGL();
      }
      break;
    }


    default: // avoid warning
      break;
  }
}

//-----------------------------------------------------------------------------

double glViewer::wheelZoomFactor(){
  return wZoomFactor_;
}

//-----------------------------------------------------------------------------

double glViewer::wheelZoomFactorShift(){
  return wZoomFactorShift_;
}

//-----------------------------------------------------------------------------

void glViewer::setWheelZoomFactor(double _factor){
  wZoomFactor_ = _factor;
}

//-----------------------------------------------------------------------------

void glViewer::setWheelZoomFactorShift(double _factor){
  wZoomFactorShift_ = _factor;
}

//-----------------------------------------------------------------------------

void glViewer::viewWheelEvent( QWheelEvent* _event)
{
  double factor = wZoomFactor_;

  if (_event->modifiers() == Qt::ShiftModifier)
    factor = wZoomFactorShift_;

  switch (projectionMode())
  {
    case PERSPECTIVE_PROJECTION:
    {
      double d = -(double)_event->delta() / 120.0 * 0.2 * factor * trackball_radius_/3;
      translate( ACG::Vec3d(0.0, 0.0, d) );
      updateGL();
      break;
    }

    case ORTHOGRAPHIC_PROJECTION:
    {
      double d = (double)_event->delta() / 120.0 * 0.2 * factor * orthoWidth_;
      orthoWidth_ += d;
      updateProjectionMatrix();
      updateGL();
      break;
    }
  }


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
  QTime t;
  t.start();
  makeCurrent();
  rotate( lastRotationAxis_, lastRotationAngle_ );
  updateGL();

  if (!properties_.updateLocked()) {
    msecs += t.elapsed();
    if (count==10) {
      assert(statusbar_!=0);
      char s[100];
      sprintf( s, "%.3f fps", (10000.0 / (float)msecs) );
      statusbar_->showMessage(s,2000);
      count = msecs = 0;
    }
    else
      ++count;
  }
}

void glViewer::applyProperties() {
  glstate_->set_clear_color( properties_.backgroundColor() );

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
  std::cerr << "Properties updated" << std::endl;

  makeCurrent();

  applyProperties();

  updateGL();
}


void glViewer::snapshot()
{
   makeCurrent();

   glView_->raise();
   qApp->processEvents();
   makeCurrent();
   paintGL();
   glFinish();

   QImage snapshot;
   copyToImage(snapshot, 0, 0, glWidth(), glHeight(), GL_BACK);

   QFileInfo fi(properties_.snapshotName());

   QString fname = fi.path() + QDir::separator() +fi.baseName() + "." + QString::number(properties_.snapshotCounter()) + ".";

   QString format="png";

   if (fi.completeSuffix() == "ppm")
     format="ppmraw";

   fname += format;

   bool rval=snapshot.save(fname,format.toUpper().toLatin1());


   assert(statusbar_!=0);
   if (rval)
   {
     statusbar_->showMessage(QString("snapshot: ")+fname,5000);
   }
   else
   {
     statusbar_->showMessage(QString("could not save snapshot to ")+fname);
   }

}

//=============================================================================
//=============================================================================
