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

#ifndef VSI_ZOOMBUTTON_HH
#define VSI_ZOOMBUTTON_HH

//== INCLUDES =================================================================
#include <QGraphicsPixmapItem>
#include <QGraphicsLayoutItem>
#include <QString>
#include <QTimer>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class GraphicsScene;

/// Scene zoom in/out widget
class ZoomButton : public QObject, public QGraphicsPixmapItem, public QGraphicsLayoutItem
{
  Q_OBJECT

  public:

    /// Zoom type
    enum Type
    {
      In,
      Out
    };

  public:

    /// Constructor
    ZoomButton (GraphicsScene *_scene, QGraphicsItem *_parent, Type _type);

    /// Destructor
    ~ZoomButton ();

    /// Activates the timer for zoom with center at _pos (will be called if an element is moved above)
    void activate (QPointF _pos);

    /// Stop the timer
    void deactivate ();

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

    Type type_;

    QTimer timer_;

    QPointF pos_;
};

//=============================================================================
}
//=============================================================================

#endif
