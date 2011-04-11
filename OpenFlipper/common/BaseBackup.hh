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
*   $Revision: 10745 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-01-26 10:23:50 +0100 (Wed, 26 Jan 2011) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef BASEBACKUP_HH
#define BASEBACKUP_HH

#include <QString>
#include <OpenFlipper/common/BaseObjectData.hh>
#include <OpenFlipper/common/UpdateType.hh>
#include <OpenFlipper/common/perObjectData.hh>

/**
 * @brief Class that encapsulates a backup
 */
class BaseBackup
{

  public:
    BaseBackup(QString _name);
    BaseBackup(BaseObjectData* _object, QString _name, UpdateType _type);
    virtual ~BaseBackup();

  public:
    virtual void apply();
    QString name();
    void setLinks(IdList _objectIDs);
    int id();

    bool blocked();
    
  protected:
    //backup of the perObjectData objects
    std::vector< std::pair<QString, PerObjectData*> > objectDatas_;

    BaseObjectData*  object_;
    QString          name_;
    IdList           links_;

    int              id_;
};

#endif //BASEBACKUP_HH
