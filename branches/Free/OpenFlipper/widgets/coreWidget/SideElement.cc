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
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QAction>
#include <QFrame>
#include <QMouseEvent>
#include <QFont>
#include <QDialog>

#include <OpenFlipper/common/GlobalOptions.hh>

#include "SideElement.hh"

//== IMPLEMENTATION ==========================================================

SideElement::SideElement (SideArea *_parent, QWidget *_w, QString _name) :
  parent_ (_parent),
  widget_ (_w),
  name_ (_name),
  active_ (0),
  dialog_ (0)
{
  QFont font;
  font.setBold (false);

  QHBoxLayout *hl = new QHBoxLayout;

  SideElement::TopArea *tr = new SideElement::TopArea (this);

  label_ = new QLabel (_name);
  label_->setFont (font);

  detachButton_ = new QToolButton ();
  hl->addWidget (label_);
  hl->addStretch (1);
  hl->addWidget (detachButton_);

  detachAction_ = new QAction (QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"button-detach.png"), "", this);
  detachAction_->setToolTip ("Show as separate window.");
  detachAction_->setCheckable (true);
  detachButton_->setDefaultAction (detachAction_);

  connect (detachAction_, SIGNAL (triggered (bool)), this, SLOT (detachPressed (bool)));

  tr->setLayout (hl);

  QFrame *f = new QFrame ();
  f->setFrameShape (QFrame::HLine);

  mainLayout_ = new QVBoxLayout;

  mainLayout_->addWidget (f);
  mainLayout_->addWidget (tr);
  mainLayout_->addWidget (_w);
  mainLayout_->setSpacing (0);
  mainLayout_->setContentsMargins (1,1,1,1);
  setLayout (mainLayout_);

  _w->hide ();
}

//-----------------------------------------------------------------------------

SideElement::~SideElement ()
{
  if (dialog_)
    dialog_->close ();
  widget_->setParent (0);
}

//-----------------------------------------------------------------------------

void SideElement::labelPress ()
{
  if (dialog_)
  {
    dialog_->raise ();
    dialog_->activateWindow ();
  }
  else
  {
    active_ = !active_;
    if (active_)
      widget_->show ();
    else
      widget_->hide ();

    QFont font;
    font.setBold (active_);
    label_->setFont (font);
  }
}

//-----------------------------------------------------------------------------

void SideElement::detachPressed (bool checked_)
{
  if (checked_)
  {
    mainLayout_->removeWidget (widget_);
    dialog_ = new QDialog(0, Qt::Window);
    dialog_->setWindowTitle (name_);
    dialog_->setLayout (new QVBoxLayout);
    dialog_->resize (widget_->size ());
    if (window ())
    {
      int x = (window ()->width () - widget_->width ()) / 2;
      x += window ()->x ();
      x = qMax (0, x);
      int y = (window ()->height () - widget_->height ()) / 2;
      y += window ()->y ();
      y = qMax (0, y);
      dialog_->move (x, y);
    }
    dialog_->layout ()->addWidget (widget_);
    dialog_->show ();
    widget_->setAttribute(Qt::WA_DeleteOnClose, false);
    widget_->show ();

    connect (dialog_, SIGNAL(finished (int)), this, SLOT(dialogClosed ()));

    QFont font;
    font.setBold (true);
    font.setItalic (true);
    label_->setFont (font);
  }
  else if (dialog_)
    dialog_->close ();

}

//-----------------------------------------------------------------------------

void SideElement::dialogClosed ()
{
  dialog_ = 0;
  mainLayout_->addWidget (widget_);
  widget_->setAttribute(Qt::WA_DeleteOnClose, true);

  if (active_)
    widget_->show ();
  else
    widget_->hide ();

  detachAction_->setChecked (false);
  QFont font;
  font.setBold (active_);
  label_->setFont (font);
}

//-----------------------------------------------------------------------------

void SideElement::saveState (QSettings &_settings)
{
  _settings.beginGroup (name_);
  _settings.setValue ("Active", active_);
  _settings.setValue ("Detached", (dialog_ != 0));
  _settings.setValue ("DialogGeometry", (dialog_) ? dialog_->saveGeometry (): QByteArray());
  _settings.endGroup ();
}

//-----------------------------------------------------------------------------
void SideElement::restoreState (QSettings &_settings)
{
  _settings.beginGroup (name_);

  active_ = _settings.value ("Active", active_).toBool ();

  if (active_)
    widget_->show ();
  else
    widget_->hide ();

  QFont font;
  font.setBold (active_);
  label_->setFont (font);

  if (_settings.value ("Detached", false).toBool () && !dialog_)
    detachPressed (true);

  if (dialog_)
    dialog_->restoreGeometry (_settings.value ("DialogGeometry").toByteArray ());

  _settings.endGroup ();
}

//-----------------------------------------------------------------------------

SideElement::TopArea::TopArea (SideElement *_e) :
  e_ (_e)
{
}

//-----------------------------------------------------------------------------

void SideElement::TopArea::mousePressEvent (QMouseEvent *_event)
{
  e_->labelPress ();
  _event->accept ();
}

//=============================================================================
//=============================================================================

