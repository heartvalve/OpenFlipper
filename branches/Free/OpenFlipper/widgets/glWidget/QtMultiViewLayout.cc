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
//  CLASS QtMultiViewLayout - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "QtMultiViewLayout.hh"

//== NAMESPACES ===============================================================


//== CLASS IMPLEMENTATION======================================================

QtMultiViewLayout::QtMultiViewLayout (QGraphicsLayoutItem * _parent) :
QGraphicsLayout (_parent),
mode_ (SingleView),
spacing_ (2),
primary_ (0)
{
  items_[0] = 0;
  items_[1] = 0;
  items_[2] = 0;
  items_[3] = 0;
}

//-----------------------------------------------------------------------------

void QtMultiViewLayout::addItem (QGraphicsWidget *_item, unsigned int _pos)
{
  if (_pos > 3)
    return;
  items_[_pos] = _item;
}

//-----------------------------------------------------------------------------

void QtMultiViewLayout::setMode (MultiViewMode _mode)
{
  mode_ = _mode;
  invalidate();
}

//-----------------------------------------------------------------------------

void QtMultiViewLayout::setPrimary (unsigned int _i)
{
  if ((int) _i > count ())
    return;
  primary_ = _i;
  invalidate();
}

//-----------------------------------------------------------------------------

void QtMultiViewLayout::setSpacing (unsigned int _s)
{
  spacing_ = _s;
  invalidate();
}

//-----------------------------------------------------------------------------

int QtMultiViewLayout::count() const
{
  int rv = 0;

  for (int i = 0; i < 4; i++)
    if (items_[i])
      ++rv;

  return rv; 
}

//-----------------------------------------------------------------------------

QGraphicsLayoutItem * QtMultiViewLayout::itemAt(int _i) const
{
  if (_i < 0 || _i > 3)
    return 0;
  return items_[_i];
}

//-----------------------------------------------------------------------------

void QtMultiViewLayout::removeAt (int _index)
{
  if (_index < 0 || _index > 3)
    return;
  items_[_index] = 0;
}

//-----------------------------------------------------------------------------

QSizeF QtMultiViewLayout::sizeHint(Qt::SizeHint /*_which*/, const QSizeF & _constraint) const
{
  return _constraint;
}

//-----------------------------------------------------------------------------

void QtMultiViewLayout::setGeometry(const QRectF & rect)
{
  QGraphicsLayoutItem::setGeometry (rect);
  reLayout ();
}

//-----------------------------------------------------------------------------

void QtMultiViewLayout::reLayout ()
{
  MultiViewMode mode = mode_;
  QRectF r = contentsRect ();

  if (!items_[primary_])
    return;
  if (count() != 4)
    mode = SingleView;

  if (mode == SingleView)
  {
    for (int i = 0; i < 4; i++)
      items_[i]->hide();
    items_[primary_]->show ();
  } else if ( mode == DoubleView) {
    for (int i = 0; i < 2; i++)
      if (items_[i])
        items_[i]->show();
    for (int i = 2; i < 4; i++)
      if (items_[i])
        items_[i]->hide();
  }else {
    for (int i = 0; i < 4; i++)
      if (items_[i])
        items_[i]->show();
  }

  switch (mode)
  {
    case SingleView:
      items_[primary_]->setGeometry (r);
      break;
    case DoubleView:
    {
      int width  = (r.width() - spacing_) / 2;
      int height = r.height();
      items_[0]->resize (width, height);
      items_[1]->resize (width, height);

      items_[0]->setPos(r.topLeft());
      items_[1]->setPos(r.x() + width + spacing_, r.y());
    }
    break;
    case Grid:
    {
      int width = (r.width() - spacing_) / 2;
      int height = (r.height() - spacing_) / 2;
      items_[0]->resize (width, height);
      items_[1]->resize (r.width() - width - spacing_, height);
      items_[2]->resize (width, r.height() - height - spacing_);
      items_[3]->resize (r.width() - width - spacing_,r.height() - height - spacing_);
      items_[0]->setPos (r.topLeft());
      items_[1]->setPos (r.x() + width + spacing_, r.y());
      items_[2]->setPos (r.x(), r.y() + height + spacing_);
      items_[3]->setPos (r.x() + width + spacing_, r.y() + height + spacing_);
    }
    break;
    case HSplit:
    {
      int order[4];
      order[0] = primary_;
      for (unsigned int i = 0, j = 1; i < 4; i++)
        if (i != primary_)
          order[j++] = i;
      int width = (r.width() - spacing_) * 3 / 4;
      int ewidth = r.width() - spacing_ - width;
      int eheight = (r.height() - (spacing_ * 2)) / 3;
      items_[order[0]]->resize (width, r.height());
      items_[order[1]]->resize (ewidth, eheight);
      items_[order[2]]->resize (ewidth, eheight);
      items_[order[3]]->resize (ewidth, r.height() - ((eheight + spacing_) * 2));
      items_[order[0]]->setPos (r.topLeft());
      items_[order[1]]->setPos (r.x() + width + spacing_, r.y());
      items_[order[2]]->setPos (r.x() + width + spacing_, r.y() + eheight + spacing_);
      items_[order[3]]->setPos (r.x() + width + spacing_,
          r.y() + ((eheight + spacing_) * 2));
    }
    break;
  }
}


//=============================================================================
//=============================================================================
