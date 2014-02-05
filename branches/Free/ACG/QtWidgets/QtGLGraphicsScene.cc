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
//  CLASS QtGLGraphicsScene - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "QtGLGraphicsScene.hh"
#include <QPainter>
#include <QPaintEngine>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QApplication>

//== NAMESPACES ===============================================================

namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ===========================================================

QtGLGraphicsScene::QtGLGraphicsScene(QtBaseViewer* _w) :
  QGraphicsScene (),
  w_(_w)
{
}

 
void QtGLGraphicsScene::drawBackground(QPainter *_painter, const QRectF &_rect)
{
  // Check for switch in qt4.6 to OpenGL2
  #if QT_VERSION >= 0x040600
  if (_painter->paintEngine()->type() != QPaintEngine::OpenGL && _painter->paintEngine()->type() != QPaintEngine::OpenGL2 ) {
    std::cerr << "QtGLGraphicsScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view\n";
    return;
  }
  #else
  if (_painter->paintEngine()->type() != QPaintEngine::OpenGL ) {
    std::cerr << "QtGLGraphicsScene: drawBackground needs a QGLWidget to be set as viewport on the graphics view\n";
    return;
  }
  #endif
  
  // Initialize background first
  _painter->setBackground(QApplication::palette().window());
  _painter->eraseRect(_rect);
  
  // From now on we do OpenGL direct painting on the scene
  #if QT_VERSION >= 0x040600
  // Tell Qt that we directly use OpenGL
  _painter->beginNativePainting();
  #endif

  // Clear the depth buffer (This is required since QT4.6 Otherwise we get an emtpty scene!
  glClear(GL_DEPTH_BUFFER_BIT);
  
  w_->paintGL();

      #if QT_VERSION >= 0x040600
  // The rest is painting through QT again. 
  _painter->endNativePainting();
  #endif
}


void QtGLGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* _e)
{
  QGraphicsScene::mousePressEvent(_e);
  if (_e->isAccepted())
    return;

  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseButtonPress ,p, _e->screenPos(), _e->button(),
	         _e->buttons(), _e->modifiers());
  w_->glMousePressEvent(&me);
  _e->accept();
}

void QtGLGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _e)
{
  QGraphicsScene::mouseDoubleClickEvent(_e);
  if (_e->isAccepted())
    return;

  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseButtonDblClick ,p, _e->screenPos(), _e->button(),
	         _e->buttons(), _e->modifiers());
  w_->glMouseDoubleClickEvent(&me);
  _e->accept();
}

void QtGLGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* _e)
{
  QGraphicsScene::mouseReleaseEvent(_e);
  if (_e->isAccepted())
    return;

  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseButtonRelease ,p, _e->screenPos(), _e->button(),
	         _e->buttons(), _e->modifiers());
  w_->glMouseReleaseEvent(&me);
  _e->accept();
}

void QtGLGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* _e)
{
  QGraphicsScene::mouseMoveEvent(_e);
  if (_e->isAccepted())
    return;

  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QMouseEvent me(QEvent::MouseMove ,p, _e->screenPos(), _e->button(),
	         _e->buttons(), _e->modifiers());
  w_->glMouseMoveEvent(&me);
  _e->accept();
}

void QtGLGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent* _e)
{
  QGraphicsScene::wheelEvent(_e);
  if (_e->isAccepted())
    return;
  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QWheelEvent we(p, _e->screenPos(), _e->delta(), _e->buttons(),
		 _e->modifiers(), _e->orientation());
  w_->glMouseWheelEvent(&we);
  _e->accept();
}

void QtGLGraphicsScene::keyPressEvent(QKeyEvent* _e)
{
  QGraphicsScene::keyPressEvent(_e);
  if (_e->isAccepted())
    return;
  w_->glKeyPressEvent (_e);
}

void QtGLGraphicsScene::keyReleaseEvent(QKeyEvent* _e)
{
  QGraphicsScene::keyReleaseEvent(_e);
  if (_e->isAccepted())
    return;
  w_->glKeyReleaseEvent (_e);
}

void QtGLGraphicsScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* _e)
{
  QGraphicsScene::contextMenuEvent(_e);
  if (_e->isAccepted())
    return;

  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QContextMenuEvent ce(static_cast<QContextMenuEvent::Reason> (_e->reason()),
		       p, _e->screenPos(), _e->modifiers());
  w_->glContextMenuEvent(&ce);
  _e->accept();
}

void QtGLGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent* _e)
{
  QGraphicsScene::dragEnterEvent(_e);
  if (_e->isAccepted())
    return;

  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QDragEnterEvent de(p, _e->possibleActions(), _e->mimeData(), _e->buttons(),
		     _e->modifiers ());
  w_->glDragEnterEvent(&de);
  _e->accept();
}

void QtGLGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent* _e)
{
  QGraphicsScene::dropEvent(_e);
  if (_e->isAccepted())
    return;
  QPoint p (_e->scenePos().x(), _e->scenePos().y());
  QDropEvent de(p, _e->possibleActions(), _e->mimeData(), _e->buttons(),
		_e->modifiers ());
  w_->glDropEvent(&de);
  _e->accept();
}

//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
