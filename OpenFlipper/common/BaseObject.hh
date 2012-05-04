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

//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalDefines.hh>
#include <OpenFlipper/common/DataTypes.hh>
#include <OpenFlipper/common/UpdateType.hh>
#include <QObject>
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
class DLLEXPORTONLY BaseObject : public QObject {
  
  Q_OBJECT 
  
  friend class BaseObjectData;

  public :

    /** Creates a copy of this Object. Currently it will not have any per Object data attached.
     *  Its automatically attached to the objectRoot.
     */
    BaseObject(const BaseObject& _object);

    /** Creates a new object. If the parent is 0 and the objectroot does not exist, it will have no parent.
    * If the objectroot exists and parent is 0, it will be appended to the objectroot.
    * If a parent is given, it is appended to this object.
    */
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
  /** @name Flag handling (source, target, ...)
   * @{ */
  //===========================================================================

  signals:
    void objectSelectionChanged(int _objectId);
    
  public:
    /** Is this item selected as a target item?
      * Most algorithms operate on target meshes. These meshes are also considered as active.
      * Blending for inactive meshes is handled by DataControlPlugin so emit objectSelectionChanged
      * if you changed this value.\n
      */
    bool target();

    /** Set this item as a target
     */
    void target(bool _target);

    /** Is this item selected as a source item?
      * Some algorithms use source meshes to define their input.
      */
    bool source();

    /** Set this item as a source
     */
    void source(bool _source);

    /** Get a custom flag of this item
     */
    bool flag(QString _flag);

    /** Set a custom flag on this item
     */
    void setFlag(QString _flag, bool _set);

    /** Get all flags of this item
     */
    QStringList flags();

  private:

    /** Stores all item flags as strings. Source and target flags are represented
      * as "source" and "target" strings.
      */
    QStringList flags_;


  /** @} */

  //===========================================================================
  /** @name Object visualization
   * @{ */
  //===========================================================================
  
  signals:
    /** This slot is emitted when the visibility of the object gets changed.
    *
    * The signal is normally handled by the core to tell the plugins that an object changed its visibility
    *
    */
    void visibilityChanged(int _objectId);


  public :
    /// return if object is visible
    virtual bool visible();

    /// Sets visiblity
    virtual void visible(bool _visible);

  protected :
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
    /** \brief  This function is called to update the object
     *
     * If the object changes, the core will call this function. Normally this will update
     * the corresponding scenegraph nodes or trigger other data handling which has to be done
     * when the object changes.
     *
     * \note Do not call this function yourself to avoid unnecessary overhead(the core will call it when it is required)
     */
    virtual void update(UpdateType _type = UPDATE_ALL);

    /// Debugging function, writing the subtree to output
    void dumpTree();

    /// Returns a full copy of the object
    virtual BaseObject* copy();

  /** @} */


  //===========================================================================
    /** @name Tree Structure
    * @{ */
  //===========================================================================

  public:

    /** Get the last item of the tree (Preorder traversal of the tree)
     */
    BaseObject* last();

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

  public:
    
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
    
  public:
    
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

  public:
    
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

    /** Set the object path and filename from the given parameter.
     *
     * @param _filename path to the file.
     */
    void setFromFileName(const QString &_filename);


    /// return the path to the object ( defaults to "." if unset )
    QString path();

    /// set the path to the object.
    void setPath(const QString &_path);

    /// return the name of the object. The name defaults to NONAME if unset.
    QString name( );


     /* set the name of the object. ( If you overwrite it, call BaseObject::setName(_name ) it in your funtion first)
     * this is may not the filename of the given object, because one file can have multiple objects
     */
    virtual void setName(QString _name );

    /// return the filename of the object
    QString filename();

    /// set the filename for this object
    void setFileName(const QString &_filename);

  private:

    /// path to the file from which the object is loaded ( defaults to "." )
    QString path_;
    QString filename_;

  signals:
    /** This signal is emitted when properties of the object have been changed like its name
    * or the parent changed
    */
    void objectPropertiesChanged(int _objectId);

  private:

    /// Object/FileName ( defaults to NONAME )
    QString name_;


    /** @} */

  //===========================================================================
  /** @name Object Payload
   * \anchor baseObjectPerObjectDataFunctions
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
    
    /** @} */
    
    //===========================================================================
    /** @name Object Payload functions for internal use only!
    * @{ */
    //===========================================================================
    
    /** \brief get reference to map of all perObject Datas
    *
    * Don't use this function! It's only for the backup Plugin to store and restore
    * perObjectDatas!
    */
    QMap<QString, PerObjectData*>& getPerObjectDataMap();
    
    
  private:

    QMap<QString, PerObjectData* > dataMap_;

  /** @} */

};


//=============================================================================
#endif // BASEOBJECT_HH defined
//=============================================================================
