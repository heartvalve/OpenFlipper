/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file Types.hh
 * This File contains the basic available datatypes in the Framework.
 */


#ifndef GROUPOBJECT_HH
#define GROUPOBJECT_HH


//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalDefines.hh>
#include <OpenFlipper/common/BaseObject.hh>
#include <QList>

//== TYPEDEFS =================================================================


//== CLASS DEFINITION =========================================================


/** This class provides grouping functionality. Datacontrol normally manages grouping.
 */
class DLLEXPORT GroupObject : public BaseObject {
  public:

    /** copy constructor
     *
     */
    GroupObject(const GroupObject& _object );


    /** constructor
     *
     * @param _groupName Name of the new Group object
     * @param _parent    The parent object of this object
     */
    GroupObject(QString _groupName = "Group" , GroupObject *_parent = 0 );

    /// destructor
    virtual ~GroupObject();

    /** return a full copy of this object. The object will not be a part of the object tree.
     *  This has to be done with the setParent() function.
     */
    BaseObject* copy();


    /** @} */
};


//=============================================================================
#endif // GROUPOBJECT_HH defined
//=============================================================================
