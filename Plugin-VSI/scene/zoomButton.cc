/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
#include <QPixmap>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

#include <OpenFlipper/common/GlobalOptions.hh>

#include "zoomButton.hh"
#include "graphicsScene.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::ZoomButton - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ZoomButton::ZoomButton (GraphicsScene *_scene, QGraphicsItem *_parent, Type _type) :
  QGraphicsPixmapItem (_parent),
  scene_ (_scene),
  type_ (_type)
{

  // Load icon depending on type
  switch (type_)
  {
    case In:
      setPixmap (OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"zoom-in.png");
      break;
    case Out:
      setPixmap (OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"zoom-out.png");
      break;
  }

  setOpacity (0.4);

  setAcceptHoverEvents (true);

  connect (&timer_, SIGNAL (timeout ()), this, SLOT (timeout ()));
}

//------------------------------------------------------------------------------

/// Destructor
ZoomButton::~ZoomButton ()
{
}

//------------------------------------------------------------------------------

// make the widget opaque if the mouse is over it
void ZoomButton::hoverEnterEvent (QGraphicsSceneHoverEvent * /*_event*/)
{
  setOpacity (1.0);
}

//------------------------------------------------------------------------------

// make the widget transparent if the mouse leaves it
void ZoomButton::hoverLeaveEvent (QGraphicsSceneHoverEvent * /*_event*/)
{
  setOpacity (0.4);
}

//------------------------------------------------------------------------------

// zoom the scene on mouse press
void ZoomButton::mousePressEvent (QGraphicsSceneMouseEvent *_event)
{
  _event->accept ();

  switch (type_)
  {
    case In:
      scene_->scaleElements (1.25);
      break;
    case Out:
      scene_->scaleElements (0.8);
      break;
  }

  QRectF rect = scene_->sceneRect ();
  pos_ = QPointF (rect.x () + (rect.width () / 2.0), rect.y () + (rect.height () / 2.0));

  // start timer for zooming during the mouse is pressed
  timer_.start (500);
}

//------------------------------------------------------------------------------

// stop zooming on release
void ZoomButton::mouseReleaseEvent (QGraphicsSceneMouseEvent *_event)
{
  _event->accept ();
  timer_.stop ();
}

//------------------------------------------------------------------------------

// zoom on mouse wheel
void ZoomButton::wheelEvent (QGraphicsSceneWheelEvent *_event)
{
   _event->accept ();

  qreal delta;

  if (_event->delta () > 0)
    delta = 1.25;
  else
    delta = 0.8;

  scene_->scaleElements (delta);
}

//------------------------------------------------------------------------------

// zoom the scene on timeout
void ZoomButton::timeout ()
{
  switch (type_)
  {
    case In:
      scene_->scaleElements (1.25, pos_);
      break;
    case Out:
      scene_->scaleElements (0.8, pos_);
      break;
  }
}

//------------------------------------------------------------------------------

/// Activates the timer for zoom with center at _pos (will be called if an element is moved above)
void ZoomButton::activate (QPointF _pos)
{
  pos_ = _pos;
  setOpacity (1.0);
  if (!timer_.isActive ())
    timer_.start (500);
}

//------------------------------------------------------------------------------

/// Stop the timer
void ZoomButton::deactivate ()
{
  setOpacity (0.4);
  timer_.stop ();
}

//------------------------------------------------------------------------------

/// Sets the geometry 
void ZoomButton::setGeometry (const QRectF &_rect)
{
  QGraphicsPixmapItem::setPos (_rect.topLeft ());
  QGraphicsLayoutItem::setGeometry (_rect);
}

//------------------------------------------------------------------------------

// size information for layouting
QSizeF ZoomButton::sizeHint (Qt::SizeHint _which, const QSizeF &/*_constraint*/) const
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
}
