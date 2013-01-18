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

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "TreeItem.hh"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

signals:
   // the connected TreeView changed data
   void dataChangedInside(int _id, int _column, const QVariant& _value);
   // an object was moved via dragNdrop
   void moveBaseObject(int _id, int _newParentId);

public:

    /// Constructor
    TreeModel(QObject *_parent = 0);

    /// Destructor
    ~TreeModel();

//===========================================================================
/** @name inherited from QAbstractItemModel
  * @{ */
//===========================================================================

public:

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

    /** \brief Return the number of columns
     *
     * @param  _parent unused
     * @return return always 4
     */
    int columnCount(const QModelIndex &_parent = QModelIndex()) const;

    /** \brief Set Data at 'index' to 'value'
     *
     * @param  index a ModelIndex defining the positin in the model
     * @param  value the new value
     * @param  role unused
     * @return return if the data was set successfully
     */
    bool setData(const QModelIndex &index, const QVariant &value , int role);

/** @} */

//===========================================================================
/** @name Internal DataStructure (the TreeItem Tree)
  * @{ */
//===========================================================================

public:

    /// Return the ModelIndex corresponding to a given TreeItem and Column
    QModelIndex getModelIndex(TreeItem* _object, int _column );

    /// Check if the given item is the root item
    bool isRoot(TreeItem* _item);

    /// Get the name of a given object
    bool getObjectName(TreeItem* _object , QString& _name);

    /// Get the TreeItem corresponding to a given ModelIndex
    TreeItem *getItem(const QModelIndex &index) const;

    /// Get the name of a TreeItem corresponding to a given ModelIndex
    QString itemName(const QModelIndex &index) const;

    /// Get the id of a TreeItem corresponding to a given ModelIndex
    int itemId(const QModelIndex &index) const;


    /// The object with the given id has been changed. Check if model also has to be changed
    void objectChanged(int id_);

    /// The object with the given id has been added. add it to the internal tree
    void objectAdded(BaseObject* _object);

    /// The object with the given id has been deleted. delete it from the internal tree
    void objectDeleted(int id_);

    /// move the item to a new parent
    void moveItem(TreeItem* _item, TreeItem* _parent );
private:

    /// Root item of the tree
    TreeItem* rootItem_;

/** @} */

//===========================================================================
/** @name Drag and Drop
  * @{ */
//===========================================================================

public:

    /// supported drag & Drop actions
    Qt::DropActions supportedDropActions() const;

    /// stores the mimeType for Drag & Drop
    QStringList mimeTypes() const;

    /// get the mimeData for a given ModelIndex
    QMimeData* mimeData(const QModelIndexList& indexes) const;

    /** \brief This is called when mimeData is dropped
     *
     * @param data   The dropped data
     * @param action The definition of the dropAction which occurred
     * @param row    Unused
     * @param column Unused
     * @param parent Parent under which the drop occurred
     * @return returns if the drop was successful
     */
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

/** @} */

};

#endif

