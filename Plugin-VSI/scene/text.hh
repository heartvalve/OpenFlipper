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

#ifndef VSI_TEXT_HH
#define VSI_TEXT_HH

//== INCLUDES =================================================================
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QString>
#include <QTimer>
#include <QPen>
#include <QBrush>
#include <QPainterPath>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class GraphicsScene;

//== CLASS DEFINITION =========================================================

/** Layoutable text widget implementation with an optional background.
  * The background can va convex or concave sides.
  */
class Text : public QObject, public QGraphicsSimpleTextItem, public QGraphicsLayoutItem
{
  Q_OBJECT

  public:

    /// Constructor
    Text (QGraphicsItem *_parent = 0);
    Text (const QString &_text, QGraphicsItem *_parent = 0);

    /// Destrucotr
    ~Text ();

    /// Sets the geometry
    virtual void setGeometry (const QRectF &_rect);

    /// Returns the shape for proper repainting/event handling
    QPainterPath shape () const;

    /// Background painting
    virtual void paint (QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget = 0);

    /// Bounding rectangle
    virtual QRectF boundingRect () const;

    /// Returns the current background brush
    QBrush backgroundBrush () { return backgroundBrush_; };

    /// Sets the background brush
    virtual void setBackgroundBrush (QBrush _brush);

    /// Returns the current background pen
    QPen backgroundPen () { return backgroundPen_; };

    /// Sets the background pen
    virtual void setBackgroundPen (QPen _pen);

    /// Enables background painting
    void setBackground (bool _leftOut, bool _rightOut);

    /// Disables backgorund painting
    void clearBackground ();

    /// Should this widget be stretchable in horizontal direction
    void setHorizontalStretch (bool _stretch);

    /// Placement of the text in a stretched widget 
    void setAlignment (Qt::Alignment _alignment);

    /// Set displayed text
    void setText (QString _text) {text_ = _text; update (); };

    /// Get displayed text
    QString text () { return text_; };

  protected:

    // size information for layouting
    virtual QSizeF sizeHint ( Qt::SizeHint _which, const QSizeF & _constraint = QSizeF()) const;

  private:

    QString text_;

    bool backgroundSet_;

    QBrush backgroundBrush_;
    QPen backgroundPen_;

    bool leftOut_;
    bool rightOut_;

    Qt::Alignment alignment_;
    bool hStretch_;
};

//=============================================================================
}
//=============================================================================

#endif
