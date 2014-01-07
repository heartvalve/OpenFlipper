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
//  Standard Functions
//
//=============================================================================

/**
 * \file PluginFunctions.hh
 * This file contains functions which can be used by plugins to access data in the framework.
 */

//
#ifndef PLUGINFUNCTIONS_HH
#define PLUGINFUNCTIONS_HH

#include <QPair>
#include <QFileDialog>

#include <OpenFlipper/common/Types.hh>

#include <ACG/Scenegraph/SceneGraph.hh>
#include <OpenFlipper/BasePlugin/PluginFunctionsViewControls.hh>

//== FORWARDDECLARATIONS ======================================================
class ViewObjectMarker;
class QGLWidget;

/** The Namespace PluginFunctions contains functions for all plugins. These functions should be used to get the
 *  objects to work on or to set modes in the examiner widget. */
namespace PluginFunctions {

//=======================================
// Get Source/Target objects
/** @name Active Objects
* @{ */
//=======================================

/** \brief Get the picked mesh
 * @param _node_idx Node index returned by examiner picking
 * @param _object returns the object which contains the mesh
 * @return true if mesh was found, false if picked object is not a mesh or not found
 */
DLLEXPORT
bool getPickedObject(const unsigned int _node_idx , BaseObjectData*& _object);

/** @} */

//=======================================
// Get Objects by their identifier
    /** @name Identifier handling
    * @{ */
//=======================================

/** \brief Get the identifiers of all objects marked as a source object.
 *
 * @param _identifiers ( vector returning the source object identifiers )
 * @return false, if no object is selected as source
*/
DLLEXPORT
bool getSourceIdentifiers( std::vector<int>& _identifiers  );

/** \brief Get the identifiers of all objects marked as a target object.
 *
 * @param _identifiers ( vector returning the target object identifiers )
 * @return false, if no object is selected as target
*/
DLLEXPORT
bool getTargetIdentifiers( std::vector<int>& _identifiers  );

/** \brief Get identifiers of all objects
 *
 * @param _identifiers ( vector returning the identifiers )
 * @return false, if no object is found
*/
DLLEXPORT
bool getAllObjectIdentifiers( std::vector<int>& _identifiers  );

/** \brief Get identifiers of all meshes
 *
 * @param _identifiers ( vector returning the identifiers )
 * @return false, if no mesh is found
*/
DLLEXPORT
bool getAllMeshes( std::vector<int>& _identifiers  );

/** \brief Get the object which has the given identifier.
 *
 *  Every loaded object has a unique identifier which is stored in the id field of the object container.
 *  Use this function to get the object which has this id. This can be used for a consistent mapping
 *  even if the data objects change during plugin operations (e.g. selection and main algorithm).\n
 *  This function only returns visible objects.
 * @param _identifier Object id to search for
 * @param _object  returns the object
 * @return Object found?
 */
DLLEXPORT
bool getObject(  const int _identifier , BaseObject*& _object );

/** This functions returns the object with the given id regardless of the type of object.
 * See getObject(  const int _identifier , BaseObject*& _object ) for more details.
 */
DLLEXPORT
bool getObject(  const int _identifier , BaseObjectData*& _object );

/** This functions returns the object's id with the given name.
 */
DLLEXPORT
int getObjectId( const QString& _name );

/** \brief Check if an object with this identifier exists.
 *
 * Searches through the Data containers and checks if the object with the given identifier exists
 * @param _identifier  Object id to search for
 */
DLLEXPORT
bool objectExists(  const int _identifier );

/// Get the number of available objects
DLLEXPORT
int objectCount();

/// Get the number of target objects
DLLEXPORT
int targetCount();

/// Get the number of source objects
DLLEXPORT
int sourceCount();

/// Get the number of visible objects
DLLEXPORT
int visibleCount();

/** @} */

//=======================================
// Get/set status of examiner
    /** @name Examiner handling
    * @{ */
//=======================================


/// Get the number of viewers
DLLEXPORT
int viewers( );

/** \brief Disable the core light handling
*
* Use this function to disable OpenFlippers Core light handling.
* Use this function only if the light is handled by your plugin
* Normally this function is called by the light plugin which
* fully takes control over the glLighting and adds its own light nodes.
*/
DLLEXPORT
void disableExaminerLightHandling();

/** \brief returns if internal light handling is active.
*
* Internal light handling could only be deactivated. From than on a plugin
* has to manage all light handling.
*/
DLLEXPORT
bool examinerLightHandling();

/// Set the active id of the examiner which got the last mouse events
DLLEXPORT
void setActiveExaminer( const unsigned int _id );

/// Get the id of the examiner which got the last mouse events
DLLEXPORT
unsigned int activeExaminer();

/// Get the encoded view for the active examiner
DLLEXPORT
QString getEncodedExaminerView();

/// Get the encoded view for the given
DLLEXPORT
QString getEncodedExaminerView(int _viewerId);

/// Set the encoded view for the active examiner
DLLEXPORT
void setEncodedExaminerView( QString _view );

/// Set the encoded view for the given
DLLEXPORT
void setEncodedExaminerView(int _viewerId , QString _view );

/**
 * Set center of scene
 */
DLLEXPORT
void setSceneCenter(const ACG::Vec3d& _center, int _viewer );

/** Execute picking operation on scenegraph
 * This picking function will pick in the last active examiner context which is automatically
 * Set by mouseevents from the core
 */
DLLEXPORT
bool scenegraphPick( ACG::SceneGraph::PickTarget _pickTarget, const QPoint &_mousePos, unsigned int &_nodeIdx, unsigned int &_targetIdx, ACG::Vec3d *_hitPointPtr );

/** Execute picking operation on scenegraph
 * This picking function will pick in the specified examiner context
 */
DLLEXPORT
bool scenegraphPick( const unsigned int _examiner ,ACG::SceneGraph::PickTarget _pickTarget, const QPoint &_mousePos, unsigned int &_nodeIdx, unsigned int &_targetIdx, ACG::Vec3d *_hitPointPtr );

/** Execute picking operation on scenegraph
 * This picking function will pick in the last active examiner context which is automatically
 * Set by mouseevents from the core
 */
DLLEXPORT
bool scenegraphRegionPick( ACG::SceneGraph::PickTarget                _pickTarget,
                           const QRegion&                             _region,
                           QList<QPair<unsigned int, unsigned int> >& _list,
                           QVector<float>*                            _depths = 0,
                           QVector<ACG::Vec3d>*                       _points = 0);

/** Execute picking operation on scenegraph
 * This picking function will pick in the specified examiner context
 */
DLLEXPORT
bool scenegraphRegionPick( const unsigned int                         _examiner,
                           ACG::SceneGraph::PickTarget                _pickTarget,
                           const QRegion&                             _region,
                           QList<QPair<unsigned int, unsigned int> >& _list,
                           QVector<float>*                            _depths = 0,
                           QVector<ACG::Vec3d>*                       _points = 0);

/** Execute Scenegraph traversal with action and use the last active examiner
 *  If you are reacting on a mouseEvent you should use this function as it will
 *  automatically set the right view
 */
DLLEXPORT
void traverse( ACG::SceneGraph::MouseEventAction  &_action );

/// Get the current Picking mode
DLLEXPORT
const std::string pickMode ();

/// Set the current Picking mode for all examiner widgets
DLLEXPORT
void pickMode ( const std::string& _mode);

/// Returns a QImage of the current View
DLLEXPORT
void getCurrentViewImage(QImage& _image);

/** \brief get scenegraph root node
*
* Get the real root node of the scenegraph.This node is the topmost
* node of the scenegraph. Normally you do not need to use this node.
* Except you are writing a renderer plugin.
* All objects should be added below the data root node which you can get
* with getRootNode().
*/
DLLEXPORT
ACG::SceneGraph::BaseNode* getSceneGraphRootNode();

/** \brief Get the root node for data objects
*
* Get the root node for the objects. This node is a separator node.
* All nodes belonging to objects have to be placed below this node.
* Add a separatornode for each object below this node! */
DLLEXPORT
ACG::SceneGraph::BaseNode* getRootNode();

/** \brief Add a global node
*
* The node will be added as a global node. Only the global status nodes
* will be above this node.
*/
DLLEXPORT
void addGlobalNode(ACG::SceneGraph::BaseNode* _node);

/** \brief Adds a global status node.
*
* The node will be added at the top of the scenegraph, before all other nodes except
* The scenegraphs real node. It will therefore influence all nodes in the scenegraph.
*/
void addGlobalStatusNode(ACG::SceneGraph::BaseNode* _node);

/** \brief Add scenegraph node modifing object rendering
*
* This function adds nodes in front of the object root node.
* Therefore all objects renderings will be modified by the
* state changes in the added node. This might be usefull for
* adding for example a slicing node, which adds clipping planes
* such that the objects will be sliced.
*/
DLLEXPORT
void addObjectRenderingNode(ACG::SceneGraph::BaseNode* _node);

/// Set the current Action Mode (PickMode,ExamineMode,...)
DLLEXPORT
void actionMode ( Viewer::ActionMode _mode);

/// Get the current Action mode
DLLEXPORT
Viewer::ActionMode actionMode();

/// Sets the main QGLWidget for gl data sharing.
DLLEXPORT
void shareGLWidget (QGLWidget* _widget);

/// Returns the main QGLWidget for gl data sharing.
DLLEXPORT
QGLWidget* shareGLWidget ();

/** Lock scene rotation via mouse
 *
 * @param _mode allow or disallow rotation
 */
DLLEXPORT
void allowRotation(bool _mode);

/** \brief Map coordinates of GL Widget to global coordinates
 *
 */
DLLEXPORT
QPoint mapToGlobal( const QPoint _point );

/** \brief Map global coordinates to GL Widget local coordinates
 *
 */
DLLEXPORT
QPoint mapToLocal( const QPoint _point );

/** Set current ViewObjectMarker (will be reseted to default on pick mode change)
 *
 * @param _marker Object marker
 */
DLLEXPORT
void setViewObjectMarker (ViewObjectMarker *_marker);

/** Set the default ViewObjectMarker
 *
 * @param _marker Object marker
 */
DLLEXPORT
void setDefaultViewObjectMarker (ViewObjectMarker *_marker);

/** Collect and return COMMENT properties of all meshes.
 */
DLLEXPORT
QStringList collectObjectComments(bool visibleOnly, bool targetedOnly);

/** Collect and return serialized materials of all meshes.
 */
DLLEXPORT
QStringList collectObjectMaterials(bool visibleOnly, bool targetedOnly);

/// Get the default ViewObjectMarker
DLLEXPORT
ViewObjectMarker* defaultViewObjectMarker ();

/** @} */


//=======================================
// Iterators for object Access
    /** @name Iterators
    * @{ */
//=======================================

typedef QStringList IteratorRestriction;

const QStringList ALL_OBJECTS;
const QStringList TARGET_OBJECTS ("target");
const QStringList SOURCE_OBJECTS ("source");

/** \brief Core Data Iterator
 *
 * This is the core iterator for the whole framework. You should use this iterator to access your data.\n
 * You can choose if the iterator returns only Target, Source or all objects.\n
 * Additionally you can set the type of objects returned by the iterator.
 * The ObjectIterator will only return real Objects. Groups are not considered to be objects
 */
class DLLEXPORT ObjectIterator {

   public :

      /// type of the Objects the iterator works on
      typedef BaseObjectData  value_type;

      /// handle type (just an int)
      typedef BaseObjectData* value_handle;

      /// reference type
      typedef value_type&     reference;

      /// basic pointer type
      typedef value_type*     pointer;

   /** \brief Use this constructor for iterating through your data.
    *
    * @param _restriction Use this parameter to define which objects will be returned.\n
    *                     You can select between ALL_OBJECTS , TARGET_OBJECTS , SOURCE_OBJECTS.
    * @param _dataType Use this parameter to select the returned object types.
    *                  You can use DATA_ALL DATA_POLY_MESH DATA_TRIANGLE_MESH DATA_VOLUME
    */
   ObjectIterator(IteratorRestriction _restriction = ALL_OBJECTS , DataType _dataType = DATA_ALL );

   /// additional constructor starting at a given position
   ObjectIterator(BaseObjectData* pos, IteratorRestriction _restriction = ALL_OBJECTS , DataType _dataType = DATA_ALL );

   /// return the current position of the iterator
   operator value_handle() { return pos_;  };

   /// compare iterators
   bool operator==( const ObjectIterator& _rhs) const;

   /// compare iterators
   bool operator!=( const ObjectIterator& _rhs) const;

   /// assign iterators
   ObjectIterator& operator=( const ObjectIterator& _rhs);

   /// dereference
   pointer operator->();

   /// next element
   ObjectIterator& operator++();

   /// last element
   ObjectIterator& operator--();

   /// dereference the iterator
   BaseObjectData* operator*();

   /// return current position of the iterator
   BaseObjectData* index() { return pos_; };

   private :
      /// current position of the iterator
      BaseObjectData* pos_;

      /// returned data types of the iterator
      DataType dataType_;

      /// Restriction of the iterator
      IteratorRestriction restriction_;

      /** Takes an object and goes through the object tree to the next BaseObjectData
        *  It stops at the root node.
        */
      inline void proceedToNextBaseObjectData(BaseObject*& _object);

};

/** \brief Core Data Iterator used to iterate over all objects (Including groups)
 *
 * This iterator is a more low level one not only returning really visible objects but also
 * Data containers ( e.g. groups... )
 * You can choose if the iterator returns only Target, Source or all objects.\n
 * Additionally you can set the type of objects returned by the iterator.
 */
class DLLEXPORT BaseObjectIterator {

   public :

      /// type of the Objects the iterator works on
      typedef BaseObject  value_type;

      /// handle type (just an int)
      typedef BaseObject* value_handle;

      /// reference type
      typedef value_type&     reference;

      /// basic pointer type
      typedef value_type*     pointer;

   /** \brief Use this constructor for iterating through your data.
    *
    * @param _restriction Use this parameter to define which objects will be returned.\n
    *                     You can select between ALL_OBJECTS , TARGET_OBJECTS , SOURCE_OBJECTS.
    * @param _dataType Use this parameter to select the returned object types.
    *                  You can use DATA_ALL DATA_POLY_MESH DATA_TRIANGLE_MESH DATA_VOLUME
    */
   BaseObjectIterator(IteratorRestriction _restriction = ALL_OBJECTS , DataType _dataType = DATA_ALL );

   /// additional constructor starting at a given position
   BaseObjectIterator(BaseObject* pos, IteratorRestriction _restriction = ALL_OBJECTS , DataType _dataType = DATA_ALL );

   /// return the current position of the iterator
   operator value_handle() { return pos_;  };

   /// compare iterators
   bool operator==( const BaseObjectIterator& _rhs);

   /// compare iterators
   bool operator!=( const BaseObjectIterator& _rhs);

   /// assign iterators
   BaseObjectIterator& operator=( const BaseObjectIterator& _rhs);

   /// dereference
   pointer operator->();

   /// next element
   BaseObjectIterator& operator++();

   /// last element
   BaseObjectIterator& operator--();

   /// dereference the iterator
   BaseObject* operator*();

   /// return current position of the iterator
   BaseObject* index() { return pos_; };

   private :
      /// current position of the iterator
      BaseObject* pos_;

      /// returned data types of the iterator
      DataType dataType_;

      /// Restriction of the iterator
      IteratorRestriction restriction_;

};

// /// Return Iterator to Mesh End
// MeshIterator meshes_end();

/// Return Iterator to Object End
DLLEXPORT
ObjectIterator objectsEnd();

/// Return Iterator to Object End
DLLEXPORT
BaseObjectIterator baseObjectsEnd();

/** @} */

//=======================================
// Dont Use functions below!!!
    /** @name Do never use!!
    * @{ */
//=======================================

/** Set the global DataContainer*/
DLLEXPORT
void setDataRoot( BaseObject* _root );

/** @} */

//=======================================
    /** @name Getting data from objects and casting between them
     * @{ */
//=======================================

/** \brief Cast an BaseObject to a BaseObjectData if possible
 *
 * @param _object The object should be of type BaseObject. If the content is a BaseObjectData, a
 *                a BaseObjectData is returned. Otherwise a NULL pointer is returned.
 */
DLLEXPORT
BaseObjectData* baseObjectData( BaseObject* _object );

/** @} */


/** \brief Return unique viewer id
 *
 * This function returns a id which is unique to all running Openflippers on that machine.
 * This id changes when you restart the viewer!
 */
DLLEXPORT
int viewerId();

/// Get the root of the object structure
DLLEXPORT
BaseObject*& objectRoot();

/**
 * The same as QFileDialog::getOpenFileName, except the dialog remembers its
 * last location within the file systems and opens at the same location the
 * next time.
 *
 * @param configProperty The name of the property in which to store the
 *                       last location. Should be of the form "Plugin-Foo/OpenBarFile".
 *
 * @param defaultDir If the property doesn't exist yet, defaultDir is used
 *                   as the initial location.
 *
 * @param parent       Parent qt widget
 * @param caption      Caption of the dialog
 * @param defaultDir   Default directory when dialog is shown
 * @param filter       name filters
 * @param selectedFilter currently selected filter
 * @param options      filedialog options
 *
 */
DLLEXPORT
QString getOpenFileName(const QString &configProperty,
                        QWidget * parent = 0, const QString & caption = QString(),
                        const QString & defaultDir = QString(), const QString & filter = QString(),
                        QString * selectedFilter = 0, QFileDialog::Options options = 0);
/**
 * The same as QFileDialog::getSaveFileName, except the dialog remembers its
 * last location within the file systems and opens at the same location the
 * next time.
 *
 * @param configProperty The name of the property in which to store the
 *                       last location. Should be of the form "Plugin-Foo/SaveBarFile".
 *
 * @param defaultDir If the property doesn't exist yet, defaultDir is used
 *                   as the initial location.
 *
 * @param parent       Parent qt widget
 * @param caption      Caption of the dialog
 * @param defaultDir   Default directory when dialog is shown
 * @param filter       name filters
 * @param selectedFilter currently selected filter
 * @param options      filedialog options
 */
DLLEXPORT
QString getSaveFileName(const QString &configProperty,
                        QWidget * parent = 0, const QString & caption = QString(),
                        const QString & defaultDir = QString(), const QString & filter = QString(),
                        QString * selectedFilter = 0, QFileDialog::Options options = 0);

}

#endif //PLUGINFUNCTIONS_HH
