/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
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
#include <QPainter>
#include <QPaintEngine>

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
  orthoWidth_       = 2.0;
  isRotating_       = false;
  lookAround_       = false;
  near_             = 0.1;
  far_              = 100.0;
  fovy_             = 45.0;

  sceneGraphRoot_   = 0;

  normalsMode_      = DONT_TOUCH_NORMALS;
  projectionMode_   = PERSPECTIVE_PROJECTION;
  navigationMode_	= NORMAL_NAVIGATION;


  light_matrix_.identity();

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


void glViewer::sceneGraph(ACG::SceneGraph::BaseNode* _root, const bool _setCenter)
{
//   if (sceneGraphRoot_ == _root)
//     return;

  sceneGraphRoot_ = _root;

  if (sceneGraphRoot_ )
  {
    // get scene size
    ACG::SceneGraph::BoundingBoxAction act;
    ACG::SceneGraph::traverse(sceneGraphRoot_, act);

    ACG::Vec3d bbmin = (ACG::Vec3d) act.bbMin();
    ACG::Vec3d bbmax = (ACG::Vec3d) act.bbMax();

    if ( ( bbmin[0] > bbmax[0] ) ||
         ( bbmin[1] > bbmax[1] ) ||
         ( bbmin[2] > bbmax[2] )   )
      setScenePos( ACG::Vec3d( 0.0,0.0,0.0 ) , 1.0, _setCenter );
    else
      setScenePos( ( bbmin + bbmax )        * 0.5,
                   ( bbmax - bbmin ).norm() * 0.5,
                   _setCenter);
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


void glViewer::updateProjectionMatrix()
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


void glViewer::setScenePos(const ACG::Vec3d& _center, double _radius, const bool _setCenter)
{
	if(_setCenter) {
		scene_center_ = trackball_center_ = _center;
	}

	scene_radius_ = trackball_radius_ = _radius;

// 	orthoWidth_ = 2.0   * scene_radius_;
	near_       = 0.001 * scene_radius_;
	far_        = 10.0  * scene_radius_;

	updateProjectionMatrix();
	updateGL();
        
        emit viewChanged();
}

//-----------------------------------------------------------------------------

void glViewer::setSceneCenter( const ACG::Vec3d& _center ) {

	scene_center_ = trackball_center_ = _center;
}

//----------------------------------------------------------------------------


void glViewer::viewingDirection( const ACG::Vec3d& _dir, const ACG::Vec3d& _up )
{
  // calc eye point for this direction
  ACG::Vec3d eye = scene_center_ - _dir*(3.0*scene_radius_);

  glstate_->reset_modelview();
  glstate_->lookAt((ACG::Vec3d)eye, (ACG::Vec3d)scene_center_, (ACG::Vec3d)_up);
  
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

      ACG::SceneGraph::DrawAction action( properties_.drawMode() , false);
      ACG::SceneGraph::traverse(sceneGraphRoot_, action, *glstate_, properties_.drawMode() );

      if( blending_ )
      {
        ACG::SceneGraph::DrawAction action(properties_.drawMode(), true);
        ACG::SceneGraph::traverse(sceneGraphRoot_, action, *glstate_, properties_.drawMode());
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
      ACG::SceneGraph::traverse(sceneGraphRoot_, action);

      glEnable(GL_LIGHTING);
      glEnable(GL_BLEND);
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

  draw_lights();
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
  }

  drawScene_anaglyphStereo ();
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
  
  emit viewChanged();

}


//-----------------------------------------------------------------------------


void glViewer::viewAll()
{
	makeCurrent();

	// update scene graph (get new bounding box and set projection right, including near and far plane)
	properties_.lockUpdate();

	// move center (in camera coords) to origin and translate in -z dir
	translate(-(glstate_->modelview().transform_point(scene_center_))
			- ACG::Vec3d(0.0, 0.0, 3.0 * scene_radius_));

	orthoWidth_ = 1.1 * scene_radius_;
	double aspect = (double) glWidth() / (double) glHeight();
	if (aspect > 1.0)
		orthoWidth_ *= aspect;

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

  customAnaglyphSupported_ = glewIsSupported("GL_ARB_fragment_program") &&
      (glewIsSupported("GL_ARB_texture_rectangle") ||
       glewIsSupported("GL_EXT_texture_rectangle") ||
       glewIsSupported("GL_NV_texture_rectangle"));

  initialized_ = true;

  if (sceneGraphRoot_)
  {
    sceneGraph(sceneGraphRoot_, true);
    viewAll ();
  }
}


//-----------------------------------------------------------------------------

/**
 *
 */
void  glViewer::draw_lights() {
//   makeCurrent();
// //
//   glMatrixMode(GL_MODELVIEW);
//   glPushMatrix();
//   glLoadIdentity();
//   glMultMatrixd(light_matrix_.data());
//
//   std::cerr << "light_matrix_\n" << light_matrix_ << std::endl;
// //
//   glPointSize(3);
// //
//   glColor3f(1.0,1.0,1.0);
// //   glDisable(GL_LIGHTING);
// //   glDisable(GL_BLEND);
//   glBegin(GL_LINES);
//     glVertex3f(0.0,  0.0, -1000.0);
//     glVertex3f(0.0,  0.0, 10000.0);
//     glVertex3f(0.0, -1000.0,0.0);
//     glVertex3f(0.0, 1000.0,0.0);
//     glVertex3f(1000.0,0.0,0.0);
//     glVertex3f(-1000.0,0.0,0.0);
// //     glVertex3d(0.0,  0.0, 1.1);
// //     glVertex3d(-1.0,  1.0, 0.8);
// //     glVertex3d( 1.0,  1.0, 0.8);
//
//   glEnd();
// //   glEnable(GL_LIGHTING);
// //   glEnable(GL_BLEND);
// //
//   glPopMatrix();

}

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

    // light sources
    update_lights();

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
  _view += QString::number(glWidth()) + " " +  QString::number(glHeight()) + " " + QString::number(projectionMode_) + " " + QString::number(orthoWidth_) + "\n";
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
  orthoWidth_ = split[35].toDouble(&ok); if ( !ok ) { std::cerr << "Error in decoding View!" << std::endl; return false; }

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
  if(!OpenFlipper::Options::showWheelsAtStartup()) {

	  slotHideWheels();
  }

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
  double dz=_dist*0.5/M_PI*scene_radius_*2.0;
  translate(ACG::Vec3d(0,0,dz));
  updateGL();

  emit viewChanged();
}

//-----------------------------------------------------------------------------


void glViewer::grabGLArea()
{
  glareaGrabbed_ = true;

  if (cursorPainter_)
    cursorPainter_->setCursor(Qt::BlankCursor);
  else
    setCursor(Qt::BlankCursor);
  grabMouse();
  grabKeyboard();
}

void glViewer::releaseGLArea()
{
  glareaGrabbed_ = false;

  ungrabMouse();
  ungrabKeyboard();

  if (cursorPainter_)
    cursorPainter_->setCursor(Qt::ArrowCursor);
  else
    setCursor(Qt::ArrowCursor);
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
        lightMouseEvent(&me);
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
      lightMouseEvent(&me);
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
      lightMouseEvent(&me);
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
        lightMouseEvent(&me);
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

		treatNormalNavigation(_event);

	} else if (navigationMode_ == FIRSTPERSON_NAVIGATION) {

		treatFirstPersonNavigation(_event);
	}
        
}

//----------------------------------------------------------------------------

void glViewer::treatFirstPersonNavigation( QMouseEvent* _event) {

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

void glViewer::treatNormalNavigation( QMouseEvent* _event ) {

	// Normal navigation mode is selected
	QPointF f(mapFromScene(QPointF(_event->pos().x(), _event->pos().y())));
	QPoint pos(f.x(), f.y());

	switch (_event->type()) {
	case QEvent::MouseButtonPress: {
		// shift key -> set rotation center
		if (_event->modifiers() & Qt::ShiftModifier) {
			ACG::Vec3d c;
			if (fast_pick(pos, c)) {
				trackball_center_ = c;
				trackball_radius_ = std::max(scene_radius_, (c - glstate_->eye()).norm() * 0.9f);
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

			// move in z direction
			if ((_event->buttons() & Qt::LeftButton) && (_event->buttons() & Qt::MidButton)) {
				switch (projectionMode()) {
				case PERSPECTIVE_PROJECTION: {
					value_y = scene_radius_ * ((newPoint2D.y() - lastPoint2D_.y())) * 3.0 / (double) glHeight();
					translate(ACG::Vec3d(0.0, 0.0, value_y * factor));
					updateGL();
                                        emit viewChanged();
					break;
				}

				case ORTHOGRAPHIC_PROJECTION: {
					value_y = ((newPoint2D.y() - lastPoint2D_.y())) * orthoWidth_ / (double) glHeight();
					orthoWidth_ -= value_y * factor;
					updateProjectionMatrix();
					updateGL();
                                        emit viewChanged();
					break;
				}
				}
			}

			// move in x,y direction
			else if ((_event->buttons() & Qt::MidButton) || (!allowRotation_ && (_event->buttons() & Qt::LeftButton))) {
				value_x = scene_radius_ * ((newPoint2D.x() - lastPoint2D_.x())) * 2.0 / (double) glWidth();
				value_y = scene_radius_ * ((newPoint2D.y() - lastPoint2D_.y())) * 2.0 / (double) glHeight();
				translate(ACG::Vec3d(value_x * factor, -value_y * factor, 0.0));
			}

			// rotate
			else if (allowRotation_ && (_event->buttons() & Qt::LeftButton)) {
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

void
glViewer::lightMouseEvent(QMouseEvent* _event)
{
  QPointF f (mapFromScene(QPointF(_event->pos().x(), _event->pos().y())));
  QPoint pos (f.x(), f.y());

  switch (_event->type())
  {
    case QEvent::MouseButtonPress:
    {
      lastPoint_hitSphere_ = mapToSphere( lastPoint2D_= pos,
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
   QPoint newPoint2D = pos;

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

void glViewer::viewWheelEvent( QWheelEvent* _event)
{
  double factor = properties_.wheelZoomFactor();

  if (_event->modifiers() == Qt::ShiftModifier)
    factor = properties_.wheelZoomFactorShift();

  if (projectionMode() == PERSPECTIVE_PROJECTION || stereo_)
  {
    double d = -(double)_event->delta() / 120.0 * 0.2 * factor * trackball_radius_/3;
    translate( ACG::Vec3d(0.0, 0.0, d) );
    updateGL();
  }
  else
  {
    double d = (double)_event->delta() / 120.0 * 0.2 * factor * orthoWidth_;
    orthoWidth_ += d;
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

void glViewer::snapshot(QImage& _image, bool _offScreenRendering)
{

  if ( _offScreenRendering ){
    QGLFramebufferObject fb( glstate_->context_width(), glstate_->context_height(), QGLFramebufferObject::CombinedDepthStencil);

    if ( fb.isValid() ){

      fb.bind();
      qApp->processEvents();
      makeCurrent();
      glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      paintGL();
      glFinish();

      _image = fb.toImage().copy(scenePos().x(), scenePos().y(), glWidth(), glHeight());

      return;
    }
  }

  makeCurrent();

  qApp->processEvents();
  makeCurrent();
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  paintGL();
  glFinish();

  copyToImage(_image, scenePos().x(), scenePos().y(), glWidth(), glHeight(), GL_BACK);
}

void glViewer::snapshot()
{
   makeCurrent();

   qApp->processEvents();
   makeCurrent();
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   paintGL();
   glFinish();

   QImage snapshot;
   copyToImage(snapshot, scenePos().x(), scenePos().y(), glWidth(), glHeight(), GL_BACK);

   QFileInfo fi(properties_.snapshotName());

   QString fname = fi.path() + QDir::separator() +fi.baseName() + "." + QString::number(properties_.snapshotCounter()) + ".";

   QString format="png";

   if (fi.completeSuffix() == "ppm")
     format="ppmraw";

   fname += format;

   bool rval=snapshot.save(fname,format.toUpper().toLatin1());


   if (rval)
   {
     emit statusMessage (QString(tr("snapshot: "))+fname,5000);
   }
   else
   {
     emit statusMessage (QString(tr("could not save snapshot to "))+fname);
   }

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
  else if (stereo_ && OpenFlipper::Options::stereoMousePick() &&
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
    double zerop = near_ + ((far_ - near_) * OpenFlipper::Options::focalDistance ());
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

