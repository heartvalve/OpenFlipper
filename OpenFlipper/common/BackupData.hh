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

#ifndef BACKUPDATA_HH
#define BACKUPDATA_HH

#include <QString>
#include <OpenFlipper/common/perObjectData.hh>
#include <OpenFlipper/common/UpdateType.hh>
#include <OpenFlipper/common/BaseBackup.hh>
#include <OpenFlipper/common/GlobalDefines.hh>

#define OBJECT_BACKUPS "Object Backups"

/**
 * @brief Abstract class that is used to store backups
 *
 * The default implementation can only backup perObjectDatas
 */
class DLLEXPORT BackupData : public PerObjectData
{

  public:
    /// Constructor
    BackupData(BaseObjectData* _object = 0);
    /// Destructor
    virtual ~BackupData();

  public:
    /// store a backup
    void storeBackup(BaseBackup* _backup);

    /// perform an undo if possible
    void undo();
    /// perform a redo if possible
    void redo();

    /// return the id of the next undo backup
    int undoID();
    /// return the id of the next redo backup
    int redoID();
    /// return the id of the current backup state
    int currentID();
    
    /// return the name of the next undo backup
    QString undoName();
    /// return the name of the next redo backup
    QString redoName();

    /// return if an undo backup is available
    bool undoAvailable();
    /// return if a redo backup is available
    bool redoAvailable();

    /// return if an undo backup is blocked
    bool undoBlocked();
    /// return if a redo backup is blocked
    bool redoBlocked();

    /// remove all backups
    void clear();

    /// add links to other backups for the current state
    /// this is used to ensure that the backups are only applied simultaneously
    void setLinks(IdList _objectIDs);

    /// return the current state
    BaseBackup* currentState();

    /// return the maximum of backups which are saved
    size_t maxBackups();

    /// set the maximum of saved backups
    void setMaxBackups(size_t _max);

  protected:
    std::vector< BaseBackup* > undoStates_;
    std::vector< BaseBackup* > redoStates_;
    BaseBackup* currentState_;

    BaseObjectData* object_;

    size_t maxBackups_;
};

#endif //BACKUPDATA_HH
