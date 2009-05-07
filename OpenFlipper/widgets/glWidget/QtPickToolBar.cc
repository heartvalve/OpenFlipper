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
//   $Revision: 5601 $
//   $Author: moebius $
//   $Date: 2009-04-09 09:19:47 +0200 (Do, 09. Apr 2009) $
//
//=============================================================================

//=============================================================================
//
//  CLASS QtPickToolbar
//
//=============================================================================


//== GLOBAL DEFINITIONS=========================================================

#define BACKGROUND_RED   0xff
#define BACKGROUND_GREEN 0xff
#define BACKGROUND_BLUE  0xff
#define BACKGROUND_ALPHA 0xcf

#define SLIDE_DURATION 1000


//== INCLUDES =================================================================


#include <OpenFlipper/common/GlobalOptions.hh>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDialog>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStatusBar>
#include <QStatusTipEvent>

#include "QtPickToolbar.hh"
#include "QtGraphicsButton.hh"


//== IMPLEMENTATION ==========================================================

QtPickToolbar::QtPickToolbar(QMainWindow *_core, QGraphicsItem *_parent) :
  QGraphicsProxyWidget(_parent),
  toolbar_(0),
  core_(_core)
{
  setCacheMode (QGraphicsItem::DeviceCoordinateCache);
  setWindowFrameMargins (2, 2, 2, 2);
  setZValue (2.0);

  hide ();
}

//-----------------------------------------------------------------------------

void QtPickToolbar::attachToolbar (QToolBar *_t)
{
  if (!_t)
  {
    detachToolbar ();
    return;
  }

  if (_t == toolbar_)
    return;

  toolbar_ = _t;
  toolbar_->setParent(0);

  setWidget (toolbar_);
  setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

  show ();

  updateGeometry ();
}

//-----------------------------------------------------------------------------

void QtPickToolbar::detachToolbar ()
{
  if (toolbar_)
  {
    setWidget (0);
    hide ();
    toolbar_->setParent(0);
    toolbar_ = 0;
  }
}

//-----------------------------------------------------------------------------

void QtPickToolbar::paintWindowFrame(QPainter *_painter,
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
  _painter->drawRoundedRect(-2, -6, w + 4, h + 8, 4, 4);

  _painter->setPen(Qt::SolidLine);


}

//-----------------------------------------------------------------------------

Qt::WindowFrameSection QtPickToolbar::windowFrameSectionAt(const QPointF &_pos) const
{
  return Qt::NoSection;
}

//-----------------------------------------------------------------------------

void QtPickToolbar::updateGeometry ()
{
  if (parentWidget () && widget ())
  {
    resize (qMin ((int)parentWidget ()->geometry ().width () - 20, widget ()->sizeHint ().width ()),
            widget ()->sizeHint ().height ());
    setPos ((parentWidget ()->geometry ().width () - geometry ().width ()) / 2, 0);
  }
}

//-----------------------------------------------------------------------------

bool QtPickToolbar::eventFilter(QObject *_obj, QEvent *_event)
{
  /* The QGraphicsProxyWidged does not sent the StatusTip messages to the main
     application window status bar. So we have to do it manually.
  */
  if (_event->type () == QEvent::StatusTip)
  {
    if (core_->statusBar ())
      core_->statusBar ()->showMessage (dynamic_cast<QStatusTipEvent *>(_event)->tip());

    return QGraphicsProxyWidget::eventFilter(_obj, _event);
  }
  else
    return QGraphicsProxyWidget::eventFilter(_obj, _event);
}

//=============================================================================
//=============================================================================
