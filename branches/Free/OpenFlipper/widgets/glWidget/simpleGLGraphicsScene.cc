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
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS SimpleGLGraphicsScene - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/widgets/glWidget/QtBaseViewer.hh>
#include <QApplication>
#include <QPainter>
#include <QPaintEngine>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>

#include "simpleGLGraphicsScene.hh"
#include "CursorPainter.hh"

//== NAMESPACES ===============================================================

//== IMPLEMENTATION ===========================================================

SimpleGLGraphicsScene::SimpleGLGraphicsScene () :
  QGraphicsScene (),
  view_(),
  initialized_(false)
{
  cursorPainter_ = new CursorPainter (this);
  cursorPainter_->setEnabled( OpenFlipperSettings().value("Core/Gui/glViewer/glMouse",true).toBool() );
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
    cursorPainter_->initializeGL ();
    initialized_ = true;
  }

  if (cursorPainter_->enabled())
  {
    // avoid projection matrix stack overflow
    GLdouble mat[16];
    glGetDoublev(GL_PROJECTION_MATRIX, mat);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix ();

    glPushAttrib (GL_ALL_ATTRIB_BITS);
    view_->updateCursorPosition(cursorPainter_->cursorPosition ());
    glPopAttrib ();

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd (mat);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix ();
    glClear(GL_DEPTH_BUFFER_BIT);
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
  cursorPainter_->registerViewer (view_);
}

//-----------------------------------------------------------------------------

bool SimpleGLGraphicsScene::event(QEvent *_event)
{
  if (_event->type() == QEvent::Enter)
  {
    cursorPainter_->setMouseIn (true);
  }
  else if (_event->type() == QEvent::Leave)
  {
    cursorPainter_->setMouseIn (false);
    update ();
  }
  else if (cursorPainter_ && _event->type() == QEvent::GraphicsSceneMouseMove)
  {
    QGraphicsSceneMouseEvent *e = static_cast<QGraphicsSceneMouseEvent*>(_event);
    cursorPainter_->updateCursorPosition (e->scenePos ());
    update ();
  }
  return QGraphicsScene::event (_event);
}

//=============================================================================
//=============================================================================


