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
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================




//=============================================================================
//
//  CLASS QtGLGraphicsScene - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "QtGLGraphicsScene.hh"
#include <OpenFlipper/widgets/glWidget/QtBaseViewer.hh>
#include <QPainter>
#include <QPaintEngine>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>

//== NAMESPACES ===============================================================

//== IMPLEMENTATION ===========================================================

QtGLGraphicsScene::QtGLGraphicsScene(std::vector< glViewer *> *_views) :
  QGraphicsScene (),
  views_(_views)
{
}

/*
void QtGLGraphicsScene::drawBackground(QPainter *_painter, const QRectF &)
{
    if (_painter->paintEngine()->type() != QPaintEngine::OpenGL) {
        std::cerr << "QtGLGraphicsScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view\n";
        return;
    }

    w_->paintGL();

}

*/

glViewer* QtGLGraphicsScene::findView (const QPointF &p)
{
  for (unsigned int i = 0; i < views_->size (); i++)
  {
      if (views_->at(i)->contains(views_->at(i)->mapFromScene (p)))
	  return views_->at(i);
  }
  return NULL;
}

void QtGLGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* _e)
{
  QGraphicsScene::mousePressEvent(_e);
  if (_e->isAccepted())
    return;
  glViewer *v = findView (_e->scenePos());
  if (!v)
    return;

  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseButtonPress ,p, _e->screenPos(), _e->button(),
	         _e->buttons(), _e->modifiers());
  v->glMousePressEvent(&me);
  _e->accept();
}

void QtGLGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _e)
{
  QGraphicsScene::mouseDoubleClickEvent(_e);
  if (_e->isAccepted())
    return;
  glViewer *v = findView (_e->scenePos());
  if (!v)
    return;

  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseButtonDblClick ,p, _e->screenPos(), _e->button(),
	         _e->buttons(), _e->modifiers());
  v->glMouseDoubleClickEvent(&me);
  _e->accept();
}

void QtGLGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* _e)
{
  QGraphicsScene::mouseReleaseEvent(_e);
  if (_e->isAccepted())
    return;
  glViewer *v = findView (_e->scenePos());
  if (!v)
    return;

  QPointF f (v->mapFromScene (_e->scenePos()));
  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseButtonRelease ,p, _e->screenPos(), _e->button(),
	         _e->buttons(), _e->modifiers());
  v->glMouseReleaseEvent(&me);
  _e->accept();
}

void QtGLGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _e)
{
  QGraphicsScene::mouseMoveEvent(_e);
  if (_e->isAccepted())
    return;
  glViewer *v = findView (_e->scenePos());
  if (!v)
    return;

  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseMove ,p, _e->screenPos(), _e->button(),
	         _e->buttons(), _e->modifiers());
  v->glMouseMoveEvent(&me);
  _e->accept();
}

void QtGLGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent* _e)
{
  QGraphicsScene::wheelEvent(_e);
  if (_e->isAccepted())
    return;

  glViewer *v = findView (_e->scenePos());
  if (!v)
    return;

  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QWheelEvent we(p, _e->screenPos(), _e->delta(), _e->buttons(),
		 _e->modifiers(), _e->orientation());
  v->glMouseWheelEvent(&we);
  _e->accept();
}


//=============================================================================
//=============================================================================
