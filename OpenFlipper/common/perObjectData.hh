/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




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
    
    /** \brief Copy Constructor
    * You have to reimplement this copy constructor for your object to get copied by the core. Otherwise, 
    * per Object data will not be copied when the core creates a copy of your object.
    */
    PerObjectData(const PerObjectData& _b );

    virtual ~PerObjectData();
};


//=============================================================================
#endif // PEROBJECTDATA_HH defined
//=============================================================================
