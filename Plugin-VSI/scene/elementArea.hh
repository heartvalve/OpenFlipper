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
  Q_INTERFACES(QGraphicsItem)

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
    void paint (QPainter * /*_painter*/, const QStyleOptionGraphicsItem * /*_option*/, QWidget * /*_widget*/ = 0) {};

  private:
    GraphicsScene *scene_;

    QList<SceneElement *> elements_;

    QRectF elementsBoundingRect_;
};

}

#endif
