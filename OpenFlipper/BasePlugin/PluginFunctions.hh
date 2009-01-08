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

#include <OpenFlipper/common/Types.hh>

// #include <ACG/Math/VectorT.hh>
#include <ACG/QtWidgets/QtExaminerViewer.hh>
#include <ACG/QtWidgets/QtBaseViewer.hh>
#include <ACG/GL/GLState.hh>

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
bool get_picked_object(const unsigned int _node_idx , BaseObjectData*& _object);

/** \brief Delete the object with the given id
 *
 *  Deletes the given object ( you have to emit the right signals yourself)\n
 *  updateView , objectListUpdate ... )
 * @return successful?
 * */
DLLEXPORT
bool deleteObject( const int _id );

/** \brief Delete all objects
 *
 * */
DLLEXPORT
void deleteAll( );

/** \brief Create a copy of the object with the given id
 *
 * Creates a copy of an object. All scenegraph nodes will be created. The object will
 * not be part of the object tree. This has to be done by setParent of the object.
 * @return Pointer to new object or 0 if failed;
 * */
DLLEXPORT
int copyObject( const int _id );




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
bool get_source_identifiers( std::vector<int>& _identifiers  );

/** \brief Get the identifiers of all objects marked as a target object.
 *
 * @param _identifiers ( vector returning the target object identifiers )
 * @return false, if no object is selected as target
*/
DLLEXPORT
bool get_target_identifiers( std::vector<int>& _identifiers  );

/** \brief Get identifiers of all meshes
 *
 * @param _identifiers ( vector returning the identifiers )
 * @return false, if no mesh is found
*/
DLLEXPORT
bool get_all_meshes( std::vector<int>& _identifiers  );

/** \brief Get identifiers of all objects
 *
 * @param _identifiers ( vector returning the identifiers )
 * @return false, if no mesh is found
*/
DLLEXPORT
bool get_all_object_identifiers( std::vector<int>& _identifiers  );


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
bool get_object(  const int _identifier , BaseObject*& _object );

/** This functions returns the object with the given id regardless of the type of object.
 * See get_object(  int _identifier , BaseObject*& _object ) for more details.
 */
DLLEXPORT
bool get_object(  const int _identifier , BaseObjectData*& _object );

/** \brief Check if an object with this identifier exists.
 *
 * Searches through the Data containers and checks if the object with the given identifier exists
 * @param _identifier  Object id to search for
 */
DLLEXPORT
bool object_exists(  const int _identifier );

/// Get the number of available objects
DLLEXPORT
int object_count();

/// Get the number of target objects
DLLEXPORT
int target_count();

/// Get the number of source objects
DLLEXPORT
int source_count();

/// Get the number of visible objects
DLLEXPORT
int visible_count();

/** @} */

//=======================================
// Get/set status of examiner
    /** @name Examiner handling
    * @{ */
//=======================================
/// Set the internal examiner pointer ( DO NOT USE!! )
DLLEXPORT
void set_examiner( std::vector< ACG::QtWidgets::QtExaminerViewer* > _examiner_widgets );

/// Set the active id of the examiner which got the last mouse events
DLLEXPORT
void setActiveExaminer( const unsigned int _id );

/// Set the internal scenegraph root node pointer ( DO NOT USE!! )
DLLEXPORT
void set_sceneGraphRootNode( SeparatorNode* _root_node );

/// Set the internal data root node pointer ( DO NOT USE!! )
DLLEXPORT
void set_rootNode( SeparatorNode* _root_node );

/** Execute picking operation on scenegraph
 * This picking function will pick in the last active examiner context which is automatically
 * Set by mouseevents from the core
 */
DLLEXPORT
bool scenegraph_pick( ACG::SceneGraph::PickTarget _pickTarget, const QPoint &_mousePos, unsigned int &_nodeIdx, unsigned int &_targetIdx, ACG::Vec3d *_hitPointPtr );

/** Execute picking operation on scenegraph
 * This picking function will pick in the specified examiner context
 */
DLLEXPORT
bool scenegraph_pick( const unsigned int _examiner ,ACG::SceneGraph::PickTarget _pickTarget, const QPoint &_mousePos, unsigned int &_nodeIdx, unsigned int &_targetIdx, ACG::Vec3d *_hitPointPtr );

/** Execute Scenegraph traversal with action and use the last active examiner
 *  If you are reacting on a mouseEvent you should use this function as it will
 *  automatically set the right view
 */
DLLEXPORT
void traverse( ACG::SceneGraph::MouseEventAction  &_action );

/// Execute Scenegraph traversal with action and a specified examiner
void traverse(  const unsigned int _examiner, ACG::SceneGraph::MouseEventAction  &_action );

/// Get the current Picking mode
DLLEXPORT
const std::string & pickMode ();

/// Set the current Picking mode for all examiner widgets
DLLEXPORT
void pickMode ( std::string _mode);

/// Set pick mode for a specific examiner
DLLEXPORT
void pickMode ( const unsigned int _examiner, std::string _mode);

/// Get the current gl state from examiner
DLLEXPORT
ACG::GLState&  glState();

/// Returns a QImage of the current View
DLLEXPORT
void getCurrentViewImage(QImage& _image);

/// Get the root node
DLLEXPORT
ACG::SceneGraph::BaseNode* getSceneGraphRootNode();

/// Get the root node
DLLEXPORT
ACG::SceneGraph::BaseNode* getRootNode();

/// Add a node under the root node
DLLEXPORT
void addNode(ACG::SceneGraph::BaseNode* _node);

/// Add a node between root node and its children
DLLEXPORT
void addGlobalNode(ACG::SceneGraph::BaseNode* _node);

/// Set the current Action Mode (PickMode,ExamineMode,...)
DLLEXPORT
void actionMode ( ACG::QtWidgets::QtBaseViewer::ActionMode _mode);

/// Get the current Action mode
DLLEXPORT
ACG::QtWidgets::QtBaseViewer::ActionMode actionMode();

/** Lock scene rotation via mouse
 *
 * @param _mode allow or disallow rotation
 */
DLLEXPORT
void allowRotation(bool _mode);


/** @} */


//=======================================
// View settings
    /** @name View settings
    * @{ */
//=======================================

/** \brief Set the viewing direction
 * @param _dir direction
 * @param _up up vector
 */
DLLEXPORT
void viewingDirection(const ACG::Vec3d &_dir, const ACG::Vec3d &_up);

/** \brief Set the Scene position
 * @param _center Center of the current scene
 * @param _radius Radius of the scene ( Use scene_radius to get the current radius )
 */
DLLEXPORT
void setScenePos(const ACG::Vec3d& _center, const double _radius);

/** \brief Set the scene position (Same as  setScenePos(const ACG::Vec3d& _center, double _radius) )
 *
 */
DLLEXPORT
void setScenePos(const ACG::Vec3d& _center);

/** \brief Get the current scene center
 *
 */
DLLEXPORT
const ACG::Vec3d& sceneCenter();

/** \brief Returns the current scene radius from the examiner widget
 *
 * Returns the Radius of the scene
 */
DLLEXPORT
double sceneRadius();

/** \brief Translate viewer pos by given vector
 *
 * Translates the scene by a given vector. ( This is only a view transformation and does not
 * effect the scene center. To really translate the scene, use setScenePos );
 * @param _vector translation
 */
DLLEXPORT
void translate( const ACG::Vec3d &_vector );

/** \brief Rotate Scene around axis
 *
 * Rotates the current scene.
 * @param _axis   Rotation axis
 * @param _angle  Rotation Angle
 * @param _center Rotation Center
 */
DLLEXPORT
void rotate(const ACG::Vec3d&  _axis,
            const double       _angle,
            const ACG::Vec3d&  _center);


/** \brief Go to home position
 */
DLLEXPORT
void viewHome();

/** \brief View the whole scene
 */
DLLEXPORT
void viewAll();

/** \brief Get the current viewing Direction
 */
DLLEXPORT
ACG::Vec3d viewingDirection();

/** \brief Get the current viewer position
 */
DLLEXPORT
ACG::Vec3d eyePos();

/** \brief Get the current up vector
 */
DLLEXPORT
ACG::Vec3d upVector();

/** \brief Switch to orthographic Projection
 *
 */
DLLEXPORT
void orthographicProjection();

/** \brief Switch to perspective Projection
 *
 */
DLLEXPORT
void perspectiveProjection();

/** \brief Switch to a different draw mode
 *
 */
DLLEXPORT
void setDrawMode( const unsigned int _mode );

/** \brief Get the current draw Mode
 *
 */
DLLEXPORT
unsigned int drawMode( );

/** \brief Set the background color of the examiner widget.
 *
 */
DLLEXPORT
void setBackColor( OpenMesh::Vec4f _color);


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

/** @} */


//=======================================
// Do animations in examiner viewer
    /** @name Animations
    * @{ */
//=======================================

/**  Fly to point and set new viewing direction (animated).
 * @param _position New viewer position ( the new eye point of the viewer )
 * @param _center   The new scene center ( the point we are looking at )
 * @param _time     Animation time in ms
 */
DLLEXPORT
void flyTo (const ACG::Vec3d &_position, const ACG::Vec3d &_center, double _time=1000.0);

/** @} */


//=======================================
// Iterators for object Access
    /** @name Iterators
    * @{ */
//=======================================

enum IteratorRestriction {
   ALL_OBJECTS,
   TARGET_OBJECTS,
   SOURCE_OBJECTS
};

/** \brief Core Data Iterator
 *
 * This is the core iterator for the whole framework. You should use this iterator to access your data.\n
 * You can Choose if the iterator returns only Target, Source or all objects.\n
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
   bool operator==( const ObjectIterator& _rhs);

   /// compare iterators
   bool operator!=( const ObjectIterator& _rhs);

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

// /// Return Iterator to Mesh End
// MeshIterator meshes_end();

/// Return Iterator to Object End
DLLEXPORT
ObjectIterator objects_end();

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

/// Get the root of the object structure
DLLEXPORT
BaseObject*& objectRoot();

}

#endif //PLUGINFUNCTIONS_HH
