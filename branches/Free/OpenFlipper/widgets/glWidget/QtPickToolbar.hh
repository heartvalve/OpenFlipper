/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
\*===========================================================================*/

#ifndef QT_PICK_TOOLBAR_
#define QT_PICK_TOOLBAR_

//=============================================================================
//
//  CLASS QtPickToolbar
//
//=============================================================================

//== INCLUDES =================================================================

#include <QSettings>
#include <QMainWindow>

#include <QGraphicsProxyWidget>
#include <OpenFlipper/common/GlobalDefines.hh>

//== FORWARDDECLARATIONS ======================================================

class QToolBar;

//== CLASS DEFINITION =========================================================


/** \class QtPickToolbar QtPickToolbar.hh <OpenFlipper/widgets/glWidget/QtPickToolbar.hh>

   A graphics scene widget that displays a toolbar inside the graphics scene
 **/

class DLLEXPORT QtPickToolbar : public QGraphicsProxyWidget
{
    Q_OBJECT

  public:
    /** Create a pick toolbar.
    \param _parent parent graphics item
    */
    QtPickToolbar (QMainWindow *_core, QGraphicsItem *_parent = 0);

    /// recalculate geometry
    void updateGeometry ();

    /// attach a QToolBar
    void attachToolbar (QToolBar *_m);

    /// detach toolbar
    void detachToolbar ();

  private:

    /// paints decoration
    virtual void paintWindowFrame(QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget = 0);

    /// categorize frame area
    virtual Qt::WindowFrameSection windowFrameSectionAt(const QPointF &_pos) const;

    /// event filter to track status tip events
    bool eventFilter(QObject *_obj, QEvent *_event);

  private:

    // child widget
    QToolBar *toolbar_;

    QMainWindow *core_;
};

//=============================================================================
//=============================================================================
#endif // QT_SLIDE_WINDOW_ defined
//=============================================================================

