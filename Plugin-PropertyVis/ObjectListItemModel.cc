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

#include "ObjectListItemModel.hh"

#include <algorithm>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

ObjectListItemModel::ObjectListItemModel() {
}

ObjectListItemModel::~ObjectListItemModel() {
}

int ObjectListItemModel::rowCount(const QModelIndex & parent) const {
    return objects_.size() + 1;
}

QVariant ObjectListItemModel::data(const QModelIndex & index, int role) const {
    switch (role) {
        case Qt::DisplayRole:
            if (index.row() <= 0) return tr("<none>");
            return objects_[index.row() - 1].getName();
        case Qt::UserRole:
            if (index.row() <= 0) return QVariant::fromValue<int>(-1);
            return QVariant::fromValue(objects_[index.row() - 1].getId());
        default:
            return QVariant::Invalid;
    }
}

void ObjectListItemModel::removeObject(int _id)
{
    beginResetModel();
    for (int i = (int)objects_.size()-1; i>=0; i--)
    {
        if (objects_[i].getId() == _id)
            objects_.erase(objects_.begin()+i);
    }
    endResetModel();
}

void ObjectListItemModel::refresh(const DataType &datatype) {
    std::vector<ObjectInfo> objects;
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, datatype);
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
        objects.push_back(ObjectInfo(o_it->name(), o_it->id()));
    }

    std::sort(objects.begin(), objects.end());

    if (objects != objects_) {
        beginResetModel();
        objects.swap(objects_);
        endResetModel();
    }
}
