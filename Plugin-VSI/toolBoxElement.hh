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

#ifndef TOOLBOXELEMENT_HH
#define TOOLBOXELEMENT_HH

//== INCLUDES =================================================================
#include <QWidget>
#include <QString>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class Element;

//== CLASS DEFINITION =========================================================

/** Tool box widget that represents a visual script element
 */
class ToolBoxElement : public QWidget
{
  Q_OBJECT

  public:
    /// Constructor
    ToolBoxElement (Element *_element, QWidget *_parent = NULL);

    /// Destructor
    ~ToolBoxElement ();

    /// Starts drag on mouse press
    void mousePressEvent(QMouseEvent *_event);

  protected:
    // painting
    void paintEvent(QPaintEvent *_event);

  private:
    Element *element_;
};

//=============================================================================
}
//=============================================================================

#endif
