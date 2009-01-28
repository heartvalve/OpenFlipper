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
//  CLASS QtBaseViewer - IMPLEMENTATION
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


QtBaseViewer::QtBaseViewer( QWidget* _parent,
			    const char* /* _name */ ,
			    QStatusBar *_statusBar,
			    const QGLFormat* _format,
			    const QtBaseViewer* _share) :
  QWidget(_parent),
  statusbar_(_statusBar),
  glareaGrabbed_(false),
  updateLocked_(false),
  projectionUpdateLocked_(false),
  blending_(true),
  snapshotName_("snap.png"),
  snapshotCounter_(0),
  snapshot_(0),
  pick_mode_name_(""),
  pick_mode_idx_(-1),
  renderPicking_(false),
  pickRendererMode_(ACG::SceneGraph::PICK_ANYTHING)

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
  faceOrientation_  = CCW_ORIENTATION;
  projectionMode_   = PERSPECTIVE_PROJECTION;
  backFaceCulling_  = false;
  twoSidedLighting_ = true;
  animation_        = false;

  light_matrix_.identity();


  snapshot_=new QImage;

  trackMouse_ = false;

  // stereo
  stereo_ = false;


  pickMenu_ = 0;
  drawMenu_ = 0;


  // init action modes: Examine & Pick
  actionMode_ = PickingMode;
  lastActionMode_ = PickingMode;
  examineMode();


  // Note: we start locked (initialization of updateLocked_)
  // will be unlocked in initializeGL()

  QSizePolicy sp = sizePolicy();
  sp.setHorizontalPolicy( QSizePolicy::Expanding );
  sp.setVerticalPolicy( QSizePolicy::Expanding );
  sp.setHorizontalStretch( 1 );
  sp.setVerticalStretch( 1 );
  setSizePolicy( sp );

  redrawTime_.start ();

}


//-----------------------------------------------------------------------------


QtBaseViewer::~QtBaseViewer()
{
  delete snapshot_;
  delete glstate_;
}


//-----------------------------------------------------------------------------


QSize
QtBaseViewer::sizeHint() const
{
  return QSize( 600, 600 );
}


//-----------------------------------------------------------------------------


void QtBaseViewer::setStatusBar(QStatusBar* _sb)
{
  statusbar_ = _sb;
}

//-----------------------------------------------------------------------------


void QtBaseViewer::makeCurrent() {
  glWidget_->makeCurrent();
}

void QtBaseViewer::swapBuffers() {
  glWidget_->swapBuffers();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::sceneGraph(ACG::SceneGraph::BaseNode* _root)
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


void QtBaseViewer::lockUpdate()
{
  updateLocked_ = true;
  //  QToolTip::add(moveButton_, "Switch to <b>move</b> mode (display locked)");
}


void QtBaseViewer::unlockUpdate()
{
  //  QToolTip::add(moveButton_,"Switch to <b>move</b> mode");
  updateLocked_ = false;
}


void QtBaseViewer::unlockAndUpdate()
{
  unlockUpdate();
  updateGL();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::trackMouse(bool _track)
{
  trackMouse_ = _track;
}


//-----------------------------------------------------------------------------

void QtBaseViewer::perspectiveProjection()
{
  projectionMode(PERSPECTIVE_PROJECTION);
  updateGL();
}


void QtBaseViewer::orthographicProjection()
{
  projectionMode(ORTHOGRAPHIC_PROJECTION);
  updateGL();
}


void QtBaseViewer::toggleProjectionMode()
{
  if (projectionMode_ == ORTHOGRAPHIC_PROJECTION)
    projectionMode(PERSPECTIVE_PROJECTION);
  else
    projectionMode(ORTHOGRAPHIC_PROJECTION);

  // sync
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));

  updateGL();
}


void QtBaseViewer::projectionMode(ProjectionMode _p)
{
  if ((projectionMode_ = _p) == ORTHOGRAPHIC_PROJECTION)
    emit projectionModeChanged( true );
  else
    emit projectionModeChanged( false );

  updateProjectionMatrix();
}


void QtBaseViewer::updateProjectionMatrix()
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


void QtBaseViewer::setScenePos(const ACG::Vec3d& _center, double _radius)
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


void QtBaseViewer::viewingDirection( const ACG::Vec3d& _dir, const ACG::Vec3d& _up )
{
  // calc eye point for this direction
  ACG::Vec3d eye = scene_center_ - _dir*(3.0*scene_radius_);

  glstate_->reset_modelview();
  glstate_->lookAt((ACG::Vec3d)eye, (ACG::Vec3d)scene_center_, (ACG::Vec3d)_up);
}


//-----------------------------------------------------------------------------


void QtBaseViewer::actionMode(ActionMode _am)
{
  emit actionModeChanged( _am );


  trackMouse(false);


  if (_am != actionMode_)
  {
    lastActionMode_ = actionMode_;
    actionMode_ = _am;
  }


  switch (actionMode_)
  {
    case ExamineMode:
    {
      glView_->setCursor(Qt::PointingHandCursor);
      glBase_->setCursor(Qt::PointingHandCursor);
      break;
    }


    case LightMode:
    {
      glView_->setCursor(Qt::PointingHandCursor);
      glBase_->setCursor(Qt::PointingHandCursor);
      break;
    }


    case PickingMode:
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


    case QuestionMode:
    {
      glView_->setCursor(Qt::WhatsThisCursor);
      glBase_->setCursor(Qt::WhatsThisCursor);
      break;
    }
  }


  emit(signalActionModeChanged(actionMode_));

  //emit pickmodeChanged with either the name of the current pickmode or an empty string
  if(actionMode_ == PickingMode)
    emit(signalPickModeChanged(pick_mode_name_));
  else
    emit(signalPickModeChanged(""));
}


//-----------------------------------------------------------------------------


void QtBaseViewer::faceOrientation(FaceOrientation _ori)
{
  makeCurrent();

  switch ( faceOrientation_ = _ori ) {
    case CCW_ORIENTATION:
      glFrontFace( GL_CCW );
      break;

    case CW_ORIENTATION:
      glFrontFace( GL_CW );
      break;
  }

  updateGL();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::backFaceCulling(bool _b)
{
  emit functionMenuUpdate();

  makeCurrent();

  if ( (backFaceCulling_ = _b) )
    glEnable( GL_CULL_FACE );
  else
    glDisable( GL_CULL_FACE );

  updateGL();
}


void QtBaseViewer::twoSidedLighting(bool _b)
{
  emit functionMenuUpdate();

  makeCurrent();
  glstate_->set_twosided_lighting(twoSidedLighting_=_b);
  updateGL();
}


void QtBaseViewer::animation(bool _b)
{
  emit functionMenuUpdate();
  makeCurrent();
  animation_ = _b;
  updateGL();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::normalsMode(NormalsMode _mode)
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
QtBaseViewer::copyToImage( QImage& _image,
			   unsigned int /* _l */ , unsigned int /* _t */ ,
			   unsigned int /* _w */ , unsigned int /* _h */ ,
			   GLenum /* _buffer */ )
{
  makeCurrent();
  _image = glWidget_->grabFrameBuffer(true);
}


//-----------------------------------------------------------------------------


void QtBaseViewer::drawNow()
{
  makeCurrent();
  paintGL();
  swapBuffers();
  glView_->repaint();
}

void QtBaseViewer::updateGL()
{
  if (!isUpdateLocked() && !isHidden() )
  {
    glScene_->update();
  }
}



//-----------------------------------------------------------------------------


void QtBaseViewer::drawScene()
{
  QTime  timer;
  timer.start();


  // adjust clipping planes
  ACG::Vec3d c = glstate_->modelview().transform_point(scene_center_);
  near_   = std::max(0.0001f * scene_radius_,  -(c[2] + scene_radius_));
  far_    = std::max(0.0002f * scene_radius_,  -(c[2] - scene_radius_));
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


void QtBaseViewer::drawScene_mono()
{
  emit(signalDrawScene(glstate_));

  if (sceneGraphRoot_)
  {
    if (! renderPicking_ ) {
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
      ACG::SceneGraph::PickAction action(pickRendererMode_);
      ACG::SceneGraph::traverse(sceneGraphRoot_, action, *glstate_);

      glEnable(GL_LIGHTING);
    }
  }
}


//-----------------------------------------------------------------------------


void
QtBaseViewer::drawScene_stereo()
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


void QtBaseViewer::setHome()
{
  home_modelview_          = glstate_->modelview();
  home_inverse_modelview_  = glstate_->inverse_modelview();
  homeOrthoWidth_          = orthoWidth_;
  home_center_             = trackball_center_;
  home_radius_             = trackball_radius_;
}


void QtBaseViewer::home()
{
  makeCurrent();
  glstate_->set_modelview(home_modelview_, home_inverse_modelview_);
  orthoWidth_ = homeOrthoWidth_;
  trackball_center_ = home_center_;
  trackball_radius_ = home_radius_;
  updateProjectionMatrix();
  updateGL();

  // sync
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
}


//-----------------------------------------------------------------------------


void QtBaseViewer::viewAll()
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

  // sync
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
}


//-----------------------------------------------------------------------------


void QtBaseViewer::flyTo(const QPoint& _pos, bool _move_back)
{
  makeCurrent();

  unsigned int nodeIdx, targetIdx;
  ACG::Vec3d hitPoint;

  if (pick( ACG::SceneGraph::PICK_ANYTHING, _pos, nodeIdx, targetIdx, &hitPoint))
  {
    if (projectionMode_ == PERSPECTIVE_PROJECTION)
    {
      ACG::Vec3d eye(glState().eye());
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

    // sync with external viewer
    emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
  }
}


void QtBaseViewer::flyTo(const ACG::Vec3d&  _position,
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


void QtBaseViewer::setView(const ACG::GLMatrixd& _modelview,
			                  const ACG::GLMatrixd& _inverse_modelview)
{
  makeCurrent();
  glstate_->set_modelview(_modelview, _inverse_modelview);
  updateGL();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::initializeGL()
{
  // we use GLEW to manage extensions
  // initialize it first
  glewInit();


  // lock update
  lockUpdate();

  // init GL state
  glstate_->initialize();

  // OpenGL state
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glDisable(GL_DITHER);
  glShadeModel( GL_FLAT );


  projectionMode(   projectionMode_   );
  normalsMode(      normalsMode_      );
  faceOrientation(  faceOrientation_  );
  backFaceCulling(  backFaceCulling_  );
  twoSidedLighting( twoSidedLighting_ );


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


  // emit initialization signal
  emit(signalInitializeGL());


  // unlock update (we started locked)
  unlockUpdate();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::update_lights()
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


void QtBaseViewer::rotate_lights(ACG::Vec3d& _axis, double _angle)
{
  light_matrix_.rotate(_angle, _axis[0], _axis[1], _axis[2], ACG::MULT_FROM_LEFT);
  update_lights();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::paintGL()
{
  static bool initialized = false;
  if (!initialized)
  {
    // we use GLEW to manage extensions
    // initialize it first
    glewInit();

    // lock update
    lockUpdate();

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

    // emit initialization signal
    emit(signalInitializeGL());

    // unlock update (we started locked)
    unlockUpdate();

    initialized = true;
  }

  if (!isUpdateLocked())
  {
    lockUpdate();

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
    faceOrientation(  faceOrientation_  );
    backFaceCulling(  backFaceCulling_  );

    // light sources
    update_lights();

    glstate_->setState ();

    glColor4f(1.0,0.0,0.0,1.0);

    // clear (stereo mode clears buffers on its own)
    if (!stereo_)
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    unlockUpdate();

    // draw scene
    drawScene();

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glPopAttrib ();
  }
}


//-----------------------------------------------------------------------------


void QtBaseViewer::resizeGL(int _w, int _h)
{
  updateProjectionMatrix();
  glstate_->viewport(0, 0, _w, _h);
  updateGL();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::encodeView(QString& _view)
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


bool QtBaseViewer::decodeView(const QString& _view)
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


void QtBaseViewer::actionCopyView()
{
  QString view; encodeView(view);
  QApplication::clipboard()->setText(view);
}


//-----------------------------------------------------------------------------


void QtBaseViewer::actionPasteView()
{
  QString view; view=QApplication::clipboard()->text();
  decodeView(view);
}


//-----------------------------------------------------------------------------


void QtBaseViewer::actionDrawMenu( QAction * _action )
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
QtBaseViewer::createWidgets(const QGLFormat* _format,
                            QStatusBar* _sb,
                            const QtBaseViewer* _share)
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


void QtBaseViewer::updatePopupMenu()
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


void QtBaseViewer::hidePopupMenus()
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


void QtBaseViewer::translate(const ACG::Vec3d& _trans)
{
  makeCurrent();
  glstate_->translate(_trans[0], _trans[1], _trans[2], ACG::MULT_FROM_LEFT);
}


//-----------------------------------------------------------------------------


void QtBaseViewer::initModelviewMatrix()
{
  makeCurrent();
  glstate_->reset_modelview();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::rotate(const ACG::Vec3d&  _axis,
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


unsigned int QtBaseViewer::glWidth() const {
  return glView_->width();
}
unsigned int QtBaseViewer::glHeight() const {
  return glView_->height();
}
QSize QtBaseViewer::glSize() const {
  return glView_->size();
}
QPoint QtBaseViewer::glMapFromGlobal( const QPoint& _pos ) const {
  return glView_->mapFromGlobal(_pos);
}

QPoint QtBaseViewer::glMapToGlobal( const QPoint& _pos ) const {
  return glView_->mapToGlobal(_pos);
}


//-----------------------------------------------------------------------------


void
QtBaseViewer::slotNodeChanged(ACG::SceneGraph::BaseNode* _node)
{
  emit(signalNodeChanged(_node));
  updateGL();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::slotWheelX(double _dAngle)
{
  rotate(ACG::Vec3d(1,0,0),ACG::QtWidgets::QtWheel::deg(ACG::QtWidgets::QtWheel::clip(_dAngle)));
  updateGL();

  // sync
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
}

void QtBaseViewer::slotWheelY(double _dAngle)
{
  rotate(ACG::Vec3d(0,1,0),ACG::QtWidgets::QtWheel::deg(ACG::QtWidgets::QtWheel::clip(_dAngle)));
  updateGL();

  // sync
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
}

void QtBaseViewer::slotWheelZ(double _dist)
{
  double dz=_dist*0.5/M_PI*scene_radius_*2.0;
  translate(ACG::Vec3d(0,0,dz));
  updateGL();

  // sync
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
}


//-----------------------------------------------------------------------------

void QtBaseViewer::sceneRectChanged(const QRectF &rect)
{
  glBase_->setGeometry (rect);
}

//-----------------------------------------------------------------------------


void QtBaseViewer::grabGLArea()
{
  glareaGrabbed_ = true;

  glView_->setCursor(Qt::BlankCursor);
  glBase_->setCursor(Qt::BlankCursor);
  glView_->grabMouse();
  glView_->grabKeyboard();
}

void QtBaseViewer::releaseGLArea()
{
  glareaGrabbed_ = false;

  glView_->releaseMouse();
  glView_->releaseKeyboard();
  glView_->setCursor(Qt::ArrowCursor);
  glBase_->setCursor(Qt::ArrowCursor);
}


//-----------------------------------------------------------------------------


void QtBaseViewer::glContextMenuEvent(QContextMenuEvent* _event)
{

}


//-----------------------------------------------------------------------------


void QtBaseViewer::glMousePressEvent(QMouseEvent* _event)
{
  // right button pressed => popup menu (ignore here)
  if (_event->button() != Qt::RightButton )
  {
    switch (actionMode_)
    {
      case ExamineMode:
        if ((_event->modifiers() & Qt::ControlModifier)) // drag&drop
          emit startDragEvent( _event );
        else
          viewMouseEvent(_event); // examine
        break;

      case LightMode:
	lightMouseEvent(_event);
	break;

      case PickingMode: // give event to application
        emit(signalMouseEvent(_event, pick_mode_name_));
        emit(signalMouseEvent(_event));
        break;

      case QuestionMode: // give event to application
        emit(signalMouseEventIdentify(_event));
        break;
    }
  }
}


//-----------------------------------------------------------------------------


void QtBaseViewer::glMouseDoubleClickEvent(QMouseEvent* _event)
{
  switch (actionMode_)
  {
    case ExamineMode:
      viewMouseEvent(_event);
      break;

    case LightMode:
      lightMouseEvent(_event);
      break;

    case PickingMode: // give event to application
      emit(signalMouseEvent(_event, pick_mode_name_));
      emit(signalMouseEvent(_event));
      break;

    case QuestionMode: // give event to application
      emit(signalMouseEventIdentify(_event));
      break;
  }
}


//-----------------------------------------------------------------------------


void QtBaseViewer::glMouseMoveEvent(QMouseEvent* _event)
{
  switch ( actionMode_ )
  {
    case ExamineMode:
      viewMouseEvent(_event);
      break;

    case LightMode:
      lightMouseEvent(_event);
      break;

    case PickingMode:
      // give event to application
      // deliver mouse moves with no button down, if tracking is enabled,
      if ((_event->buttons() & (Qt::LeftButton | Qt::MidButton | Qt::RightButton))
          || trackMouse_)
      {
        emit(signalMouseEvent(_event, pick_mode_name_));
        emit(signalMouseEvent(_event));
      }
      break;

    case QuestionMode: // give event to application
      emit(signalMouseEventIdentify(_event));
      break;

    default: // avoid warning
      break;
  }
}


//-----------------------------------------------------------------------------


void QtBaseViewer::glMouseReleaseEvent(QMouseEvent* _event)
{
//   if (_event->button() == Qt::RightButton )
//     hidePopupMenus();

  if (_event->button() != Qt::RightButton ||
      (actionMode_ == PickingMode ) )
  {
    switch ( actionMode_ )
    {
      case ExamineMode:
        viewMouseEvent(_event);
        break;

      case LightMode:
        lightMouseEvent(_event);
        break;

      case PickingMode: // give event to application
        emit(signalMouseEvent(_event, pick_mode_name_));
        emit(signalMouseEvent(_event));
        break;

      case QuestionMode: // give event to application
        emit(signalMouseEventIdentify(_event));
        break;

      default: // avoid warning
        break;
    }
  }

  isRotating_ = false;
}


//-----------------------------------------------------------------------------


void QtBaseViewer::glMouseWheelEvent(QWheelEvent* _event)
{
  switch ( actionMode_ )
  {
    case ExamineMode:
      viewWheelEvent(_event);
      break;

    case PickingMode: // give event to application
      emit(signalWheelEvent(_event, pick_mode_name_));
      break;

    default: // avoid warning
      break;
  }

  isRotating_ = false;
}

//-----------------------------------------------------------------------------


void QtBaseViewer::updatePickMenu()
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


void QtBaseViewer::actionPickMenu( QAction * _action )
{
  int _id = _action->data().toInt();
  if (_id < (int) pick_modes_.size() )
  {
    pickMode( _id );
  }

  actionMode( PickingMode );

  hidePopupMenus();
}

//=============================================================================
//=============================================================================
