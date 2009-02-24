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

//=============================================================================
//
//  CLASS QtSlideWindow
//
//=============================================================================


//== GLOBAL DEFINITIONS=========================================================

#define BACKGROUND_RED   0xff
#define BACKGROUND_GREEN 0xff
#define BACKGROUND_BLUE  0xff
#define BACKGROUND_ALPHA 0xcf

#define SLIDE_DURATION 500


//== INCLUDES =================================================================


#include <OpenFlipper/common/GlobalOptions.hh>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QTimeLine>
#include <QGraphicsItemAnimation>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDialog>
#include <QVBoxLayout>

#include "QtSlideWindow.hh"
#include "QtGraphicsButton.hh"


//== IMPLEMENTATION ==========================================================

QtSlideWindow::QtSlideWindow(QString _name, QGraphicsItem *_parent) :
  QGraphicsProxyWidget(_parent),
  name_(_name),
  mainWidget_(0),
  autohideButton_(0),
  detachButton_(0),
  hideAnimation_(0),
  dialog_(0)
{
  setCacheMode (QGraphicsItem::DeviceCoordinateCache);
  setWindowFrameMargins (2, 15, 2, 2);
  setZValue (2.0);

  QImage autohide (OpenFlipper::Options::iconDirStr () + OpenFlipper::Options::dirSeparator () + "button-autohide.png");
  QImage detach (OpenFlipper::Options::iconDirStr () + OpenFlipper::Options::dirSeparator () + "button-detach.png");

  autohideButton_ = new QtGraphicsButton (autohide, this, 12, 12);
  detachButton_ = new QtGraphicsButton (detach, this, 12, 12);

  autohideButton_->setCheckable (true);
  autohideButton_->setChecked (true);
  autohideButton_->setPos (geometry().width() - 12, -13);
  detachButton_->setPos (geometry().width() - 25, -13);

  connect (detachButton_, SIGNAL(pressed ()), this, SLOT(detachPressed ()));
  connect (autohideButton_, SIGNAL(pressed ()), this, SLOT(autohidePressed ()));

  hideTimeLine_ = new QTimeLine (SLIDE_DURATION);
  hideTimeLine_->setFrameRange (0, 100);
  
  hideAnimation_ = new QGraphicsItemAnimation;
  hideAnimation_->setItem (this);
  hideAnimation_->setTimeLine (hideTimeLine_);

  for (int i = 0; i < geometry ().height (); ++i)
    hideAnimation_->setTranslationAt (i / geometry ().height (), 0, geometry ().height () - i);

  hide ();
}

//-----------------------------------------------------------------------------

void QtSlideWindow::attachWidget (QWidget *_m)
{
  if (!_m)
    return;

  mainWidget_ = _m;
  mainWidget_->setParent(0);
  setWidget (mainWidget_);
  setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

  show ();
  if (autohideButton_->isChecked ())
  {
    hideTimeLine_->setCurrentTime (0);
    hideAnimation_->setStep (0.0);
  }
  else
  {
    hideTimeLine_->setCurrentTime (SLIDE_DURATION);
    hideAnimation_->setStep (1.0);
  }
  updateGeometry ();
}

//-----------------------------------------------------------------------------

void QtSlideWindow::detachWidget ()
{
  setWidget (0);
  hide ();
  mainWidget_->setParent(0);
  mainWidget_ = 0;

  if (dialog_)
  {
    disconnect (dialog_, SIGNAL(finished (int)), this, SLOT(dialogClosed ()));
    dialog_->close ();
    delete dialog_;
    dialog_ = 0;
  }
}

//-----------------------------------------------------------------------------

void QtSlideWindow::paintWindowFrame(QPainter *_painter,
                                     const QStyleOptionGraphicsItem *_option,
                                     QWidget *_widget)
{
  int w = geometry().width();
  int h = geometry().height();

  _painter->setRenderHint(QPainter::Antialiasing, true);
  _painter->setBrush(QBrush(QColor(BACKGROUND_RED,
                                   BACKGROUND_GREEN,
                                   BACKGROUND_BLUE,
                                   BACKGROUND_ALPHA)));
  _painter->setPen(Qt::NoPen);
  _painter->drawRoundedRect(-2, -15, w + 4, h + 40, 4, 4);

  _painter->setPen(Qt::SolidLine);

  _painter->drawText(2,-13,w - 4, 11, Qt::AlignCenter, name_);

}

//-----------------------------------------------------------------------------

bool QtSlideWindow::windowFrameEvent(QEvent *_e)
{
  if (_e->type() == QEvent::GraphicsSceneMousePress ||
      _e->type() == QEvent::GraphicsSceneMouseRelease)
  {
    QGraphicsSceneMouseEvent *ge = dynamic_cast<QGraphicsSceneMouseEvent*>(_e);
    if (windowFrameSectionAt(ge->pos()) != Qt::TopSection)
    {
      _e->accept();
      return false;
    }
  }
  return QGraphicsProxyWidget::windowFrameEvent(_e);
}

//-----------------------------------------------------------------------------

Qt::WindowFrameSection QtSlideWindow::windowFrameSectionAt(const QPointF &_pos) const
{
  if (_pos.x() >= 2 && _pos.x() < geometry().width() - 2 - (13 * 2) &&
      _pos.y() >= -15 && _pos.y() <= -10)
  {
    return Qt::TopSection;
  }
  return Qt::NoSection;
}

//-----------------------------------------------------------------------------

void QtSlideWindow::hoverEnterEvent (QGraphicsSceneHoverEvent *)
{
  if (autohideButton_->isChecked ())
  {
    hideTimeLine_->setDirection (QTimeLine::Forward);
    if (hideTimeLine_->state () == QTimeLine::NotRunning)
      hideTimeLine_->start ();
  }
}

//-----------------------------------------------------------------------------

void QtSlideWindow::hoverLeaveEvent (QGraphicsSceneHoverEvent *)
{
  if (autohideButton_->isChecked ())
  {
    hideTimeLine_->setDirection (QTimeLine::Backward);
    if (hideTimeLine_->state () == QTimeLine::NotRunning)
      hideTimeLine_->start ();
  }
}

//-----------------------------------------------------------------------------

void QtSlideWindow::resizeEvent (QGraphicsSceneResizeEvent *_event)
{
  QGraphicsProxyWidget::resizeEvent (_event);
  if (hideAnimation_)
  {
    hideAnimation_->clear ();
    for (int i = 0; i < geometry ().height (); ++i)
      hideAnimation_->setTranslationAt (i / geometry ().height (), 0, geometry ().height () - i);
  }
}

//-----------------------------------------------------------------------------

void QtSlideWindow::moveEvent (QGraphicsSceneMoveEvent *_event)
{
  QGraphicsProxyWidget::moveEvent (_event);
  if (hideAnimation_)
  {
    hideAnimation_->clear ();
    for (int i = 0; i < geometry ().height (); ++i)
      hideAnimation_->setTranslationAt (i / geometry ().height (), 0, geometry ().height () - i);
  }
}

//-----------------------------------------------------------------------------

void QtSlideWindow::detachPressed ()
{
  setWidget (0);
  dialog_ = new QDialog(0, Qt::CustomizeWindowHint | Qt::WindowTitleHint);
  dialog_->setWindowTitle (name_);
  dialog_->setLayout (new QVBoxLayout);
  dialog_->resize (mainWidget_->size ());
  dialog_->layout ()->addWidget (mainWidget_);
  hide ();
  dialog_->show ();
  mainWidget_->setAttribute(Qt::WA_DeleteOnClose, false);
  connect (dialog_, SIGNAL(finished (int)), this, SLOT(dialogClosed ()));

}

//-----------------------------------------------------------------------------

void QtSlideWindow::dialogClosed ()
{
  dialog_ = 0;
  mainWidget_->setParent(0);
  setWidget (mainWidget_);
  setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
  mainWidget_->setAttribute(Qt::WA_DeleteOnClose, true);
  show ();
  if (autohideButton_->isChecked ())
  {
    hideTimeLine_->setCurrentTime (0);
    hideAnimation_->setStep (0.0);
  }
  else
  {
    hideTimeLine_->setCurrentTime (SLIDE_DURATION);
    hideAnimation_->setStep (1.0);
  }
  updateGeometry ();

}

//-----------------------------------------------------------------------------

void QtSlideWindow::autohidePressed ()
{
  if (!autohideButton_->isChecked ())
  {
    hideTimeLine_->setDirection (QTimeLine::Forward);
    if (hideTimeLine_->state () == QTimeLine::NotRunning &&
        hideTimeLine_->currentTime () != SLIDE_DURATION)
      hideTimeLine_->start ();
  }
}

//-----------------------------------------------------------------------------

void QtSlideWindow::updateGeometry ()
{
  if (parentWidget () && mainWidget_)
  {
    setPos (8, parentWidget ()->geometry ().height () - geometry ().height ());
    resize (parentWidget ()->geometry ().width () - 20,
            mainWidget_->size ().height ());
    if (autohideButton_)
      autohideButton_->setPos (geometry().width() - 12, -13);
    if (detachButton_)
      detachButton_->setPos (geometry().width() - 25, -13);
  }
}

//=============================================================================
//=============================================================================
