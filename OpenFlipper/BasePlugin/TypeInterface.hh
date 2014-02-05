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


#ifndef TYPEINTERFACE_HH
#define TYPEINTERFACE_HH

#include <OpenFlipper/common/Types.hh>

/** \file TypeInterface.hh
*
* Interface for registering types in OpenFlipper. \ref typeInterfacePage
*/


 /** \brief Interface class for type definitions
 *
 * \n
 * \ref typeInterfacePage "Detailed description"
 * \n
 *
 * This interface is used to register new types in OpenFlipper. The type plugins are loaded before all other plugins.
 * They have only the registerType function which registers the type to the core and a function to add new
 * objects at runtime.
 */

class TypeInterface {

  signals:

    /** \brief Emit this signal if an empty object has been created
     *
     * @param _id Id of the added object
     */
    virtual void emptyObjectAdded( int _id ) {};

  public:

    /// Destructor
    virtual ~TypeInterface() {};

  public slots:

    virtual bool registerType() = 0;
    
    /** \brief Create an empty object
       *
       * When this slot is called you have to create an object of your supported type.
       * @return Id of the new Object
       */
    virtual int addEmpty() = 0;
    
    /** \brief Return your supported object type( e.g. DATA_TRIANGLE_MESH )
    *
    * The function is used from addEmpty in the core to check if your plugin can create an object of
    * a given dataType. If so, your addEmpty function will be invoked to create it.
    */
    virtual DataType supportedType() = 0;

    /** \brief This slot should be implemented in a TypePlugin to generate type specific backups
     *
     * @param _id Id of the added object
     * @param _name name of the backup
     * @param _type the type of backup that needs to be done
     */
    virtual void generateBackup( int _id, QString _name, UpdateType _type ){};

};


/** \page typeInterfacePage Type Interface
\n
\image html TypeInterface.png
\n

This interface is used to register new types in OpenFlipper. The type plugins are loaded before all other plugins.
They have only the registerType function which registers the type to the core and a function to create objects
of the new type. The type itself has to be defined in the ObjectTypes subdirectory.

 \section TypeExample Example using custom data types

 Adding a custom data type to %OpenFlipper needs the following requirements in order to work:

 - The definition of the typeId constant, e.g.:
 \code
 #define DATA_MY_DATA typeId("MyDataType")
 \endcode
 Note: Your data type is then referenced as DATA_MY_DATA during runtime.
 - The specification of an object class for your object type that is derived from
   BaseObjectData.
 - The specification of helper functions (usually within the PluginFunctions namespace)
   allowing the casting from BaseObjectData to your object type class.

 See detailed examples for each of the three points for already existing data types in
 OpenFlipperRoot/ObjectTypes.

 Once the object class is specified, the type plugin will be responsible for its handling including

 - Adding new objects to the scenegraph
 - Setting the initial name of an object
 - Etc.

 So, type plugins usually consist of only few lines of code. Here an example of
 a type plugin handling an example object data type as mentioned above:

 \code
 bool MyDataTypePlugin::registerType() {

     addDataType("MyDataType",tr("MyDataType"));
     setTypeIcon( "MyDataType", "myDataType.png");

     return true;
 }

 int MyDataTypePlugin::addEmpty() {

     // Create new object
     MyObject* object = new MyObject();

     object->setName( QString("My Object %1.mob").arg(objectCount) );
     object->update();
     object->show();

     // Tell core that an object has been added
     emit emptyObjectAdded ( object->id() );

     return object->id();
 }
 \endcode

 Now, each time a plugin emits addEmptyObject(DATA_MY_DATA), the addEmpty() function will
 add the object to the scenegraph and return the newly created object's id.

 \section Backups
 Backups are very specific to the underlying data structures of certain types. Therefore the type
 plugins also manage backups. Backups itself are managed by perObjectData objects based on
 the BackupData class.
 When the slot gets called, we first have to check, if an BackupData is already available. If not,
 we create one. Than the backup is generated and passed to the backup data attached to the object.

 You have to derive your backups from the BaseBackup class, where you only need to implement the
 apply function and your constructor. In the following example, this class is named TriMeshBackup.

 \code
 void TypeTriangleMeshPlugin::generateBackup( int _id, QString _name, UpdateType _type ){

  // Get the object corresponding to the id
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_id, object);
  TriMeshObject* meshObj = PluginFunctions::triMeshObject(object);

  // Safety check
  if ( meshObj != 0 ){

    //get backup object data
    BackupData* backupData = 0;

    // If a backup data has already been attached, get it, otherwise create it.
    if ( object->hasObjectData( OBJECT_BACKUPS ) )
      backupData = dynamic_cast< BackupData* >(object->objectData(OBJECT_BACKUPS));
    else{
      //add backup data
      backupData = new BackupData(object);
      object->setObjectData(OBJECT_BACKUPS, backupData);
    }

    // Create a new backup
    TriMeshBackup* backup = new TriMeshBackup(meshObj, _name, _type);

    // Store it in the backup data
    backupData->storeBackup( backup );
  }
 }
 \endcode

To use the TypeInterface:
<ul>
<li> include TypeInterface.hh in your plugins header file
<li> derive your plugin from the class TypeInterface
<li> add Q_INTERFACES(TypeInterface) to your plugin class
<li> Implement all slots of this Interface
</ul>

*/





Q_DECLARE_INTERFACE(TypeInterface,"OpenFlipper.TypeInterface/1.1")

#endif // TYPEINTERFACE_HH
