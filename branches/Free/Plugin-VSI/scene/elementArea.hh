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

#ifndef VSI_ELEMENTAREA_HH
#define VSI_ELEMENTAREA_HH

//== INCLUDES =================================================================
#include <QGraphicsItem>

//== NAMESPACES ===============================================================
namespace VSI {

//== FORWARDDECLARATIONS ======================================================
class GraphicsScene;
class SceneElement;

//== CLASS DEFINITION =========================================================

/** Widget that acts as parent for all elements in scene.
  * It is used to realize movement and scaling of all its child elements
  */
class ElementArea : public QObject, public QGraphicsItem
{
  Q_OBJECT

  public:

    /// Constructor
    ElementArea (GraphicsScene *_scene);

    /// Destructor
    ~ElementArea ();

    /// Add a child element
    void addElement (SceneElement *_element);

    /// Remove an element, if its removeable or _force is set
    bool removeElement (SceneElement *_element, bool _force = false);

    /// Bounding rectangle
    QRectF boundingRect () const;

    /// All elements
    const QList<SceneElement *>& elements () const;

    /// for QGraphicsItem
    void paint (QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget = 0) {};

  private:
    GraphicsScene *scene_;

    QList<SceneElement *> elements_;

    QRectF elementsBoundingRect_;
};

}

#endif
