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
 *   $Revision: 7676 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-11-30 12:47:58 +0100 (Mo, 30. Nov 2009) $                   *
 *                                                                           *
\*===========================================================================*/


#ifndef TYPEINTERFACE_HH
#define TYPEINTERFACE_HH

#include <OpenFlipper/common/Types.hh>

 /** \brief Interface class for type definitions
  *
  * This interface is used to register new types in OpenFlipper. The type plugins are loaded before all other plugins.
  * They have only the registerType function which registers the type to the core. The type itself has to be
  * defined in the ObjectTypes subdirectory.
 */

class TypeInterface {

  public:

    /// Destructor
    virtual ~TypeInterface() {};  

    virtual bool registerType() = 0;
    
    /** \brief Create an empty object
       *
       * When this slot is called you have to create an object of your supported type.
       * @param _type Data type of object that will be created
       * @return Id of the new Object
       */
    virtual int addEmpty() = 0;
    
    /** \brief Return your supported object type( e.g. DATA_TRIANGLE_MESH )
    *
    * If you support multiple datatypes you can use the bitwise or to combine them here.
    * The function is used from addEmpty to check if your plugin can create an object of
    * a given dataType.
    */
    virtual DataType supportedType() = 0;
};

Q_DECLARE_INTERFACE(TypeInterface,"OpenFlipper.TypeInterface/1.0")

#endif // TYPEINTERFACE_HH
