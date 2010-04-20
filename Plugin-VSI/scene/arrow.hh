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

#ifndef VSI_ARROW_HH
#define VSI_ARROW_HH

//== INCLUDES =================================================================
#include <QGraphicsWidget>
#include <QGraphicsPathItem>
#include <QString>
#include <QTimer>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class GraphicsScene;

//== CLASS DEFINITION =========================================================

/// Scene movement widget
class Arrow : public QObject, public QGraphicsPixmapItem, public QGraphicsLayoutItem
{
  Q_OBJECT

  public:

    /// Movement direction
    enum Direction
    {
      North,
      South,
      East,
      West,
      Center
    };

  public:

    /// Constructor
    Arrow (GraphicsScene *_scene, QGraphicsItem *_parent, Direction _dir);

    /// Destructor
    ~Arrow ();

    /// Activates the timer for movement (will be called if an element is moved above)
    void activate ();

    /// Stop the timer
    void reset ();

    /// Highlights the widget if the scene can be moved in this direction
    void setHighlight (bool _highlight);

    /// Sets the geometry 
    virtual void setGeometry (const QRectF &_rect);

  protected:

    // catch mouse hover events
    virtual void hoverEnterEvent (QGraphicsSceneHoverEvent *_event);
    virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent *_event);

    // catch mouse press/release events
    virtual void mousePressEvent (QGraphicsSceneMouseEvent *_event);
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *_event);

    // catch mouse wheel events
    virtual void wheelEvent (QGraphicsSceneWheelEvent *_event);

    // size information for layouting
    virtual QSizeF sizeHint ( Qt::SizeHint _which, const QSizeF & _constraint = QSizeF()) const;

  private slots:
    void timeout ();

  private:
    GraphicsScene *scene_;

    Direction dir_;

    QTimer timer_;

    int interval_;

    bool moveSelected_;

    bool highlight_;
};

//=============================================================================
}
//=============================================================================

#endif
