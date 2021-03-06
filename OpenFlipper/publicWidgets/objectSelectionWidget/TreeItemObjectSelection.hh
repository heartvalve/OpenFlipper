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

#ifndef TREEITEM_HH
#define TREEITEM_HH

#include <QString>
#include <QList>
#include <vector>

#include <OpenFlipper/common/Types.hh>

class TreeItemObjectSelection {

  public :

    TreeItemObjectSelection(int _id, QString _name, DataType _type, TreeItemObjectSelection* _parent);

  // static members
  public:
    /// id
    int id();

    /// dataType
    DataType dataType();
    bool dataType(DataType _type);

    /// group
    int group();
    bool isGroup();

  private:
    int id_;
    DataType dataType_;

  // dynamic members
  public:

    /// visible
    bool visible();
    void visible(bool _visible);

    /// name
    QString name( );
    void name( QString _name );

  private:
    bool visible_;
    QString name_;

  // tree traversal
  public:

    /** Get the next item of the tree (Preorder traversal of the tree)
     */
    TreeItemObjectSelection* next();

    /** level of the current object ( root node has level 0)
     */
    int level();

  private:
    /// Parent item or 0 if rootnode
    TreeItemObjectSelection *parentItem_;

    /// Children of this node
    QList<TreeItemObjectSelection*> childItems_;

  public:
    //===========================================================================
    /** @name Tree : Parent nodes
    * @{ */
    //===========================================================================

    /// get the row of this item from the parent
    int row() const;

    /// Get the parent item ( 0 if rootitem )
    TreeItemObjectSelection *parent();

    /// Set the parent pointer
    void setParent(TreeItemObjectSelection* _parent);

    /** @} */

    //===========================================================================
    /** @name Tree : Children
    * @{ */
    //===========================================================================

    /// Check if the element exists in the subtree of this element
    TreeItemObjectSelection* childExists(int _objectId);

    /// Check if the element exists in the subtree of this element
    TreeItemObjectSelection* childExists(QString _name);

    /// add a child to this node
    void appendChild(TreeItemObjectSelection *child);

    /// return a child
    TreeItemObjectSelection *child(int row);

    /// get the number of children
    int childCount() const;

    /// Remove a child from this object
    void removeChild( TreeItemObjectSelection* _item );

    /// get all leafes of the tree below this object ( These will be all visible objects )
    QList< TreeItemObjectSelection* > getLeafs();

    /// delete the whole subtree below this item ( The item itself is not touched )
    void deleteSubtree();

};


//=============================================================================
#endif // TREEITEM_HH defined
//=============================================================================
