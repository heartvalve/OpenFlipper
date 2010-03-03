//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2009 by Computer Graphics Group, RWTH Aachen
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

#ifndef VSI_CONNECTION_HH
#define VSI_CONNECTION_HH

//== INCLUDES =================================================================
#include <QGraphicsLineItem>
#include <QPointF>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================

class ConnectionPoint;
class ElementInOut;
class ElementInput;
class ElementOutput;
class GraphicsScene;

//== CLASS DEFINITION =========================================================

/** Represents a connection between input and output
  */
class Connection : public QObject, public QGraphicsPathItem
{
  Q_OBJECT

  public:

    /// Constructor
    Connection (ConnectionPoint *_start, QGraphicsScene *_scene);
    Connection (ConnectionPoint *_start, ConnectionPoint *_end, QGraphicsScene *_scene);

    /// Destructor
    ~Connection ();

    /// called to update position on element movement
    void updatePositions ();

    /// Input of this connection
    ElementInput *input ();

    /// Output of this connection
    ElementOutput *output ();

    /// way of the connection
    const QPolygonF & way () const;

    /// invalidate way
    void invalidate ();

  protected:

    friend class GraphicsScene;

    // mouse handling
    void mouseMoveEvent (QGraphicsSceneMouseEvent *_event);
    void mousePressEvent (QGraphicsSceneMouseEvent *_event);
    void mouseReleaseEvent (QGraphicsSceneMouseEvent *_event);

  private:

    // helper to localize a connection point
    ConnectionPoint *cPointAt (QPointF _pnt);

    // generates a curved line out of the way polygon
    void updateLine ();

  private:

    QGraphicsItem *elementArea_;
    GraphicsScene *scene_;

    ConnectionPoint *p1_;
    ConnectionPoint *p2_;

    ConnectionPoint *old_;

    QPolygonF way_;
};

}

#endif
