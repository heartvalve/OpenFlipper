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

#ifndef BASEBACKUP_HH
#define BASEBACKUP_HH

#include <QString>
#include <OpenFlipper/common/BaseObjectData.hh>
#include <OpenFlipper/common/UpdateType.hh>
#include <OpenFlipper/common/perObjectData.hh>
#include <OpenFlipper/common/GlobalDefines.hh>

/**
 * @brief Class that encapsulates a backup
 */
class DLLEXPORT BaseBackup
{

  public:
    BaseBackup(QString _name);

    /** \brief constructor
     *
     * @param _object Pointer to the object, this backup will belong to
     * @param _name   Name of the object
     * @param _type   Type of change which can be used to create more specific backups (e.g. selection only)
     *
     */
    BaseBackup(BaseObjectData* _object, QString _name, UpdateType _type);

    virtual ~BaseBackup();

  public:

    /** \brief Revert this backup
     *
     * This function has to be implemented. It will be called when the backup should be restored.
     *
     * \note You have to call the BaseBackup::apply() first in your implementation of this function.
     *
     * You have to restore your backup to the corresponding object when this function gets called.
     */
    virtual void apply();

    /** \brief Get the backups name)
     *
     */
    QString name();


    /** \brief Set links to corresponding backups
     *
     * Backups can be grouped together. The backups store the links to their group members via
     * this function.
     */
    void setLinks(IdList _objectIDs);

    /** \brief get id of this backup
     *
     * Automatically generated id for this backup.
     */
    int id();

    /** \brief Returns if this backup is blocked
     *
     * When Backups are grouped together, they can not be reversed o their own but
     * oly together with the others in that backup group. This flag indicates, that
     * the backup is part of a group and can not be reversed on its own.
     */
    bool blocked();
    
  protected:
    /// Backup of the perObjectData objects
    std::vector< std::pair<QString, PerObjectData*> > objectDatas_;

    BaseObjectData*  object_;
    QString          name_;
    IdList           links_;

    int              id_;
};

#endif //BASEBACKUP_HH
