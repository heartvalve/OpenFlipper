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



//=============================================================================
//
//  CLASS QtGraphicsButton - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "QtGraphicsButton.hh"

//== IMPLEMENTATION ==========================================================

QtGraphicsButton::QtGraphicsButton (const QImage &_image, QGraphicsItem *_parent, int _width, int _height) :
  QGraphicsItem (_parent),
  checkable_(false),
  checked_(false),
  pressed_(false),
  over_(false),
  width_(_width),
  height_(_height)
{
  if (width_ <= 0)
    width_ = _image.width ();
  if (height_ <= 0)
    height_ = _image.height ();

  // scale image to button dimensions
  QImage i = _image.scaled (width_, height_, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).convertToFormat (QImage::Format_ARGB32);

  checkedPix_ = QPixmap::fromImage (i);

  // generate black/white and transparent images for the other button states
  QImage normal (width_, height_, QImage::Format_ARGB32);
  QImage over (width_, height_, QImage::Format_ARGB32);

  for (int x = 0; x < width_; x++)
    for (int y = 0; y < height_; y++)
  {
    QRgb pix = i.pixel (x, y);
    over.setPixel (x, y, qRgba (qRed (pix), qGreen (pix), qBlue (pix), qAlpha (pix) * 0.5));
    normal.setPixel (x, y, qRgba (qGray (pix), qGray (pix), qGray (pix), qAlpha (pix) * 0.3));
  }
  overPix_ = QPixmap::fromImage (over);
  normalPix_ = QPixmap::fromImage (normal);

  setAcceptHoverEvents (true);
}

//-----------------------------------------------------------------------------

QRectF QtGraphicsButton::boundingRect () const
{
  return QRectF (QPointF(0, 0), QSizeF (width_, height_));
}

//-----------------------------------------------------------------------------

void QtGraphicsButton::paint (QPainter *_painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  _painter->setClipping (false);
  if (pressed_ || checked_)
    _painter->drawPixmap (0, 0, checkedPix_);
  else if (over_)
    _painter->drawPixmap (0, 0, overPix_);
  else
    _painter->drawPixmap (0, 0, normalPix_);
}

//-----------------------------------------------------------------------------

void QtGraphicsButton::setCheckable (bool _value)
{
  checkable_ = _value;
}

//-----------------------------------------------------------------------------

void QtGraphicsButton::setChecked (bool _value)
{
  checked_ = _value;
}

//-----------------------------------------------------------------------------

bool QtGraphicsButton::isChecked () const
{
  return checked_;
}

//-----------------------------------------------------------------------------

void QtGraphicsButton::hoverEnterEvent (QGraphicsSceneHoverEvent *)
{
  over_ = true;
  update ();
}

//-----------------------------------------------------------------------------

void QtGraphicsButton::hoverLeaveEvent (QGraphicsSceneHoverEvent *)
{
  over_ = false;
  update ();
}

//-----------------------------------------------------------------------------

void QtGraphicsButton::mousePressEvent ( QGraphicsSceneMouseEvent *_event)
{
  _event->accept ();
  pressed_ = true;
  if (checkable_)
    checked_ = !checked_;
  update ();

  emit pressed ();
}

//-----------------------------------------------------------------------------

void QtGraphicsButton::mouseReleaseEvent (QGraphicsSceneMouseEvent *_event)
{
  _event->accept ();
  pressed_ = false;
  update ();
}

//-----------------------------------------------------------------------------

QVariant QtGraphicsButton::itemChange (GraphicsItemChange _change, const QVariant &_value)
{
  // reset state if button was hidden
  if (_change == QGraphicsItem::ItemVisibleHasChanged)
  {
    pressed_ = false;
    over_ = false;
  }
  return QGraphicsItem::itemChange (_change, _value);
}

//=============================================================================
