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
//  CLASS SimpleViewer - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/widgets/glWidget/QtBaseViewer.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/Scenegraph/SceneGraphAnalysis.hh>
#include "simpleGLGraphicsScene.hh"


#include "simpleViewer.hh"

//== NAMESPACES ===============================================================


//== CLASS DEFINITION =========================================================


SimpleViewer::SimpleViewer(QWidget* _parent) :
  QGraphicsView (_parent),
  props_(-1),
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
  setFrameStyle(QFrame::NoFrame);

  unsigned int maxPases = 1;
  ACG::Vec3d bbmin,bbmax;
  ACG::SceneGraph::analyzeSceneGraph(PluginFunctions::getSceneGraphRootNode(),maxPases,bbmin,bbmax);

  mainWidget_->sceneGraph ( PluginFunctions::getSceneGraphRootNode(), maxPases,bbmin,bbmax,true);

  props_.drawMode (OpenFlipper::Options::defaultDrawMode(0));
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

