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
#include "matrix4x4.hh"
#include "matrix4x4Widget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::TypeMatrix4x4 - IMPLEMENTATION
//
//=============================================================================

/// Constructor
TypeMatrix4x4::TypeMatrix4x4()
{
  types_ << "Matrix" << "Matrix4x4";

  hints_ << "default";
}

//------------------------------------------------------------------------------

/// Has configuration widget
bool TypeMatrix4x4::hasWidget()
{
  return true;
}

//------------------------------------------------------------------------------

/// Return configuration widget
TypeWidget * TypeMatrix4x4::widget(QMap< QString, QString > _hints, QString _typeName, QWidget *_parent)
{
  return new Matrix4x4Widget (_hints, _typeName, _parent);
}

//------------------------------------------------------------------------------
}