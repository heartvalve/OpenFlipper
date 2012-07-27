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
*   $Revision: 10272 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2010-11-24 17:45:10 +0100 (Mi, 24 Nov 2010) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef TREEITEM_HH
#define TREEITEM_HH

#include <QString>
#include <QList>
#include <vector>

#include <OpenFlipper/common/Types.hh>

class TreeItem {

  public :

    TreeItem(int _id, QString _name, DataType _type, TreeItem* _parent);

    ~TreeItem();

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
    /// target
    bool target();
    void target(bool _target);

    /// source
    bool source();
    void source(bool _source);

    /// visible
    bool visible();
    void visible(bool _visible);

    /// name
    QString name( );
    void name( QString _name );

  private:
    bool target_;
    bool source_;
    bool visible_;
    QString name_;

  // tree traversal
  public:

    /** Get the next item of the tree (Preorder traversal of the tree)
     */
    TreeItem* next();

    /** level of the current object ( root node has level 0)
     */
    int level();

  private:
    /// Parent item or 0 if rootnode
    TreeItem *parentItem_;

    /// Children of this node
    QList<TreeItem*> childItems_;

  public:
    //===========================================================================
    /** @name Tree : Parent nodes
    * @{ */
    //===========================================================================

    /// get the row of this item from the parent
    int row() const;

    /// Get the parent item ( 0 if rootitem )
    TreeItem *parent();

    /// Set the parent pointer
    void setParent(TreeItem* _parent);

    /** @} */

    //===========================================================================
    /** @name Tree : Children
    * @{ */
    //===========================================================================

    /// Check if the element exists in the subtree of this element
    TreeItem* childExists(int _objectId);

    /// add a child to this node
    void appendChild(TreeItem *child);

    /// return a child
    TreeItem *child(int row);

    /// get the number of children
    int childCount() const;

    /// Remove a child from this object
    void removeChild( TreeItem* _item );

    /// get all leafes of the tree below this object ( These will be all visible objects )
    QList< TreeItem* > getLeafs();

    /// delete the whole subtree below this item ( The item itself is not touched )
    void deleteSubtree();

};


//=============================================================================
#endif // TREEITEM_HH defined
//=============================================================================
