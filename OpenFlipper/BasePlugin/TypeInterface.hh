/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
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


#ifndef TYPEINTERFACE_HH
#define TYPEINTERFACE_HH

#include <OpenFlipper/common/Types.hh>

 /** \brief Interface class for type definitions
  *
  * This interface is used to register new types in OpenFlipper. The type plugins are loaded before all other plugins.
  * They have only the registerType function which registers the type to the core. The type itself has to be
  * defined in the ObjectTypes subdirectory.
  *
  * \section TypeExample Example using custom data types
  *
  * Adding a custom data type to %OpenFlipper needs the following requirements in order to work:
  *
  * - The definition of the typeId constant, e.g.:
  * \code
  * #define DATA_MY_DATA typeId("MyDataType")
  * \endcode
  * Note: Your data type is then referenced as DATA_MY_DATA during runtime.
  * - The specification of an object class for your object type that is derived from
  *   BaseObjectData.
  * - The specification of helper functions (usually within the PluginFunctions namespace)
  *   allowing the casting from BaseObjectData to your object type class.
  *
  * See detailed examples for each of the three points for already existing data types in
  * OpenFlipperRoot/ObjectTypes.
  *
  * Once the object class is specified, the type plugin will be responsible for its handling including
  *
  * - Adding new objects to the scenegraph
  * - Setting the initial name of an object
  * - Etc.
  *
  * So, type plugins usually consist of only few lines of code. Here an example of
  * a type plugin handling an example object data type as mentioned above:
  *
  * \code
  * bool MyDataTypePlugin::registerType() {
  *
  *     addDataType("MyDataType",tr("MyDataType"));
  *     setTypeIcon( "MyDataType", "myDataType.png");
  *
  *     return true;
  * }
  *
  * int MyDataTypePlugin::addEmpty() {
  *
  *     // Create new object
  *     MyObject* object = new MyObject();
  *
  *     object->setName( QString("My Object %1.mob").arg(objectCount) );
  *     object->update();
  *     object->show();
  *
  *     // Tell core that an object has been added
  *     emit emptyObjectAdded ( object->id() );
  *
  *     return object->id();
  * }
  * \endcode
  *
  * Now, each time a plugin emits addEmptyObject(DATA_MY_DATA), the addEmpty() function will
  * add the object to the scenegraph and return the newly created object's id.
  */

class TypeInterface {

  signals:
    
    /** \brief Emit this signal if an empty object has been created
       * @param _id Id of the added object
       */
      virtual void emptyObjectAdded( int /*_id*/ ) {};
  
  public:

    /// Destructor
    virtual ~TypeInterface() {};
    
  public slots:

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
