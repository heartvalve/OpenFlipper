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

#include "TreeItem.hh"

//--------------------------------------------------------------------------------

TreeItem::TreeItem(int _id, QString _name, DataType _type, TreeItem* _parent) :
  id_(_id),
  dataType_(_type),
  visible_(true),
  name_(_name),
  parentItem_(_parent)
{
}


// ===============================================================================
// Static Members
// ===============================================================================

int TreeItem::id() {
  return id_;
}

//--------------------------------------------------------------------------------

bool TreeItem::dataType(DataType _type) {
  if ( _type == DATA_ALL ) {
    return true;
  }

  return ( (dataType_ & _type) );
}

//--------------------------------------------------------------------------------

DataType TreeItem::dataType() {
  return dataType_;
}

//--------------------------------------------------------------------------------

int TreeItem::group() {
  // Skip root node
  if ( parent() == 0 )
    return -1;

  // Dont count root node as a group
  if ( parent()->parent() == 0 )
    return -1;

  // Only consider groups
  if ( !parent()->dataType(DATA_GROUP) )
    return -1;

  // Get the group id
  return ( parent()->id() );
}

//--------------------------------------------------------------------------------

bool TreeItem::isGroup() {
  return ( dataType(DATA_GROUP) );
}

// ===============================================================================
// Dynamic Members
// ===============================================================================

bool TreeItem::visible() {
  return visible_;
}

//--------------------------------------------------------------------------------

void TreeItem::visible(bool _visible) {
  visible_ = _visible;
}

//--------------------------------------------------------------------------------

QString TreeItem::name() {
  return name_;
}

//--------------------------------------------------------------------------------

void TreeItem::name(QString _name ) {
  name_ = _name;
}

// ===============================================================================
// Tree Structure
// ===============================================================================

TreeItem* TreeItem::next() {
  // Visit child item of this node
  if ( childItems_.size() > 0 ) {
     return childItems_[0];
  }

  // No Child Item so visit the next child of the parentItem_
  if ( parentItem_ ) {

    TreeItem* parentPointer = parentItem_;
    TreeItem* thisPointer   = this;

    // while we are not at the root node
    while ( parentPointer ) {

      // If there is an unvisited child of the parent, return this one
      if ( parentPointer->childCount() > ( thisPointer->row() + 1) ) {
        return parentPointer->childItems_[ thisPointer->row() + 1 ];
      }

      // Go to the next level
      thisPointer   = parentPointer;
      parentPointer = parentPointer->parentItem_;

    }

    return thisPointer;
  }

  return this;

}

//--------------------------------------------------------------------------------

int TreeItem::level() {
  int level = 0;
  TreeItem* current = this;

  // Go up and count the levels to the root node
  while ( current->parent() != 0 ) {
    level++;
    current = current->parent();
  }

  return level;
}

//--------------------------------------------------------------------------------

int TreeItem::row() const
{
    if (parentItem_)
        return parentItem_->childItems_.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

//--------------------------------------------------------------------------------

TreeItem* TreeItem::parent()
{
  return parentItem_;
}

//--------------------------------------------------------------------------------

void TreeItem::setParent(TreeItem* _parent) {
  parentItem_ = _parent;
}

//--------------------------------------------------------------------------------

void TreeItem::appendChild(TreeItem *item)
{
    childItems_.append(item);
}

//--------------------------------------------------------------------------------

TreeItem *TreeItem::child(int row)
{
    return childItems_.value(row);
}

//--------------------------------------------------------------------------------

int TreeItem::childCount() const
{
    return childItems_.count();
}

//--------------------------------------------------------------------------------

TreeItem* TreeItem::childExists(int _objectId) {

  // Check if this object has the requested id
  if ( id_ == _objectId )
    return this;

  // search in children
  for ( int i = 0 ; i < childItems_.size(); ++i ) {
    TreeItem* tmp = childItems_[i]->childExists(_objectId);
    if ( tmp != 0)
      return tmp;
  }

  return 0;
}

//--------------------------------------------------------------------------------

TreeItem* TreeItem::childExists(QString _name) {

  // Check if this object has the requested id
  if ( name() == _name )
    return this;

  // search in children
  for ( int i = 0 ; i < childItems_.size(); ++i ) {
    TreeItem* tmp = childItems_[i]->childExists(_name);
    if ( tmp != 0)
      return tmp;
  }

  return 0;
}

//--------------------------------------------------------------------------------

void TreeItem::removeChild( TreeItem* _item ) {

  bool found = false;
  QList<TreeItem*>::iterator i;
  for (i = childItems_.begin(); i != childItems_.end(); ++i) {
     if ( *i == _item ) {
        found = true;
        break;
     }
  }

  if ( !found ) {
    std::cerr << "TreeItem: Illegal remove request" << std::endl;
    return;
  }

  childItems_.erase(i);
}

//--------------------------------------------------------------------------------

QList< TreeItem* > TreeItem::getLeafs() {

  QList< TreeItem* > items;

  for ( int i = 0 ; i < childItems_.size(); ++i ) {
    items = items + childItems_[i]->getLeafs();
  }

  // If we are a leave...
  if ( childCount() == 0 )
    items.push_back(this);

  return items;
}

//--------------------------------------------------------------------------------

void TreeItem::deleteSubtree() {

  // call function for all children of this node
  for ( int i = 0 ; i < childItems_.size(); ++i) {

    // remove the subtree recursively
    childItems_[i]->deleteSubtree();

    // delete child
    delete childItems_[i];
  }

  // clear the array
  childItems_.clear();
}

//=============================================================================
