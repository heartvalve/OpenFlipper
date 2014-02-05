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

#ifndef QT_SLIDE_WINDOW_
#define QT_SLIDE_WINDOW_

//=============================================================================
//
//  CLASS QtSlideWindow
//
//=============================================================================

//== INCLUDES =================================================================

#include <QSettings>
#include <QGraphicsProxyWidget>
#include <QPropertyAnimation>
#include <QTimer>

#include <OpenFlipper/common/GlobalDefines.hh>

//== FORWARDDECLARATIONS ======================================================

class QtGraphicsButton;
class QTimeLine;
class QGraphicsItemAnimation;

//== CLASS DEFINITION =========================================================


/** \class QtSlideWindow QtSlideWindow.hh <OpenFlipper/widgets/glWidget/QtSlideWindow.hh>

   A graphics scene widget that has a hover slide effect and detach functionality
   for a child widget
 **/

class DLLEXPORT QtSlideWindow : public QGraphicsProxyWidget
{
    Q_OBJECT

  public:
    /** Create a glViewer.
    \param _name displayed titlebar name
    \param _parent parent graphics item
    */
    QtSlideWindow (QString _name = 0, QGraphicsItem *_parent = 0);

    /// recalculate geometry
    void updateGeometry ();

    /// attach a child widget
    void attachWidget (QWidget *_m);

    /// detach child widget
    void detachWidget ();


    /// saves the current state
    void saveState (QSettings &_settings);

    /// restores the state
    void restoreState (QSettings &_settings);

    /// Call this to correctly set start and ending positions
    void updateParentGeometry();

  private:

    /// paints decoration
    virtual void paintWindowFrame(QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget = 0);

    /// track frame events
    virtual bool windowFrameEvent(QEvent *_e);

    /// categorize frame area
    virtual Qt::WindowFrameSection windowFrameSectionAt(const QPointF &_pos) const;

    /// hove event tracking
    virtual void hoverEnterEvent (QGraphicsSceneHoverEvent *_event);
    virtual void hoverLeaveEvent (QGraphicsSceneHoverEvent *_event);

    /// size & position event tracking
    virtual void resizeEvent (QGraphicsSceneResizeEvent *_event);
    virtual void moveEvent (QGraphicsSceneMoveEvent *_event);

  private slots:
    /// detach button pressed
    void detachPressed ();

    /// detached dialog closed
    void dialogClosed ();

    /// autohide button presed
    void autohidePressed ();

    /// Slide widget up
    void slideUp();

    /// Slide widget down
    void slideDown();

    /// Slot is called whenever the animation is finished
    void animationFinished();

    /// Start actual slide down
    void startSlideDownAnimation();

  private:

    // name
    QString name_;

    // child widget
    QWidget *mainWidget_;

    // buttons
    QtGraphicsButton *autohideButton_;
    QtGraphicsButton *detachButton_;

    // detached dialog
    QDialog *dialog_;

    // Starting position (for animation)
    QPointF startP_;

    // Ending position (for animation)
    QPointF endP_;

    // Track if widget is at bottom position
    bool down_;

    // Animation object
    QPropertyAnimation* animation_;

    // Is widget animating in this moment?
    bool animating_;

    // Wait some milliseconds before sliding widget down
    QTimer* timer_;
};

//=============================================================================
//=============================================================================
#endif // QT_SLIDE_WINDOW_ defined
//=============================================================================

