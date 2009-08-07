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
//  CLASS QtGLGraphicsScene - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include "QtGLGraphicsScene.hh"
#include "QtMultiViewLayout.hh"
#include <OpenFlipper/widgets/glWidget/QtBaseViewer.hh>
#include <QApplication>
#include <QPainter>
#include <QPaintEngine>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>

//== NAMESPACES ===============================================================

//== IMPLEMENTATION ===========================================================

QtGLGraphicsScene::QtGLGraphicsScene(std::vector< glViewer *> *_views,
				     QtMultiViewLayout *_layout) :
  QGraphicsScene (),
  views_(_views),
  layout_(_layout),
  cursorPainter_(0)
{
}

//-----------------------------------------------------------------------------

void QtGLGraphicsScene::drawBackground(QPainter *_painter, const QRectF &_rect)
{
  if (_painter->paintEngine()->type() != QPaintEngine::OpenGL) {
    std::cerr << "QtGLGraphicsScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view\n";
    return;
  }

  static bool initialized = false;
  if (!initialized)
  {
    // we use GLEW to manage extensions
    // initialize it first
    glewInit();
    for (unsigned int i = 0; i < views_->size (); i++)
    {
      views_->at(i)->initializeGL ();
    }
    if (cursorPainter_)
      cursorPainter_->initializeGL ();
    initialized = true;
  }

  if (cursorPainter_ && cursorPainter_->enabled())
  {
    // avoid projection matrix stack overflow
    GLdouble mat[16];
    glGetDoublev(GL_PROJECTION_MATRIX, mat);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix ();

    glPushAttrib (GL_ALL_ATTRIB_BITS);
    for (unsigned int i = 0; i < views_->size (); i++)
    {
      if (views_->at(i)->isVisible())
        views_->at(i)->updateCursorPosition(cursorPainter_->cursorPosition ());
    }
    glPopAttrib ();

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd (mat);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix ();
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  _painter->setBackground(QApplication::palette().window());
  _painter->eraseRect(_rect);

  for (unsigned int i = 0; i < views_->size (); i++)
  {
    if (views_->at(i)->isVisible())
      views_->at(i)->paintGL();
  }


  if (layout_->mode() != QtMultiViewLayout::SingleView)
  {
    glViewer *v = views_->at(PluginFunctions::activeExaminer());

    QPen pen(Qt::red);
    pen.setWidth (2);
    _painter->setPen(pen);
    _painter->drawLine(v->scenePos().x(), v->scenePos().y(),
                       v->scenePos().x(),
                       v->scenePos().y() + v->size().height() - 1);
    _painter->drawLine(v->scenePos().x() + v->size().width(), v->scenePos().y(),
                       v->scenePos().x() + v->size().width(),
                       v->scenePos().y() + v->size().height() - 1);
    _painter->drawLine(v->scenePos().x(), v->scenePos().y() - 1,
                       v->scenePos().x() + v->size().width(),
                       v->scenePos().y() - 1);
    _painter->drawLine(v->scenePos().x(),
                       v->scenePos().y() + v->size().height() - 1,
                       v->scenePos().x() + v->size().width(),
                       v->scenePos().y() + v->size().height() - 1);
  }
}

//-----------------------------------------------------------------------------

glViewer* QtGLGraphicsScene::findView (const QPointF &_p, bool _setActive)
{
  for (unsigned int i = 0; i < views_->size (); i++)
  {
    if (views_->at(i)->contains(views_->at(i)->mapFromScene (_p)))
    {
      if (_setActive && PluginFunctions::activeExaminer() != i)
      {
        PluginFunctions::setActiveExaminer (i);
	update();
      }
      return views_->at(i);
    }
  }
  return NULL;
}

//-----------------------------------------------------------------------------

void QtGLGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _e)
{
  QGraphicsScene::mouseMoveEvent(_e);
  if (_e->isAccepted())
    return;

  glViewer *v = findView (_e->scenePos());
  if (!v)
    return;

  v->mouseMoveEvent(_e);
}

//-----------------------------------------------------------------------------

void QtGLGraphicsScene::setCursorPainter(CursorPainter * _cursorPainter)
{
  cursorPainter_ = _cursorPainter;
}

//-----------------------------------------------------------------------------

bool QtGLGraphicsScene::event(QEvent *_event)
{
  if (cursorPainter_ && _event->type() == QEvent::Enter)
  {
    cursorPainter_->setMouseIn (true);
  }
  else if (cursorPainter_ && _event->type() == QEvent::Leave)
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

