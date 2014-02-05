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
#include <cmath>

#include <QPolygonF>
#include <QGraphicsItem>
#include <QFontMetrics>
#include <QPen>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>

#include "button.hh"

#define BACK_OFFSET 2

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::Button - IMPLEMENTATION
//
//=============================================================================

/// Constructor
Button::Button (QGraphicsItem *_parent) :
  Text (_parent),
  glow_ (false)
{
  setBackground (true, true);
  setAcceptHoverEvents (true);
}

//------------------------------------------------------------------------------

/// Constructor
Button::Button (const QString &_text, QGraphicsItem *_parent) :
  Text (_text, _parent),
  glow_ (false)
{
  setBackground (true, true);
  setAcceptHoverEvents (true);
}

//------------------------------------------------------------------------------

/// Destructor
Button::~Button ()
{
}

//------------------------------------------------------------------------------

// glow on mouse enter
void Button::hoverEnterEvent (QGraphicsSceneHoverEvent * /*_event*/)
{
  glow_ = true;
  update ();

  QPen pen = backgroundPen_;
  pen.setWidthF (1.0);
  pen.setColor (QColor (127, 166, 218));
  Text::setBackgroundPen (pen);
}

//------------------------------------------------------------------------------

// stop glowing on mouse leave
void Button::hoverLeaveEvent (QGraphicsSceneHoverEvent * /*_event*/)
{
  glow_ = false;
  update ();

  Text::setBackgroundPen (backgroundPen_);
}

//------------------------------------------------------------------------------

// emit pressed () on mouse press
void Button::mousePressEvent (QGraphicsSceneMouseEvent *_event)
{
  QBrush brush = backgroundBrush_;
  
  brush.setColor (brush.color ().lighter (50));
  Text::setBackgroundBrush (brush);
  emit pressed ();
  _event->accept ();
}

//------------------------------------------------------------------------------

// change brush back to normal
void Button::mouseReleaseEvent (QGraphicsSceneMouseEvent *_event)
{
  Text::setBackgroundBrush (backgroundBrush_);
  _event->accept ();
}

//------------------------------------------------------------------------------

/// Sets the background brush
void Button::setBackgroundBrush(QBrush _brush)
{
  backgroundBrush_ = _brush;
  Text::setBackgroundBrush (_brush);
}

//------------------------------------------------------------------------------

/// Sets the background pen
void Button::setBackgroundPen(QPen _pen)
{
  backgroundPen_ = _pen;
  Text::setBackgroundPen (_pen);
}

//------------------------------------------------------------------------------

/// Button glow painting
void Button::paint (QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget)
{
  if (glow_)
  {
    QPainterPath path = shape ();
    QPen curr = _painter->pen ();

    QPen pen = backgroundPen_;
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    pen.setColor (QColor (127, 166, 218, 64));
    pen.setWidthF (7.0);
    _painter->setPen (pen);
    _painter->drawPath (path);
    pen.setColor (QColor (127, 166, 218, 128));
    pen.setWidthF (5.0);
    _painter->setPen (pen);
    _painter->drawPath (path);
    pen.setColor (QColor (127, 166, 218, 192));
    pen.setWidthF (3.0);
    _painter->setPen (pen);
    _painter->drawPath (path);

    _painter->setPen (curr);
  }
  Text::paint (_painter, _option, _widget);
}

//------------------------------------------------------------------------------

/// Bounding rectangle
QRectF Button::boundingRect() const
{
  QRectF rect = Text::boundingRect ();

  if (rect.width () && rect.height ())
    rect.adjust(-3, -3, 3, 3);

  return rect;
}

//------------------------------------------------------------------------------

/// Sets the geometry
void Button::setGeometry(const QRectF & _rect)
{
  Text::setGeometry (_rect);
}

//------------------------------------------------------------------------------
}
