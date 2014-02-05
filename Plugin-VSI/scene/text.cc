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

#include "text.hh"

#define BACK_OFFSET 2

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::Text - IMPLEMENTATION
//
//=============================================================================

/// Constructor
Text::Text (QGraphicsItem *_parent) :
  QGraphicsSimpleTextItem (_parent),
  backgroundSet_ (false),
  leftOut_(true),
  rightOut_(true),
  alignment_ (Qt::AlignLeft),
  hStretch_ (true)
{
  setBrush (Qt::white);
}

//------------------------------------------------------------------------------

/// Constructor
Text::Text (const QString &_text, QGraphicsItem *_parent) :
  QGraphicsSimpleTextItem (_text, _parent),
  text_ (_text),
  backgroundSet_ (false),
  leftOut_(true),
  rightOut_(true),
  alignment_ (Qt::AlignLeft),
  hStretch_ (true)
{
  setBrush (Qt::white);
}

//------------------------------------------------------------------------------

/// Destructor
Text::~Text ()
{
}

//------------------------------------------------------------------------------

/// Sets the geometry
void Text::setGeometry (const QRectF &_rect)
{
  QFontMetrics fn (font ());

  int width = _rect.width ();

  if (backgroundSet_)
  {
    if (rightOut_)
      width -= _rect.height () / 2;
    else
      width -= BACK_OFFSET * 2;

    if (leftOut_)
      width -= _rect.height () / 2;
    else
      width -= BACK_OFFSET * 2;
  }

  if (fn.boundingRect (text_).width () > width)
  {
    QString nt = text_ + "...";
    while (fn.boundingRect (nt).width () > width && nt.length () > 3)
      nt.remove (nt.length () - 4, 1);
    QGraphicsSimpleTextItem::setText (nt);
  }
  else
    QGraphicsSimpleTextItem::setText (text_);

  QGraphicsSimpleTextItem::setPos (_rect.topLeft ());
  QGraphicsLayoutItem::setGeometry (_rect);
}

//------------------------------------------------------------------------------

// size information for layouting
QSizeF Text::sizeHint (Qt::SizeHint _which, const QSizeF &/*_constraint*/) const
{
  QFontMetrics fn (font ());
  QSizeF sh;

  int w = 0;
  if (rightOut_)
    w += (fn.height () / 2) + (BACK_OFFSET * 2);
  else
    w += BACK_OFFSET * 2;
  if (leftOut_)
    w += (fn.height () / 2) + (BACK_OFFSET * 2);
  else
    w += BACK_OFFSET * 2;

  QSizeF bOff = QSizeF (w, BACK_OFFSET * 2);

  switch (_which) {
    case Qt::MinimumSize:
        sh = QSizeF(fn.boundingRect ("...").width (), fn.height ());
        if (backgroundSet_)
          sh += bOff;
        break;
    case Qt::PreferredSize:
        sh = QSizeF(fn.boundingRect (text_).width (), fn.height ());
        if (backgroundSet_)
          sh += bOff;
        break;
    case Qt::MaximumSize:
        sh = QSizeF(fn.boundingRect (text_).width (), fn.height ());
        if (backgroundSet_)
          sh += bOff;
        if (hStretch_)
          sh += QSizeF (65535, 0);
        break;
    case Qt::MinimumDescent:
        sh = QSizeF(0, fn.descent ());
    default:
      break;
  }

  return sh;
}

//------------------------------------------------------------------------------

/// Background painting
void Text::paint (QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget)
{
  qreal h = geometry ().size ().height () / 2.0;

  if (backgroundSet_)
  {
    QPainterPath path;

    path.moveTo (geometry ().size ().width () / 2, 0);
    if (leftOut_)
    {
      path.lineTo (h, 0);
      path.arcTo (0, 0, h * 2.0, h * 2.0, 90, 180);
    }
    else
    {
      path.lineTo (-h, 0);
      path.arcTo (-h * 2, 0, h * 2.0, h * 2.0, 90, -180);
    }

    if (rightOut_)
    {
      path.lineTo (geometry ().size ().width () - h, geometry ().size ().height ());
      path.arcTo (geometry ().size ().width () - (h * 2), 0, h * 2.0, h * 2.0, -90, 180);
    }
    else
    {
      path.lineTo (geometry ().size ().width () + h, geometry ().size ().height ());
      path.arcTo (geometry ().size ().width (), 0, h * 2.0, h * 2.0, -90, -180);
    }
    path.lineTo (geometry ().size ().width () / 2, 0);

    _painter->setBrush (backgroundBrush_);
    _painter->setPen (backgroundPen_);
    _painter->drawPath (path);


    QLinearGradient lG;

    lG.setStart (0, h);
    lG.setFinalStop(0, h * 2);
    lG.setColorAt(0, Qt::transparent);
    lG.setColorAt(1, QColor (0, 0, 0, 192));

    _painter->setBrush (lG);
    _painter->setPen (Qt::NoPen);
    _painter->drawPath(path);

    lG.setStart (0, h);
    lG.setFinalStop(0, 0);
    lG.setColorAt(0, Qt::transparent);
    lG.setColorAt(1, QColor (255, 255, 255, 192));

    QPen pen;
    pen.setColor (Qt::transparent);
    pen.setWidthF (4);
    _painter->setBrush (lG);
    _painter->setPen (pen);
    _painter->drawPath(path);

    if (leftOut_)
      _painter->translate (h, BACK_OFFSET);
    else
      _painter->translate (BACK_OFFSET * 2, BACK_OFFSET);
  }

  QFontMetrics fn (font ());
  int trans;
  if (alignment_ == Qt::AlignHCenter)
  {
    trans = geometry ().size ().width ();

    if (backgroundSet_)
    {
      if (rightOut_)
        trans -= h;
      else
        trans -= BACK_OFFSET * 2;
      if (leftOut_)
        trans -= h;
      else
        trans -= BACK_OFFSET * 2;
    }

    _painter->translate ((trans - fn.boundingRect (QGraphicsSimpleTextItem::text ()).width ()) / 2, 0);
  }
  else if (alignment_ == Qt::AlignRight)
  {
    trans = geometry ().size ().width ();

    if (backgroundSet_)
    {
      if (rightOut_)
        trans -= h;
      else
        trans -= BACK_OFFSET * 2;
      if (leftOut_)
        trans -= h;
      else
        trans -= BACK_OFFSET * 2;
    }

    _painter->translate (trans - fn.boundingRect (QGraphicsSimpleTextItem::text ()).width (), 0);
  }
  else
  {

  }

  QGraphicsSimpleTextItem::paint (_painter, _option, _widget);
}

//------------------------------------------------------------------------------

/// Enables background painting
void Text::setBackground(bool _leftOut, bool _rightOut)
{
  backgroundSet_ = true;
  leftOut_       = _leftOut;
  rightOut_      = _rightOut;
  updateGeometry ();
}

//------------------------------------------------------------------------------

/// Placement of the text in a stretched widget 
void Text::setAlignment(Qt::Alignment _alignment)
{
  if (_alignment & Qt::AlignLeft)
    alignment_ = Qt::AlignLeft;
  else if (_alignment & Qt::AlignHCenter)
    alignment_ = Qt::AlignHCenter;
  else if (_alignment & Qt::AlignRight)
    alignment_ = Qt::AlignRight;
}

//------------------------------------------------------------------------------

/// Should this widget be stretchable in horizontal direction
void Text::setHorizontalStretch(bool _stretch)
{
  hStretch_ = _stretch;
  updateGeometry ();
}

//------------------------------------------------------------------------------

/// Disables backgorund painting
void Text::clearBackground()
{
  backgroundSet_ = false;
  updateGeometry ();
}

//------------------------------------------------------------------------------

/// Bounding rect
QRectF Text::boundingRect() const
{
  QFontMetrics fn (font ());
  int x, w;

  if (!backgroundSet_)
  {
    QRectF rect = QGraphicsSimpleTextItem::boundingRect ();

    if (alignment_ == Qt::AlignHCenter)
      rect.translate ((geometry ().size ().width () - fn.boundingRect (QGraphicsSimpleTextItem::text ()).width ()) / 2, 0);
    else if (alignment_ == Qt::AlignRight)
      rect.translate (geometry ().size ().width () - fn.boundingRect (QGraphicsSimpleTextItem::text ()).width (), 0);

    return rect;
  }

  w = geometry ().size ().width ();
  x = 0;

  if (!leftOut_)
  {
    w += (fn.height () / 2) + (BACK_OFFSET * 2);
    x -= (fn.height () / 2) + (BACK_OFFSET * 2);
  }

  if (!rightOut_)
  {
    w += (fn.height () / 2) + (BACK_OFFSET * 2);
  }

  return QRectF (QPointF (x, 0), QSizeF(w, geometry ().size ().height ()));
}

//------------------------------------------------------------------------------

/// Sets the background brush
void Text::setBackgroundBrush(QBrush _brush)
{
  backgroundBrush_ = _brush;
  update ();
}

//------------------------------------------------------------------------------

/// Sets the background pen
void Text::setBackgroundPen(QPen _pen)
{
  backgroundPen_ = _pen;
  update ();
}

//------------------------------------------------------------------------------

/// Returns the shape for proper repainting/event handling
QPainterPath Text::shape() const
{
  if (!backgroundSet_)
  {
    QPainterPath shape = QGraphicsSimpleTextItem::shape ();

    QFontMetrics fn (font ());

    if (alignment_ == Qt::AlignHCenter || alignment_ == Qt::AlignRight)
    {
      QTransform trans;
      if (alignment_ == Qt::AlignHCenter)
        trans = QTransform::fromTranslate ((geometry ().size ().width () - fn.boundingRect (QGraphicsSimpleTextItem::text ()).width ()) / 2, 0);
      else if (alignment_ == Qt::AlignRight)
        trans = QTransform::fromTranslate (geometry ().size ().width () - fn.boundingRect (QGraphicsSimpleTextItem::text ()).width (), 0);

      QList<QPolygonF> poly = shape.toSubpathPolygons (trans);

      shape = QPainterPath ();

      foreach (const QPolygonF &p, poly)
        shape.addPolygon(p);
    }

    return shape;
  }

  qreal h = geometry ().size ().height () / 2.0;

  QPainterPath path;

  path.moveTo (geometry ().size ().width () / 2, 0);
  if (leftOut_)
  {
    path.lineTo (h, 0);
    path.arcTo (0, 0, h * 2.0, h * 2.0, 90, 180);
  }
  else
  {
    path.lineTo (-h, 0);
    path.arcTo (-h * 2, 0, h * 2.0, h * 2.0, 90, -180);
  }

  if (rightOut_)
  {
    path.lineTo (geometry ().size ().width () - h, geometry ().size ().height ());
    path.arcTo (geometry ().size ().width () - (h * 2), 0, h * 2.0, h * 2.0, -90, 180);
  }
  else
  {
    path.lineTo (geometry ().size ().width () + h, geometry ().size ().height ());
    path.arcTo (geometry ().size ().width (), 0, h * 2.0, h * 2.0, -90, -180);
  }
  path.lineTo (geometry ().size ().width () / 2, 0);

  return path;
}

//------------------------------------------------------------------------------
}

