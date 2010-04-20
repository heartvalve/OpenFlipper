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

#ifndef VSI_TRASH_HH
#define VSI_TRASH_HH

//== INCLUDES =================================================================
#include <QGraphicsPixmapItem>
#include <QGraphicsLayoutItem>
#include <QString>
#include <QTimer>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class GraphicsScene;

//== CLASS DEFINITION =========================================================

/** Trash widget that removes elements from the scene_
  */
class Trash : public QObject, public QGraphicsPixmapItem, public QGraphicsLayoutItem
{
  Q_OBJECT

  public:

    /// Constructor
    Trash (GraphicsScene *_scene, QGraphicsItem *_parent);

    /// Destructor
    ~Trash ();

    /// Makes the trash opaque (will be called of an element is moved above this widget)
    void activate ();

    /// Makes the trash transparent (will be called of an element is moved away from this widget)
    void deactivate ();

    /// Sets the geometry
    virtual void setGeometry (const QRectF &_rect);

  protected:
    // catch mouse hover events
    virtual void hoverEnterEvent (QGraphicsSceneHoverEvent *_event);
    virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent *_event);

    // catch mouse press events
    virtual void mousePressEvent (QGraphicsSceneMouseEvent *_event);

    // size information for layouting
    virtual QSizeF sizeHint ( Qt::SizeHint _which, const QSizeF & _constraint = QSizeF()) const;

  private:
    GraphicsScene *scene_;
};

//=============================================================================
}
//=============================================================================

#endif
