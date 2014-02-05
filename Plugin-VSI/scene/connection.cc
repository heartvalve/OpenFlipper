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
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

#include <QPen>

#include "elementInOut.hh"
#include "elementInput.hh"
#include "elementOutput.hh"
#include "connection.hh"
#include "connectionPoint.hh"
#include "graphicsScene.hh"
#include "elementArea.hh"
#include "wayfind.hh"

#include "../parser/inout.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::Connection - IMPLEMENTATION
//
//=============================================================================

/// Constructor
Connection::Connection (ConnectionPoint *_start, QGraphicsScene *_scene) :
  QGraphicsPathItem (dynamic_cast<GraphicsScene *>(_scene)->elementArea ()),
  scene_ (dynamic_cast<GraphicsScene *>(_scene)),
  p1_ (_start),
  p2_ (0),
  old_ (0)
{

  elementArea_ = scene_->elementArea ();

  QPen p = pen ();

  p.setCapStyle (Qt::RoundCap);
  p.setColor (QColor (128, 128, 128));
  
  if (p1_->inOut ()->inOut ()->typeString () == "data")
    p.setWidth (4);
  else
    p.setWidth (2);
  setPen (p);
}

/// Constructor
Connection::Connection (ConnectionPoint *_start, ConnectionPoint *_end, QGraphicsScene *_scene) :
  QGraphicsPathItem (dynamic_cast<GraphicsScene *>(_scene)->elementArea ()),
  scene_ (dynamic_cast<GraphicsScene *>(_scene)),
  p1_ (_start),
  p2_ (_end)
{
  elementArea_ = scene_->elementArea ();

  QPen p = pen ();

  p.setCapStyle (Qt::RoundCap);

  if (p1_->inOut ()->inOut ()->typeString () == "data")
    p.setWidth (4);
  else
    p.setWidth (2);
  setPen (p);

  if (!p1_->inOut ()->validConnection (p2_->inOut ()))
  {
    deleteLater ();
    p1_ = p2_ = 0;
    return;
  }

  way_ = QPolygonF ();

  p.setColor (QColor (0, 0, 0));
  setPen (p);

  p1_->inOut ()->addConnection (this);
  p2_->inOut ()->addConnection (this);
}

/// Destructor
Connection::~Connection ()
{
  if (p1_)
    p1_->inOut ()->removeConnection (this);
  if (p2_)
    p2_->inOut ()->removeConnection (this);
}

// handle mouse movement
void Connection::mouseMoveEvent (QGraphicsSceneMouseEvent *_event)
{
  QPen p = pen ();

  ConnectionPoint *pnt = cPointAt (_event->scenePos ());

  if (pnt)
  { if (p1_->inOut ()->validConnection (pnt->inOut ()))
      p.setColor (QColor (0, 255, 0));
    else
      p.setColor (QColor (255, 0, 0));
  }
  else
    p.setColor (QColor (128, 128, 128));
  setPen (p);

  way_ = scene_->wayFind ()->findWay (this, p1_->connectPos ().toPoint (), elementArea_->mapFromScene (_event->scenePos ()).toPoint ());
  updateLine ();
}

// Start new connection on mouse press
void Connection::mousePressEvent (QGraphicsSceneMouseEvent *_event)
{
  if (p1_)
    p1_->inOut ()->removeConnection (this);
  if (p2_)
    p2_->inOut ()->removeConnection (this);

  QPointF p = p1_->connectPos () - _event->scenePos ();
  qreal d1 = (p.x () * p.x()) + (p.y () * p.y ());
  p = p2_->connectPos () - _event->scenePos ();
  qreal d2 = (p.x () * p.x()) + (p.y () * p.y ());

  if (d2 > d1)
  {
    old_ = p1_;
    p1_ = p2_;
  }

  way_ = scene_->wayFind ()->findWay (this, p1_->connectPos ().toPoint (), elementArea_->mapFromScene (_event->scenePos ()).toPoint ());
  updateLine ();
}

// make a connection on relase at a valid connection point
void Connection::mouseReleaseEvent (QGraphicsSceneMouseEvent *_event)
{
  scene_->setActiveConnection (0);

  p2_ = cPointAt (_event->scenePos ());

  if (!p2_ || !p1_->inOut ()->validConnection (p2_->inOut ()))
  {
    if (old_)
      scene_->contentChange ();

    deleteLater ();
    p1_ = p2_ = 0;
    return;
  }

  way_ = scene_->wayFind ()->findWay (this, p1_->connectPos ().toPoint (), p2_->connectPos ().toPoint ());
  updateLine ();

  QPen p = pen ();

  p.setColor (QColor (0, 0, 0));
  setPen (p);

  if (old_ != p2_)
    scene_->contentChange ();

  old_ = p2_;

  p1_->inOut ()->addConnection (this);
  p2_->inOut ()->addConnection (this);

}

// helper to localize a connection point
ConnectionPoint *Connection::cPointAt (QPointF _pnt)
{

  ConnectionPoint *pnt = 0;

  foreach (QGraphicsItem *i, scene ()->items (_pnt))
  {
    ConnectionPoint *pt = dynamic_cast<ConnectionPoint *>(i);

    if (pt)
    {
      pnt = pt;
    }
  }

  return pnt;
}

/// called to update position on element movement
void Connection::updatePositions ()
{
  way_ = scene_->wayFind ()->findWay (this, p1_->connectPos ().toPoint (), p2_->connectPos ().toPoint ());
  updateLine ();
}

/// Input of this connection
ElementInput *Connection::input ()
{
  ElementInput *i = dynamic_cast<ElementInput *> (p1_->inOut ());
  if (!i)
    i = dynamic_cast<ElementInput *> (p2_->inOut ());
  return i;
}

/// Output of this connection
ElementOutput *Connection::output ()
{
  ElementOutput *o = dynamic_cast<ElementOutput *> (p1_->inOut ());
  if (!o && p2_)
    o = dynamic_cast<ElementOutput *> (p2_->inOut ());
  return o;
}

/// way of the connection
const QPolygonF & VSI::Connection::way () const
{
  return way_;
}

/// invalidate way
void Connection::invalidate()
{
  way_ = QPolygonF ();
  setPath (QPainterPath ());
}

// generates a curved line out of the way polygon
void Connection::updateLine()
{
  QPainterPath path;

  if (way_.size () <= 1)
  {
    setPath (path);
    return;
  }
  else if (way_.size () == 2)
  {
    path.addPolygon(way_);
    setPath (path);
    return;
  }

  path.moveTo (way_[0]);

  QPointF a,b,c;

  for (int i = 2; i < way_.size (); i++)
  {
    a = way_[i-2];
    b = way_[i-1];
    c = way_[i];

    QLineF l1 (a,b), l2 (b,c);

    int rad = qMin (40.0, qMin (l1.length() / 2, l2.length() / 2));

    if (rad > 5)
      rad -= rad % 5;

    if (a.x () == b.x ())
    {
      if (a.y () > b.y ())
      {
        path.lineTo (b.x (), b.y () + rad);
        if (c.x () > b.x ())
        {
          path.arcTo (b.x (), b.y (), 2 * rad, 2 * rad, 180, -90);
        }
        else
        {
          path.arcTo (b.x () - (rad * 2), b.y (), 2 * rad, 2 * rad, 0, 90);
        }
      }
      else
      {
        path.lineTo (b.x (), b.y () - rad);
        if (c.x () > b.x ())
        {
          path.arcTo (b.x (), b.y () - (rad * 2), 2 * rad, 2 * rad, 180, 90);
        }
        else
        {
          path.arcTo (b.x () - (rad * 2), b.y () - (rad * 2), 2 * rad, 2 * rad, 0, -90);
        }
      }
    }
    else
    {
      if (a.x () > b.x ())
      {
        path.lineTo (b.x () + rad, b.y ());
        if (c.y () > b.y ())
        {
          path.arcTo (b.x (), b.y (), 2 * rad, 2 * rad, 90, 90);
        }
        else
        {
          path.arcTo (b.x (), b.y () - (rad * 2), 2 * rad, 2 * rad, 270, -90);
        }
      }
      else
      {
        path.lineTo (b.x () - rad, b.y ());
        if (c.y () > b.y ())
        {
          path.arcTo (b.x () - (rad * 2), b.y (), 2 * rad, 2 * rad, 90, -90);
        }
        else
        {
          path.arcTo (b.x () - (rad * 2), b.y () - (rad * 2), 2 * rad, 2 * rad, 270, 90);
        }
      }
    }
  }

  path.lineTo (c);

  setPath (path);
}

}
