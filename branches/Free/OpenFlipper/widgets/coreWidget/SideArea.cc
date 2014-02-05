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

#include <QVBoxLayout>

#include "SideArea.hh"
#include "SideElement.hh"
#include <OpenFlipper/common/GlobalOptions.hh>
//== IMPLEMENTATION ==========================================================

SideArea::SideArea (QWidget *_parent) :
  QWidget (_parent),
  lastPos_(0)
{

  layout_ = new QVBoxLayout;
  layout_->setSpacing (0);

  QVBoxLayout *l = new QVBoxLayout;
  l->addLayout (layout_);
  l->addStretch(1);

  setLayout (l);
}

//-----------------------------------------------------------------------------

void SideArea::addItem (QObject const * const _plugin, QWidget *_w, QString _name, QIcon *_icon)
{
  SideElement *e = new SideElement (this, _w, _name, _icon);
  layout_->addWidget (e);
  items_.push_back (e);
  plugins_.push_back(_plugin);
  itemNames_.push_back(_name);
}

//-----------------------------------------------------------------------------

void SideArea::addItem (QObject const * const _plugin, QWidget *_w, QString _name)
{
  SideElement *e = new SideElement (this, _w, _name);
  layout_->addWidget (e);
  items_.push_back (e);
  plugins_.push_back(_plugin);
  itemNames_.push_back(_name);
}

//-----------------------------------------------------------------------------

void SideArea::moveItemToPosition(const QString& _name, int _position) {

    // Position is in valid range
    if(_position < 0 || _position >= items_.size())
        return;

    // Search item
    QVector<SideElement*>::iterator it = items_.begin();
    for(; it != items_.end(); ++it) {
        if( (*it)->name() == _name )
            break;
    }

    if(it != items_.end()) {
        layout_->removeWidget(*it);
        layout_->insertWidget(_position, (*it));
    }
}

//-----------------------------------------------------------------------------

void SideArea::moveItemToPosition(QObject const * const _plugin, const QString& _name, int _position) {

    // Position is in valid range
    if(_position < 0 || _position >= items_.size())
        return;

    // Search item
    QVector<SideElement*>::iterator it = items_.begin();
    int i = 0;
    for(; it != items_.end(); ++it, ++i) {
        if(   ((*it)->name() == _name)
           && (plugins_[i] == _plugin)   )
            break;
    }

    bool active = (*it)->active();

    if(it != items_.end()) {
        layout_->removeWidget(*it);
        layout_->insertWidget(_position, (*it));
        if (active)
          (*it)->show();
    }
}

//-----------------------------------------------------------------------------

int SideArea::getNumberOfWidgets() const {
    return items_.size();
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
  plugins_.clear();
  itemNames_.clear();
  lastPos_ = 0;
}

//-----------------------------------------------------------------------------

void SideArea::expandAll()
{
  foreach (SideElement *e, items_)
  {
    e->setActive(true);
  }
}

void SideArea::expand(QWidget *sideElementWidget, bool expand)
{
  foreach (SideElement *e, items_)
  {
    if (e->widget() == sideElementWidget)
        e->setActive(expand);
  }
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

//-----------------------------------------------------------------------------

void SideArea::saveViewModeState(const QString& _viewMode) {
  foreach (SideElement *e, items_) {
    sideElementState_[_viewMode + e->name()] = e->active();
  }
}

//-----------------------------------------------------------------------------

void SideArea::restoreViewModeState(const QString& _viewMode) {
  foreach (SideElement *e, items_) {
    e->setActive(sideElementState_[_viewMode + e->name()]);
  }
}

//-----------------------------------------------------------------------------

void SideArea::setElementActive(QString _name, bool _active)
{
  for (int i=0; i < items_.count(); i++)
    if ( items_[i]->name() == _name ){
      items_[i]->setActive(_active);

      return;
    }
}

//-----------------------------------------------------------------------------

const QList<const QObject *>& SideArea::plugins() {
  return plugins_;
}

//-----------------------------------------------------------------------------

const QStringList& SideArea::names() {
  return itemNames_;
}


//=============================================================================
//=============================================================================
