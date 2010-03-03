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
#include "bool.hh"
#include "boolWidget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::TypeBool - IMPLEMENTATION
//
//=============================================================================

/// Constructor
TypeBool::TypeBool()
{
  types_ << "Bool";

  hints_ << "default";

  convertTypes_ << "Float" << "Int";
}

//------------------------------------------------------------------------------

/// Return configuration widget
TypeWidget * TypeBool::widget(QMap< QString, QString > _hints, QString _typeName, QWidget * _parent)
{
  return new BoolWidget (_hints, _typeName, _parent);
}

//------------------------------------------------------------------------------

/// Has configuration widget
bool TypeBool::hasWidget()
{
  return true;
}

//------------------------------------------------------------------------------
}