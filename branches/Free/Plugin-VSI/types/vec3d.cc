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
#include "vec3d.hh"
#include "vec3dWidget.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::TypeVec3D - IMPLEMENTATION
//
//=============================================================================

/// Constructor
TypeVec3D::TypeVec3D()
{
  types_ << "Vector" << "Vec3D" << "Vec3d" << "vec3d";

  hints_ << "default";
}

//------------------------------------------------------------------------------

/// Has configuration widget
bool TypeVec3D::hasWidget()
{
  return true;
}

//------------------------------------------------------------------------------

/// Return configuration widget
TypeWidget * TypeVec3D::widget(QMap< QString, QString > _hints, QString _typeName, QWidget *_parent)
{
  return new Vec3DWidget (_hints, _typeName, _parent);
}

//------------------------------------------------------------------------------
}