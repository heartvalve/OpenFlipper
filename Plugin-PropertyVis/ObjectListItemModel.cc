/*
 * ObjectListItemModel.cc
 *
 *  Created on: Feb 29, 2012
 *      Author: ebke
 */

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
