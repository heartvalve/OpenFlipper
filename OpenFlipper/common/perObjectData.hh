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
//  Types
//
//=============================================================================

/**
 * \file perObjectData.hh
 * This File contains a basic data class used to attach data to objects.
 */


#ifndef PEROBJECTDATA_HH
#define PEROBJECTDATA_HH


//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalDefines.hh>

//== TYPEDEFS =================================================================


//== CLASS DEFINITION =========================================================

/** \brief Object Payload
 *
 * This class is used to add arbitrary data to objects
 * in OpenFlipper. You can derive any kind of class from
 * PerObjectData and attach it to an object. See
 * BaseObject::setObjectData() for more details.
 *
 * */
class DLLEXPORT PerObjectData {

  public :
    /// You have to provide your own constructor for your object
    PerObjectData();

    virtual ~PerObjectData();

};


//=============================================================================
#endif // PEROBJECTDATA_HH defined
//=============================================================================
