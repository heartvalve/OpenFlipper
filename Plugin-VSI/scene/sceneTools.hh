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

#ifndef VSI_SCENETOOLS_HH
#define VSI_SCENETOOLS_HH

//== INCLUDES =================================================================
#include <QGraphicsProxyWidget>
#include <QString>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class GraphicsScene;
class Arrow;
class Trash;
class ZoomButton;
class FunctionDisplay;

//== CLASS DEFINITION =========================================================

/** Widget that acts as parent for the scene manipulation widgets (Arrows, ZoomButtons, Trash, ...)
  * It layouts itself (and also its childs) to the visible scene rectangle. It also
  * forwards events from the scene to its child widgets
  */
class SceneTools : public QGraphicsWidget
{
  Q_OBJECT

  public:

    /// Constructor
    SceneTools (GraphicsScene *_scene);

    /// Destructor
    ~SceneTools ();

    /// Handles mouse movement (will be called by the scene, if the mouse with a draged element is moved)
    void mouseMove (QPointF _pos);

    /// Handles mouse release (will be called by the scene, if the element is dropped)
    void mouseRelease (QPointF _pos, QGraphicsItem *_item);

  private slots:

    // handle scene size changes
    void sceneRectChanged (const QRectF &_rect);

    // update arrow depending on visible scene rect
    void updateArrows ();

  private:
    GraphicsScene *scene_;

    Arrow *arrows_[5];
    Trash *trash_;
    ZoomButton *zoom_[2];

    FunctionDisplay *fDisplay_;
};

//=============================================================================
}
//=============================================================================

#endif
