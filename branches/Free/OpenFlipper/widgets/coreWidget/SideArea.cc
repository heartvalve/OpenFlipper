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
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================

//== INCLUDES =================================================================

#include <QVBoxLayout>

#include "SideArea.hh"
#include "SideElement.hh"

//== IMPLEMENTATION ==========================================================

SideArea::SideArea (QWidget *_parent) :
  QWidget (_parent)
{

  layout_ = new QVBoxLayout;
  layout_->setSpacing (0);

  QVBoxLayout *l = new QVBoxLayout;
  l->addLayout (layout_);
  l->addStretch(1);

  setLayout (l);
}

//-----------------------------------------------------------------------------

void SideArea::addItem (QWidget *_w, QString _name)
{
  SideElement *e = new SideElement (this, _w, _name);
  layout_->addWidget (e);
  items_.push_back (e);
}

//-----------------------------------------------------------------------------

void SideArea::clear ()
{
  foreach (SideElement *e, items_)
  {
    layout_->removeWidget (e);
    delete e;
  }
  items_.clear ();
}

//-----------------------------------------------------------------------------

void SideArea::saveState (QSettings &_settings)
{
  _settings.beginGroup ("SideArea");
  foreach (SideElement *e, items_)
  {
    e->saveState (_settings);
  }
  _settings.endGroup ();
}

//-----------------------------------------------------------------------------
void SideArea::restoreState (QSettings &_settings)
{
  _settings.beginGroup ("SideArea");
  foreach (SideElement *e, items_)
  {
    e->restoreState (_settings);
  }
  _settings.endGroup ();
}

//=============================================================================
//=============================================================================
