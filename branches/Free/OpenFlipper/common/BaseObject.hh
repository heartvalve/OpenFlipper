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




//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file BaseObject.hh
 * This File contains the Basic object class for all Objects (Includes also non visual objects such as Groups).
 */


#ifndef BASEOBJECT_HH
#define BASEOBJECT_HH


#ifndef DLLEXPORT
#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif
#endif

//== INCLUDES =================================================================

#include <QString>
#include <QList>
#include <QStringList>
#include <vector>
#include <QMap>
#include "perObjectData.hh"

//== TYPEDEFS =================================================================


//== CLASS DEFINITION =========================================================

/**
 * This is the basic Data class providing the functions common to all objects.
 * If the Datacontrol Plugin is available, a Tree structure will be generated allowing groups of objects.
 */
class DLLEXPORT BaseObject {
  friend class BaseObjectData;

  public :
    BaseObject( BaseObject *_parent = 0);

    virtual ~BaseObject();

  //===========================================================================
  /** @name Object Identification
   * @{ */
  //===========================================================================
  public:
    /** return the unique id of the object. These ids will be generated every time an object is created.
     * It will stay valid during the runtime of the application.
     */
    int id();

    /** return the persistent id of an object ( This id can be managed by a database ). It should
     * be persistent across program starts. It will be -1 if the object has not been registered by
     * a database. This id will only be set if a database plugin manages it.
     */
    int persistentId();

    /** set the persistent id of the object
     */
    void persistentId( int _id );

  private:
    /** \brief Unique ID for this Object
    *
    *  If you need to identify an Object in your plugin then use this id to find it. It does not change during the
    * Objects lifetime. In PluginFunctions.hh are some Functions which help with finding objects using this id.
    */
    int id_;

    /** \brief Persistent ID for this Object
    *
    *  If you need to identify an Object acroos program starts in your plugin then use this id to find it.
    * It will not change across program restarts. This id will only be set if a database plugin manages it.
    */
    int persistentId_;

  //===========================================================================
  /** @name Data Type Handling
    * @{ */
  //===========================================================================

  public:
    /** Check the if the object is of the given type
     * @param _type checks, if the object is of the given type
     */
    bool dataType(DataType _type);

    /** return the dataType of the object
     */
    DataType dataType();

    /** set the object type
     * @param _type the type of the object (if it has a type defined, it will output a warning)
     */
    void setDataType(DataType _type);

  private:
    /** This Field describes the Data Types available in the object. \n
     *  You should check this Field, if your plugin can handle the data in the object.
     */
    DataType objectType_;

  /** @} */

  //===========================================================================
  /** @name Data
   * @{ */
  //===========================================================================

  public:
    /** Clean all data structures of the object
      *
      * */
    virtual void cleanup();

  /** @} */

  //===========================================================================
  /** @name Object Information
   * @{ */
  //===========================================================================
  public:
    /// Get all Info for the Object as a string
    virtual QString getObjectinfo();

    /// Print all information about the object
    virtual void printObjectInfo();

  /** @} */

  //===========================================================================
  /** @name Source/target handling
   * @{ */
  //===========================================================================

  public:
    /** Is this item selected as a target item?
     */
    bool target();

    /** Set this item as a target
     */
    void target(bool _target);

    /** Is this item selected as a source item?
     */
    bool source();

    /** Set this item as a source
     */
    void source(bool _source);

  private:

    /** Is this item selected as a target item?
    * Most algorithms operate on target meshes. These meshes are also considered as active.
    * Blending for inactive meshes is handled by DataControlPlugin so emit updated_objects
     * if you changed this value.\n
    */
    bool target_;

    /** Is this item selected as a source item?
     * Some algorithms use source meshes to define their input.
     */
    bool source_;

  /** @} */

  //===========================================================================
  /** @name Object visualization
   * @{ */
  //===========================================================================

  public :
  /// return if object is visible
  bool visible();

  private :
    /** Show/hide/ Object\n
    * Visibility is handled by DataControlPlugin so emit updated_objects if you changed this value
    * defaults to visible
    */
    bool visible_;

  /** @} */

  //===========================================================================
  /** @name Content
   * @{ */
  //===========================================================================

  public:
    /// This function is called to update the object
    virtual void update();

    /// Debugging function, writing the subtree to output
    void dumpTree();

  /** @} */


  //===========================================================================
    /** @name Tree Structure
    * @{ */
  //===========================================================================

  public:

    /** Get the next item of the tree (Preorder traversal of the tree)
     */
    BaseObject* next();

    /** level of the current object ( root node has level 0)
     */
    int level();

  private:
    /// Parent item or 0 if rootnode
    BaseObject *parentItem_;

    /// Children of this node
    QList<BaseObject*> childItems_;

  public:
    //===========================================================================
    /** @name Tree : Parent nodes
    * @{ */
    //===========================================================================

    /// get the row of this item from the parent
    int row() const;

    /// Get the parent item ( 0 if rootitem )
    BaseObject *parent();

    /// Set the parent pointer
    void setParent(BaseObject* _parent);

    /** @} */

    //===========================================================================
    /** @name Tree : Children
    * @{ */
    //===========================================================================

    /// Check if the element exists in the subtree of this element
    BaseObject* childExists(int _objectId);

    /// Check if the element exists in the subtree of this element
    BaseObject* childExists(QString _name);

    /// add a child to this node
    void appendChild(BaseObject *child);

    /// return a child
    BaseObject *child(int row);

    /// get the number of children
    int childCount() const;

    /// Remove a child from this object
    void removeChild( BaseObject* _item );

    /// get all leafes of the tree below this object ( These will be all visible objects )
    QList< BaseObject* > getLeafs();

    /// delete the whole subtree below this item ( The item itself is not touched )
    void deleteSubtree();

    /** @} */

    //===========================================================================
    /** @name Grouping
    * @{ */
    //===========================================================================

    /** Return the primary group of this object or -1 if ungrouped.
     * As this is a tree structure this returns the first group of this object.
     * Groups of groups are only suppurted via the other functions.
     * @return Primary group of this object or -1
     */
    int group();

    /// Check if object is a group
    bool isGroup();

    /** Check if this item belongs to a group with this id
     *
     *  @param _id id of the group
     */
    bool isInGroup( int _id );

    /** Check if this item belongs to a group with this name
      *
      * @param _name Name of the group
      */
    bool isInGroup( QString _name );

    /** Get a vector of all Group ids this object belongs to ( this function omits the root object )
    */
    std::vector< int > getGroupIds();

    /** Get a vector of all group names  this object belongs to ( this function omits the root object )
     */
    QStringList getGroupNames();


    /** @} */

    //===========================================================================
    /** @name Name and Path handling
    * @{ */
    //===========================================================================

    /// return the name of the object. The name defaults to NONAME if unset.
    QString name( );

    /// set the name of the object. ( If you overwrite it, call BaseObject::setName(_name ) it in your funtion first)
    virtual void setName( QString _name );


  private:

    /// Object/FileName ( defaults to NONAME )
    QString name_;


    /** @} */

  //===========================================================================
  /** @name Object Payload
   * @{ */
  //===========================================================================

  public:

    /** Set a pointer to your object data
     * Your data class has to be derived from PerObjectData and should implement a destructor.
     * use dynamic_casts to cast between your object and the Baseclass.
     *
     * @param _dataName Define a name for your data
     * @param _data a pointer to your object data
     */
    void setObjectData( QString _dataName , PerObjectData* _data );

    /// Clear the object data pointer ( this will not delete the object!! )
    void clearObjectData( QString _dataName );

    /// Checks if object data with given name is available
    bool hasObjectData( QString _dataName );

    /// Returns the object data pointer
    PerObjectData* objectData( QString _dataName );

    /// Delete all data attached to this object ( calls delete on each object )
    void deleteData();

  private:

    QMap<QString, PerObjectData* > dataMap_;

  /** @} */

};


//=============================================================================
#endif // BASEOBJECT_HH defined
//=============================================================================
