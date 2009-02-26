//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
// 
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include <OpenFlipper/common/Types.hh>

class TreeItem;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    /// Constructor
    TreeModel(QObject *_parent = 0);

    /// Destructor
    ~TreeModel();

    /// Get the data of the corresponding entry
    QVariant data(const QModelIndex &index, int role) const;

    /// return the types of the corresponding entry
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /// return the header data of the model
    QVariant headerData(int section, 
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    /// Get the ModelIndex at given row,column
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;

    /// Get the parent ModelIndex
    QModelIndex parent(const QModelIndex &index) const;

    /// get the number of rows
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /// get the number of columns
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /// The object with the given id has been updated. Check if model has to be changed
    void updatedObject(int id_);

    /// Set the data at the given index
    bool setData(const QModelIndex &index, const QVariant &value , int role);

    /// Get the BaseObject corresponding to a given ModelIndex
    BaseObject *getItem(const QModelIndex &index) const;

    /// Return the ModelIndex corresponding to a given BaseObject and Column
    QModelIndex getModelIndex(BaseObject* _object, int _column );

    /// Check if the given item is the root item
    bool isRoot(BaseObject* _item);

    /// Get the name of a given object
    bool getObjectName(BaseObject* _object , QString& _name);


//===========================================================================
/** @name Drag and Drop
  * @{ */
//===========================================================================

    /// supported drag & Drop actions
    Qt::DropActions supportedDropActions() const;

    /// stores the mimeType for Drag & Drop
    QStringList mimeTypes() const;

    /// get the mimeData for a given ModelIndex
    QMimeData* mimeData(const QModelIndexList& indexes) const;

    /// Called when mimeData is dropped somewhere
    bool dropMimeData(const QMimeData *data,
     Qt::DropAction action, int row, int column, const QModelIndex &parent);

/** @} */

private:
    /// Rootitem of the tree
    BaseObject* rootItem_;
    
    /** Mapping of the group ids to their parent
     * Use this to check if a group with the given id exists and which item 
     * represents this group
     */
    std::map< int, TreeItem* > map_;

    /// Recursively update source,target selection up to the root of the tree
    void updateSourceSelection(BaseObject* _obj );
    void updateTargetSelection(BaseObject* _obj );

};

///@todo : Save group names on exit
///@todo : Load group names
///@todo : emit update objects in the right place
///@todo : emit dataChanged instead of reset();

#endif

