/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS QtBaseViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtBaseViewer.hh"
#include <QDrag>
#include <QMimeData>

//== NAMESPACES ===============================================================

namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ==========================================================

void QtBaseViewer::startDrag()
{
  QString view;
  encodeView(view);

  QDrag     * drag = new QDrag( this );
  QMimeData * mime_data = new QMimeData();

  mime_data->setText( view );
  drag->setMimeData( mime_data );
  drag->start();
}


void QtBaseViewer::glDragEnterEvent(QDragEnterEvent* _event)
{
  if ( externalDrag_ ) {
    emit dragEnterEvent(_event);
  } else {
    if ( _event->mimeData()->hasFormat("text/plain") )
      _event->acceptProposedAction();
  }
}


void QtBaseViewer::glDropEvent(QDropEvent* _event)
{
  if ( externalDrag_ ) {
    emit dropEvent( _event );
  } else {
    if ( _event->source() != this )
    {
      QString view( _event->mimeData()->text() );
      decodeView(view);
      _event->acceptProposedAction();
    }
  }
}

//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
