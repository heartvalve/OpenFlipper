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
//   $Revision: 3663 $
//   $Author: moebius $
//   $Date: 2008-11-06 14:55:30 +0100 (Do, 06. Nov 2008) $
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

//== IMPLEMENTATION ==========================================================

void QtBaseViewer::glDragEnterEvent(QDragEnterEvent* _event)
{
 emit dragEnterEvent(_event);
}


void QtBaseViewer::glDropEvent(QDropEvent* _event)
{
 emit dropEvent( _event );
}

//=============================================================================
//=============================================================================
