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

#ifndef VSI_FILENAME_HH
#define VSI_FILENAME_HH

//== INCLUDES =================================================================
#include "parser/type.hh"

//== NAMESPACES ===============================================================
namespace VSI {

//== CLASS DEFINITION =========================================================

/** Filename visual scripting type class
  */

class TypeFilename : public Type {
  public:

    /// Constructor
    TypeFilename ();

    /// Has configuration widget
    bool hasWidget ();

    /// Returns configuration widget
    TypeWidget *widget (QMap<QString, QString> _hints, QString _typeName, QWidget *_parent = NULL);
};

//=============================================================================
}
//=============================================================================

#endif
