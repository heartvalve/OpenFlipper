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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================



#ifndef QT_GRAPHICS_BUTTON_
#define QT_GRAPHICS_BUTTON_

//=============================================================================
//
//  CLASS QtGraphicsButton - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <QGraphicsItem>

//== CLASS DEFINITION =========================================================

/** Simple Button implementation for QGraphicsScene
*/


class QtGraphicsButton : public QObject, public QGraphicsItem
{
  Q_OBJECT

  public:

    /** Create a glViewer.
    \param _image button imahe
    \param _parent parent graphics scene item
    \param _width button width
    \param _height button height
    */

    QtGraphicsButton (const QImage &_image, QGraphicsItem *_parent = 0, int _width = -1, int _height = -1);

  public:

    /// returns the bounding rect
    virtual QRectF boundingRect () const;

    /// paints the button
    virtual void paint (QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget = 0);

    /// event tracking
    virtual void hoverEnterEvent (QGraphicsSceneHoverEvent *_event);
    virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent *_event);
    virtual void mousePressEvent (QGraphicsSceneMouseEvent *_event);
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *_event);

    virtual QVariant itemChange (GraphicsItemChange _change, const QVariant &_value);

    /// makes the button checkable
    void setCheckable (bool _value);

    /// sets button checked state
    void setChecked (bool _value);

    /// returns button checked state
    bool isChecked () const;

  signals:

    /// signals a button press
    void pressed ();

  private:

    /// button state
    bool checkable_;
    bool checked_;
    bool pressed_;
    bool over_;

    /// button size
    int width_;
    int height_;

    /// pixmaps for different button states
    QPixmap normalPix_;
    QPixmap overPix_;
    QPixmap checkedPix_;
};

#endif