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
//   $Revision: 5162 $
//   $Author: moebius $
//   $Date: 2009-02-27 17:40:45 +0100 (Fr, 27. Feb 2009) $
//
//=============================================================================

#ifndef VSI_GRAPHICSVIEW_HH
#define VSI_GRAPHICSVIEW_HH

//== INCLUDES =================================================================

#include <QGraphicsView>

//== NAMESPACES ===============================================================

namespace VSI {

//== CLASS DEFINITION =========================================================


/** Graphics view Widget that is used to display the VSI::GraphicsScene scene.
*/

class GraphicsView : public QGraphicsView
{
  Q_OBJECT

  public:
    /// Constructor
    GraphicsView(QWidget* _parent = NULL);

  signals:
    /// Inform the scene about size changes
    void sceneRectChanged(const QRectF &rect);

  protected:
    // handle resize events
    virtual void resizeEvent(QResizeEvent *_event);
};

//=============================================================================
}
//=============================================================================

#endif
