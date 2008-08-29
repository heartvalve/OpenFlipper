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
 * \file Types.hh 
 * This File contains the basic available datatypes in the Framework.
 */


#ifndef GROUPOBJECT_HH
#define GROUPOBJECT_HH

 
#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

//== INCLUDES =================================================================

#include <QList>

//== TYPEDEFS =================================================================
   

//== CLASS DEFINITION =========================================================


/** This class provides grouping functionality. Datacontrol normally manages grouping.
 */
class DLLEXPORT GroupObject : public BaseObject {
  public:
    /** constructor
     * 
     * @param _groupName Name of the new Group object
     */
    GroupObject(QString _groupName = "Group" , GroupObject *parent = 0 );
    
    /// destructor
    virtual ~GroupObject();
   
    
    /** @} */ 
};

             
//=============================================================================
#endif // GROUPOBJECT_HH defined
//=============================================================================
