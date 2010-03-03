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
#include "type.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//=============================================================================
//
//  CLASS VSI::Type - IMPLEMENTATION
//
//=============================================================================

/// Constructor
Type::Type()
{
}

//------------------------------------------------------------------------------

/// Destructor
Type::~ Type()
{
}

//------------------------------------------------------------------------------

/// Names of Types
QStringList Type::supportedTypes()
{
  return types_;
}

//------------------------------------------------------------------------------

/// Hints that may be stored for this type in the xml metadata
QStringList Type::supportedHints()
{
  return hints_;
}

//------------------------------------------------------------------------------

/// Can this type be converted to the given type?
bool Type::canConvertTo(QString _type)
{
  return convertTypes_.contains (_type) || types_.contains (_type) ;
}

//------------------------------------------------------------------------------
}