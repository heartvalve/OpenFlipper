/*
 * PropertyNameListModel.hh
 *
 *  Created on: Feb 29, 2012
 *      Author: ebke
 */

#ifndef PROPERTYNAMELISTMODEL_HH_
#define PROPERTYNAMELISTMODEL_HH_

#include <OpenMesh/Core/Utils/BaseProperty.hh>

#include <QAbstractListModel>

#include <set>
#include <vector>
#include <algorithm>

class PropertyNameListModel: public QAbstractListModel {
    public:
        enum ENTITY_FILTER {
            EF_ANY      = 0x0F,
            EF_FACE     = 0x01,
            EF_EDGE     = 0x02,
            EF_HALFEDGE = 0x04,
            EF_VERTEX   = 0x08,
        };

        static const char *entity2str(ENTITY_FILTER entity);

        class TypeInfoWrapper {
            public:
                TypeInfoWrapper(const std::type_info & ti, const char *friendlyName) : ti(&ti), friendlyName(friendlyName) {}
                TypeInfoWrapper(const std::type_info & ti) : ti(&ti),friendlyName("") {}

                operator const std::type_info *() const { return ti; }
                operator const std::type_info &() const { return *ti; }
                operator const char *() const { return friendlyName; }

                const std::type_info *operator->() const { return ti; }
                const std::type_info &get() const { return *ti; }
                const char *getName() const { return friendlyName; }

                bool operator==(const TypeInfoWrapper & other) const {
                    /*
                     * We compare name strings, not the type ids themselves because
                     * the same type from different SOs will give different type ids.
                     */
                    return strcmp(ti->name(), other.ti->name()) == 0;
                }

                bool operator<(const TypeInfoWrapper & other) const {
                    return strcmp(ti->name(), other.ti->name()) < 0;
                }

            private:
                const std::type_info *ti;
                const char *friendlyName;
        };

        class PROP_INFO {
            public:
                PROP_INFO(const std::string &propName, const TypeInfoWrapper &typeinfo, ENTITY_FILTER entity) :
                    propName_(propName), typeinfo_(typeinfo), entity(entity) {}

                QString toString() const {
                    return tr("%3 %1 : %2").arg(propName_.c_str()).arg(friendlyTypeName()).arg(QString::fromUtf8(entity2str(entity)));
                }

                bool operator==(const PROP_INFO &rhs) const {
                    return propName_ == rhs.propName_ && typeinfo_ == rhs.typeinfo_ && entity == rhs.entity;
                }

                bool operator<(const PROP_INFO &rhs) const {
                    if (entity != rhs.entity) return entity < rhs.entity;

                    int result = propName_.compare(rhs.propName_);
                    if (result) return result < 0;

                    return typeinfo_ < rhs.typeinfo_;
                }

                inline bool isFaceProp() const { return entity == EF_FACE; }
                inline bool isEdgeProp() const { return entity == EF_EDGE; }
                inline bool isHalfedgeProp() const { return entity == EF_HALFEDGE; }
                inline bool isVertexProp() const { return entity == EF_VERTEX; }

                inline const std::string &propName() const { return propName_; }
                inline const char *friendlyTypeName() const { return typeinfo_.getName(); }
                inline const TypeInfoWrapper &typeinfo() const { return typeinfo_; }
                inline ENTITY_FILTER entityType() const { return entity; }

            private:
                std::string propName_;
                TypeInfoWrapper typeinfo_;
                ENTITY_FILTER entity;
        };

    public:
        PropertyNameListModel(QObject *parent = 0);
        virtual ~PropertyNameListModel();

        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        /**
         * @param typeIdFilter If NULL, do not filter by type.
         */
        template<typename MeshT>
        void refresh(MeshT *mesh, ENTITY_FILTER entityFilterMask, std::type_info *typeIdFilter) {
            std::vector<PROP_INFO> propList;

            if (mesh) {
                if (entityFilterMask & EF_FACE)
                    gatherProperties(mesh, mesh->fprops_begin(), mesh->fprops_end(), typeIdFilter, std::back_inserter(propList), EF_FACE);

                if (entityFilterMask & EF_EDGE)
                    gatherProperties(mesh, mesh->eprops_begin(), mesh->eprops_end(), typeIdFilter, std::back_inserter(propList), EF_EDGE);

                if (entityFilterMask & EF_HALFEDGE)
                    gatherProperties(mesh, mesh->hprops_begin(), mesh->hprops_end(), typeIdFilter, std::back_inserter(propList), EF_HALFEDGE);

                if (entityFilterMask & EF_VERTEX)
                    gatherProperties(mesh, mesh->vprops_begin(), mesh->vprops_end(), typeIdFilter, std::back_inserter(propList), EF_VERTEX);
            }

            std::sort(propList.begin(), propList.end());

            if (propList != propList_) {
                if (!tryInsertionsDeletions(propList)) {
                    beginResetModel();
                    propList_.swap(propList);
                    endResetModel();
                }
            }
        }

        /** Implements an unsophisticated heuristic to translate the difference
         * between propList and propList_ into a series of insertions and deletions.
         *
         * @return True if such a sequence was found, false otherwise.
         */
        bool tryInsertionsDeletions(std::vector<PROP_INFO> &propList);

        template<typename MeshT, typename OUTPUT_ITERATOR>
        void gatherProperties(MeshT *mesh,
                              typename MeshT::prop_iterator props_first,
                              typename MeshT::prop_iterator props_last,
                              std::type_info *typeIdFilter, OUTPUT_ITERATOR oit,
                              ENTITY_FILTER entity) {

            for (typename MeshT::prop_iterator pit = props_first; pit != props_last; ++pit) {
                OpenMesh::BaseProperty * const baseProp = *pit;
                if (!baseProp) continue;

                TypeInfoWrapper bp_type = typeid(*baseProp);
                TYPE_INFO_SET::const_iterator sane_prop_it = sane_prop_types.find(bp_type);
                if ((typeIdFilter == 0 || TypeInfoWrapper(*typeIdFilter) == bp_type) && sane_prop_it != sane_prop_types.end()) {
                    *oit++ = PROP_INFO(baseProp->name(), *sane_prop_it, entity);
                }
            }
        }

        const PROP_INFO &operator[] (size_t index) {
            return propList_[index];
        }

    protected:
        std::vector<PROP_INFO> propList_;

    public:
        static const TypeInfoWrapper proptype_bool;
        static const TypeInfoWrapper proptype_int;
        static const TypeInfoWrapper proptype_uint;
        static const TypeInfoWrapper proptype_double;
        static const TypeInfoWrapper proptype_Vec3d;
        static const TypeInfoWrapper proptype_Vec3f;
        static const TypeInfoWrapper proptype_SkinWeights;

    private:
        typedef std::set<TypeInfoWrapper> TYPE_INFO_SET;
        static const TypeInfoWrapper prop_types[];
        static const TYPE_INFO_SET sane_prop_types;
};

#endif /* PROPERTYNAMELISTMODEL_HH_ */
