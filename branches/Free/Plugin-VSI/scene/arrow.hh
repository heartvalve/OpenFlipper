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
  Q_INTERFACES(QGraphicsLayoutItem)

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
