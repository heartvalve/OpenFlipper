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
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================




//=============================================================================
//
//  CLASS QtGLViewerLayout - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <QGraphicsLayout>
#include <QGraphicsLayoutItem>
#include <QGraphicsWidget>

//== NAMESPACES ===============================================================

//== CLASS DEFINITION =========================================================


/** Graphics scene layout for examiner widget.
*/

class QtGLViewerLayout : public QGraphicsLayout
{
  public:

    QtGLViewerLayout (QGraphicsLayoutItem * _parent = 0);

    /// Add Wheel Widget to Layout
    void addWheelX (QGraphicsWidget *_item);
    void addWheelY (QGraphicsWidget *_item);
    void addWheelZ (QGraphicsWidget *_item);

    /// Pure virtual functions that have to be implemented
    virtual int count() const;
    virtual QGraphicsLayoutItem * itemAt(int _i) const;
    virtual void removeAt (int _index);

    virtual QSizeF sizeHint(Qt::SizeHint _which, const QSizeF & _constraint = QSizeF()) const;

    /// Tracks geometry changes
    virtual void setGeometry(const QRectF & rect);


  private:

    /// Recalculate layout
    void reLayout ();

    /// Items
    QGraphicsWidget *wheelX_;
    QGraphicsWidget *wheelY_;
    QGraphicsWidget *wheelZ_;

    QVector<QGraphicsWidget *> items_;

};

//=============================================================================

//=============================================================================
