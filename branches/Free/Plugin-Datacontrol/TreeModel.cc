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




#include <QtGui>
#include <QBrush>

#include "TreeModel.hh"


#include <iostream>


#include <OpenFlipper/common/Types.hh>
#include "../OpenFlipper/BasePlugin/PluginFunctions.hh"

/// Constructor
TreeModel::TreeModel( QObject *_parent) : QAbstractItemModel(_parent)
{
  rootItem_ = PluginFunctions::objectRoot();
}

/// Destructor
TreeModel::~TreeModel()
{
  /// @todo : delete in core
//   delete rootItem_;
}

/// Return the number of columns
int TreeModel::columnCount(const QModelIndex &/*_parent*/) const
{
  // Name,Visible,Source,Target -> 4
  return (4);
}

/// Returns the data stored under the given role for the item referred to by the index
QVariant TreeModel::data(const QModelIndex &index, int role) const
{

  // Skip invalid requests
  if (!index.isValid())
      return QVariant();
  
  // Get the corresponding tree item
  BaseObject *item = static_cast<BaseObject*>(index.internalPointer());
  
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
      
      if (role != Qt::DisplayRole )
        return QVariant();
      
      return QVariant(item->name());
      
      break;
    // Visible
    case 1 :
      if (role == Qt::CheckStateRole ) {
        bool visibility = false;
        // visiblity group
        if (item->isGroup()){
          QList< BaseObject* > children = item->getLeafs();
          
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
          QList< BaseObject* > children = item->getLeafs();
          
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
          QList< BaseObject* > children = item->getLeafs();
          
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

/// Returns the item flags for the given index
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    
    // Show/Source/Target
    if ( ( index.column() == 1 ) || 
         ( index.column() == 2 ) ||
         ( index.column() == 3 ) )
      return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if ( index.column() == 0 )
      return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    
    return Qt::ItemIsEnabled;
}

/// Returns the data in the header
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

/// Returns the index of the item in the model specified by the given row, column and parent index.
QModelIndex TreeModel::index(int row, int column, const QModelIndex &_parent) const
{
    if (!hasIndex(row, column, _parent))
        return QModelIndex();

    BaseObject *parentItem;

    if (!_parent.isValid())
        parentItem = rootItem_;
    else
        parentItem = static_cast<BaseObject*>(_parent.internalPointer());

    BaseObject *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

/// Return index of parent item
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    BaseObject *childItem = static_cast<BaseObject*>(index.internalPointer());
    BaseObject *parentItem = childItem->parent();

    if (parentItem == rootItem_)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

/// Returns the number of rows
int TreeModel::rowCount(const QModelIndex &_parent) const
{
    BaseObject *parentItem;
    if (_parent.column() > 0)
        return 0;

    if (!_parent.isValid())
        parentItem = rootItem_;
    else
        parentItem = static_cast<BaseObject*>(_parent.internalPointer());

    return parentItem->childCount();
}


void TreeModel::updatedObject(int _id) {


  // If _id is -1 we have to rescan the tree for inconsitent objects
  bool scan = ( _id == -1 );
  
  // Check if we already have this item
  BaseObject* tmp = rootItem_->childExists( _id );
  bool itemExists   = ( (tmp != 0) );
  
//   std::cerr << "updated Object" << std::endl;
  
  
  // object has been created
  if ( !scan && !itemExists ) {
    std::cerr << "Newly added Item" << std::endl;
    
//     // If group is -1 add it to the top level
//     if ( object->group() == -1 ) {
//       QList<QVariant> columnData;
//       columnData << object->name() 
//                 << QVariant(object->visible() ) 
//                 << QVariant(object->source()) 
//                 << QVariant(object->target());
//       tmp = new TreeItem( columnData , rootItem_ );
//       tmp->objectId(object->id());
//       rootItem_->appendChild(tmp);
//       
      reset();
//       
//       //emit ...
//       return;
//     }
//     
//     // check if we already saw this group
//     if ( map_.count( object->group() ) == 0 ) {
//       
//       QList<QVariant> columnData;
//       columnData << "Group " + QString::number(object->group() ) 
//                 << QVariant(false) 
//                 << QVariant(false) 
//                 << QVariant(false);
//       TreeItem* tmp = new TreeItem( columnData , rootItem_ );
//       map_[ object->group() ] = tmp;
//       
//       rootItem_->appendChild(tmp);
//     }
//     
//     // Add the item to the matching group
//     TreeItem* groupItem = map_[ object->group() ];
//     
//     // Create the new item
//     QList<QVariant> columnData;
//     columnData << object->name()  
//               << QVariant( object->visible() ) 
//               << QVariant( object->source() ) 
//               << QVariant( object->target() );
//     
//     TreeItem* tmp = new TreeItem(columnData, groupItem );
//     tmp->objectId(object->id());
//     
//     groupItem->appendChild(tmp);
//     
//     reset();
//     
//     //emit ...
//     
//     return;
  }
  
  // Modified item.
  if ( !scan && itemExists ) {
    QModelIndex index = createIndex(tmp->row(), 0, tmp);
//     QModelIndex index1 = createIndex(tmp->row(), 1, tmp);

//     std::cerr << "TODO: modified item" << std::endl; 
    
    // actually a row has been added by the item

    emit dataChanged( index,index );

    return;
  }
  
  if ( scan ) {
//     std::cerr << "Scan" << std::endl;
    reset();
    
//     std::cerr << "Scan done" << std::endl;
//     
//     bool update = false;
//     
//     std::cerr << "Scan with id " << _id << std::endl;
//     while ( true ) {
//       
//       // get an inconsistent item ( object id does not exist anymore )
//       tmp = rootItem_->inConsistent();
//       
//       // Tree is clean now
//       if ( tmp == 0 ) 
//         break;
//       
//       // Item has been removed, update required
//       update = true;
//       
//       TreeItem* parentItem;
//       
//       // Delete parent groups if empty
//       while (true) {
//         parentItem = tmp->parent();
//         
//         //if this was the rootItem, we stop here
//         if ( parentItem == 0 )
//           break;
//         
//         // Remove child from the tree and delete it
//         parentItem->removeChild(tmp);
//         delete (tmp);
//         
//         // Stop if the parent has one child
//         tmp = parentItem;
//         if ( parentItem->childCount() > 0 ) {
//           reset();
//           break;
//         }
//       }
//     }
//     
//     if (update)
//       reset();
//     
//     /// @todo : emit columnsRemoved ( const QModelIndex & parent, int start, int end )
//     
//     emit dataChanged( index(0,0),index(0,0) );
//     
//     return;
  }
    
//   std::cerr << "Warning, undefined object configuration" << std::endl; 
    
    //   reset();
}

/// Return item at given index
BaseObject *TreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        BaseObject *item = static_cast<BaseObject*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem_;
}

/// Set Data at 'index' to 'value'
bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int /*role*/)
{
  BaseObject *item = getItem(index);
  
  if ( item->isGroup() ) {
    int childCount = 0;
    // Decide on column what to do with the value
    switch ( index.column() ) {
      // Name
      case 0 :
        if ( value.toString() == "" ) 
          return false; 
        
        item->setName( value.toString() );
        break;
        
       // visible
      case 1 : 
        if ( value.toInt() == Qt::Unchecked ) {
                 
          QList< BaseObject* > children = item->getLeafs();
          
          childCount = children.size();

          for (int i=0; i < children.size(); i++){
              BaseObjectData* child = dynamic_cast< BaseObjectData* > (children[i]);
              if (child)
                child->hide();
          }
        }
      
        if ( value.toInt() == Qt::Checked ) {
          
          QList< BaseObject* > children = item->getLeafs();
          
          childCount = children.size();

          for (int i=0; i < children.size(); i++){
            BaseObjectData* child = dynamic_cast< BaseObjectData* > (children[i]);
            if (child)
              child->show();
          }
        }
      
        break;  
      
      // source
      case 2 : 
        if ( value.toInt() == Qt::Unchecked ) {
                 
          QList< BaseObject* > children = item->getLeafs();

          childCount = children.size();
          
          for (int i=0; i < children.size(); i++){
              if (children[i])
                children[i]->source(false);
          }
        }
      
        if ( value.toInt() == Qt::Checked ) {
          
          QList< BaseObject* > children = item->getLeafs();
          
          childCount = children.size();

          for (int i=0; i < children.size(); i++){
              if (children[i])
                children[i]->source(true);
          }
        } 
        break;
      
      //target
      case 3 : 
        if ( value.toInt() == Qt::Unchecked ) {
                 
          QList< BaseObject* > children = item->getLeafs();

          childCount = children.size();
          
          for (int i=0; i < children.size(); i++){
              if (children[i])
                children[i]->target(false);
          }
        }
      
        if ( value.toInt() == Qt::Checked ) {
          
          QList< BaseObject* > children = item->getLeafs();
          
          childCount = children.size();

          for (int i=0; i < children.size(); i++){
              if (children[i])
                children[i]->target(true);
          }
        }
        break;
    }

    if (childCount > 0){
      BaseObject* obj = item->getLeafs()[childCount-1];

      QModelIndex childIndex = createIndex(obj->row(),index.column() , obj);

      emit dataChanged( index,childIndex );
    }else
      emit dataChanged( index, index );

    return true; 
  }

  // <- Item is not a group
      
  // Decide on column what to do with the value ( abort if anything goes wrong)
  switch ( index.column() ) {
    // Name :
    case 0 :
      if ( value.toString() == "" ) 
        return false; 
      item->setName(value.toString());
      break;
      
    // visible
    case 1 : 
      if ( value.toInt() == Qt::Unchecked ) {
        
        BaseObjectData* baseObject = dynamic_cast< BaseObjectData* >(item);
        
        if ( baseObject )
          baseObject->hide();
        else 
          std::cerr << " Trying to hide invisible object e.g. Group " << std::endl;
      }
      
      if ( value.toInt() == Qt::Checked ) {
        BaseObjectData* baseObject = dynamic_cast< BaseObjectData* >(item);
        
        if ( baseObject )
          baseObject->show();
        else
          std::cerr << " Trying to show invisible object e.g. Group " << std::endl;
      }
      
      break;      
      
    // source
    case 2 :  
      if ( value.toInt() == Qt::Unchecked ) {
        item->source(false);
      }
      
      if ( value.toInt() == Qt::Checked ) {
        item->source(true);
      }
      
      break;
    
    //target
    case 3 : 
      if ( value.toInt() == Qt::Unchecked ) {
        item->target(false);
      }
      
      if ( value.toInt() == Qt::Checked ) {
        item->target(true);
      }
      
      break;
    
    default:
      
      break;
      
  }
  
  emit dataChanged(index,index);
  return true;
}

/// return if an object is equal to the root object
bool TreeModel::isRoot(BaseObject * _item) {
  return ( _item == rootItem_ ); 
}


