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
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QPalette>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>

#include "sceneTools.hh"
#include "sceneElement.hh"
#include "graphicsScene.hh"
#include "arrow.hh"
#include "zoomButton.hh"
#include "trash.hh"
#include "functionDisplay.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::SceneTools - IMPLEMENTATION
//
//=============================================================================

/// Constructor
SceneTools::SceneTools (GraphicsScene *_scene) :
  scene_ (_scene)
{
  QRectF rect = scene_->sceneRect();

  arrows_[0] = new Arrow (_scene, this, Arrow::North);
  arrows_[1] = new Arrow (_scene, this, Arrow::South);
  arrows_[2] = new Arrow (_scene, this, Arrow::East);
  arrows_[3] = new Arrow (_scene, this, Arrow::West);
  arrows_[4] = new Arrow (_scene, this, Arrow::Center);

  QGraphicsGridLayout *aLayout = new QGraphicsGridLayout;
  aLayout->addItem (arrows_[0], 0, 1, Qt::AlignCenter);
  aLayout->addItem (arrows_[1], 2, 1, Qt::AlignCenter);
  aLayout->addItem (arrows_[2], 1, 2, Qt::AlignCenter);
  aLayout->addItem (arrows_[3], 1, 0, Qt::AlignCenter);
  aLayout->addItem (arrows_[4], 1, 1, Qt::AlignCenter);

  trash_ = new Trash (_scene, this);

  QGraphicsLinearLayout *zLayout = new QGraphicsLinearLayout (Qt::Horizontal);
  zoom_[0] = new ZoomButton (_scene, this, ZoomButton::In);
  zoom_[1] = new ZoomButton (_scene, this, ZoomButton::Out);
  zLayout->addItem (zoom_[0]);
  zLayout->addItem (zoom_[1]);

  fDisplay_ = new FunctionDisplay (scene_);
  scene_->addItem (fDisplay_);

  setGeometry (rect);
  connect (scene_, SIGNAL (sceneRectChanged (const QRectF &)),
           this, SLOT (sceneRectChanged (const QRectF &)));

  QGraphicsGridLayout *mLayout = new QGraphicsGridLayout;

  mLayout->addItem (aLayout, 0, 2, Qt::AlignTop | Qt::AlignRight);
  mLayout->addItem (zLayout, 2, 0, Qt::AlignBottom | Qt::AlignLeft);
  mLayout->addItem (trash_, 2, 2, Qt::AlignBottom | Qt::AlignRight);
  mLayout->setColumnStretchFactor (1, 1);
  mLayout->setRowStretchFactor (1, 1);
  mLayout->setContentsMargins (7, 7, 7, 7);

  setLayout (mLayout);
}

//------------------------------------------------------------------------------

/// Destructor
SceneTools::~SceneTools ()
{
}

//------------------------------------------------------------------------------

// handle scene size changes
void SceneTools::sceneRectChanged (const QRectF &_rect)
{
  setGeometry (_rect);

  fDisplay_->setPos(_rect.topLeft ());
  updateArrows ();
}

//------------------------------------------------------------------------------

// update arrow depending on visible scene rect
void SceneTools::updateArrows ()
{
  QRectF bb = scene_->elementsBoundingRect ();
  QRectF rect = scene_->sceneRect();

  if (bb.top () < rect.top ())
    arrows_[0]->setHighlight (true);
  else
    arrows_[0]->setHighlight (false);

  if (bb.bottom () > rect.bottom ())
    arrows_[1]->setHighlight (true);
  else
    arrows_[1]->setHighlight (false);

  if (bb.right () > rect.right ())
    arrows_[2]->setHighlight (true);
  else
    arrows_[2]->setHighlight (false);

  if (bb.left () < rect.left ())
    arrows_[3]->setHighlight (true);
  else
    arrows_[3]->setHighlight (false);

  if (rect.center () != bb.center ())
    arrows_[4]->setHighlight (true);
  else
    arrows_[4]->setHighlight (false);
}

//------------------------------------------------------------------------------

/// Handles mouse movement (will be called by the scene, if the mouse with a draged element is moved)
void SceneTools::mouseMove (QPointF _pos)
{
  for (unsigned int i = 0; i < 5; i++)
  {
    if (arrows_[i]->contains (arrows_[i]->mapFromScene (_pos)))
      arrows_[i]->activate ();
    else
      arrows_[i]->reset ();
  }

  for (unsigned int i = 0; i < 2; i++)
  {
    if (zoom_[i]->contains (zoom_[i]->mapFromScene (_pos)))
      zoom_[i]->activate (_pos);
    else
      zoom_[i]->deactivate ();
  }

  if (trash_->contains (trash_->mapFromScene (_pos)))
    trash_->activate ();
  else
    trash_->deactivate ();
}

//------------------------------------------------------------------------------

/// Handles mouse release (will be called by the scene, if the element is dropped)
void SceneTools::mouseRelease (QPointF _pos, QGraphicsItem *_item)
{
  for (unsigned int i = 0; i < 5; i++)
  {
      arrows_[i]->reset ();
  }
  for (unsigned int i = 0; i < 2; i++)
  {
      zoom_[i]->deactivate ();
  }
  trash_->deactivate ();

  if (trash_->contains (trash_->mapFromScene (_pos)))
  {
    SceneElement *se = dynamic_cast<SceneElement *> (_item);
    if (se && scene_->removeElement (se))
      delete se;
  }
}

//------------------------------------------------------------------------------
}

