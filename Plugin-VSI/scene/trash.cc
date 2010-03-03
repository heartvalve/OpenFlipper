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

//== INCLUDES =================================================================
#include <QPixmap>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>

#include <OpenFlipper/common/GlobalOptions.hh>

#include "trash.hh"
#include "graphicsScene.hh"
#include "sceneElement.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::Trash - IMPLEMENTATION
//
//=============================================================================


/// Constructor
Trash::Trash (GraphicsScene *_scene, QGraphicsItem *_parent) :
  QGraphicsPixmapItem (_parent),
  scene_ (_scene)
{
  setPixmap (OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"user-trash.png");

  setOpacity (0.4);

  setAcceptHoverEvents (true);
}

//------------------------------------------------------------------------------

/// Destructor
Trash::~Trash ()
{
}

//------------------------------------------------------------------------------

// make the trash opaque if mouse is over and we have selected elements
void Trash::hoverEnterEvent (QGraphicsSceneHoverEvent *_event)
{
  if (!scene_->selectedItems ().isEmpty ())
    setOpacity (1.0);
}

//------------------------------------------------------------------------------

// make trash transparent on mouse leave
void Trash::hoverLeaveEvent (QGraphicsSceneHoverEvent *_event)
{
  setOpacity (0.4);
}

//------------------------------------------------------------------------------

// delete selected elements on mouse press
void Trash::mousePressEvent (QGraphicsSceneMouseEvent *_event)
{
  if (!scene_->selectedItems ().isEmpty ())
    foreach (QGraphicsItem *e, scene_->selectedItems ())
    {
      SceneElement *se = dynamic_cast<SceneElement *> (e);
      if (se && scene_->removeElement (se))
        delete se;
    }
}

//------------------------------------------------------------------------------

/// Makes the trash opaque (will be called of an element is moved above this widget)
void Trash::activate ()
{
  setOpacity (1.0);
}

//------------------------------------------------------------------------------

/// Makes the trash transparent (will be called of an element is moved away from this widget)
void Trash::deactivate ()
{
  setOpacity (0.4);
}

//------------------------------------------------------------------------------

/// Sets the geometry
void Trash::setGeometry (const QRectF &_rect)
{
  QGraphicsPixmapItem::setPos (_rect.topLeft ());
  QGraphicsLayoutItem::setGeometry (_rect);
}

//------------------------------------------------------------------------------

// size information for layouting
QSizeF Trash::sizeHint (Qt::SizeHint _which, const QSizeF &_constraint) const
{
  QSizeF sh;
  switch (_which) {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
    case Qt::MaximumSize:
      sh = QSizeF (pixmap ().width (), pixmap ().height ());
      break;
    default:
      break;
  }

  return sh;
}

//------------------------------------------------------------------------------
}
