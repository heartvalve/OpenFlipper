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

#ifndef VSI_CONNECTIONPOINT_HH
#define VSI_CONNECTIONPOINT_HH

//== INCLUDES =================================================================
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QGraphicsSceneMouseEvent>
#include <QString>
#include <QTimer>
#include <QPointF>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class GraphicsScene;
class ElementInOut;

//== CLASS DEFINITION =========================================================

/** Widget that represents the point where connection can be connected to/from.
  * It also displays the current status of its input/output in a different color
  */
class ConnectionPoint : public QObject, public QGraphicsEllipseItem, public QGraphicsLayoutItem
{
  Q_OBJECT
  Q_INTERFACES(QGraphicsLayoutItem)

  public:

    /// State of the connection point
    enum State {
      NoValue = 0,
      Optional = 1,
      RuntimeInput = 2,
      UserInput = 3,
      Connected = 4,
      OutputNotConnected = 5
    };

    /// Constructor
    ConnectionPoint (ElementInOut *_e, QGraphicsItem *_parent);

    /// Destructor
    ~ConnectionPoint ();

    /// Input/output element
    ElementInOut *inOut () const { return inout_; };

    /// Position for connections
    QPointF connectPos ();

    /// Sets the geometry
    virtual void setGeometry (const QRectF &_rect);

    /// sets the state
    void setState (State _state);

    /// Sets the diameter
    void setWidth (int _width);

    // painting
    void paint (QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget = 0);

  protected:

    // start a new connection on press
    void mousePressEvent (QGraphicsSceneMouseEvent *_event);

    // highlight connection if mouse over
    void hoverEnterEvent (QGraphicsSceneHoverEvent *_event);
    void hoverLeaveEvent (QGraphicsSceneHoverEvent *_event);

    // size informations for layouting
    virtual QSizeF sizeHint ( Qt::SizeHint _which, const QSizeF & _constraint = QSizeF()) const;

  private:

    ElementInOut *inout_;

    int width_;
};

}

#endif
