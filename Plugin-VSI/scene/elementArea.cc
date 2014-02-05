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

//== INCLUDES =================================================================
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

#include "../parser/element.hh"
#include "elementArea.hh"
#include "graphicsScene.hh"
#include "sceneElement.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::ElementArea - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ElementArea::ElementArea (GraphicsScene *_scene) :
  scene_ (_scene)
{
}

//------------------------------------------------------------------------------

/// Destructor
ElementArea::~ElementArea ()
{
}

//------------------------------------------------------------------------------

/// Bounding rectangle
QRectF ElementArea::boundingRect () const
{
  return elementsBoundingRect_;
}

//------------------------------------------------------------------------------

/// Add a child element
void ElementArea::addElement (SceneElement *_element)
{
  foreach (QGraphicsItem *e, elements_)
    e->setSelected (false);

  elements_.append (_element);
  _element->setParentItem (this);
  prepareGeometryChange ();
  elementsBoundingRect_ = _element->boundingRect () | _element->childrenBoundingRect ();

  _element->setSelected (true);
  update ();
}

//------------------------------------------------------------------------------

/// Remove an element, if its removeable or _force is set
bool ElementArea::removeElement (SceneElement *_element, bool _force)
{
 
  if (!_force && _element->element ()->flags () & ELEMENT_FLAG_NO_DELETE)
    return false;
  if (elements_.contains (_element))
  {
    elements_.removeAll (_element);
    _element->setParentItem (0);
    prepareGeometryChange();
    elementsBoundingRect_ = childrenBoundingRect();
    return true;
  }
  else
    return false;
}

//------------------------------------------------------------------------------

/// All elements
const QList<SceneElement *>& ElementArea::elements () const
{
  return elements_;
}

//------------------------------------------------------------------------------
}
