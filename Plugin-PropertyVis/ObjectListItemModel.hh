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

#ifndef OBJECTLISTITEMMODEL_HH_
#define OBJECTLISTITEMMODEL_HH_

#include <QAbstractItemModel>
#include <OpenFlipper/common/DataTypes.hh>

class ObjectListItemModel: public QAbstractListModel {
    public:
        ObjectListItemModel();
        virtual ~ObjectListItemModel();

        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

        void removeObject(int _id);

        void refresh(const DataType &datatype);

    protected:
        class ObjectInfo {
            public:
                ObjectInfo(const QString &name, const int id) : name(name), id(id) {}

                bool operator<(const ObjectInfo &rhs) const {
                    const int nameCmp = name.compare(rhs.name);
                    if (!nameCmp) return id < rhs.id;
                    return nameCmp < 0;
                }

                bool operator==(const ObjectInfo &rhs) const {
                    return id == rhs.id && name == rhs.name;
                }

                const QString &getName() const { return name; }
                int getId() const { return id; }

            private:
                QString name;
                int id;
        };

        std::vector<ObjectInfo> objects_;
};

#endif /* OBJECTLISTITEMMODEL_HH_ */
