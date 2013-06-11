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

//== INCLUDES =================================================================
#include <cmath>

#include <QPainterPath>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

#include <OpenFlipper/common/GlobalOptions.hh>

#include "arrow.hh"
#include "graphicsScene.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::Arrow - IMPLEMENTATION
//
//=============================================================================


/// Constructor
Arrow::Arrow (GraphicsScene *_scene, QGraphicsItem *_parent, Direction _dir) :
  QGraphicsPixmapItem (_parent),
  scene_ (_scene),
  dir_ (_dir),
  highlight_ (false)
{
  // load icon depending on direction
  switch (_dir)
  {
    case North:
      setPixmap (OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"go-up.png");
      break;
    case South:
      setPixmap (OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"go-down.png");
      break;
    case East:
      setPixmap (OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"go-right.png");
      break;
    case West:
      setPixmap (OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"go-left.png");
      break;
    case Center:
      setPixmap (OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"go-center.png");
      break;
  }

  setOpacity ((highlight_)? 0.5 : 0.2);

  setAcceptHoverEvents (true);

  connect (&timer_, SIGNAL (timeout ()), this, SLOT (timeout ()));
}

//------------------------------------------------------------------------------

/// Destructor
Arrow::~Arrow ()
{
}

//------------------------------------------------------------------------------

// make the widget opaque if the mouse is over it
void Arrow::hoverEnterEvent (QGraphicsSceneHoverEvent * /*_event*/)
{
  setOpacity (1.0);
}

//------------------------------------------------------------------------------

// make the widget transparent if the mouse leaves it
void Arrow::hoverLeaveEvent (QGraphicsSceneHoverEvent * /*_event*/)
{
  setOpacity ((highlight_)? 0.5 : 0.2);
}

//------------------------------------------------------------------------------

// move the scene on mouse press
void Arrow::mousePressEvent (QGraphicsSceneMouseEvent *_event)
{
  _event->accept ();

  qreal dx = 0.0, dy = 0.0;

  switch (dir_)
  {
    case North:
      dy = 5;
      break;
    case South:
      dy = -5;
      break;
    case East:
      dx = -5;
      break;
    case West:
      dx = 5;
      break;
    case Center:
      {
        QRectF bb = scene_->elementsBoundingRect ();
        QRectF rect = scene_->sceneRect();
        dx = qMax (qMin ((rect.center () - bb.center ()).x (), 5.0), -5.0);
        dy = qMax (qMin ((rect.center () - bb.center ()).y (), 5.0), -5.0);
      }
      break;
  }
  scene_->moveElements (dx, dy, true);

  // start timer for movement during the mouse is pressed
  timer_.start (500);
  moveSelected_ = true;
}

//------------------------------------------------------------------------------

// stop movement on release
void Arrow::mouseReleaseEvent (QGraphicsSceneMouseEvent *_event)
{
  _event->accept ();
  timer_.stop ();
}

//------------------------------------------------------------------------------

// move on mouse wheel
void Arrow::wheelEvent (QGraphicsSceneWheelEvent *_event)
{
   _event->accept ();

  qreal dx = 0.0, dy = 0.0;
  
  switch (dir_)
  {
    case North:
    case South:
      dy = 1;
      break;
    case East:
    case West:
      dx = 1;
      break;
    case Center:
      break;
  }
  dx *= _event->delta () / 4;
  dy *= _event->delta () / 4;
  scene_->moveElements (dx, dy, true);
}

//------------------------------------------------------------------------------

// move the scene on timeout and decrease timer interval for faster movement
void Arrow::timeout ()
{
  if (timer_.interval () == 500)
    interval_ = 50;
  timer_.setInterval (interval_ );
  if (interval_ > 5)
    interval_--;

  qreal dx = 0.0, dy = 0.0;
  
  switch (dir_)
  {
    case North:
      dy = 1;
      break;
    case South:
      dy = -1;
      break;
    case East:
      dx = -1;
      break;
    case West:
      dx = 1;
      break;
    case Center:
      {
        QRectF bb = scene_->elementsBoundingRect ();
        QRectF rect = scene_->sceneRect();
        dx = qMax (qMin ((rect.center () - bb.center ()).x (), 2.0), -2.0);
        dy = qMax (qMin ((rect.center () - bb.center ()).y (), 2.0), -2.0);
      }
      break;
  }
  scene_->moveElements (dx, dy, moveSelected_);
}

//------------------------------------------------------------------------------

/// Activates the timer for movement (will be called if an element is moved above)
void Arrow::activate ()
{
  setOpacity (1.0);
  moveSelected_ = false;
  if (!timer_.isActive ())
    timer_.start (500);
}

//------------------------------------------------------------------------------

/// Stop the timer
void Arrow::reset ()
{
  setOpacity ((highlight_)? 0.5 : 0.2);
  timer_.stop ();
}

//------------------------------------------------------------------------------

// set geometry
void Arrow::setGeometry (const QRectF &_rect)
{
  QGraphicsPixmapItem::setPos (_rect.topLeft ());
  QGraphicsLayoutItem::setGeometry (_rect);
}

//------------------------------------------------------------------------------

// return size information for layouting
QSizeF Arrow::sizeHint (Qt::SizeHint _which, const QSizeF &/*_constraint*/) const
{
  QSizeF sh;
  switch (_which) {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
    case Qt::MaximumSize:
      sh = QSizeF (pixmap ().width (), pixmap ().height ());
      break;
    default:
      break;
  }

  return sh;
}

//------------------------------------------------------------------------------

/// Highlights the widget if the scene can be moved in this direction
void Arrow::setHighlight (bool _highlight)
{
  highlight_ = _highlight;

  if (opacity () != 1.0)
    setOpacity ((highlight_)? 0.5 : 0.2);

}

//------------------------------------------------------------------------------
}

