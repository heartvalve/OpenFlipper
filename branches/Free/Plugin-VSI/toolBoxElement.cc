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
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QMimeData>
#include <QDrag>
#include <QMouseEvent>
#include <QPalette>

#include "toolBoxElement.hh"
#include "parser/element.hh"

#define BACKGROUND_RED   0x00
#define BACKGROUND_GREEN 0x00
#define BACKGROUND_BLUE  0x00
#define BACKGROUND_ALPHA 0xff

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS ToolBoxElement - IMPLEMENTATION
//
//=============================================================================

/// Constructor
ToolBoxElement::ToolBoxElement (Element *_element, QWidget *_parent) :
  QWidget (_parent),
  element_ (_element)
{
  QVBoxLayout *layout = new QVBoxLayout ();

  layout->setContentsMargins (2, 2, 2, 2);

  QLabel *label = new QLabel (element_->shortDescription (), this);

  label->setAlignment (Qt::AlignHCenter);
  label->setWordWrap (true);

  QPalette p = label->palette ();
  p.setBrush (QPalette::WindowText, Qt::white);
  p.setBrush (QPalette::ButtonText, Qt::white);
  label->setPalette (p);
  
  layout->addWidget (label);

  setToolTip (element_->longDescription ());

  setLayout (layout);
}

//------------------------------------------------------------------------------

/// Destructor
ToolBoxElement::~ToolBoxElement ()
{
}

//------------------------------------------------------------------------------

// Paint a rounded rect as background
void ToolBoxElement::paintEvent(QPaintEvent * /*_event*/)
{
  QPainter p (this);
  p.setRenderHint(QPainter::Antialiasing);

  p.setBrush(QBrush(QColor(BACKGROUND_RED,
                                   BACKGROUND_GREEN,
                                   BACKGROUND_BLUE,
                                   BACKGROUND_ALPHA)));
  p.setPen(QColor(BACKGROUND_RED + 0x30,
                          BACKGROUND_GREEN + 0x30,
                          BACKGROUND_BLUE + 0x30,
                          BACKGROUND_ALPHA));
  p.drawRoundedRect(0, 0, width (), height (), 4, 4);

}

//------------------------------------------------------------------------------

/// Starts drag on mouse press
void ToolBoxElement::mousePressEvent(QMouseEvent *_event)
{
  QMimeData *mimeData = new QMimeData;

  QPoint off = _event->pos() - QPoint (width () / 2, height () / 2);

  QString data = QString::number (off.x ()) + ";";
  data += QString::number (off.y ()) + ";" + element_->name ();

  // internal mimetype
  mimeData->setData ("application/x-openflipper.vsi",data.toLocal8Bit ());

  QDrag *drag = new QDrag(this);
  drag->setMimeData(mimeData);

  QPixmap pixmap (size ());
  pixmap.fill(Qt::transparent);

  render (&pixmap);

  drag->setPixmap(pixmap);
  drag->setHotSpot(_event->pos());

  drag->exec ();
}

//------------------------------------------------------------------------------
}

