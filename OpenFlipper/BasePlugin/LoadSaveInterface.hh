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

#ifndef LOADSAVEINTERFACE_HH
#define LOADSAVEINTERFACE_HH

#include <OpenFlipper/common/Types.hh>

/** \file LoadSaveInterface.hh
*
* Interface for adding and removing objects.\ref loadSaveInterfacePage
*/

/** \brief Interface for all plugins which want to Load or Save files and create Objects
  *
  * \ref loadSaveInterfacePage "Detailed description"
  * \n
  *
  * Using this interface you can instruct the core to open/save objects or
  * to create new empty objects.
 */
class LoadSaveInterface {

   public:

      /// Destructor
      virtual ~LoadSaveInterface() {};

   signals :
      /** \brief Save object to a file
       *
       * @param _id Object to Save
       * @param _filename Filename to save it to (Leave as "" to automatically determine filename)
      */
      virtual void save(int _id , QString _filename ) {}

      /** \brief Load object from file with a specific DataType
       *
       * @param _filename Filename of the File to load
       * @param _type Type to be loaded
       * @param _id Here the id of the loaded object is returned. id is -1 something went wrong
       */
      virtual void load(QString _filename, DataType _type, int& _id) {};


      /** Add an empty object of the given type
       *
       * @param _type Type to be created
       * @param _id Here the id of the loaded object is returned. id is -1 something went wrong
       */
      virtual void addEmptyObject( DataType _type, int& _id) {};

      /** Create a copy of an existing object
       *
       * @param _oldId id of the object to copy
       * @param _newId id of the new object created
       */
      virtual void copyObject( int _oldId, int& _newId) {};


      /** \brief DEPRECATED HERE (Moved to Type Interface)! Emit this signal if an empty object has been created
       *
       * Deprecated! Objects are added by the Type Plugins and this is signal is in the TypeInterface now
       *
       * @param _id Id of the added object
       */
      virtual void emptyObjectAdded( int _id ) {};

      /** \brief Delete an object
       *
       * @param _id Id of the object
       */
      virtual void deleteObject( int _id ) {};

      /** \brief Delete all Objects
       *
       */
      virtual void deleteAllObjects() {};

      /** \brief Get all file filters that are registered
       *
       */
      virtual void getAllFileFilters(QStringList& _filters) {};

  private slots :

    /**  \brief A file has been opened
     *
     *  This slot is called if a file has been opened by the core.\n
     *  @param _id Id of the new object
     */
    virtual void fileOpened( int _id ) {};

    /**  \brief An empty object has been added
     *
     *  This slot is called if an empty object has been added by the core.\n
     *  @param _id Id of the new object
     */
    virtual void addedEmptyObject( int _id ) {};

    /** \brief An object was deleted
      *
      * This function is called by the core if an object gets deleted. It is called immediately
      * before the object is removed from the scenegraph. So if this function is invoked, the object still
      * exists. All plugins get informed via this slot.\
      *
      * After this function got called for all plugins, the object is removed from the scene with all
      * nodes attached to it and all PerObjectData attached to it.
      *
      * @param _id Id of the object that is deleted.
      */
    virtual void objectDeleted( int _id ){};

};

/** \page loadSaveInterfacePage Load/Save Interface
 * \n
\image html loadSaveInterface.png
\n

The LoadSaveInterface can be used by plugins to add new objects to the scene either by creating empty objects
or by loading them from files. The interface also triggers saving of existing objects to files.


\section loadSaveInterface_load_save Load/Save Files
You can load a file from within your plugin via
\code
  emit LoadSaveInterface::load(QString _filename, DataType _type, int& _id);
\endcode
and to save an object to a file, call LoadSaveInterface::
\code
  emit LoadSaveInterface::save(int _id , QString _filename );
\endcode

The corresponding File Plugin will than be activated and load or save the object. for load, the id of the new object
is returned.

\section loadSaveInterface_createObjects Creating/Copying Objects
To add objects to the scene you can use the following two functions:
\code
  emit LoadSaveInterface::addEmptyObject( DataType _type, int& _id);
\endcode
This will create a new object of the given DataType and return the id of it, while
\code
  emit LoadSaveInterface::copyObject( int _oldId, int& _newId);
\endcode
will copy the given object and return the new object id.

\section loadSaveInterface_deleteObjects Deleting Objects

You can delete a specific object via
\code
  emit LoadSaveInterface::deleteObject( int _id );
\endcode
or clear the whole scene with:
\code
  emit LoadSaveInterface::deleteAllObjects();
\endcode

\section loadSaveInterface_callbacks Object Information
Additionally the interface informs plugins if new objects have been added to the scenes or if previous objects
have been removed.
LoadSaveInterface::fileOpened( int _id ) will be triggered each time an object is loaded from a file. If simply an
empty object has been added, the slot LoadSaveInterface::addedEmptyObject( int _id ) will be called. If an object
gets deleted, the LoadSaveInterface::objectDeleted( int _id ) will be called immediately before the object is removed
from the scenegraph. So if this function is invoked, the object still exists. All plugins get informed via this slot.

If you want to know if the whole scene got cleared, use BaseInterface::slotAllCleared( ).

\section loadSaveInterface_usage Usage
To use the LoadSaveInterface:
<ul>
<li> include LoadSaveInterface.hh in your plugins header file
<li> derive your plugin from the class LoadSaveInterface
<li> add Q_INTERFACES(LoadSaveInterface) to your plugin class
<li> And add the signals or slots you want to use to your plugin class (You don't need to implement all of them)
</ul>

*/


Q_DECLARE_INTERFACE(LoadSaveInterface,"OpenFlipper.LoadSaveInterface/1.1")

#endif // LOADSAVEINTERFACE_HH
