/*
 * ObjectListItemModel.hh
 *
 *  Created on: Feb 29, 2012
 *      Author: ebke
 */

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
