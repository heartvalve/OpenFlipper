#ifndef TREEITEM_HH
#define TREEITEM_HH

#include <QString>
#include <QList>
#include <vector>

#include <OpenFlipper/common/Types.hh>

class TreeItem {

  public :

    TreeItem(int _id, QString _name, DataType _type, TreeItem* _parent);

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

    /// Check if the element exists in the subtree of this element
    TreeItem* childExists(QString _name);

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
