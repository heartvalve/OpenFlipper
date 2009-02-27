//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  CLASS QtBaseViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtBaseViewer.hh"

//== NAMESPACES ===============================================================

namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ==========================================================

void QtBaseViewer::startDrag()
{
  QString view;
  encodeView(view);

  QDrag     * drag = new QDrag( this );
  QMimeData * mime_data = new QMimeData;

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
