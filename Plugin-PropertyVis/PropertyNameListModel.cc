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

#include "PropertyNameListModel.hh"

#include <OpenMesh/Core/Utils/Property.hh>
#include <ACG/Math/VectorT.hh>

#include <map>
#include <vector>

const PropertyNameListModel::TypeInfoWrapper PropertyNameListModel::proptype_bool =
        PropertyNameListModel::TypeInfoWrapper(typeid(OpenMesh::PropertyT<bool>), "bool");
const PropertyNameListModel::TypeInfoWrapper PropertyNameListModel::proptype_int =
        PropertyNameListModel::TypeInfoWrapper(typeid(OpenMesh::PropertyT<int>), "int");
const PropertyNameListModel::TypeInfoWrapper PropertyNameListModel::proptype_uint =
        PropertyNameListModel::TypeInfoWrapper(typeid(OpenMesh::PropertyT<unsigned int>), "unsigned int");
const PropertyNameListModel::TypeInfoWrapper PropertyNameListModel::proptype_double =
        PropertyNameListModel::TypeInfoWrapper(typeid(OpenMesh::PropertyT<double>), "double");
const PropertyNameListModel::TypeInfoWrapper PropertyNameListModel::proptype_Vec3d =
        PropertyNameListModel::TypeInfoWrapper(typeid(OpenMesh::PropertyT<ACG::Vec3d>), "Vec3d");
const PropertyNameListModel::TypeInfoWrapper PropertyNameListModel::proptype_Vec3f =
        PropertyNameListModel::TypeInfoWrapper(typeid(OpenMesh::PropertyT<ACG::Vec3f>), "Vec3f");

#ifdef ENABLE_SKELETON_SUPPORT
  #include <ObjectTypes/Skeleton/BaseSkin.hh>
  const PropertyNameListModel::TypeInfoWrapper PropertyNameListModel::proptype_SkinWeights =
        PropertyNameListModel::TypeInfoWrapper(typeid(OpenMesh::PropertyT<BaseSkin::SkinWeights>), "SkinWeights");
#endif

/*
 * I'd love to do this with boost::assign but I'm not allowed to. :-(
 */

const PropertyNameListModel::TypeInfoWrapper PropertyNameListModel::prop_types[] = {
                                                              proptype_bool,
                                                              proptype_int,
                                                              proptype_uint,
                                                              proptype_double,
                                                              proptype_Vec3d,
                                                              proptype_Vec3f,
#ifdef ENABLE_SKELETON_SUPPORT
                                                              proptype_SkinWeights,
#endif
};

#ifdef ENABLE_SKELETON_SUPPORT
const PropertyNameListModel::TYPE_INFO_SET PropertyNameListModel::sane_prop_types(prop_types, prop_types + 7);
#else
const PropertyNameListModel::TYPE_INFO_SET PropertyNameListModel::sane_prop_types(prop_types, prop_types + 6);
#endif

const char *PropertyNameListModel::entity2str(ENTITY_FILTER entity) {
    switch (entity) {
        case EF_EDGE:
            return "→";
        case EF_FACE:
            return "△";
        case EF_HALFEDGE:
            return "⇀";
        case EF_VERTEX:
            return "•";
        default:
            return "error";
    }
}


PropertyNameListModel::PropertyNameListModel(QObject *parent) :
    QAbstractListModel(parent) {
}

PropertyNameListModel::~PropertyNameListModel() {
}

int PropertyNameListModel::rowCount(const QModelIndex & parent) const {
    return propList_.size();
}

QVariant PropertyNameListModel::data(const QModelIndex & index, int role) const {
    switch (role) {
        case Qt::DisplayRole:
            return propList_[index.row()].toString();
        default:
            return QVariant::Invalid;
    }
}

QVariant PropertyNameListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    switch (role) {
        case Qt::DisplayRole:
            return tr("Some header. %1 %2").arg(section).arg(orientation);
            break;
        default:
            return QAbstractListModel::headerData(section, orientation, role);
    }
}

namespace {
class InsDel {
    public:
        enum OP { INS, DEL };
        InsDel(OP op, int first, int count, int before) : op(op), first(first), count(count), before(before) {}
        OP op;
        int first, count, before;
};
}

bool PropertyNameListModel::tryInsertionsDeletions(std::vector<PROP_INFO> &propList) {

    std::vector<InsDel> result;
    typedef std::vector<PROP_INFO>::iterator IT;
    int correction = 0;
    IT oldIt, newIt;
    for (oldIt = propList_.begin(), newIt = propList.begin(); oldIt < propList_.end() && newIt < propList.end(); ++oldIt, ++newIt) {
        if (*oldIt == *newIt) continue;
        const IT nextNew = std::find(newIt+1, propList.end(), *oldIt);
        if (nextNew != propList.end()) {
            const int count = std::distance(newIt, nextNew);
            result.push_back(InsDel(InsDel::INS, std::distance(propList.begin(), newIt), count, std::distance(propList_.begin(), oldIt) + correction));
            correction += count;
            newIt += count;
            continue;
        }
        const IT nextOld = std::find(oldIt+1, propList_.end(), *newIt);
        if (nextOld != propList_.end()) {
            const int count = std::distance(oldIt, nextOld);
            result.push_back(InsDel(InsDel::DEL, std::distance(propList_.begin(), oldIt) + correction, count, 0));
            correction -= count;
            oldIt += count;
            continue;
        }
        return false;
    }
    if (oldIt < propList_.end() && newIt < propList.end()) {
        return false;
    }

    if (oldIt < propList_.end())
        result.push_back(InsDel(InsDel::DEL, std::distance(propList_.begin(), oldIt) + correction, std::distance(oldIt, propList_.end()), 0));
    if (newIt < propList.end())
        result.push_back(InsDel(InsDel::INS, std::distance(propList.begin(), newIt), std::distance(newIt, propList.end()), propList_.size() + correction));

    for (std::vector<InsDel>::iterator it = result.begin(); it != result.end(); ++it) {
        if (it->op == InsDel::INS) {
            beginInsertRows(QModelIndex(), it->before, it->before + it->count - 1);
            propList_.insert(propList_.begin() + it->before, propList.begin() + it->first, propList.begin() + it->first + it->count);
            endInsertRows();
        } else {
            beginRemoveRows(QModelIndex(), it->first, it->first + it->count - 1);
            propList_.erase(propList_.begin() + it->first, propList_.begin() + it->first + it->count);
            endRemoveRows();
        }
    }

    if (propList_ != propList) {
        std::cerr << "Apparently, the function PropertyNameListModel::tryInsertionsDeletions() has an implementation error." << std::endl;
        throw std::logic_error("Apparently, the function PropertyNameListModel::tryInsertionsDeletions() has an implementation error.");
    }

    return true;
}
