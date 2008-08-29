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
//   $Revision: 1720 $
//   $Author: moebius $
//   $Date: 2008-05-09 14:15:53 +0200 (Fri, 09 May 2008) $
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


#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

//== INCLUDES =================================================================

//== TYPEDEFS =================================================================


//== CLASS DEFINITION =========================================================

class DLLEXPORT PerObjectData {

  public :
    /// You have to provide your own constructor for your object
    PerObjectData();

    virtual ~PerObjectData();

};


//=============================================================================
#endif // PEROBJECTDATA_HH defined
//=============================================================================
