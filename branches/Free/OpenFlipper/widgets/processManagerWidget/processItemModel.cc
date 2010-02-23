/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision: 7820 $                                                       *
 *   $Author: wilden $                                                      *
 *   $Date: 2009-12-09 14:30:12 +0100 (Wed, 09 Dec 2009) $                   *
 *                                                                           *
\*===========================================================================*/

#include <QtGui>
#include <QBrush>

#include "processItemModel.hh"

#include <iostream>

//******************************************************************************

/** \brief Constructor
 *
 * @param _parent parent Object
 */
ProcessItemModel::ProcessItemModel( QObject *_parent) : QAbstractItemModel(_parent) {
    rootItem_ = new ProcessItem(tr("Root"), tr("Root"), 0.0f, 1.0f, 0);
}

//******************************************************************************

/** \brief Destructor
 *
 */
ProcessItemModel::~ProcessItemModel() {

}

//******************************************************************************

/** \brief Return the number of columns
 *
 * @param unused
 * @return return always 4
 */
int ProcessItemModel::columnCount(const QModelIndex &/*_parent*/) const {
  // Process,Description,Progress,Controls -> 4
  return (4);
}

//******************************************************************************

/** \brief Returns the data stored under the given role for the item referred to by the index
 *
 * @param index a ModelIndex that defines the item in the tree
 * @param role defines the kind of data requested
 * @return requested data
 */
QVariant ProcessItemModel::data(const QModelIndex &index, int role) const {

  // Skip invalid requests
  if (!index.isValid())
      return QVariant();

  // Get the corresponding tree item
  ProcessItem *item = static_cast<ProcessItem*>(index.internalPointer());

  if ( item == rootItem_ ) {
    std::cerr << "Root" << std::endl;
  }

  // Set the background color of the objects row
  if ( role == Qt::BackgroundRole ) {
    return QVariant( QBrush(QColor(100,100,100) ) );
  }
  


  switch ( index.column() ) {
    // Process
    case 0 :

      if (role != Qt::DisplayRole && role != Qt::EditRole )
        return QVariant();

      return QVariant( item->name() );

      break;
    // Description
    case 1 :
        if (role != Qt::DisplayRole && role != Qt::EditRole )
            return QVariant();
        
        return QVariant( item->description() );
        
        break;
    // Progress
    case 2 :
        if (role != Qt::DisplayRole && role != Qt::EditRole )
            return QVariant();
        
        return QVariant( item->progress() / item->progressMax() );
        
        break;

    // Controls
    case 3 :
        
        break;

    default:
      return QVariant();
  }

  return QVariant();
}


//******************************************************************************

/** \brief Returns the item flags for the given index
 *
 * @param index ModelIndex that defines an item in the tree
 * @return flags for the given ModelIndex
 */
Qt::ItemFlags ProcessItemModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags = 0;

    // Process/Description/Progress
    if ( ( index.column() == 0 ) ||
         ( index.column() == 1 ) ||
         ( index.column() == 2 ) )
        flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
    if ( index.column() == 4 )
        flags = Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
      flags = Qt::ItemIsEnabled;
    
    return flags;
}


//******************************************************************************

/** \brief Returns the data in the header
 *
 * @param section the column in the header
 * @param orientation header orientation (only horizontal handled)
 * @param role the role that defines the type of data
 * @return the requested data
 */
QVariant ProcessItemModel::headerData(int section, Qt::Orientation orientation,
                                int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {

      switch (section) {
        case 0 : return QVariant("Process");
        case 1 : return QVariant("Description");
        case 2 : return QVariant("Progress");
        case 3 : return QVariant("Controls");
        default :
          return QVariant();
      }
    }
    return QVariant();
}

//******************************************************************************

/** \brief Returns the index of the item in the model specified by the given row, column and parent index.
 *
 * @param row the row
 * @param column the column
 * @param _parent parent item
 * @return corresponding ModelIndex
 */
QModelIndex ProcessItemModel::index(int row, int column, const QModelIndex &_parent) const
{
//     if (!hasIndex(row, column, _parent))
//         return QModelIndex();

    ProcessItem *parentItem;

    if (!_parent.isValid())
        parentItem = rootItem_;
    else
        parentItem = static_cast<ProcessItem*>(_parent.internalPointer());

    ProcessItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}


//******************************************************************************

/** \brief Return index of parent item
 *
 * @param index a ModelIndex
 * @return parent of the given ModelIndex
 */
QModelIndex ProcessItemModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ProcessItem *childItem = static_cast<ProcessItem*>(index.internalPointer());
    ProcessItem *parentItem = childItem->parent();

    if (parentItem == rootItem_)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}


//******************************************************************************

/** \brief Returns the number of rows under given parent
 *
 * @param _parent parent Item
 * @return number of rows that are children of given parent
 */
int ProcessItemModel::rowCount(const QModelIndex &_parent) const
{
    ProcessItem *parentItem;
    if (_parent.column() > 0)
        return 0;

    if (!_parent.isValid())
        parentItem = rootItem_;
    else
        parentItem = static_cast<ProcessItem*>(_parent.internalPointer());

    return parentItem->childCount();
}


//******************************************************************************


/** \brief Return item at given index
 *
 * @param index a ModelIndex
 * @return item at given index
 */
ProcessItem* ProcessItemModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        ProcessItem *item = static_cast<ProcessItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem_;
}


//******************************************************************************

/** \brief Return item-name at given index
 *
 * @param index a ModelIndex
 * @return name of the item at given index
 */
QString ProcessItemModel::itemName(const QModelIndex &index) const
{
    if (index.isValid()) {
        ProcessItem *item = static_cast<ProcessItem*>(index.internalPointer());
        if (item)
          return item->name();
    }
    return "not found";
}

//******************************************************************************

/** \brief Return index of given item
 *
 * Warning: Only use this function if you know that all ModelIndices are created
 *
 * @param _object an object
 * @param _column a column
 * @return index of object and column
 */
QModelIndex ProcessItemModel::getModelIndex(ProcessItem* _object, int _column ){

  // root item gets an invalid QModelIndex
  if ( _object == rootItem_ )
    return QModelIndex();

  QModelIndex index = createIndex(_object->row(), _column, _object);

  return index;
}

//******************************************************************************

/** \brief Set Data at 'index' to 'value'
 *
 * @param index a ModelIndex defining the positin in the model
 * @param value the new value
 * @param  unused
 * @return return if the data was set successfully
 */
bool ProcessItemModel::setData(const QModelIndex &index, const QVariant &value, int /*role*/)
{
  
  
  return true;
}


//******************************************************************************

/** \brief return if an object is equal to the root object
 *
 * @param _item the item to be checked
 * @return is it the root object?
 */
bool ProcessItemModel::isRoot(ProcessItem * _item) {
  return ( _item == rootItem_ );
}