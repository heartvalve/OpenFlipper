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
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsGridLayout>

#include "baseWidget.hh"

#include "functionDisplay.hh"
#include "graphicsScene.hh"
#include "elementFunction.hh"
#include "text.hh"
#include "button.hh"

#include "../parser/function.hh"
#include "../parser/element.hh"

#define BACKGROUND_RED   0x0
#define BACKGROUND_GREEN 0x0
#define BACKGROUND_BLUE  0x0
#define BACKGROUND_ALPHA 0xff


//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::FunctionDisplay - IMPLEMENTATION
//
//=============================================================================

/// Constructor
FunctionDisplay::FunctionDisplay (GraphicsScene *_scene) :
  scene_ (_scene)
{
  QGraphicsGridLayout *tL = new QGraphicsGridLayout;

  // If the scene belongs to a function, then display its name and add the "Go back" button
  if (scene_->function ())
  {

    Text *f = new Text (scene_->function ()->function ()->element ()->shortDescription (), this);
    Text *fn = new Text (scene_->function ()->function ()->shortDescription (), this);
    Button *goBack = new Button (tr("Go back"), this);

    f->setBackground (true, false);
    f->setBrush (Qt::black);
    f->setBackgroundBrush (Qt::gray);
    fn->setBackground (true, false);
    fn->setBrush (Qt::black);
    fn->setBackgroundBrush (Qt::white);
    goBack->setBrush (Qt::black);
    goBack->setBackgroundBrush (Qt::lightGray);
    goBack->setBackground (true, true);

    connect (goBack, SIGNAL (pressed ()), BaseWidget::getBaseWidget(), SLOT (popScene ()));

    QFont font = f->font ();
    fn->setFont (font);
    font.setBold (true);
    f->setFont (font);


    tL->addItem (f, 0, 0, Qt::AlignVCenter | Qt::AlignLeft);
    tL->addItem (fn, 0, 1, Qt::AlignVCenter | Qt::AlignLeft);
    tL->addItem (goBack, 0, 2, Qt::AlignVCenter | Qt::AlignLeft);
  }
  // Scene is the main scene
  else
  {
    Text *f = new Text (tr("Main"), this);
    f->setBackground (true, true);
    f->setBrush (Qt::black);
    f->setBackgroundBrush (Qt::white);

    tL->addItem (f, 0, 0);

    QFont font = f->font ();
    font.setBold (true);
    f->setFont (font);
  }

  tL->setHorizontalSpacing (1);
  tL->setContentsMargins (3, 3, 3, 3);
  setLayout (tL);

  setMinimumWidth (tL->preferredWidth());
}

//------------------------------------------------------------------------------

/// Destructor
FunctionDisplay::~FunctionDisplay ()
{
}

//------------------------------------------------------------------------------

/// Paints the background
void FunctionDisplay::paint (QPainter *_painter, const QStyleOptionGraphicsItem *_option, QWidget *_widget)
{
  int w = geometry().width();
  int h = geometry().height();

  _painter->setRenderHint(QPainter::Antialiasing, true);

  _painter->setBrush(QBrush(QColor(BACKGROUND_RED,
                                   BACKGROUND_GREEN,
                                   BACKGROUND_BLUE,
                                   BACKGROUND_ALPHA)));
  
  _painter->setPen(QColor(BACKGROUND_RED,
                          BACKGROUND_GREEN,
                          BACKGROUND_BLUE,
                          BACKGROUND_ALPHA));

  QPainterPath path;
  path.moveTo (0, 0);
  path.lineTo (0, h);
  path.lineTo (w - 10, h);
  path.arcTo (w - 20, h - 20, 20, 20, -90, 90);
  path.lineTo (w, 0);
  path.lineTo (0, 0);
  _painter->drawPath (path);


  QLinearGradient lG;
  QRadialGradient rG;

  _painter->setPen (Qt::NoPen);

  lG.setStart (w - 5, 0);
  lG.setFinalStop(w, 0);
  lG.setColorAt(0, Qt::transparent);
  lG.setColorAt(1, QColor (255, 255, 255, 128));
  _painter->setBrush (lG);
  _painter->drawRect (w - 5, 0, 5, h - 10);

  lG.setStart (0, h - 5);
  lG.setFinalStop (0, h);
  _painter->setBrush (lG);
  _painter->drawRect (0, h - 5, w - 10, 5);

  path = QPainterPath ();
  path.moveTo (w - 10, h);
  path.arcTo (w - 20, h - 20, 20, 20, -90, 90);
  path.lineTo (w - 10, h - 10);
  path.lineTo (w - 10, h);
  rG.setCenter (w - 10, h - 10);
  rG.setFocalPoint(w -10, h - 10);
  rG.setRadius (10);
  rG.setColorAt (0, Qt::transparent);
  rG.setColorAt (0.5, Qt::transparent);
  rG.setColorAt (1, QColor (255, 255, 255, 128));
  _painter->setBrush (rG);
  _painter->drawPath(path);

  QGraphicsWidget::paint (_painter, _option, _widget);
}

//------------------------------------------------------------------------------
}

