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

#ifndef VSI_BUTTON_HH
#define VSI_BUTTON_HH

//== INCLUDES =================================================================
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QString>
#include <QTimer>
#include <QPen>
#include <QBrush>

#include "text.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class GraphicsScene;

//== CLASS DEFINITION =========================================================

/** Widgets that represents a clickable button based on VSI::Text
  */
class Button : public Text
{
  Q_OBJECT

  public:

    /// Constructor
    Button (QGraphicsItem *_parent = 0);
    Button (const QString &_text, QGraphicsItem *_parent = 0);

    /// Destructor
    ~Button ();

    /// Bounding rectangle
    QRectF boundingRect () const;

    /// Sets the geometry
    void setGeometry (const QRectF &_rect);

    /// Button glow painting
    void paint (QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget = 0);

    /// Sets the background brush
    void setBackgroundBrush (QBrush _brush);

    /// Sets the background pen
    void setBackgroundPen (QPen _pen);

  signals:
    /// emmited if the button gets pressed
    void pressed ();

  protected:

    // catch mouse hover events
    virtual void hoverEnterEvent (QGraphicsSceneHoverEvent *_event);
    virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent *_event);

    // catch mouse press/release events
    virtual void mousePressEvent (QGraphicsSceneMouseEvent *_event);
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *_event);

  private:
    QBrush backgroundBrush_;
    QPen backgroundPen_;

    bool glow_;

};

}

#endif
