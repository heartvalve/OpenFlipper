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
  layout_(_layout)
{
}


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

void QtGLGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* _e)
{
  QGraphicsScene::mousePressEvent(_e);
  if (_e->isAccepted())
    return;
  glViewer *v = findView (_e->scenePos(), true);
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
  glViewer *v = findView (_e->scenePos(), true);
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

  glViewer *v = findView (_e->scenePos(), true);
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
