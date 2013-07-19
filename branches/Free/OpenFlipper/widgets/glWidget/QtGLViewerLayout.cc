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




//=============================================================================
//
//  CLASS QtGLViewerLayout - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "QtGLViewerLayout.hh"

//== NAMESPACES ===============================================================


//== CLASS IMPLEMENTATION======================================================

QtGLViewerLayout::QtGLViewerLayout (QGraphicsLayoutItem * _parent) :
  QGraphicsLayout (_parent),
  wheelX_ (0),
  wheelY_ (0),
  wheelZ_ (0)
{
}

//-----------------------------------------------------------------------------

void QtGLViewerLayout::addWheelX (QGraphicsWidget *_item)
{
  if (wheelX_)
    items_.remove(items_.indexOf(wheelX_));
  wheelX_ = _item;
  items_.append(_item);
}

//-----------------------------------------------------------------------------

void QtGLViewerLayout::addWheelY (QGraphicsWidget *_item)
{
  if (wheelY_)
    items_.remove(items_.indexOf(wheelY_));
  wheelY_ = _item;
  items_.append(_item);
}

//-----------------------------------------------------------------------------

void QtGLViewerLayout::addWheelZ (QGraphicsWidget *_item)
{
  if (wheelZ_)
    items_.remove(items_.indexOf(wheelZ_));
  wheelZ_ = _item;
  items_.append(_item);
}

//-----------------------------------------------------------------------------

int QtGLViewerLayout::count() const
{
  return items_.size(); 
}

//-----------------------------------------------------------------------------

QGraphicsLayoutItem * QtGLViewerLayout::itemAt(int _i) const
{
  if (_i < 0 || _i >= items_.size())
    return 0;

  return items_[_i];
}

//-----------------------------------------------------------------------------

void QtGLViewerLayout::removeAt (int _index)
{
  if (_index < 0 || _index >= items_.size())
    return;

  if (items_[_index] == wheelX_)
    wheelX_ = 0;
  if (items_[_index] == wheelY_)
    wheelY_ = 0;
  if (items_[_index] == wheelZ_)
    wheelZ_ = 0;

  items_.remove(_index);
}

//-----------------------------------------------------------------------------

QSizeF QtGLViewerLayout::sizeHint(Qt::SizeHint /*_which*/, const QSizeF & _constraint) const
{
  return _constraint;
}

//-----------------------------------------------------------------------------

void QtGLViewerLayout::setGeometry(const QRectF & rect)
{
  QGraphicsLayoutItem::setGeometry (rect);
  reLayout ();
}

//-----------------------------------------------------------------------------

void QtGLViewerLayout::reLayout ()
{
  if (!wheelX_ || !wheelY_ || !wheelZ_)
    return;

  QRectF r = contentsRect ();
  float scale = qMin(300.0, qMin(r.width(), r.height())) / 300.0;

  foreach (QGraphicsWidget *item, items_)
  {
    if (item->size() != item->preferredSize ())
      item->resize (item->preferredSize ());
    item->resetTransform();
    item->setScale (scale);
  }

  wheelX_->setPos (r.left(),
		   r.bottom() - ((wheelY_->size().height() + wheelX_->size().height()) * scale));
  wheelY_->setPos (r.left() + (wheelX_->size().width() * scale),
		   r.bottom() - (wheelY_->size().height() * scale));
  wheelZ_->setPos (r.right() - (wheelZ_->size().width() * scale),
		   r.bottom() - ((wheelY_->size().height() + wheelZ_->size().height()) * scale));
}


//=============================================================================
//=============================================================================
