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
//   $Revision: 5162 $
//   $Author: moebius $
//   $Date: 2009-02-27 17:40:45 +0100 (Fr, 27. Feb 2009) $
//
//=============================================================================




//=============================================================================
//
//  CLASS SimpleViewer - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/widgets/glWidget/QtBaseViewer.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include "simpleGLGraphicsScene.hh"

#include "simpleViewer.hh"

//== NAMESPACES ===============================================================


//== CLASS DEFINITION =========================================================


SimpleViewer::SimpleViewer(QWidget* _parent) :
  QGraphicsView (_parent),
  actionMode_ (Viewer::ExamineMode)
{
  QGLWidget *share = PluginFunctions::shareGLWidget (); 
  if (!share)
    initialize (QGLFormat::defaultFormat (), 0);
  else
    initialize (share->format(), share);
}

SimpleViewer::~SimpleViewer()
{
}

void SimpleViewer::resizeEvent(QResizeEvent *_event) {
  QGraphicsView::resizeEvent(_event);
  if (scene())
  {
    scene()->setSceneRect(QRect(QPoint(0, 0), _event->size()));
    mainWidget_->setGeometry (QRect(QPoint(0, 0), _event->size()));
  }
  
}


//=============================================================================
//=============================================================================

void SimpleViewer::initialize (const QGLFormat & _format, QGLWidget *_shareWidget)
{

  connect (&props_, SIGNAL( getPickMode(std::string&) ),
           this, SLOT( getPickMode(std::string&) ),Qt::DirectConnection );
  connect (&props_, SIGNAL( getActionMode(Viewer::ActionMode&) ),
           this, SLOT( getActionMode(Viewer::ActionMode&) ),Qt::DirectConnection );
  connect (&props_, SIGNAL( setActionMode(const Viewer::ActionMode) ),
           this, SLOT( setActionMode(const Viewer::ActionMode)), Qt::DirectConnection );

  glWidget_ = new QGLWidget (_format, 0, _shareWidget);

  setFocusPolicy (Qt::StrongFocus);
  setAcceptDrops (true);
  setViewport(glWidget_);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

  scene_ = new SimpleGLGraphicsScene ();

  setScene (scene_);

  mainWidget_ = new glViewer (scene_, glWidget_, props_);

  scene_->addItem (mainWidget_);
  scene_->setView (mainWidget_);
  mainWidget_->setGeometry (QRect(QPoint(0, 0), size()));

  mainWidget_->sceneGraph (PluginFunctions::getSceneGraphRootNode (), true);

  props_.drawMode (OpenFlipper::Options::standardDrawMode());
}

void SimpleViewer::setActionMode(const Viewer::ActionMode _am)
{
  actionMode_ = _am;
}

void SimpleViewer::getActionMode(Viewer::ActionMode & _am)
{
  _am = actionMode_;
}

void SimpleViewer::getPickMode(std::string & _name)
{
  _name = "SimpleViewer";
}

Viewer::ViewerProperties & SimpleViewer::properties()
{
  return props_;
}

glViewer * SimpleViewer::viewer()
{
  return mainWidget_;
}

