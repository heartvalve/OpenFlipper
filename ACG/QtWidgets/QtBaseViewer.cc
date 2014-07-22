/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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



//=============================================================================
//
//  CLASS QtBaseViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include <ACG/GL/acg_glew.hh>

#include "QtBaseViewer.hh"
#include "QtGLGraphicsScene.hh"
#include "QtGLGraphicsView.hh"
#include "QtSceneGraphWidget.hh"
#include "QtWheel.hh"
#include "../Scenegraph/DrawModes.hh"
#include "../GL/gl.hh"

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

#include "move.xpm"
#include "light.xpm"
#include "info.xpm"
#include "home.xpm"
#include "set_home.xpm"
#include "viewall.xpm"
#include "pick.xpm"
#include "persp.xpm"
#include "ortho.xpm"
#include "scenegraph.xpm"
#include "mono.xpm"


#define homeIcon         home_xpm
#define sethomeIcon      set_home_xpm
#define moveIcon         move_xpm
#define lightIcon        light_xpm
#define questionIcon     info_xpm
#define viewallIcon      viewall_xpm
#define pickIcon         pick_xpm
#define perspectiveIcon  persp_xpm
#define orthoIcon        ortho_xpm
#define sceneGraphIcon   scenegraph_xpm
#define monoIcon         mono_xpm


#ifdef max
#  undef max
#endif

#ifdef min
#  undef min
#endif


//== NAMESPACES ===============================================================

namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ==========================================================

static const char          VIEW_MAGIC[] =
  "ACG::QtWidgets::QGLViewerWidget encoded view";

//== IMPLEMENTATION ==========================================================


QtBaseViewer::QtBaseViewer( QWidget* _parent,
			    const char* /* _name */ ,
			    QStatusBar *_statusBar,
			    const QGLFormat* _format,
			    const QtBaseViewer* _share,
			    Options _options ) :
  QWidget(_parent),
  statusbar_(0),
  glareaGrabbed_(false),
  updateLocked_(false),
  projectionUpdateLocked_(false),
  blending_(true),
  sceneGraphDialog_(0),
  options_(_options),
  privateStatusBar_(0),
  disableKeyHandling_(false),
  externalDrag_(false),
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
  glstate_ = new GLState();


  // state
  orthoWidth_       = 2.0;
  isRotating_       = false;
  near_             = 0.1;
  far_              = 100.0;
  fovy_             = 45.0;

  focalDist_        = 3.0;
  eyeDist_          = 0.01;

  sceneGraphRoot_   = 0;
  curDrawMode_      = SceneGraph::DrawModes::NONE;
  availDrawModes_   = SceneGraph::DrawModes::NONE;

  normalsMode_      = DONT_TOUCH_NORMALS;
  faceOrientation_  = CCW_ORIENTATION;
  projectionMode_   = PERSPECTIVE_PROJECTION;
  navigationMode_   = NORMAL_NAVIGATION;
  
  
  backFaceCulling_  = false;
  twoSidedLighting_ = false;
  animation_        = false;

  light_matrix_.identity();


  snapshot_=new QImage;

  trackMouse_ = false;
  popupEnabled_ = true;


  // stereo
  stereo_ = false;


  pickMenu_ = 0;
  funcMenu_ = 0;
  drawMenu_ = 0;


  // init action modes: Examine & Pick
  actionMode_ = PickingMode;
  lastActionMode_ = PickingMode;
  examineMode();


  // clipboard sync stuff
  synchronized_ = false;
  skipNextSync_ = false;

  socket_ = new QUdpSocket();

  for (int i=6666; i<6676; ++i)
    if ( socket_->bind( i ) )
    {
      std::cout << "listen on port " << i << "\n";
      break;
    }

  add_sync_host("127.0.0.1");



  // Note: we start locked (initialization of updateLocked_)
  // will be unlocked in initializeGL()


  // Actions

  action_.insert( "Background",        new QAction( "Background color", this ) );
  action_.insert( "Snapshot",          new QAction( "Snapshot", this ) );
  action_.insert( "SnapshotName",      new QAction( "Set snapshot name", this ) );
  action_.insert( "SnapshotSavesView", new QAction( "Snapshot saves view", this ) );
  action_.insert( "CopyView",          new QAction( "Copy view", this ) );
  action_.insert( "PasteView",         new QAction( "Paste view", this ) );
  action_.insert( "PasteDropSize",     new QAction( "Paste/Drop effects size", this ) );
  action_.insert( "Synchronize",       new QAction( "Synchronize", this ) );
  action_.insert( "Animation",         new QAction( "Animation", this ) );
  action_.insert( "BackfaceCulling",   new QAction( "Backface culling", this ) );
  action_.insert( "TwoSidedLighting",  new QAction( "Two-sided lighting", this ) );

  connect( action_["Background"], SIGNAL( triggered() ),
	   this, SLOT( actionBackground() ) );
  connect( action_["Snapshot"], SIGNAL( triggered() ),
	   this, SLOT( actionSnapshot() ) );
  connect( action_["SnapshotName"], SIGNAL( triggered() ),
	   this, SLOT( actionSnapshotName() ) );
  connect( action_["SnapshotSavesView"], SIGNAL( triggered() ),
	   this, SLOT( actionSnapshotSavesView() ) );
  connect( action_["CopyView"], SIGNAL( triggered() ),
	   this, SLOT( actionCopyView() ) );
  connect( action_["PasteView"], SIGNAL( triggered() ),
	   this, SLOT( actionPasteView() ) );
  connect( action_["PasteDropSize"], SIGNAL( triggered() ),
	   this, SLOT( actionPasteDropSize() ) );
  connect( action_["Synchronize"], SIGNAL( triggered() ),
	   this, SLOT( actionSynchronize() ) );
  connect( action_["Animation"], SIGNAL( triggered() ),
	   this, SLOT( actionAnimation() ) );
  connect( action_["BackfaceCulling"], SIGNAL( triggered() ),
	   this, SLOT( actionBackfaceCulling() ) );
  connect( action_["TwoSidedLighting"], SIGNAL( triggered() ),
	   this, SLOT( actionTwoSidedLighting() ) );

  action_["SnapshotSavesView"]->setCheckable( true );
  action_["PasteDropSize"]->setCheckable( true );
  action_["Synchronize"]->setCheckable( true );
  action_["Animation"]->setCheckable( true );
  action_["BackfaceCulling"]->setCheckable( true );
  action_["TwoSidedLighting"]->setCheckable( true );


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
  delete privateStatusBar_;
  delete snapshot_;
  delete glstate_;
  delete sceneGraphDialog_;
  delete socket_;
  //  delete socket_notifier_;
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
  if (_sb==0)
  {
    if (privateStatusBar_==0)
      privateStatusBar_=new QStatusBar(this);
    statusbar_=privateStatusBar_;
    if (options_ & ShowPrivateStatusBar)
      privateStatusBar_->show();
    else
      privateStatusBar_->hide();
  }
  else {
    statusbar_ = _sb;
  }
}


//-----------------------------------------------------------------------------


void QtBaseViewer::applyOptions(int _options)
{
  if (_options&ShowPrivateStatusBar)
    setStatusBar(0);
  else if (privateStatusBar_!=0)
    privateStatusBar_->hide();

  if (_options&ShowToolBar)        buttonBar_->show();
  else                             buttonBar_->hide();
  if (_options&ShowPickButton)     pickButton_->show();
  else                             pickButton_->hide();
  if (_options&ShowQuestionButton) questionButton_->show();
  else                             questionButton_->hide();
  if (_options&ShowWheelX)         wheelX_->show();
  else                             wheelX_->hide();
  if (_options&ShowWheelY)         wheelY_->show();
  else                             wheelY_->hide();
  if (_options&ShowWheelZ)         wheelZ_->show();
  else                             wheelZ_->hide();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::makeCurrent() {
  glWidget_->makeCurrent();
}

void QtBaseViewer::swapBuffers() {
  glWidget_->swapBuffers();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::sceneGraph(SceneGraph::BaseNode* _root)
{
  sceneGraphRoot_ = _root;

  if (sceneGraphRoot_)
  {
    // get draw modes
    SceneGraph::CollectDrawModesAction action;
    SceneGraph::traverse(sceneGraphRoot_, action);
    availDrawModes_ = action.drawModes();
    updatePopupMenu();

    // get scene size
    SceneGraph::BoundingBoxAction act;
    SceneGraph::traverse(sceneGraphRoot_, act);

    Vec3d bbmin = (Vec3d) act.bbMin();
    Vec3d bbmax = (Vec3d) act.bbMax();

    if ( ( bbmin[0] > bbmax[0] ) ||
         ( bbmin[1] > bbmax[1] ) ||
         ( bbmin[2] > bbmax[2] )   )
      setScenePos( Vec3d( 0.0,0.0,0.0 ) , 1.0 );
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


void QtBaseViewer::enablePopupMenu(bool _enable)
{
  popupEnabled_ = _enable;

  if ( popupEnabled_ ) {
    glView_->setContextMenuPolicy( Qt::DefaultContextMenu );
  } else {
    glView_->setContextMenuPolicy( Qt::CustomContextMenu );
  }

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
  
  emit viewChanged();

  updateGL();
}


void QtBaseViewer::projectionMode(ProjectionMode _p)
{
  if ((projectionMode_ = _p) == ORTHOGRAPHIC_PROJECTION)
    projectionButton_->setIcon( QPixmap(orthoIcon) );
  else
    projectionButton_->setIcon( QPixmap(perspectiveIcon) );

  updateProjectionMatrix();
}

void QtBaseViewer::toggleNavigationMode()
{
  if (navigationMode_ == NORMAL_NAVIGATION)
    navigationMode(FIRSTPERSON_NAVIGATION);
  else
    navigationMode(NORMAL_NAVIGATION);
}


void QtBaseViewer::navigationMode(NavigationMode _n)
{
  if ((navigationMode_ = _n) == NORMAL_NAVIGATION)
    emit navigationModeChanged( true );
  else
    emit navigationModeChanged( false );
}


void QtBaseViewer::updateProjectionMatrix()
{
  if( projectionUpdateLocked_ )
    return;

  makeCurrent();

  glstate_->reset_projection();

  // In scereo mode we have to use a perspective matrix
  if (stereo_ || projectionMode_ == PERSPECTIVE_PROJECTION)
  {
    double aspect;

    if (isVisible() && glWidth() && glHeight())
      aspect = (double) glWidth() / (double) glHeight();
    else
      aspect = 1.0;

    glstate_->perspective(fovy_, (GLdouble) aspect,
                          near_, far_);
  }
  else
  {
    double aspect;

    if (isVisible() && glWidth() && glHeight())
      aspect = (double) glWidth() / (double) glHeight();
    else
      aspect = 1.0;

    glstate_->ortho( -orthoWidth_, orthoWidth_,
                     -orthoWidth_/aspect, orthoWidth_/aspect,
                     near_, far_ );
  }
  
}


//-----------------------------------------------------------------------------


void QtBaseViewer::setScenePos(const ACG::Vec3d& _center, double _radius, const bool _setCenter)
{
  if(_setCenter) {
    scene_center_ = trackball_center_ = _center;
  }

  scene_radius_ = trackball_radius_ = _radius;

  orthoWidth_ = 2.0   * scene_radius_;
  
  ACG::Vec3d c = glstate_->modelview().transform_point(scene_center_);

  // Set far plane
  far_    = std::max(0.0002f * scene_radius_,  -(c[2] - scene_radius_));

  // Set near plane
  near_   = std::max(0.0001f * scene_radius_,  -(c[2] + scene_radius_));


  updateProjectionMatrix();
  updateGL();
}

//-----------------------------------------------------------------------------

void QtBaseViewer::setSceneCenter( const ACG::Vec3d& _center ) {

	scene_center_ = trackball_center_ = _center;
}

//----------------------------------------------------------------------------


void QtBaseViewer::viewingDirection( const Vec3d& _dir, const Vec3d& _up )
{
  // calc eye point for this direction
  ACG::Vec3d eye = scene_center_ - _dir*(3.0*scene_radius_);

  glstate_->reset_modelview();
  glstate_->lookAt((ACG::Vec3d)eye, (ACG::Vec3d)scene_center_, (ACG::Vec3d)_up);
  
  emit viewChanged();
}


//-----------------------------------------------------------------------------

void QtBaseViewer::setFovy( double _fovy )
{
  // set new field of view
  fovy_ = _fovy;
  
  emit viewChanged();
}

//-----------------------------------------------------------------------------

void QtBaseViewer::actionMode(ActionMode _am)
{
  moveButton_->setDown(false);
  lightButton_->setDown(false);
  pickButton_->setDown(false);
  questionButton_->setDown(false);

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
      moveButton_->setDown(true);
      break;
    }


    case LightMode:
    {
      glView_->setCursor(Qt::PointingHandCursor);
      glBase_->setCursor(Qt::PointingHandCursor);
      lightButton_->setDown(true);
      break;
    }


    case PickingMode:
    {
      glView_->setCursor(Qt::ArrowCursor);
      glBase_->setCursor(Qt::ArrowCursor);
      pickButton_->setDown(true);
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
      questionButton_->setDown(true);
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
  makeCurrent();
  if (funcMenu_==0)  updatePopupMenu();
  if ( (backFaceCulling_ = _b) )
    ACG::GLState::enable( GL_CULL_FACE );
  else
    ACG::GLState::disable( GL_CULL_FACE );

  action_["BackfaceCulling"]->setChecked( backFaceCulling_ );
  updateGL();
}


void QtBaseViewer::twoSidedLighting(bool _b)
{
  makeCurrent();
  if (funcMenu_==0)  updatePopupMenu();
  glstate_->set_twosided_lighting(twoSidedLighting_=_b);
  action_["TwoSidedLighting"]->setChecked(twoSidedLighting_);
  updateGL();
}


void QtBaseViewer::animation(bool _b)
{
  makeCurrent();
  if (funcMenu_==0)  updatePopupMenu();
  animation_ = _b;
  action_["Animation"]->setChecked( animation_ );
  updateGL();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::normalsMode(NormalsMode _mode)
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


  // *****************************************************************
  // Adjust clipping planes
  // *****************************************************************
  // Far plane
  ACG::Vec3d c = glstate_->modelview().transform_point(scene_center_);
  
  // Set far plane
  far_    = std::max(0.0002f * scene_radius_,  -(c[2] - scene_radius_));
  
  // Set near plane
  near_   = std::max(0.0001f * scene_radius_,  -(c[2] + scene_radius_));
  
  
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
      SceneGraph::DrawAction action(curDrawMode_, *glstate_ , false);
      SceneGraph::traverse(sceneGraphRoot_, action);

      if( blending_ )
      {
        SceneGraph::DrawAction action(curDrawMode_, *glstate_, true);
        SceneGraph::traverse(sceneGraphRoot_, action);
      }
    } else {

      // prepare GL state
      makeCurrent();

      ACG::GLState::disable(GL_LIGHTING);
      glClear(GL_DEPTH_BUFFER_BIT);

      // do the picking
      SceneGraph::PickAction action(*glstate_, pickRendererMode_, curDrawMode_);
      SceneGraph::traverse(sceneGraphRoot_, action);

      ACG::GLState::enable(GL_LIGHTING);
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
  ACG::GLState::drawBuffer(GL_BACK_LEFT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawScene_mono();


  // right eye
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(l-offset2, r-offset2, b, t, near_, far_);
  glTranslatef(offset, 0.0, 0.0);
  glMatrixMode(GL_MODELVIEW);
  ACG::GLState::drawBuffer(GL_BACK_RIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawScene_mono();
  ACG::GLState::drawBuffer(GL_BACK);
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
  
  emit viewChanged();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::viewAll()
{
  makeCurrent();

  // move center (in camera coords) to origin and translate in -z dir
  translate(-(glstate_->modelview().transform_point(scene_center_))
	    - Vec3d(0.0, 0.0, 3.0*scene_radius_ ));

  orthoWidth_ = 1.1*scene_radius_;
  double aspect = (double) glWidth() / (double) glHeight();
  if (aspect > 1.0) orthoWidth_ *= aspect;
  updateProjectionMatrix();
  updateGL();

  // sync
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
  
  emit viewChanged();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::flyTo(const QPoint& _pos, bool _move_back)
{
  makeCurrent();

  unsigned int nodeIdx, targetIdx;
  Vec3d hitPoint;

  if (pick(SceneGraph::PICK_ANYTHING, _pos, nodeIdx, targetIdx, &hitPoint))
  {
    if (projectionMode_ == PERSPECTIVE_PROJECTION)
    {
      Vec3d eye(glState().eye());
      Vec3d t = hitPoint - eye;
      Vec3d e = eye + t * (_move_back ? -0.5f : 0.5f);
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
    
    emit viewChanged();
  }
}


void QtBaseViewer::flyTo(const Vec3d&  _position,
			 const Vec3d&  _center,
			 double        _time)
{
  makeCurrent();

  // compute rotation
  Vec3d c = glstate_->modelview().transform_point(_center);
  Vec3d p = glstate_->modelview().transform_point(_position);
  Vec3d view =(p-c).normalize();
  Vec3d z(0,0,1);
  Vec3d axis = (z % -view).normalize();
  double angle = acos(std::max(-1.0,
			      std::min(1.0,
				       (z | view)))) / M_PI * 180.0;

  if (angle > 175)
    axis  = Vec3d(0,1,0);


  // compute translation
  Vec3d target = glstate_->modelview().transform_point(_center);
  Vec3d trans ( -target[0],
		-target[1],
		-target[2] - (_position-_center).norm() );



  // how many frames in _time ms ?
  unsigned int  frames = (unsigned int)(_time / frame_time_);
  if (frames > 1000) frames=1000;




  // animate it
  if (frames > 10)
  {
    Vec3d t = trans / (double)frames;
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


void QtBaseViewer::setView(const GLMatrixd& _modelview,
			   const GLMatrixd& _inverse_modelview)
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
  #ifndef __APPLE__
  glewInit();
  #endif


  // lock update
  lockUpdate();

  // init GL state
  glstate_->initialize();

  // OpenGL state
  ACG::GLState::enable(GL_DEPTH_TEST);
  ACG::GLState::enable(GL_LIGHTING);
  ACG::GLState::disable(GL_DITHER);
  ACG::GLState::shadeModel( GL_FLAT );


  projectionMode(   projectionMode_   );
  normalsMode(      normalsMode_      );
  faceOrientation(  faceOrientation_  );
  backFaceCulling(  backFaceCulling_  );
  twoSidedLighting( twoSidedLighting_ );


  // light sources
  light_matrix_.identity();
  update_lights();


  // scene pos and size
  scene_center_ = trackball_center_ = Vec3d( 0.0, 0.0, 0.0 );
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

  col[0] = col[1] = col[2] = 0.7f;
  pos[3] = col[3] = 0.0f;

#define SET_LIGHT(i,x,y,z) { 			\
    pos[0]=x; pos[1]=y; pos[2]=z;		\
    glLightfv(GL_LIGHT##i, GL_POSITION, pos);	\
    glLightfv(GL_LIGHT##i, GL_DIFFUSE,  col);	\
    glLightfv(GL_LIGHT##i, GL_SPECULAR, col);	\
    ACG::GLState::enable(GL_LIGHT##i);			\
  }

  SET_LIGHT(0,  0.0f,  0.0f, 1.0f);
  SET_LIGHT(1, -1.0f,  1.0f, 0.7f);
  SET_LIGHT(2,  1.0f,  1.0f, 0.7f);

  col[0] = col[1] = col[2] = 0.3f; col[3] = 1.0f;
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, col);

  glPopMatrix();
}


void QtBaseViewer::rotate_lights(Vec3d& _axis, double _angle)
{
  light_matrix_.rotate(_angle, _axis[0], _axis[1], _axis[2], MULT_FROM_LEFT);
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
    #ifndef __APPLE__
    glewInit();
    #endif

    // lock update
    lockUpdate();

    // init GL state
    glstate_->initialize();

    // initialize lights
    light_matrix_.identity();

    // scene pos and size
    scene_center_ = trackball_center_ = Vec3d( 0.0, 0.0, 0.0 );
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

    ACG::GLState::enable(GL_DEPTH_TEST);
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::disable(GL_DITHER);
    ACG::GLState::shadeModel( GL_FLAT );

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
  // Get current matrices
  const ACG::GLMatrixd& m = glstate_->modelview();
  const ACG::GLMatrixd& p = glstate_->projection();

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
  _view += QString::number(glWidth()) + " " +  QString::number(glHeight()) + " " + QString::number(projectionMode_) + " " + QString::number(orthoWidth_) + "\n";
}


//----------------------------------------------------------------------------


bool QtBaseViewer::decodeView(const QString& _view)
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
  orthoWidth_ = split[35].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }

  // Switch to our gl context
  makeCurrent();

  // set projection mode
  if (projectionMode_ != (ProjectionMode)pMode)
    projectionMode((ProjectionMode)pMode);

  // Apply new modelview matrix
  glstate_->set_modelview(m);


  if (w>0 && h>0 &&
      action_["PasteDropSize"]->isChecked() )
  {
    glstate_->set_projection(p);
    glView_->setFixedSize(w,h);
    updateGeometry();
  }


  updateGL();


  return true;
}


//-----------------------------------------------------------------------------


void QtBaseViewer::actionDrawMenu( QAction * _action )
{
  SceneGraph::DrawModes::DrawMode mode( _action->data().toUInt() );

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
      if( SceneGraph::DrawModes::DrawMode((*aIter)->data().toUInt()) != mode )
	  (*aIter)->setChecked( false );
    }

    drawMode(mode);
  }

  hidePopupMenus();
  updateGL();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::actionBackground()
{
  const Vec4f bc = glstate_->clear_color() * 255.0;
  QColor backCol((int)bc[0], (int)bc[1], (int)bc[2]);
  QColor c = QColorDialog::getColor(backCol,this);
  if (c != backCol && c.isValid())
    backgroundColor(Vec4f(((double) c.red())   / 255.0,
			  ((double) c.green()) / 255.0,
			  ((double) c.blue())  / 255.0,
			  1.0));

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


void QtBaseViewer::actionPasteDropSize()
{
}

//-----------------------------------------------------------------------------


void QtBaseViewer::actionSynchronize()
{
  setSynchronization( action_["Synchronize"]->isChecked() );
}

//-----------------------------------------------------------------------------

void QtBaseViewer::actionSynchronize(bool _enable)
{
  setSynchronization( _enable );
}

bool QtBaseViewer::synchronization(){
  return synchronized_;
}

//-----------------------------------------------------------------------------

void QtBaseViewer::actionAnimation()
{
  animation(!animation());
}

//-----------------------------------------------------------------------------

void QtBaseViewer::actionAnimation(bool _enable)
{
  animation(_enable);
}

//-----------------------------------------------------------------------------

void QtBaseViewer::actionBackfaceCulling()
{
  backFaceCulling(!backFaceCulling());
}

//-----------------------------------------------------------------------------

void QtBaseViewer::actionBackfaceCulling(bool _enable)
{
  backFaceCulling(_enable);
}

//-----------------------------------------------------------------------------

void QtBaseViewer::actionTwoSidedLighting()
{
  twoSidedLighting(!twoSidedLighting());
}

//-----------------------------------------------------------------------------

void QtBaseViewer::actionTwoSidedLighting(bool _enable)
{
  twoSidedLighting(_enable);
}

//-----------------------------------------------------------------------------

void
QtBaseViewer::createWidgets(const QGLFormat* _format,
                            QStatusBar* _sb,
                            const QtBaseViewer* _share)
{
  statusbar_=privateStatusBar_=0;
  setStatusBar(_sb);
  drawMenu_=0;
  funcMenu_=0;
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


  // private status bar
  assert(statusbar_!=0);
  if (privateStatusBar_!=0)
    layout->addWidget(privateStatusBar_,0); // no stretch


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
  glView_->setFrameStyle(QFrame::NoFrame);

  wheelZ_=new QtWheel( 0,"wheel-z",QtWheel::Vertical);
  wheelZ_->setMinimumSize(wheelZ_->sizeHint());
  wheelZ_->setMaximumSize(wheelZ_->sizeHint());
  connect(wheelZ_,SIGNAL(angleChangedBy(double)),
	  this,SLOT(slotWheelZ(double)));
  wheelZ_->setToolTip( "Translate along <b>z-axis</b>.");
  wheelZ_->setWhatsThis( "Translate along <b>z-axis</b>.");
  if ((options_&ShowWheelZ)==0)
    wheelZ_->hide();

  wheelY_=new QtWheel( 0,"wheel-y",QtWheel::Horizontal);
  wheelY_->setMinimumSize(wheelY_->sizeHint());
  wheelY_->setMaximumSize(wheelY_->sizeHint());
  connect(wheelY_,SIGNAL(angleChangedBy(double)),
          this,SLOT(slotWheelY(double)));
  wheelY_->setToolTip("Rotate around <b>y-axis</b>.");
  wheelY_->setWhatsThis( "Rotate around <b>y-axis</b>.");
  if ((options_&ShowWheelY)==0)
    wheelY_->hide();

  wheelX_=new QtWheel( 0,"wheel-x",QtWheel::Vertical);
  wheelX_->setMinimumSize(wheelX_->sizeHint());
  wheelX_->setMaximumSize(wheelX_->sizeHint());
  connect(wheelX_,SIGNAL(angleChangedBy(double)),
          this,SLOT(slotWheelX(double)));
  wheelX_->setToolTip("Rotate around <b>x-axis</b>.");
  wheelX_->setWhatsThis( "Rotate around <b>x-axis</b>.");
  if ((options_&ShowWheelX)==0)
    wheelX_->hide();


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

  // If popupEnabled_ this signal will not be emitted!
  // If popupEnabled_ is set to false the Contextmenu Mode will be set to customContextMenuRequested
  // and this signal will be emitted on right click
  connect( glView_ , SIGNAL( customContextMenuRequested( const QPoint& ) ) ,
           this    , SIGNAL( signalCustomContextMenuRequested( const QPoint& ) ) );

  // is stereo possible ?
  if (format.stereo())
    std::cerr << "Stereo buffer requested: "
	      << (glWidget_->format().stereo() ? "ok\n" : "failed\n");


  // toolbar
  buttonBar_= new QToolBar( "Viewer Toolbar", work );
  buttonBar_->setOrientation(Qt::Vertical);

  moveButton_ = new QToolButton( buttonBar_ );
  moveButton_->setIcon( QPixmap(moveIcon) );
  moveButton_->setMinimumSize( 16, 16 );
  moveButton_->setMaximumSize( 32, 32 );
  moveButton_->setToolTip( "Switch to <b>move</b> mode." );
  moveButton_->setWhatsThis(
                  "Switch to <b>move</b> mode.<br>"
                  "<ul><li><b>Rotate</b> using <b>left</b> mouse button.</li>"
                  "<li><b>Translate</b> using <b>middle</b> mouse button.</li>"
                  "<li><b>Zoom</b> using <b>left+middle</b> mouse buttons.</li></ul>" );
  QObject::connect( moveButton_, SIGNAL( clicked() ),
                    this,        SLOT( examineMode() ) );

  buttonBar_->addWidget( moveButton_)->setText("Move");

  lightButton_ = new QToolButton( buttonBar_ );
  lightButton_->setIcon( QPixmap(lightIcon) );
  lightButton_->setMinimumSize( 16, 16 );
  lightButton_->setMaximumSize( 32, 32 );
  lightButton_->setToolTip("Switch to <b>light</b> mode.");
  lightButton_->setWhatsThis(
                  "Switch to <b>light</b> mode.<br>"
                  "Rotate lights using left mouse button.");
  QObject::connect( lightButton_, SIGNAL( clicked() ),
                    this,         SLOT( lightMode() ) );
  buttonBar_->addWidget( lightButton_)->setText("Light");


  pickButton_ = new QToolButton( buttonBar_ );
  pickButton_->setIcon( QPixmap(pickIcon) );
  pickButton_->setMinimumSize( 16, 16 );
  pickButton_->setMaximumSize( 32, 32 );
  pickButton_->setToolTip("Switch to <b>picking</b> mode.");
  pickButton_->setWhatsThis(
                  "Switch to <b>picking</b> mode.<br>"
                  "Use picking functions like flipping edges.<br>"
                  "To change the mode use the right click<br>"
                  "context menu in the viewer.");
  QObject::connect( pickButton_, SIGNAL( clicked() ),
                    this,        SLOT( pickingMode() ) );
  buttonBar_->addWidget( pickButton_)->setText("Pick");


  questionButton_ = new QToolButton( buttonBar_ );
  questionButton_->setIcon( QPixmap(questionIcon) );
  questionButton_->setMinimumSize( 16, 16 );
  questionButton_->setMaximumSize( 32, 32 );
  questionButton_->setToolTip("Switch to <b>identification</b> mode.");
  questionButton_->setWhatsThis(
                  "Switch to <b>identification</b> mode.<br>"
                  "Use identification mode to get information "
                  "about objects. Click on an object and see "
                  "the log output for information about the "
                  "object.");
  QObject::connect( questionButton_, SIGNAL( clicked() ),
                    this,            SLOT( questionMode() ) );
  buttonBar_->addWidget( questionButton_)->setText("Question");

  buttonBar_->addSeparator();

  homeButton_ = new QToolButton( buttonBar_ );
  homeButton_->setIcon( QPixmap(homeIcon) );
  homeButton_->setMinimumSize( 16, 16 );
  homeButton_->setMaximumSize( 32, 32 );
  homeButton_->setCheckable( false );
  homeButton_->setToolTip("Restore <b>home</b> view.");
  homeButton_->setWhatsThis(
                  "Restore home view<br><br>"
                  "Resets the view to the home view");
  QObject::connect( homeButton_, SIGNAL( clicked() ),
                    this,        SLOT( home() ) );
  buttonBar_->addWidget( homeButton_)->setText("Home");


  setHomeButton_ = new QToolButton( buttonBar_ );
  setHomeButton_->setIcon( QPixmap(sethomeIcon) );
  setHomeButton_->setMinimumSize( 16, 16 );
  setHomeButton_->setMaximumSize( 32, 32 );
  setHomeButton_->setCheckable( false );
  setHomeButton_->setToolTip("Set <b>home</b> view");
  setHomeButton_->setWhatsThis(
                  "Store home view<br><br>"
                  "Stores the current view as the home view");
  QObject::connect( setHomeButton_, SIGNAL( clicked() ),
                    this,           SLOT( setHome() ) );
  buttonBar_->addWidget( setHomeButton_)->setText("Set Home");


  viewAllButton_ = new QToolButton( buttonBar_ );
  viewAllButton_->setIcon( QPixmap(viewallIcon) );
  viewAllButton_->setMinimumSize( 16, 16 );
  viewAllButton_->setMaximumSize( 32, 32 );
  viewAllButton_->setCheckable( false );
  viewAllButton_->setToolTip("View all.");
  viewAllButton_->setWhatsThis(
                  "View all<br><br>"
                  "Move the objects in the scene so that"
                  " the whole scene is visible.");
  QObject::connect( viewAllButton_, SIGNAL( clicked() ),
                    this,           SLOT( viewAll() ) );
  buttonBar_->addWidget( viewAllButton_)->setText("View all");


  projectionButton_ = new QToolButton( buttonBar_ );
  projectionButton_->setIcon( QPixmap(perspectiveIcon) );
  projectionButton_->setMinimumSize( 16, 16 );
  projectionButton_->setMaximumSize( 32, 32 );
  projectionButton_->setCheckable( false );
  projectionButton_->setToolTip(
                "Switch between <b>perspective</b> and "
                "<b>parrallel</b> projection mode.");
  projectionButton_->setWhatsThis(
                "Switch projection modes<br><br>"
                "Switch between <b>perspective</b> and "
                "<b>parrallel</b> projection mode.");
  QObject::connect( projectionButton_, SIGNAL( clicked() ),
                    this,              SLOT( toggleProjectionMode() ) );
  buttonBar_->addWidget( projectionButton_)->setText( "Projection" );


  if (glWidget_->format().stereo())
  {
    stereoButton_ = new QToolButton( buttonBar_ );
    stereoButton_->setIcon( QPixmap(monoIcon) );
    stereoButton_->setMinimumSize( 16, 16 );
    stereoButton_->setMaximumSize( 32, 32 );
    stereoButton_->setCheckable( true );
    stereoButton_->setToolTip( "Toggle stereo viewing");
    stereoButton_->setWhatsThis(
                  "Toggle stereo mode<br><br>"
                  "Use this button to switch between stereo "
                  "and mono view. To use this feature you need "
                  "a stereo capable graphics card and a stereo "
                  "display/projection system.");
    QObject::connect( stereoButton_, SIGNAL( clicked() ),
                      this,          SLOT( toggleStereoMode() ) );
    buttonBar_->addWidget( stereoButton_)->setText( "Stereo");
  }

  buttonBar_->addSeparator();

  sceneGraphButton_ = new QToolButton( buttonBar_ );
  sceneGraphButton_->setIcon( QPixmap(sceneGraphIcon) );
  sceneGraphButton_->setMinimumSize( 16, 16 );
  sceneGraphButton_->setMaximumSize( 32, 32 );
  sceneGraphButton_->setCheckable( false );
  sceneGraphButton_->setToolTip("Toggle scene graph viewer.");
  sceneGraphButton_->setWhatsThis(
                  "Toggle scene graph viewer<br><br>"
                  "The scene graph viewer enables you to examine the "
                  "displayed scene graph and to modify certain nodes.<br><br>"
                  "There are three modi for the scene graph viewer:"
                  "<ul><li><b>hidden</b></li>"
                  "<li><b>split</b>: share space</li>"
                  "<li><b>dialog</b>: own dialog window</li></ul>"
                  "This button toggles between these modi.");
  QObject::connect( sceneGraphButton_, SIGNAL( clicked() ),
                    this,              SLOT( showSceneGraphDialog() ) );
  buttonBar_->addWidget( sceneGraphButton_)->setText( "SceneGraph" );

  glLayout_ = new QGridLayout(work);
  glLayout_->setSpacing( 0 );
  glLayout_->setMargin( 0 );

  glLayout_->addWidget(glView_,    0,0);
  glLayout_->addWidget(buttonBar_, 0,1);

  glLayout_->setColumnStretch(0,1);
  glLayout_->setColumnStretch(1,0);

  if (!(options_ & ShowToolBar))
    buttonBar_->hide();

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

  std::vector< SceneGraph::DrawModes::DrawMode > draw_mode_id;

  draw_mode_id = availDrawModes_.getAtomicDrawModes();

  for ( unsigned int i = 0; i < draw_mode_id.size(); ++i )
  {
    SceneGraph::DrawModes::DrawMode id    = draw_mode_id[i];
    std::string  descr = id.description();

    QAction * action = new QAction( descr.c_str(), drawGroup );
    action->setData( QVariant( quint64(id.getIndex() ) ) );
    action->setCheckable( true );
    action->setChecked( curDrawMode_.containsAtomicDrawMode(id) );
    drawMenuActions_.push_back( action );
  }


  drawMenu_->clear();
  drawMenu_->addActions( drawGroup->actions() );


  // function menu

  if (!funcMenu_)
  {
    funcMenu_=new QMenu( this );

    funcMenu_->addAction( action_[ "Background" ] );
    funcMenu_->addSeparator();
    funcMenu_->addAction( action_[ "Snapshot" ] );
    funcMenu_->addAction( action_[ "SnapshotName" ] );
    funcMenu_->addAction( action_[ "SnapshotSavesView" ] );
    funcMenu_->addSeparator();
    funcMenu_->addAction( action_[ "CopyView" ] );
    funcMenu_->addAction( action_[ "PasteView" ] );
    funcMenu_->addAction( action_[ "PasteDropSize" ] );
    funcMenu_->addSeparator();
    funcMenu_->addAction( action_[ "Synchronize" ] );
    funcMenu_->addSeparator();
    funcMenu_->addAction( action_[ "Animation" ] );
    funcMenu_->addAction( action_[ "BackfaceCulling" ] );
    funcMenu_->addAction( action_[ "TwoSidedLighting" ] );

    connect( funcMenu_, SIGNAL( aboutToHide() ),
	     this, SLOT( hidePopupMenus() ) );
  }


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

  if ( funcMenu_ )
  {
    funcMenu_->blockSignals(true);
    funcMenu_->hide();
    funcMenu_->blockSignals(false);
  }

  if ( pickMenu_ )
  {
    pickMenu_->blockSignals(true);
    pickMenu_->hide();
    pickMenu_->blockSignals(false);
  }
}


//-----------------------------------------------------------------------------


void QtBaseViewer::translate(const Vec3d& _trans)
{
  makeCurrent();
  glstate_->translate(_trans[0], _trans[1], _trans[2], MULT_FROM_LEFT);
}


//-----------------------------------------------------------------------------


void QtBaseViewer::initModelviewMatrix()
{
  makeCurrent();
  glstate_->reset_modelview();
}


//-----------------------------------------------------------------------------


void QtBaseViewer::rotate(const Vec3d&  _axis,
                          double        _angle,
                          const Vec3d&  _center)
{
  makeCurrent();
  Vec3d t = glstate_->modelview().transform_point(_center);
  glstate_->translate(-t[0], -t[1], -t[2], MULT_FROM_LEFT);
  glstate_->rotate(_angle, _axis[0], _axis[1], _axis[2], MULT_FROM_LEFT);
  glstate_->translate( t[0],  t[1],  t[2], MULT_FROM_LEFT);

  sync_send( glstate_->modelview(), glstate_->inverse_modelview() );
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
QtBaseViewer::showSceneGraphDialog()
{
  if (sceneGraphRoot_)
  {
    if (!sceneGraphDialog_)
    {
      sceneGraphDialog_ = new QtSceneGraphDialog( this, sceneGraphRoot_ );

      connect(this,
              SIGNAL(signalSceneGraphChanged(ACG::SceneGraph::BaseNode*)),
              sceneGraphDialog_,
              SLOT(update(ACG::SceneGraph::BaseNode*)));

      connect(sceneGraphDialog_,
              SIGNAL(signalNodeChanged(ACG::SceneGraph::BaseNode*)),
              this,
              SLOT(slotNodeChanged(ACG::SceneGraph::BaseNode*)));
    }

    sceneGraphDialog_->show();
  }
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
  rotate(Vec3d(1,0,0),QtWheel::deg(QtWheel::clip(_dAngle)));
  updateGL();

  // sync
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
  
  emit viewChanged();
}

void QtBaseViewer::slotWheelY(double _dAngle)
{
  rotate(Vec3d(0,1,0),QtWheel::deg(QtWheel::clip(_dAngle)));
  updateGL();

  // sync
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
  
  emit viewChanged();
}

void QtBaseViewer::slotWheelZ(double _dist)
{
  double dz=_dist*0.5/M_PI*scene_radius_*2.0;
  translate(Vec3d(0,0,dz));
  updateGL();

  // sync
  emit(signalSetView(glstate_->modelview(), glstate_->inverse_modelview()));
  
  emit viewChanged();
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

  if (popupEnabled_)
  {
    QPoint  cpos(QCursor::pos()), dpos, fpos, ppos;
    int     offset = 10, dw, dh, fw, fh, pw, ph;
    int     minx, maxx, miny, maxy;
    int     dx(0), dy(0);


#ifdef ARCH_DARWIN
#  define WIDTH  width()
#  define HEIGHT height()
#else
#  define WIDTH  sizeHint().width()
#  define HEIGHT sizeHint().height()
#endif



    // drawing mode menu
    if (drawMenu_)//TODO: && drawMenu_->count()>0)
    {
      dw = drawMenu_->WIDTH;
      dh = drawMenu_->HEIGHT;
      dpos = cpos + QPoint(offset, offset);
    }
    else
    {
      dpos = cpos; dw=dh=0;
    }


    // function menu
    ///@todo:    if (funcMenu_ && funcMenu_->count()>0)
    if (funcMenu_)
    {
      fw = funcMenu_->WIDTH;
      fh = funcMenu_->HEIGHT;
      fpos = cpos + QPoint(offset, -offset-fh);
    }
    else
    {
      fpos = cpos; fw=fh=0;
    }


    // pick mode menu
    ///@todo: if (pickMenu_ && pickMenu_->count()>0)
    if (pickMenu_)
    {
      pw = pickMenu_->WIDTH;
      ph = pickMenu_->HEIGHT;
      ppos = cpos + QPoint(-offset-pw, -ph/2);
    }
    else
    {
      ppos = cpos; pw=ph=0;
    }



    // handle screen boundaries
    minx = std::min(dpos.x(),    std::min(fpos.x(),    ppos.x()));
    maxx = std::max(dpos.x()+dw, std::max(fpos.x()+fw, ppos.x()+pw));
    miny = std::min(dpos.y(),    std::min(fpos.y(),    ppos.y()));
    maxy = std::max(dpos.y()+dh, std::max(fpos.y()+fh, ppos.y()+ph));


    if (minx < 0)
    {
      dx = -minx;
    }
    else if (maxx >= qApp->desktop()->width())
    {
      dx = qApp->desktop()->width() - maxx;
    }

    if (miny < 0)
    {
      dy = -miny;
    }
    else if (maxy >= qApp->desktop()->height())
    {
      dy = qApp->desktop()->height() - maxy;
    }


    dpos += QPoint(dx, dy);
    fpos += QPoint(dx, dy);
    ppos += QPoint(dx, dy);



    // popping up 3 menus only works w/o Qt menu fade/animate effects
    bool animate_menu = qApp->isEffectEnabled(Qt::UI_AnimateMenu);
    bool fade_menu    = qApp->isEffectEnabled(Qt::UI_FadeMenu);
    if (animate_menu)  qApp->setEffectEnabled(Qt::UI_AnimateMenu, false);
    if (fade_menu)     qApp->setEffectEnabled(Qt::UI_FadeMenu, false);


    // popup the 3 menus

    if (drawMenu_) ///@todo: && drawMenu_->count()>0)
    {
      //      SceneGraph::DrawModes::setQPopupMenuChecked(drawMenu_, curDrawMode_);
      drawMenu_->popup(dpos);
    }

    if (funcMenu_) ///@todo: && funcMenu_->count()>0)
      funcMenu_->popup(fpos);

    if (pickMenu_) ///@todo: && pickMenu_->count()>0)
      pickMenu_->popup(ppos);


    // restore effect state
    if (animate_menu)  qApp->setEffectEnabled(Qt::UI_AnimateMenu, true);
    if (fade_menu)     qApp->setEffectEnabled(Qt::UI_FadeMenu, true);


    _event->accept();
  }
}


//-----------------------------------------------------------------------------


void QtBaseViewer::glMousePressEvent(QMouseEvent* _event)
{
  // right button pressed => popup menu (ignore here)
  if (_event->button() == Qt::RightButton && popupEnabled_)
  {
    return;
  }
  else
  {
    switch (actionMode_)
    {
      case ExamineMode:
        if ((_event->modifiers() & Qt::ControlModifier)) // drag&drop
          if ( externalDrag_ ) {
            emit startDragEvent( _event );
          } else {
            startDrag();
          }
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
      (actionMode_ == PickingMode && !popupEnabled_) )
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

//-----------------------------------------------------------------------------

QToolBar* QtBaseViewer::getToolBar() {
  return buttonBar_;
}

QToolBar* QtBaseViewer::removeToolBar() {
  glLayout_->removeWidget( buttonBar_ );
  return buttonBar_;
}

//-----------------------------------------------------------------------------


void QtBaseViewer::moveForward() {
  if(navigationMode_ ==  FIRSTPERSON_NAVIGATION) {
    
    ACG::Vec3d dir = glstate_->viewing_direction();
    
    dir *= -0.1;
    
    glstate_->translate(dir[0], dir[1], dir[2]);
    
    updateGL();
    
    emit viewChanged();
  }
}

void QtBaseViewer::moveBack() {
  if(navigationMode_ ==  FIRSTPERSON_NAVIGATION) {
    ACG::Vec3d dir = glstate_->viewing_direction();
    
    dir *= 0.1;
    
    glstate_->translate(dir[0], dir[1], dir[2]);
    
    updateGL();
    
    emit viewChanged();
  }
}

void QtBaseViewer::strafeLeft() {
  if(navigationMode_ ==  FIRSTPERSON_NAVIGATION) {
    ACG::Vec3d dir = glstate_->right();
    
    dir *= 0.1;
    
    glstate_->translate(dir[0], dir[1], dir[2]);
    
    updateGL();
    
    emit viewChanged();
  }
}

void QtBaseViewer::strafeRight() {
  if(navigationMode_ ==  FIRSTPERSON_NAVIGATION) {
    ACG::Vec3d dir = glstate_->right();
    
    dir *= -0.1;
    
    glstate_->translate(dir[0], dir[1], dir[2]);
    
    updateGL();
    
    emit viewChanged();
  }
}

//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
