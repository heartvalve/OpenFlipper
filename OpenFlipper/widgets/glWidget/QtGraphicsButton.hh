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
  Q_INTERFACES(QGraphicsItem)

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

