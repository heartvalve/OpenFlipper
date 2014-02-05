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
*   $Revision: 11147 $                                                       *
*   $LastChangedBy: kremer $                                                 *
*   $Date: 2011-03-16 16:01:09 +0100 (Wed, 16 Mar 2011) $                    *
*                                                                            *
\*===========================================================================*/

#ifndef INFORMATIONINTERFACE_HH_
#define INFORMATIONINTERFACE_HH_

#include <OpenFlipper/common/Types.hh>

/** \file InformationInterface.hh
*
* Interface that provides means to
* get information on specific data types in OpenFlipper. \ref informationInterfacePage
*/


 /** \brief Interface class for providing information on objects
 *
 * \n
 * \ref informationInterfacePage "Detailed description"
 * \n
 *
 */

class InformationInterface {

public:
    /// Default destructor
    virtual ~InformationInterface() {};

public slots:

   /**  \brief Get data type for information requests
     *
     *  This slot is called by the core in order to identify
     *  the data types for which the plugin provides information.
     *
     *  @return The supported data types (OR'ed in case of multiple types)
     */
    virtual DataType supportedDataTypes() = 0;


    /**  \brief Show information dialog on clicked object
      *
      *  This slot is called each time the user clicks on an object.
      *  The information plugin for a specific type should display
      *  the necessary information on all entity types and the clicked
      *  object itself.
      *
      *  @param _clickedPoint The 2D point in screen coords, which the user has clicked
      *  @param _type         Data type to be registered
    */
    virtual void slotInformationRequested(const QPoint _clickedPoint, DataType _type) = 0;
};

/** \page informationInterfacePage Information Interface
\n
\image html informationInterface.png
\n

Click InformationInterface for a detailed overview of all provided slots
in the interface.

Derive from the information interface in order to provide information
on objects of a specific data type. To clarify this, we will give a short
example. Assuming we want to provide information on all mesh objects
(namely objects of type DATA_TRIANGLE_MESH and DATA_POLY_MESH).
In our plugin, we first have to override the slot that fetches the
supported data types:

\code
DataType MyInfoPlugin::supportedDataTypes() {

    return DataType(DATA_TRIANGLE_MESH | DATA_POLY_MESH);
}
\endcode

It's as simple as that. Now, each time an object is clicked in
identifier mode, slot slotInformationRequested() is called.
We ovveride this slot in order to do the actual picking
and displaying the gathered information in a dialog box:

\code
void MyInfoPlugin::slotInformationRequested(const QPoint _clickedPoint, const DataType _type) {

    // We only respond to requests for our data type
    if((_type != DATA_TRIANGLE_MESH) && (_type != DATA_POLY_MESH)) return;

    // Do the picking...

    // Display dialog box with all the information...
}
\endcode

*/

Q_DECLARE_INTERFACE(InformationInterface,"OpenFlipper.InformationInterface/1.0")

#endif /* INFORMATIONINTERFACE_HH_ */
