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
#include <QRectF>
#include <QRect>
#include <QPointF>
#include <QVector>
#include <QHash>
#include <QLinkedList>
#include <QTime>

#include <iostream>

#include <cmath>

#include "wayfind.hh"

#include "graphicsScene.hh"
#include "elementArea.hh"
#include "sceneElement.hh"
#include "connection.hh"
#include "elementInput.hh"
#include "elementOutput.hh"

#define PAIR(p) QPair<int,int>((p).x (),(p).y ())

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::WayFind - IMPLEMENTATION
//
//=============================================================================

/// Constructor
WayFind::WayFind(GraphicsScene *_scene) :
  scene_ (_scene),
  counter_ (0),
  ll_(0)
{
}

//------------------------------------------------------------------------------

/// Destructor
WayFind::~WayFind()
{
  foreach (Node *n, nodes_)
    delete n;
}

//------------------------------------------------------------------------------

/// Finds a way from _from to _to ignoring any already existent connections from _conn
QPolygonF WayFind::findWay(Connection *_conn, QPoint _from, QPoint _to)
{
  // our maximum working area
  QRect bb = scene_->elementArea ()->childrenBoundingRect ().toRect ().adjusted (-100, -100, 100, 100);

  QRegion eR;

  // combine all scene element bounding boxes and connections into one region
  foreach (SceneElement *e, scene_->elements())
  {
    eR += e->mapRectToParent (e->boundingRect ()).toRect ().adjusted (-15,-15,15,15);
    foreach (ElementInput *i, e->inputs ())
      foreach (Connection *c, i->connections ())
      {
        // ignore connection from the same output
        if (c->output () == _conn->output ())
          continue;
        if (c->way ().isEmpty ())
          continue;
        QPoint p1 = c->way ().first ().toPoint ();
        QPoint p2;
        for (int i = 1; i < c->way ().size (); i++)
        {
          p2 = c->way ()[i].toPoint ();
          eR += QRect (qMin (p1.x (), p2.x ()), qMin (p1.y (), p2.y ()), abs (p1.x () - p2.x ()), abs (p1.y () - p2.y ())).adjusted (-2, -2, 2, 2);
          p1 = p2;
        }
      }
    if (e->dataIn ())
    {
      foreach (Connection *c, e->dataIn ()->connections ())
      {
        if (c->way ().isEmpty ())
          continue;
        QPoint p1 = c->way ().first ().toPoint ();
        QPoint p2;
        for (int i = 1; i < c->way ().size (); i++)
        {
          p2 = c->way ()[i].toPoint ();
          eR += QRect (qMin (p1.x (), p2.x ()), qMin (p1.y (), p2.y ()), abs (p1.x () - p2.x ()), abs (p1.y () - p2.y ())).adjusted (-4, -4, 4, 4);
          p1 = p2;
        }
      }
    }
  }

  // 5px wide grid
  int step = 5;

  // round to grid size
  int x1 = (_from.x () / step) * step;
  int y1 = (_from.y () / step) * step;
  int x2 = (_to.x () / step) * step;
  int y2 = (_to.y () / step) * step;

  QPoint newTo = QPoint (x2, y2);

  // make sure that start and end get different nodes
  if (_from.x () != _to.x () && x1 == x2)
  {
    if (_from.x () > _to.x ())
      x1 += step;
    else
      x2 += step;
  }

  if (_from.y () != _to.y () && y1 == y2)
  {
    if (_from.y () > _to.y ())
      y1 += step;
    else
      y2 += step;
  }

  bool found = false;
  Node *n = NULL, *tmp, *tmp2, *lastIn = 0;

  // reuse old calculation if nothing changed
  if (_from == oldFrom_ && eR == oldReg_)
  {
    if (map_.contains (PAIR(newTo)))
    {
      n = map_[PAIR(newTo)];
      if (n->closed_ && n->counter_ == counter_)
        found = true;
    }
  }

  if (!found)
  {
    // increase usage counter
    counter_++;

    // create start node if not avaiable
    if (!map_.contains (QPair<int,int>(x1, y1)))
    {
      ll_ = new Node (counter_);
      nodes_.append (ll_);

      ll_->pos_ = QPoint (x1, y1);
      map_.insert (QPair<int,int>(x1, y1), ll_);

      for (unsigned int k = 0; k < 4; k++)
      {
        QPoint p2 = validPos (k, step, ll_->pos_);

        if (map_.contains (PAIR(p2)))
        {
          ll_->n_[k] = map_[PAIR(p2)];
          map_[PAIR(p2)]->n_[(k + 2) & 3] = ll_;
        }
      }
    }
    else
    {
      ll_ = map_[QPair<int,int>(x1, y1)];
      ll_->counter_ = counter_;
      ll_->prev_ = 0;
      ll_->next_ = 0;
      ll_->from_ = 0;
      ll_->f_ = 100000000;
      ll_->closed_ = false;
      ll_->cost_ = 5;
    }

    ll_->g_ = 0;
    ll_->h_ = heuristicDistance (_from, _to);
    ll_->type_ = Node::Horizontal;

  }

  while (ll_ && !found)
  {
    // take first node of the list
    n = ll_;
    ll_ = n->next_;
    if (ll_)
      ll_->prev_ = 0;

    n->closed_ = true;

    // stop if end node is found
    if (n->pos_.y () == y2 && n->pos_.x () == x2)
    {
      found = true;
      break;
    }

    // Add neighbor nodes if not yet present or reset old ones if not yet used during this round
    for (unsigned int i = 0; i < 4; i++)
      if (!n->n_[i])
      {
        QPoint p = validPos (i, step, n->pos_);
        if (bb.contains (p))
        {
          Node *nn = new Node (counter_);
          nodes_.append (nn);

          n->n_[i] = nn;

          nn->n_[(i + 2) & 3] = n;
          nn->pos_ = p;
          nn->h_ = heuristicDistance (p, newTo);

          if (eR.contains (p))
            nn->cost_ = 50;

          map_.insert (PAIR(nn->pos_), nn);

          for (unsigned int j = 0; j < 3; j++)
          {
            unsigned int k = (i - 1 + j) & 3;
            QPoint p2 = validPos (k, step, p);

            if (map_.contains (PAIR(p2)))
            {
              nn->n_[k] = map_[PAIR(p2)];
              map_[PAIR(p2)]->n_[(k + 2) & 3] = nn;
            }
          }
        }
      }
      else if (n->n_[i]->counter_ != counter_)
      {
        tmp = n->n_[i];
        tmp->counter_ = counter_;
        tmp->prev_ = 0;
        tmp->next_ = 0;
        tmp->from_ = 0;
        tmp->g_ = 100000000;
        tmp->f_ = 100000000;
        tmp->h_ = 100000000;
        tmp->closed_ = false;
        if (eR.contains (tmp->pos_))
          tmp->cost_ = 50;
        else
          tmp->cost_ = 5;
        tmp->h_ = heuristicDistance (tmp->pos_, newTo);
      }

    // update nodes in all directions
    for (unsigned int i = 0; i < 4; i++)
    {
      if (!n->n_[i])
        continue;

      if (n->n_[i]->closed_)
        continue;

      tmp = n->n_[i];

      unsigned int g = n->g_;

      // direction change?
      if ((n->type_ == Node::Horizontal && (i & 1)) || (n->type_ == Node::Vertical && (i & 1) == 0))
        g += n->cost_;
      else
        g += n->cost_ * 2;

      if (g < tmp->g_)
      {
        tmp->from_ = n;
        tmp->type_ = (i & 1)? Node::Horizontal : Node::Vertical;

        tmp->g_ = g;
        tmp->f_ = g + tmp->h_;

        // remove node from list
        if (tmp->prev_)
          tmp->prev_->next_ = tmp->next_;
        if (tmp->next_)
          tmp->next_->prev_ = tmp->prev_;
        if (tmp == ll_)
          ll_ = tmp->next_;

        // insert node at right position in sorted list
        if (!ll_ || tmp->f_ <= ll_->f_)
        {
          tmp->next_ = ll_;
          if (ll_)
            ll_->prev_ = tmp;
          ll_ = tmp;
          lastIn = tmp;
        }
        else
        {
          if (lastIn && lastIn->f_ <= tmp->f_ && lastIn != tmp)
            tmp2 = lastIn;
          else
            tmp2 = ll_;
          while (tmp2->next_ && tmp2->next_->f_ < tmp->f_)
            tmp2 = tmp2->next_;
          tmp->next_ = tmp2->next_;
          if (tmp->next_)
            tmp->next_->prev_ = tmp;
          tmp2->next_ = tmp;
          tmp->prev_ = tmp2;
          lastIn = tmp2;
        }
      }
    }
  }

  QPolygonF rv;

  // convert found way into polygon
  if (found)
  {
    bool dir = true;
    if (n->from_)
      if (n->pos_.y () != n->from_->pos_.y ())
        dir = false;

    int lastX = n->pos_.x ();
    int lastY = n->pos_.y ();

    QPoint last = _to;

    while (n)
    {
      if (dir && n->pos_.y () != lastY)
      {
        dir = false;
        lastX = n->pos_.x ();
        lastY = n->pos_.y ();
        rv.append (last);
        last = QPoint (n->pos_.x (), last.y ());
      }
      else if (!dir && n->pos_.x () != lastX)
      {
        dir = true;
        lastX = n->pos_.x ();
        lastY = n->pos_.y ();
        rv.append (last);
        last = QPoint (last.x (), n->pos_.y ());
      }

      n = n->from_;
    }

    if (dir)
      last.setY (_from.y ());
    else
      last.setX (_from.x ());

    rv.append(QPointF (last));
    rv.append(QPointF (_from));
  }
  else
  {
    rv.append(QPointF (_from));
    rv.append(QPointF (_to));
    std::cerr << "Not Found" << std::endl;
  }

  oldFrom_ = _from;
  oldReg_ = eR;

  // free unused nodes
  cleanup ();

  return rv;
}

//------------------------------------------------------------------------------

// Node constructor
WayFind::Node::Node(unsigned int _counter) :
  counter_ (_counter),
  type_ (Horizontal),
  prev_ (0),
  next_ (0),
  from_ (0),
  g_ (100000000),
  f_ (100000000),
  h_ (100000000),
  cost_ (5),
  closed_ (false)
{
  n_[0] = 0;
  n_[1] = 0;
  n_[2] = 0;
  n_[3] = 0;
}

//------------------------------------------------------------------------------

// Node destructor
WayFind::Node::~ Node()
{
}

//------------------------------------------------------------------------------

// Next position in distance _step from _pnt in direction _dir
QPoint WayFind::validPos(unsigned int _dir, int _step, QPoint _pnt)
{

  QPoint rv = _pnt;
  if (_dir == 0 || _dir == 3)
    _step = -_step;
  
  if (_dir == 1 || _dir == 3)
    rv += QPoint (_step, 0);
  else
    rv += QPoint (0, _step);

  return rv;
}

//------------------------------------------------------------------------------

// Heuristic distance from _from to _to
int WayFind::heuristicDistance(const QPoint &_from, const QPoint &_to) const
{
  QPoint p = _from - _to;
  return abs (p.x ()) + abs (p.y ());
}

//------------------------------------------------------------------------------

// cleanup ununsed nodes
void VSI::WayFind::cleanup()
{
  // only every 128 runs
  if ((counter_ & 0x7f) != 0)
    return;

  QLinkedList<Node *>::iterator it = nodes_.begin();

  while (it != nodes_.end ())
  {
    Node* n = *it;
    // nodes that hasn't be used in the last 64 rounds
    if (counter_ - n->counter_ > 64)
    {
      for (unsigned int i = 0; i < 4; i++)
        if (n->n_[i])
          n->n_[i]->n_[(i+2)&3] = NULL;
      it = nodes_.erase(it);
      map_.remove (PAIR(n->pos_));
      delete n;
    }
    else
      ++it;
  }
}

//------------------------------------------------------------------------------
}
