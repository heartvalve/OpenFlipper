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
#include "selection.hh"
#include "selectionWidget.hh"


//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::TypeSelection - IMPLEMENTATION
//
//=============================================================================

/// Constructor
TypeSelection::TypeSelection()
{
  types_ << "Selection";

  hints_ << "default" << "multiple" << "names" << "descriptions";

  convertTypes_ << "String";
}

//------------------------------------------------------------------------------

/// Return configuration widget
TypeWidget * TypeSelection::widget(QMap< QString, QString > _hints, QString _typeName, QWidget * _parent)
{
  return new SelectionWidget (_hints, _typeName, _parent);
}

//------------------------------------------------------------------------------

/// Has configuration widget
bool TypeSelection::hasWidget()
{
  return true;
}

//------------------------------------------------------------------------------
}