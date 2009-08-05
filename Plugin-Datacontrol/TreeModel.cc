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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include <QtGui>
#include <QBrush>

#include "TreeModel.hh"

#include <iostream>


#include <OpenFlipper/common/Types.hh>
#include "../OpenFlipper/BasePlugin/PluginFunctions.hh"


//******************************************************************************

/** \brief Constructor
 * 
 * @param _parent parent Object
 */
TreeModel::TreeModel( QObject *_parent) : QAbstractItemModel(_parent)
{
  rootItem_ = new TreeItem( -1, "ROOT", DATA_NONE, 0);
}


//******************************************************************************

/** \brief Destructor
 * 
 */
TreeModel::~TreeModel()
{

}


//******************************************************************************

/** \brief Return the number of columns
 * 
 * @param unused
 * @return return always 4
 */
int TreeModel::columnCount(const QModelIndex &/*_parent*/) const
{
  // Name,Visible,Source,Target -> 4
  return (4);
}


//******************************************************************************

/** \brief Returns the data stored under the given role for the item referred to by the index
 * 
 * @param index a ModelIndex that defines the item in the tree
 * @param role defines the kind of data requested
 * @return requested data
 */
QVariant TreeModel::data(const QModelIndex &index, int role) const
{

  // Skip invalid requests
  if (!index.isValid())
      return QVariant();
  
  // Get the corresponding tree item
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  
  if ( item == rootItem_ ) {
    std::cerr << "Root" << std::endl; 
  }
    
  // Set the background color of the objects row
  if ( role == Qt::BackgroundRole ) {
    if ( !item->visible() ) {
      return QVariant( QBrush(QColor(100,100,100) ) ); 
    }
  }

  switch (index.column() ) {
    // Name
    case 0 :
      
      if (role != Qt::DisplayRole && role != Qt::EditRole )
        return QVariant();
      
      return QVariant(item->name());
      
      break;
    // Visible
    case 1 :
      if (role == Qt::CheckStateRole ) {
        bool visibility = false;
        // visiblity group
        if (item->isGroup()){
          QList< TreeItem* > children = item->getLeafs();
          bool initRound = true;
          for (int i=0; i < children.size(); i++){
            if (initRound){
              visibility = children[i]->visible();
              initRound = false;
            }else if (visibility != children[i]->visible())
              return QVariant(Qt::PartiallyChecked);
          }
        }else
          //visibility item
          visibility = item->visible();
        if (visibility) 
          return QVariant(Qt::Checked);
        else
          return QVariant(Qt::Unchecked);
      }
      return QVariant();
      
      break;
    // Source
    case 2 :
      if (role == Qt::CheckStateRole ) {
        bool source = false;
        // source group
        if (item->isGroup()){
          QList< TreeItem* > children = item->getLeafs();
          
          bool initRound = true;
          for (int i=0; i < children.size(); i++){
            if (initRound){
              source = children[i]->source();
              initRound = false;
            }else if (source != children[i]->source())
              return QVariant(Qt::PartiallyChecked);
          }
        }else
          //source item
          source = item->source();
        if (source) 
          return QVariant(Qt::Checked);
        else
          return QVariant(Qt::Unchecked);
      }
      return QVariant();

    // Target    
    case 3 :
      if (role == Qt::CheckStateRole ) {
        bool target = false;
        // target group
        if (item->isGroup()){
          QList< TreeItem* > children = item->getLeafs();

          bool initRound = true;
          for (int i=0; i < children.size(); i++){
            if (initRound){
              target = children[i]->target();
              initRound = false;
            }else if (target != children[i]->target())
              return QVariant(Qt::PartiallyChecked);
          }
        }else
          //target item
          target = item->target();
        if (target) 
          return QVariant(Qt::Checked);
        else
          return QVariant(Qt::Unchecked);
      }
      return QVariant();

      break;
      
    default:
      return QVariant();
  }

}


//******************************************************************************

/** \brief Returns the item flags for the given index
 * 
 * @param index ModelIndex that defines an item in the tree
 * @return flags for the given ModelIndex
 */
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags = 0;

    // Show/Source/Target
    if ( ( index.column() == 1 ) || 
         ( index.column() == 2 ) ||
         ( index.column() == 3 ) )
      flags = Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
    if ( index.column() == 0 )
      flags = Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
      flags = Qt::ItemIsEnabled;

  // Get the corresponding tree item
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

  if ( item->isGroup() )
    return flags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
  else
    return flags | Qt::ItemIsDragEnabled;
}


//******************************************************************************

/** \brief Returns the data in the header
 * 
 * @param section the column in the header
 * @param orientation header orientation (only horizontal handled)
 * @param role the role that defines the type of data
 * @return the requested data
 */
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
      
      switch (section) {
        case 0 : return QVariant("Name");
        case 1 : return QVariant("Show");
        case 2 : return QVariant("Source");
        case 3 : return QVariant("Target");
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
QModelIndex TreeModel::index(int row, int column, const QModelIndex &_parent) const
{
//     if (!hasIndex(row, column, _parent))
//         return QModelIndex();

    TreeItem *parentItem;

    if (!_parent.isValid())
        parentItem = rootItem_;
    else
        parentItem = static_cast<TreeItem*>(_parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
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
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

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
int TreeModel::rowCount(const QModelIndex &_parent) const
{
    TreeItem *parentItem;
    if (_parent.column() > 0)
        return 0;

    if (!_parent.isValid())
        parentItem = rootItem_;
    else
        parentItem = static_cast<TreeItem*>(_parent.internalPointer());

    return parentItem->childCount();
}


//******************************************************************************

/** \brief The object with the given id has been changed. Update the model.
 * 
 * @param _id  id of an object
 */
void TreeModel::objectChanged(int _id) {

  if ( _id != -1 ){

    BaseObject* obj = 0;
    PluginFunctions::getObject(_id, obj);

    TreeItem* item = rootItem_->childExists(_id);

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

      //update source flag
      if ( obj->source() != item->source() || obj->isGroup() ){

        item->source( obj->source() );

        QModelIndex index = getModelIndex(item,2);
        if ( index.isValid() ){
          emit dataChanged( index, index);
          propagateUpwards(item->parent(), 2, false );
        }

        if ( obj->isGroup() )
          propagateDownwards(item, 2 );
      }

      //update target flag
      if ( obj->target() != item->target() || obj->isGroup() ){

        item->target( obj->target() );

        QModelIndex index = getModelIndex(item,3);
        if ( index.isValid() ){
          emit dataChanged( index, index);
          propagateUpwards(item->parent(), 3, false );
        }

        if ( obj->isGroup() )
          propagateDownwards(item, 3 );
      }

      //update parent
      if ( obj->parent() == PluginFunctions::objectRoot() && isRoot( item->parent() ) ){
        return;
      }else if ( obj->parent() == PluginFunctions::objectRoot() && !isRoot( item->parent() ) ){
        moveItem(item, rootItem_ );
      }else if ( obj->parent()->id() != item->parent()->id() ){
        TreeItem* parent = rootItem_->childExists( obj->parent()->id() );

        if (parent != 0)
          moveItem(item, parent );
      }
    }
  }
}


/** \brief The object with the given id has been added. add it to the internal tree
 * 
 * @param id_ id of the object
 */
void TreeModel::objectAdded(BaseObject* _object){

  TreeItem* parent = 0;
  //find the parent
  if ( _object->parent() == PluginFunctions::objectRoot() )
    parent = rootItem_;
  else
    parent = rootItem_->childExists( _object->parent()->id() );


  if (parent != 0){
    QModelIndex parentIndex = getModelIndex(parent, 0);

    beginInsertRows(parentIndex, parent->childCount(), parent->childCount()); //insert at the bottom

      TreeItem* item = new TreeItem( _object->id(), _object->name(), _object->dataType(), parent);

      parent->appendChild( item );

    endInsertRows();
  }

  objectChanged( _object->id() );
}

/** \brief The object with the given id has been deleted. delete it from the internal tree
 * 
 * @param id_ id of the object
 */
void TreeModel::objectDeleted(int _id){

  TreeItem* item = rootItem_->childExists(_id);

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
void TreeModel::moveItem(TreeItem* _item, TreeItem* _parent ){

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
TreeItem* TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
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
QString TreeModel::itemName(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
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
int TreeModel::itemId(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
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
QModelIndex TreeModel::getModelIndex(TreeItem* _object, int _column ){

  // root item gets an invalid QModelIndex
  if ( _object == rootItem_ )
    return QModelIndex();

  QModelIndex index = createIndex(_object->row(), _column, _object);

  return index;
}


//******************************************************************************

/** \brief Recursively update a column up to the root of the tree
 *
 * @param _item item to start with
 */
void TreeModel::propagateUpwards(TreeItem* _item, int _column, bool _value ){

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
 */
void TreeModel::propagateDownwards(TreeItem* _item, int _column ){

  for (int i=0; i < _item->childCount(); i++){

    TreeItem* current = _item->child(i);

    bool changed = false;

    switch ( _column ){

      case 1: //VISIBILTY

        if ( current->visible() != _item->visible() ){

          current->visible( _item->visible() );
          changed = true;
        }
        break;

      case 2: //SOURCE

        if ( current->source() != _item->source() ){

          current->source( _item->source() );
          changed = true;
        }
        break;

      case 3: //TARGET

        if ( current->target() != _item->target() ){

          current->target( _item->target() );
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

/** \brief Set Data at 'index' to 'value'
 * 
 * @param index a ModelIndex defining the positin in the model
 * @param value the new value
 * @param  unused
 * @return return if the data was set successfully
 */
bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int /*role*/)
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
bool TreeModel::isRoot(TreeItem * _item) {
  return ( _item == rootItem_ ); 
}

/*******************************************************************************
        drag & drop stuff
 *******************************************************************************/

/** \brief return the supported drop actions
 * 
 * @return drop actions
 */
Qt::DropActions TreeModel::supportedDropActions() const
{
    return /*Qt::CopyAction |*/ Qt::MoveAction;
}


//******************************************************************************

/** \brief return the mimeType for drag & drop
 * 
 * @return the mimeType
 */
QStringList TreeModel::mimeTypes() const
{
    QStringList types;
    types << "DataControl/dragDrop";
    return types;
}


//******************************************************************************

/** \brief generate mimeData for given ModelIndexes
 * 
 * @param indexes list of ModelIndexes
 * @return the mimeData
 */
QMimeData* TreeModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    QVector< int > rows;

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {

          if (!rows.contains( index.row() ) ){
            TreeItem *item = getItem(index);
            stream << item->id();

            rows.push_back( index.row() );
          }
        }
    }

    mimeData->setData("DataControl/dragDrop", encodedData);
    return mimeData;
}


//******************************************************************************

/** \brief this is called when mimeData is dropped
 * 
 * @param data the dropped data
 * @param action the definition of the dropAction which occured
 * @param unused
 * @param unused 
 * @param parent parent under which the drop occurred 
 * @return returns if the drop was sucessful
 */
bool TreeModel::dropMimeData(const QMimeData *data,
     Qt::DropAction action, int /*row*/, int /*column*/, const QModelIndex &parent)
{
  if (action == Qt::IgnoreAction)
      return true;

  if (!data->hasFormat("DataControl/dragDrop"))
      return false;

  QByteArray encodedData = data->data("DataControl/dragDrop");
  QDataStream stream(&encodedData, QIODevice::ReadOnly);

  QVector< int > ids;

  while (!stream.atEnd()) {
    int id;
    stream >> id;

    ids.push_back( id );
  }

  if (ids.count() == 0)
    return false;

  //get new parent
  TreeItem *newParent = getItem(parent);

  if ( newParent == 0 || !newParent->isGroup() )
    return false;

  //and move all objects
  for (int i = 0; i < ids.count(); i++){
    //tell the DataControlPlugin to move the corresponding BaseObject
    emit moveBaseObject( ids[i], newParent->id()  );
  }

  return true;

 }
