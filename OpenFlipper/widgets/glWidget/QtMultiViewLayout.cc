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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




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
  spacing_ (2)
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

  if (!items_[0])
    return;
  if (count() != 4)
    mode = SingleView;

  if (mode == SingleView)
  {
    if (items_[1])
      items_[1]->hide();
    if (items_[2])
      items_[2]->hide();
    if (items_[3])
      items_[3]->hide();
  } else   {
    if (items_[1])
      items_[1]->show();
    if (items_[2])
      items_[2]->show();
    if (items_[3])
      items_[3]->show();
  }

  switch (mode)
  {
    case SingleView:
      items_[0]->setGeometry (r);
      break;
    case Grid:
      {
	int width = (r.width() - spacing_) / 2;
	int height = (r.height() - spacing_) / 2;
	items_[0]->resize (width, height);
	items_[1]->resize (r.width() - width - spacing_, height);
	items_[2]->resize (width, r.height() - height - spacing_);
	items_[3]->resize (r.width() - width - spacing_,
                           r.height() - height - spacing_);
	items_[0]->setPos (r.topLeft());
	items_[1]->setPos (r.x() + width + spacing_, r.y());
	items_[2]->setPos (r.x(), r.y() + height + spacing_);
	items_[3]->setPos (r.x() + width + spacing_,
                           r.y() + height + spacing_);
      }
      break;
    case HSplit:
      {
	int width = (r.width() - spacing_) * 3 / 4;
	int ewidth = r.width() - spacing_ - width;
	int eheight = (r.height() - (spacing_ * 2)) / 3;
	items_[0]->resize (width, r.height());
	items_[1]->resize (ewidth, eheight);
	items_[2]->resize (ewidth, eheight);
	items_[3]->resize (ewidth, r.height() - ((eheight + spacing_) * 2));
	items_[0]->setPos (r.topLeft());
	items_[1]->setPos (r.x() + width + spacing_, r.y());
	items_[2]->setPos (r.x() + width + spacing_, r.y() + eheight + spacing_);
	items_[3]->setPos (r.x() + width + spacing_,
			   r.y() + ((eheight + spacing_) * 2));
      }
      break;
  }
}


//=============================================================================
//=============================================================================
