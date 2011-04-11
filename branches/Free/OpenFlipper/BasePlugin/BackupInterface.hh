/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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

#include <QtGui>
#include <OpenFlipper/common/Types.hh>
 
 /** \brief Create or restore backups 
  * 
  * Interface Class for Backup Plugins. The Backup Interface can be used to communicate with or write a backup plugin.
  *
  * See tutorial \ref ex6 for detailed information on how to use this plugin interface.
  * 
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
    * @param _objectid Identifier of the object to create the backup
    * @param _name     Name of the Backup, to show the user what can be recovered
    */
    virtual void createBackup( int /*_objectid*/, QString /*_name*/, UpdateType /*_type*/ = UPDATE_ALL){};

    /** \brief Tell Backup Plugin to create a backup
    *
    * Plugins which supports backups can call this function if they want to create backups.\n
    * A Backup control Plugin will do the rest.
    * @param _objectids Identifier of the object to create the backup
    * @param _name      Name of the Backup, to show the user what can be recovered
    */
    virtual void createBackup( IdList /*_objectids*/, QString /*_name*/, std::vector<UpdateType> /*_types*/){};

    /** \brief Tell Backup Plugin to undo the last action of an object
    *
    * Plugins which supports backups can call this function if they want to restore backups.\n
    * A Backup control Plugin will do the rest.
    * @param _objectid   Identifier of the object to restore
    */
    virtual void undo(int /*_objectid*/) {};

    /** \brief Tell Backup Plugin to redo the last action on an object
    *
    * Plugins which supports backups can call this function if they want to restore backups.\n
    * A Backup control Plugin will do the rest.
    * @param _objectid   Identifier of the object to restore
    */
    virtual void redo(int /*_objectid*/) {};

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
    * @param _id Identifier of the object to create the backup
    * @param _name Name of the Backup, to show the user what can be recovered
    * @param _internalId Unique identifier of the backup. This number is generated by the core and returned by the original signal. 
    */
    virtual void slotCreateBackup( int /*_objectid*/ , QString /*_name*/ , UpdateType /*_type*/ = UPDATE_ALL) {};

    /** \brief Backup for an object requested
    * 
    * This function will be called if a plugin requests a backup. You can
    * also react on this event if you reimplement this function in your plugin.
    * @param _id Identifier of the object to create the backup
    * @param _name Name of the Backup, to show the user what can be recovered
    * @param _internalId Unique identifier of the backup. This number is generated by the core and returned by the original signal. 
    */
    virtual void slotCreateBackup( IdList /*_objectids*/ , QString /*_name*/ , std::vector<UpdateType> /*_types*/) {};

    /** \brief A given object will be restored.
    *
    * This function is called before an object is restored from a backup.
    * perObjectDatas and the object will be reset to the backup state
    * after this function is called for all plugins.
    * If you have any pointers or references to the given object you have to
    * clean them up here.
    * @param _id         Identifier of the object which is about to be restored
    */
    virtual void slotAboutToRestore( int /*_objectid*/, UpdateType /*_type*/ ) {};

    /** \brief Restore Object
    *
    * This function is called after the main object is restored from a backup.
    * perObjectDatas and the object have been reset to the backup state.\n
    *
    * Here you can restore additional data which is not managed in perObjectDatas but
    * in your local plugin.\n
    *
    * @param _objectid   Identifier of the object which is about to be restored
    */
    virtual void slotRestore( int /*_objectid*/, UpdateType /*_type*/ ) {};

    /** \brief Object fully restored
    *
    * This function is called after an object and all data from other plugins
    * is restored from a backup.
    * perObjectDatas and the object have been reset to the backup state.    
    * @param _objectid   Identifier of the object which is about to be restored
    */
    virtual void slotRestored( int /*_objectid*/, UpdateType /*_type*/ ) {};

   public:

    /// Destructor
    virtual ~BackupInterface() {};

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
    virtual void aboutToRestore(int /*_objectid*/ , UpdateType /*_type*/ ) {};
  
    /** \brief Backup Plugin tells other Plugins that they should restore their own data
    *
    */
    virtual void restore(int /*_objectid*/ , UpdateType /*_type*/ ) {};
    
    /** \brief Backup Plugin tells other Plugins that a restore has happened
    *
    */
    virtual void restored( int /*_objectid*/ , UpdateType /*_type*/ ) {};
    
    
  private slots:
    
    
    /** \brief Undo the last action of an object
    *
    * This function has to be implemented in the backup management plugin. Normally
    * this function is provided by the default backup plugin and should not be used!
    * To restore data in your plugin use the slotRestore above.
    *
    * @param _objectid   Identifier of the object to restore
    */
    virtual void slotUndo(int /*_objectid*/) {};

    /** \brief Redo the last action on an object
    *
    * This function has to be implemented in the backup management plugin. Normally
    * this function is provided by the default backup plugin and should not be used!
    * To restore data in your plugin use the slotRestore above.
    *
    * @param _objectid   Identifier of the object to restore
    */
    virtual void slotRedo(int /*_objectid*/) {};

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
     * @param _id Id of the added object
     * @param _type the type of backup that needs to be done
     */
    virtual void generateBackup( int _id, QString _name, UpdateType _type ) {};

};

Q_DECLARE_INTERFACE(BackupInterface,"GUI.BackupInterface/1.0")
      
#endif // BACKUPPLUGININTERFACE_HH
