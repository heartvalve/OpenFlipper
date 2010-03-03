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

//== INCLUDES =================================================================

#include <QResizeEvent>

#include "graphicsView.hh"

//== NAMESPACES ===============================================================

namespace VSI {

//=============================================================================
//
//  CLASS VSI::GraphicsView - IMPLEMENTATION
//
//=============================================================================

/// Constructor
GraphicsView::GraphicsView(QWidget* _parent) :
  QGraphicsView (_parent)
{
  setFocusPolicy(Qt::StrongFocus);
  setAcceptDrops (true);
  setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
}

//------------------------------------------------------------------------------

// handle resize events
void GraphicsView::resizeEvent(QResizeEvent *_event) {
  QGraphicsView::resizeEvent(_event);

  if (scene())
  {
    scene()->setSceneRect(QRect(QPoint(0, 0), _event->size()));
    emit sceneRectChanged (QRectF (0, 0, _event->size().width(),_event->size().height()));
  }

}

//=============================================================================
}
//=============================================================================
