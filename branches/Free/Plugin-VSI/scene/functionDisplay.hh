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

#ifndef VSI_FUNCTIONDISPLAY_HH
#define VSI_FUNCTIONDISPLAY_HH

//== INCLUDES =================================================================
#include <QGraphicsWidget>
#include <QString>
#include <QPushButton>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class ElementFunction;
class GraphicsScene;
class Button;

//== CLASS DEFINITION =========================================================

/** Widget that will display the current visible/editable function name.
 *  It provides a "Go back" button if the function is not the "Main" function
 */
class FunctionDisplay : public QGraphicsWidget
{
  Q_OBJECT

  public:

    /// Constructor
    FunctionDisplay (GraphicsScene *_scene);

    /// Destructor
    ~FunctionDisplay ();

    /// Paints the background
    void paint (QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget = 0);

    /// Scene
    GraphicsScene *graphicsScene () { return scene_; };

  private:

    GraphicsScene *scene_;
};

//=============================================================================
}
//=============================================================================

#endif
