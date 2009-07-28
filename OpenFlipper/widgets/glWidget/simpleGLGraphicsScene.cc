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
//   $Revision: 5979 $
//   $Author: moebius $
//   $Date: 2009-05-07 10:57:06 +0200 (Do, 07. Mai 2009) $
//
//=============================================================================




//=============================================================================
//
//  CLASS SimpleGLGraphicsScene - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/widgets/glWidget/QtBaseViewer.hh>
#include <QApplication>
#include <QPainter>
#include <QPaintEngine>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>

#include "simpleGLGraphicsScene.hh"

//== NAMESPACES ===============================================================

//== IMPLEMENTATION ===========================================================

SimpleGLGraphicsScene::SimpleGLGraphicsScene () :
  QGraphicsScene (),
  view_(),
  initialized_(false)
{
}


void SimpleGLGraphicsScene::drawBackground(QPainter *_painter, const QRectF &_rect)
{
  if (_painter->paintEngine()->type() != QPaintEngine::OpenGL) {
    std::cerr << "SimpleGLGraphicsScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view\n";
    return;
  }

  if (!view_)
    return;

  if (!initialized_)
  {
    // we use GLEW to manage extensions
    // initialize it first
    glewInit();
    view_->initializeGL();
    initialized_ = true;
  }

  _painter->setBackground(QApplication::palette().window());
  _painter->eraseRect(_rect);

  view_->paintGL();
}


void SimpleGLGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _e)
{
  QGraphicsScene::mouseMoveEvent(_e);
  if (_e->isAccepted())
    return;

  if (view_)
    view_->mouseMoveEvent(_e);
}

void SimpleGLGraphicsScene::setView(glViewer * _view)
{
  view_ = _view;
}

//=============================================================================
//=============================================================================


