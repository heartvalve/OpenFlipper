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

#include <OpenFlipper/common/Types.hh>

#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QBrush>

#include "TreeModelObjectSelection.hh"

#include <iostream>



#include "../OpenFlipper/BasePlugin/PluginFunctions.hh"


//******************************************************************************

/** \brief Constructor
 * 
 * @param _parent parent Object
 */
TreeModelObjectSelection::TreeModelObjectSelection( QObject *_parent) : QAbstractItemModel(_parent)
{
  rootItem_ = new TreeItemObjectSelection( -1, "ROOT", DATA_UNKNOWN, 0);
}


//******************************************************************************

/** \brief Destructor
 * 
 */
TreeModelObjectSelection::~TreeModelObjectSelection()
{

}


//******************************************************************************


int TreeModelObjectSelection::columnCount(const QModelIndex &/*_parent*/) const
{
  // Id, Name -> 2
  return (2);
}


//******************************************************************************

/** \brief Returns the data stored under the given role for the item referred to by the index
 * 
 * @param index a ModelIndex that defines the item in the tree
 * @param role defines the kind of data requested
 * @return requested data
 */
QVariant TreeModelObjectSelection::data(const QModelIndex &index, int role) const
{

  // Skip invalid requests
  if (!index.isValid())
      return QVariant();

  // Get the corresponding tree item
  TreeItemObjectSelection *item = static_cast<TreeItemObjectSelection*>(index.internalPointer());

  if ( item == rootItem_ ) {
    std::cerr << "Root" << std::endl; 
  }

  // Set the background color of the objects row
  if ( role == Qt::BackgroundRole ) {
    if ( !item->visible() ) {
      return QVariant (QBrush (QColor (192, 192, 192)));
    }
  }

  if (role == Qt::DisplayRole)
  {
    switch (index.column ())
    {
      case 0:
        return QVariant(item->id());
      case 1:
        return QVariant(item->name());
      default:
        return QVariant ();
    }
  }
  else
    return QVariant ();
}


//******************************************************************************

/** \brief Returns the item flags for the given index
 * 
 * @param index ModelIndex that defines an item in the tree
 * @return flags for the given ModelIndex
 */
Qt::ItemFlags TreeModelObjectSelection::flags(const QModelIndex &index) const
{
  if (!index.isValid())
      return 0;

  Qt::ItemFlags flags = 0;

  // Show/Source/Target
  if ( index.column() == 0 || index.column() == 1 )
    flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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
QVariant TreeModelObjectSelection::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {

    if (section == 0)
      return QVariant("ID");
    else if (section == 1)
      return QVariant("Name");
    else
      return QVariant();

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
QModelIndex TreeModelObjectSelection::index(int row, int column, const QModelIndex &_parent) const
{
  TreeItemObjectSelection *parentItem;

  if (!_parent.isValid())
    parentItem = rootItem_;
  else
    parentItem = static_cast<TreeItemObjectSelection*>(_parent.internalPointer());

  TreeItemObjectSelection *childItem = parentItem->child(row);
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
QModelIndex TreeModelObjectSelection::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  TreeItemObjectSelection *childItem = static_cast<TreeItemObjectSelection*>(index.internalPointer());
  TreeItemObjectSelection *parentItem = childItem->parent();

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
int TreeModelObjectSelection::rowCount(const QModelIndex &_parent) const
{
  TreeItemObjectSelection *parentItem;
  if (_parent.column() > 0)
    return 0;

  if (!_parent.isValid())
    parentItem = rootItem_;
  else
    parentItem = static_cast<TreeItemObjectSelection*>(_parent.internalPointer());

  return parentItem->childCount();
}


//******************************************************************************

/** \brief The object with the given id has been changed. Update the model.
 *
 * @param _id  id of an object
 */
void TreeModelObjectSelection::objectChanged(int _id) {

  if ( _id != -1 ){

    BaseObject* obj = 0;
    PluginFunctions::getObject(_id, obj);

    TreeItemObjectSelection* item = rootItem_->childExists(_id);

    //if internal and external representation are both valid
    if (obj != 0 && item != 0){
      //update the name
      if ( obj->name() != item->name() ){

        item->name( obj->name() );

        QModelIndex index = getModelIndex(item,0);
        if ( index.isValid() )
          emit dataChanged( index, index);
      }

      //update visibility
      if ( obj->visible() != item->visible() || obj->isGroup() ){

        item->visible( obj->visible() );

        QModelIndex index0 = getModelIndex(item,0);
        QModelIndex index1 = getModelIndex(item,3);

        if ( index0.isValid() && index1.isValid() ){
          //the whole row has to be updated because of the grey background-color
          emit dataChanged( index0, index1);
          propagateUpwards(item->parent(), 1, obj->visible() );
        }

        if ( obj->isGroup() )
          propagateDownwards(item, 1 );
      }

      //update parent
      if ( obj->parent() == PluginFunctions::objectRoot() && isRoot( item->parent() ) ){
        return;
      }else if ( obj->parent() == PluginFunctions::objectRoot() && !isRoot( item->parent() ) ){
        moveItem(item, rootItem_ );
      }else if ( obj->parent()->id() != item->parent()->id() ){
        TreeItemObjectSelection* parent = rootItem_->childExists( obj->parent()->id() );

        if (parent != 0)
          moveItem(item, parent );
      }
    }
  }
}


/** \brief The object with the given id has been added. Add it to the internal tree
 *
 * @param _object The object that has been added
 */
void TreeModelObjectSelection::objectAdded(BaseObject* _object){

  objectAdded (_object, _object->parent());
}

/** \brief The object has been added. Add it to the internal tree
 *
 * @param _object The added object
 * @param _parent The parent object
 */
void TreeModelObjectSelection::objectAdded(BaseObject* _object, BaseObject* _parent){

  if (rootItem_->childExists(_object->id()))
    return;
  
  TreeItemObjectSelection* parent = 0;
  //find the parent
  if ( _parent == PluginFunctions::objectRoot() )
    parent = rootItem_;
  else
    parent = rootItem_->childExists( _parent->id() );

  if (!parent)
  {
    objectAdded(_parent);
    parent = rootItem_->childExists( _parent->id() );
  }
  
  QModelIndex parentIndex = getModelIndex(parent, 0);

  beginInsertRows(parentIndex, parent->childCount(), parent->childCount()); //insert at the bottom

  TreeItemObjectSelection* item = new TreeItemObjectSelection( _object->id(), _object->name(), _object->dataType(), parent);

  parent->appendChild( item );

  endInsertRows();

  objectChanged( _object->id() );
}

/** \brief The object with the given id has been deleted. delete it from the internal tree
 *
 * @param _id id of the object
 */
void TreeModelObjectSelection::objectDeleted(int _id){

  TreeItemObjectSelection* item = rootItem_->childExists(_id);

  if ( item != 0 && !isRoot(item) ){

    QModelIndex itemIndex   = getModelIndex(item, 0);
    QModelIndex parentIndex = itemIndex.parent();

    beginRemoveRows( parentIndex, itemIndex.row(), itemIndex.row() );

    item->parent()->removeChild(item);
    item->deleteSubtree();

    delete item;

    endRemoveRows();
  }
}

//******************************************************************************

/** \brief move the item to a new parent
 *
 * @param _item the item
 * @param _parent new parent
 */
void TreeModelObjectSelection::moveItem(TreeItemObjectSelection* _item, TreeItemObjectSelection* _parent ){

  QModelIndex itemIndex   = getModelIndex(_item, 0);
  QModelIndex oldParentIndex = itemIndex.parent();
  QModelIndex newParentIndex = getModelIndex(_parent, 0);

  //delete everything at the old location
  beginRemoveRows( oldParentIndex, itemIndex.row(), itemIndex.row() );

    _item->parent()->removeChild(_item);

  endRemoveRows();

  //insert it at the new location
  beginInsertRows(newParentIndex, _parent->childCount(), _parent->childCount() ); //insert at the bottom
    _item->setParent( _parent );
    _parent->appendChild( _item );
  endInsertRows();

  emit layoutChanged();
}

//******************************************************************************

/** \brief Return item at given index
 * 
 * @param index a ModelIndex
 * @return item at given index
 */
TreeItemObjectSelection* TreeModelObjectSelection::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItemObjectSelection *item = static_cast<TreeItemObjectSelection*>(index.internalPointer());
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
QString TreeModelObjectSelection::itemName(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItemObjectSelection *item = static_cast<TreeItemObjectSelection*>(index.internalPointer());
        if (item)
          return item->name();
    }
    return "not found";
}

//******************************************************************************

/** \brief Return item-id at given index
 * 
 * @param index a ModelIndex
 * @return item-id at given index
 */
int TreeModelObjectSelection::itemId(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItemObjectSelection *item = static_cast<TreeItemObjectSelection*>(index.internalPointer());
        if (item)
          return item->id();
    }
    return -1;
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
QModelIndex TreeModelObjectSelection::getModelIndex(TreeItemObjectSelection* _object, int _column ){

  // root item gets an invalid QModelIndex
  if ( _object == rootItem_ )
    return QModelIndex();

  QModelIndex index = createIndex(_object->row(), _column, _object);

  return index;
}

//******************************************************************************

/** \brief Return index of given item
 *
 * Warning: Only use this function if you know that all ModelIndices are created
 *
 * @param _id an object id
 * @param _column a column
 * @return index of object and column
 */
QModelIndex TreeModelObjectSelection::getModelIndex(int _id, int _column ){

  TreeItemObjectSelection *obj = rootItem_->childExists(_id);

  if (obj)
    return getModelIndex (obj, _column);

  return QModelIndex();
}


//******************************************************************************

/** \brief Recursively update a column up to the root of the tree
 *
 * @param _item   Item to start with
 * @param _column The column
 * @param _value  The value that should be propagated
 */
void TreeModelObjectSelection::propagateUpwards(TreeItemObjectSelection* _item, int _column, bool _value ){

  if ( isRoot(_item) || (!_item->isGroup()) )
    return;

  if (_column == 1){ //visibility
    _item->visible( _value );

    //the whole row has to be updated because of the grey background-color
    QModelIndex index0 = getModelIndex(_item,0);
    QModelIndex index1 = getModelIndex(_item,3);

    emit dataChanged( index0, index1);

  } else {

    QModelIndex index = getModelIndex(_item,_column);
    emit dataChanged(index, index);
  }

  propagateUpwards( _item->parent(), _column, _value );
}

//******************************************************************************

/** \brief Recursively update a column up to the root of the tree
 *
 * @param _item item to start with
 * @param _column column
 */
void TreeModelObjectSelection::propagateDownwards(TreeItemObjectSelection* _item, int _column ){

  for (int i=0; i < _item->childCount(); i++){

    TreeItemObjectSelection* current = _item->child(i);

    bool changed = false;

    switch ( _column ){

      case 1: //VISIBILTY

        if ( current->visible() != _item->visible() ){

          current->visible( _item->visible() );
          changed = true;
        }
        break;

      default:
        break;
    }

    if (changed){
      QModelIndex index = getModelIndex(current,_column);
      emit dataChanged(index, index);
    }

    if ( current->isGroup() )
      propagateDownwards(current, _column);
  }
}

//******************************************************************************

bool TreeModelObjectSelection::setData(const QModelIndex &index, const QVariant &value, int /*role*/)
{

  emit dataChangedInside( itemId(index), index.column(), value );

  return true;
}


//******************************************************************************

/** \brief return if an object is equal to the root object
 * 
 * @param _item the item to be checked
 * @return is it the root object?
 */
bool TreeModelObjectSelection::isRoot(TreeItemObjectSelection * _item) {
  return ( _item == rootItem_ ); 
}
