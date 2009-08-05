/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

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

#define SLIDE_DURATION 400


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
  dialog_(0),
  tempWidget_(new QWidget)
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

  hideAnimation_->setTranslationAt (0.0, 0, geometry ().height ());
  for (int i = 0; i < geometry ().height (); ++i)
    hideAnimation_->setTranslationAt (0.25 + (i / (geometry ().height () * 2.0)), 0, geometry ().height () - i);
  hideAnimation_->setTranslationAt (1.0, 0, 0);

  hide ();
  connect (hideTimeLine_, SIGNAL(finished()), this, SLOT(timelineFinished()));

  setAcceptsHoverEvents (true);
}

//-----------------------------------------------------------------------------

void QtSlideWindow::attachWidget (QWidget *_m)
{
  if (!_m)
    return;

  mainWidget_ = _m;
  mainWidget_->setParent(0);

  if (autohideButton_->isChecked ())
  {
    tempWidget_->setGeometry (mainWidget_->geometry ());
    setWidget (tempWidget_);
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    show ();
    hideTimeLine_->setCurrentTime (0);
    hideAnimation_->setStep (0.0);
  }
  else
  {
    setWidget (mainWidget_);
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    show ();
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
                                     const QStyleOptionGraphicsItem* /*_option*/,
                                     QWidget* /*_widget*/ )
{
  int w = geometry().width();
  int h = geometry().height();

  _painter->setRenderHint(QPainter::Antialiasing, true);
  _painter->setBrush(QBrush(QColor(BACKGROUND_RED,
                                   BACKGROUND_GREEN,
                                   BACKGROUND_BLUE,
                                   BACKGROUND_ALPHA)));
  _painter->setPen(QColor(BACKGROUND_RED / 4,
                          BACKGROUND_GREEN / 4,
                          BACKGROUND_BLUE / 4,
                          BACKGROUND_ALPHA));
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
  if (autohideButton_->isChecked () && hideTimeLine_->currentTime() != SLIDE_DURATION)
  {
    hideTimeLine_->setDirection (QTimeLine::Forward);
    if (hideTimeLine_->state () == QTimeLine::NotRunning)
      hideTimeLine_->start ();
  }
  if (!mainWidget_->isVisible ())
  {
    mainWidget_->setGeometry (tempWidget_->geometry ());
    setWidget (mainWidget_);
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    show ();
  }
}

//-----------------------------------------------------------------------------

void QtSlideWindow::hoverLeaveEvent (QGraphicsSceneHoverEvent *)
{
  if (autohideButton_->isChecked () &&
      (!scene ()->mouseGrabberItem () || !childItems ().contains (scene ()->mouseGrabberItem ())))
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
    hideAnimation_->setTranslationAt (0.0, 0, geometry ().height ());
    for (int i = 0; i < geometry ().height (); ++i)
      hideAnimation_->setTranslationAt (0.25 + (i / (geometry ().height () * 2.0)), 0, geometry ().height () - i);
    hideAnimation_->setTranslationAt (1.0, 0, 0);
  }
}

//-----------------------------------------------------------------------------

void QtSlideWindow::moveEvent (QGraphicsSceneMoveEvent *_event)
{
  QGraphicsProxyWidget::moveEvent (_event);
  if (hideAnimation_)
  {
    hideAnimation_->clear ();
    hideAnimation_->setTranslationAt (0.0, 0, geometry ().height ());
    for (int i = 0; i < geometry ().height (); ++i)
      hideAnimation_->setTranslationAt (0.25 + (i / (geometry ().height () * 2.0)), 0, geometry ().height () - i);
    hideAnimation_->setTranslationAt (1.0, 0, 0);
  }
}

//-----------------------------------------------------------------------------

void QtSlideWindow::detachPressed ()
{
  setWidget (0);
  dialog_ = new QDialog(0, Qt::Window);
  dialog_->setWindowTitle (name_);
  dialog_->setLayout (new QVBoxLayout);
  dialog_->resize (mainWidget_->size ());
  dialog_->layout ()->addWidget (mainWidget_);
  if (scene () && scene ()->views ()[0] && scene ()->views ()[0]->window ())
  {
    QWidget *w = scene ()->views ()[0]->window ();
    int x = (w->width () - mainWidget_->width ()) / 2;
    x += w->x ();
    x = qMax (0, x);
    int y = (w->height () - mainWidget_->height ()) / 2;
    y += w->y ();
    y = qMax (0, y);
    dialog_->move (x, y);
  }
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
  if (parentWidget () && widget ())
  {
    setPos (8, parentWidget ()->geometry ().height () - geometry ().height ());
    resize (parentWidget ()->geometry ().width () - 20,
            widget ()->size ().height ());
    if (autohideButton_)
      autohideButton_->setPos (geometry().width() - 12, -13);
    if (detachButton_)
      detachButton_->setPos (geometry().width() - 25, -13);
  }
}

//-----------------------------------------------------------------------------

void QtSlideWindow::saveState (QSettings &_settings)
{
  _settings.setValue ("AutoHide", autohideButton_->isChecked ());
  _settings.setValue ("Detached", (dialog_ != 0));
  _settings.setValue ("WidgedGeometry", (mainWidget_) ? mainWidget_->saveGeometry (): QByteArray());
  _settings.setValue ("DialogGeometry", (dialog_) ? dialog_->saveGeometry (): QByteArray());
}

//-----------------------------------------------------------------------------

void QtSlideWindow::restoreState (QSettings &_settings)
{
  autohideButton_->setChecked (_settings.value ("AutoHide", autohideButton_->isChecked ()).toBool ());

  if (_settings.value ("Detached", false).toBool () && !dialog_ && mainWidget_)
  {
    detachPressed ();
  }

  if (mainWidget_)
  {
    mainWidget_->restoreGeometry (_settings.value ("WidgedGeometry").toByteArray ());
    if (hideAnimation_)
    {
      hideAnimation_->clear ();
      hideAnimation_->setTranslationAt (0.0, 0, geometry ().height ());
      for (int i = 0; i < geometry ().height (); ++i)
        hideAnimation_->setTranslationAt (0.25 + (i / (geometry ().height () * 2.0)), 0, geometry ().height () - i);
      hideAnimation_->setTranslationAt (1.0, 0, 0);
    }
  }

  if (autohideButton_->isChecked ())
  {
    hideTimeLine_->setCurrentTime (0);
    hideAnimation_->setStep (0.0);
  }
  else
  {
    hideTimeLine_->setCurrentTime (SLIDE_DURATION);
    hideAnimation_->setStep (1.0);
    if (!mainWidget_->isVisible ())
    {
      mainWidget_->setGeometry (tempWidget_->geometry ());
      setWidget (mainWidget_);
      setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
      show ();
    }
  }

  if (dialog_)
    dialog_->restoreGeometry (_settings.value ("DialogGeometry").toByteArray ());
}

//-----------------------------------------------------------------------------

void QtSlideWindow::timelineFinished ()
{
  if (hideTimeLine_->currentValue () == 0.0)
  {
    tempWidget_->setGeometry (mainWidget_->geometry ());
    setWidget (tempWidget_);
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    mainWidget_->setParent (0);
    mainWidget_->hide ();
    show ();
  }
}

//=============================================================================
//=============================================================================
