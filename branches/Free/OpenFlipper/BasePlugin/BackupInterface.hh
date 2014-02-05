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

#ifndef BACKUPPLUGININTERFACE_HH 
#define BACKUPPLUGININTERFACE_HH 


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <OpenFlipper/common/Types.hh>
 
 /** \file BackupInterface.hh
*
* Interface class for backup handling.\ref backupInterfacePage
*/

/** \brief Interface class for backup handling.
  *
  * \ref backupInterfacePage "Detailed description"
  * \n
  *
  * This interface defines functions to implement backup and restore
  * for objects or object groups.
 */
class BackupInterface {
  
  //===========================================================================
  /** @name Interface definition for general Plugins
  * @{ */
  //===========================================================================
  
  signals:

    /** \brief Tell Backup Plugin to create a backup
    *
    * Plugins which supports backups can call this function if they want to create backups.\n
    * A Backup control Plugin will do the rest.
    *
    * @param _objectid Identifier of the object to create the backup
    * @param _name     Name of the Backup, to show the user what can be recovered
    * @param _type     The type of the backup (e.g. UPDATE_SELECTION)
    */
    virtual void createBackup( int _objectid, QString _name, UpdateType _type = UPDATE_ALL){};

    /** \brief Tell Backup Plugin to create a group backup
    *
    * Plugins which supports backups can call this function if they want to create group backups.\n
    * The backups specified here will be grouped together. They can only be reverted as one block and
    * not one by one. they combine backups on multiple objects to a singe backup set.
    *
    * A Backup control Plugin will do the rest.
    *
    * @param _objectids Identifier of the object to create the backup
    * @param _name      Name of the Backup, to show the user what can be recovered
    * @param _types     The types of the backups (e.g. UPDATE_SELECTION)
    */
    virtual void createBackup( IdList _objectids, QString _name, std::vector<UpdateType> _types){};

    /** \brief Tell Backup Plugin to undo the last action of an object
    *
    * Plugins which supports backups can call this function if they want to restore backups.\n
    * A Backup control Plugin will do the rest.
    * @param _objectid   Identifier of the object to restore
    */
    virtual void undo(int _objectid) {};

    /** \brief Tell Backup Plugin to redo the last action on an object
    *
    * Plugins which supports backups can call this function if they want to restore backups.\n
    * A Backup control Plugin will do the rest.
    * @param _objectid   Identifier of the object to restore
    */
    virtual void redo(int _objectid) {};

    /** \brief Tell Backup Plugin to undo the last action
    *
    * Plugins which supports backups can call this function if they want to restore backups.\n
    * A Backup control Plugin will do the rest.
    */
    virtual void undo(){};

    /** \brief Tell Backup Plugin to redo the last action
    *
    * Plugins which supports backups can call this function if they want to restore backups.\n
    * A Backup control Plugin will do the rest.
    */
    virtual void redo(){};

  private slots:

    /** \brief Backup for an object requested
    * 
    * This function will be called if a plugin requests a backup. You can
    * also react on this event if you reimplement this function in your plugin.
    *
    * @param _objectid Identifier of the object to create the backup
    * @param _name Name of the Backup, to show the user what can be recovered
    * @param _type What has been updated (This can be used to restrict the backup to certain parts of the object)
    */
    virtual void slotCreateBackup( int _objectid , QString _name , UpdateType _type = UPDATE_ALL) {};

    /** \brief Backup for an object requested
    * 
    * This function will be called if a plugin requests a backup. You can
    * also react on this event if you reimplement this function in your plugin.
    *
    * @param _objectids Identifiers of the object to create the backup
    * @param _name Name of the Backup, to show the user what can be recovered
    * @param _types What has been updated (This can be used to restrict the backup to certain parts of the objects)
    */
    virtual void slotCreateBackup( IdList _objectids , QString _name , std::vector<UpdateType> _types) {};

    /** \brief A given object will be restored.
    *
    * This function is called before an object is restored from a backup.
    * perObjectDatas and the object will be reset to the backup state
    * after this function is called for all plugins.
    *
    * If you have any pointers or references to the given object you have to
    * clean them up here.
    *
    * @param _objectid         Identifier of the object which is about to be restored
    */
    virtual void slotAboutToRestore( int _objectid ) {};

    /** \brief Object fully restored
    *
    * This function is called after an object and all data from other plugins
    * is restored from a backup.
    *
    * perObjectDatas and the object have been reset to the backup state.
    *
    * @param _objectid   Identifier of the object which is about to be restored
    */
    virtual void slotRestored( int _objectid ) {};

   public:

    /// Destructor
    virtual ~BackupInterface() {};

  /** @} */

  //===========================================================================
  /** @name Interface definition for Backup Plugins
  *
  *  These signals and slots have to be implemented if you create a plugin managing 
  *  Backups ( A Backup plugin is already provided by OpenFlipper so you don't need
  *  to use these funcions).
  * @{ */
  //===========================================================================     
  
  signals: 
    /** \brief Backup Plugin tells other Plugins that a restore will happen
    *
    */
    virtual void aboutToRestore(int _objectid ) {};

    /** \brief Backup Plugin tells other Plugins that a restore has happened
    *
    */
    virtual void restored( int _objectid ) {};
    
    
  private slots:
    
    
    /** \brief Undo the last action of an object
    *
    * This function has to be implemented in the backup management plugin. Normally
    * this function is provided by the default backup plugin and should not be used!
    * To restore data in your plugin use the slotRestore above.
    *
    * @param _objectid   Identifier of the object to restore
    */
    virtual void slotUndo(int _objectid) {};

    /** \brief Redo the last action on an object
    *
    * This function has to be implemented in the backup management plugin. Normally
    * this function is provided by the default backup plugin and should not be used!
    * To restore data in your plugin use the slotRestore above.
    *
    * @param _objectid   Identifier of the object to restore
    */
    virtual void slotRedo(int _objectid) {};

    /** \brief Undo the last action
    *
    * This function has to be implemented in the backup management plugin. Normally
    * this function is provided by the default backup plugin and should not be used!
    * To restore data in your plugin use the slotRestore above.
    *
    */
    virtual void slotUndo(){};

    /** \brief Redo the last action
    *
    * This function has to be implemented in the backup management plugin. Normally
    * this function is provided by the default backup plugin and should not be used!
    * To restore data in your plugin use the slotRestore above.
    *
    */
    virtual void slotRedo(){};

  signals:

    /** \brief This signal is emitted by a BackupPlugin and tells a TypePlugin to generate a backup
     *
     * @param _id   Id of the added object
     * @param _name Name of the backup to generate
     * @param _type the type of backup that needs to be done
     */
    virtual void generateBackup( int _id, QString _name, UpdateType _type ) {};

  /** @} */

};

/** \page backupInterfacePage Backup Interface
 * \n
\image html BackupInterface.png
\n

\section BackupInterfacePage_Overview Overview
The Backup interface can be used to manage backups within OpenFlipper. It provides abstractions for backup and
redo operations which get managed by the backup plugin.

\section backupInterfacePage_generatingBackups Generating Backups
To generate a backup of a specific object, you just have to derive from the BackupInterface and emit the
BackupInterface::createBackup() signal. A simple example looks like this:

\code
emit createBackup(object->id(),"Smoothing", UPDATE_GEOMETRY );
\endcode

We create a backup of the object with the given id. The Backup will be named "Smoothing" and the backup
system is informed that only the geometry has changed.

It is also possible to create backup groups. This is required, if you change several objects at once and
they should be restored only together.

\section backupInterfacePage_restoringBackups Restoring Backups
Backups can be restored and re-applied. You can simply emit the BackupInterface::undo() signal and the
last backup will be restored. BackupInterface::redo() will re apply the last operation stored. It is
also possible to restore backups of a specific object via BackupInterface::undo(int) and
BackupInterface::redo(int), if they are not blocked by a group backup which can only be reverted as one
operation.\n

The other plugins will be informed about a restore operation by BackupInterface::slotAboutToRestore(int)
and BackupInterface::slotRestored(int) which will be emitted directly before and after the restore operation.


\section backupInterfacePage_usage Usage
To use the BackupInterface:
<ul>
<li> include BackupInterface.hh in your plugins header file
<li> derive your plugin from the class BackupInterface
<li> add Q_INTERFACES(BackupInterface) to your plugin class
<li> And implement the required slots and functions
</ul>

*/


Q_DECLARE_INTERFACE(BackupInterface,"GUI.BackupInterface/1.1")
      
#endif // BACKUPPLUGININTERFACE_HH
