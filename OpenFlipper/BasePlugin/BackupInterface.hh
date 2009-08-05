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




//
// C++ Interface: Backup Interface
//
// Description: 
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef BACKUPPLUGININTERFACE_HH 
#define BACKUPPLUGININTERFACE_HH 

 #include <QtGui>
 #include <QMenuBar>
 #include <OpenFlipper/common/Types.hh>
 
 /** \brief Create or restore backups 
  * 
  * Interface Class for Backup Plugins. The Backup Interface can be used to communicate with or write a backup plugin.
  * 
 */
class BackupInterface {
   
   signals:
      /** Plugins which support backups should call this function if they want to create backups.\n
       * A Backup control Plugin will do the rest.
       * @param _id Identifier of the object to create the backup
       * @param _name Name of the Backup, to show the user what can be recovered
       */
      virtual void createBackup( int /*_id*/ , QString /*_name*/ ) {};
   
   private slots:
      /** This function will be called if a plugin requests a backup. You can
       * also react on this event.
       * @param _id Identifier of the object to create the backup
       * @param _name Name of the Backup, to show the user what can be recovered
       * @param _backup_id Unique Number of the backup. 
       */
      virtual void slotBackupRequested( int /*_id*/ , QString /*_name*/ , int /*_backup_id*/) {} ;
            
      /** This function will be called if a plugin requests a backup. You can
       * also react on this event.
       * @param _id Identifier of the object to create the backup
       * @param _name Name of the Backup, to show the user what can be recovered
       * @param _backup_id Unique Number of the backup. 
       */
      virtual void slotRestoreRequested( int /*_id*/ , QString /*_name*/ , int /*_backup_id*/) {};      
      
   public : 
       
      /// Destructor
      virtual ~BackupInterface() {};
};

Q_DECLARE_INTERFACE(BackupInterface,"GUI.BackupInterface/0.3")
      
#endif // BACKUPPLUGININTERFACE_HH
