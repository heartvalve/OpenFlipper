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

#ifndef MOVEPLUGIN_HH
#define MOVEPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/Plane/Plane.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#ifdef ENABLE_SKELETON_SUPPORT
  #include <ObjectTypes/Skeleton/Skeleton.hh>
#endif

#include "MoveToolbar.hh"
#include "MoveProps.hh"
#include "MoveObjectMarker.hh"

/** Plugin for moving objects and selections
*/
class MovePlugin : public QObject, BaseInterface, MouseInterface, KeyInterface, PickingInterface, ToolboxInterface, BackupInterface, LoggingInterface, ScriptInterface,ToolbarInterface, ContextMenuInterface, LoadSaveInterface, RPCInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(ContextMenuInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(ToolbarInterface)
  Q_INTERFACES(LoadSaveInterface)
  Q_INTERFACES(RPCInterface)

public:
  typedef unsigned int SelectionType;
  static const SelectionType VERTEX = 1;
  static const SelectionType EDGE   = 2;
  static const SelectionType FACE   = 4;

  signals:
    // BaseInterface
    void updateView();
    void updatedObject(int, const UpdateType& _type);
    void nodeVisibilityChanged(int);

    // PickingInterface
    void addPickMode( const std::string& _mode );
    void addHiddenPickMode( const std::string& _mode );
    void setPickModeMouseTracking (const std::string& _mode, bool _mouseTracking);
    void setPickModeToolbar (const std::string& _mode, QToolBar * _toolbar);

    // ContextMenuInterface
    void addContextMenuItem(QAction* _action , ContextMenuType _type);

    // BackupInterface
    void createBackup( int _objectid, QString _name, UpdateType _type = UPDATE_ALL);

    // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    // ScriptInterface
    void scriptInfo( QString _functionName  );
    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

    // ToolbarInterface
    void addToolbar(QToolBar* _toolbar);
    void getToolBar( QString _name, QToolBar*& _toolbar);

    // KeyInterface
    void registerKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse = false);
    
    // ToolboxInterface
    void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );
    
    // RPCInterface
    void pluginExists(QString _pluginName, bool &_exists);
    void functionExists(QString _pluginName, QString _functionName, bool &_exists);

  private slots :

    // BaseInterface
    void initializePlugin();
    void pluginsInitialized();
    
    void slotAllCleared();
    
    // LoadSaveInterface
    void objectDeleted( int _id );

    // MouseInterface
    void slotMouseWheelEvent(QWheelEvent * _event, const std::string & _mode);
    void slotMouseEvent( QMouseEvent* _event );

    // KeyInterface
    void slotKeyEvent (QKeyEvent* _event);
    void slotKeyReleaseEvent (QKeyEvent* _event);

    // PickingInterface
    void slotPickModeChanged( const std::string& _mode);

    // BackupInterface
    void slotBackupRequested( int /*_id*/ , QString /*_name*/ , int /*_backup_id*/){};
    void slotRestoreRequested( int /*_id*/ , QString /*_name*/ , int /*_backup_id*/){};

  public :

    /// Default Constructor
    MovePlugin();

    /// Destructor
    ~MovePlugin();

    /// Name of the Plugin
    QString name(){ return (QString("Move")); };

    /// Description of the Plugin
    QString description() { return (QString(tr("Moves Meshes with Manipulators"))); };

//===========================================================================
/** @name ToolBox
  * @{ */
//===========================================================================

  private:
    /// stores the current axes in the tool
    int axisA_;
    int axisB_;

  private slots:

    /// Position of manipulator in tab changed
    void slotSetPosition();

    /// Toggle the first axis for changing direction in tab
    void slotToggleAxisA();

    /// Toggle the second axis for changing direction in tab
    void slotToggleAxisB();

    /// Set Direction of manipulator in tab changed
    void slotSetDirection();

    /// perform a translation for Manipulator in tab
    void slotTranslation();

    /// Project the current manipulator onto the tangent plane of the object
    void slotProjectToTangentPlane();

    /// Move the current manipulator to the cog of the object
    void slotMoveManipToCOG();

    /// Rotate Manipulator (with values from Tab)
    void slotRotate();

    /// Scale (with values from Tab)
    void slotScale();

    /// Move target Meshes cog to the origin
    void slotMoveToOrigin();

    /// Scale Boundingbox Diagonal to unit size
    void slotUnifyBoundingBoxDiagonal();

    void slotEnableSelectionMode();

    void slotEnableObjectMode();

  private:

    /// Update the Dialog with the last clicked manipulator
    void updateManipulatorDialog();

    /// Get closest vertex to hitpoint
    template <typename MeshType>
    OpenMesh::Vec3d getNearestVertex(MeshType* _mesh, uint _fh, OpenMesh::Vec3d& _hitPoint);

    /// Get closest edge to hitpoint
    template <typename MeshType>
    OpenMesh::Vec3d getNearestEdge(MeshType* _mesh, uint _fh, OpenMesh::Vec3d& _hitPoint);

    /// Get closest face to hitpoint
    template <typename MeshType>
    OpenMesh::Vec3d getNearestFace(MeshType* _mesh, uint _fh, OpenMesh::Vec3d& _hitPoint);

    /// True if the toolbox widget is active
    bool toolboxActive_;

    /// Widget for Toolbox
    moveToolbarWidget* tool_;
    
    QIcon* toolIcon_;

/** @} */

//===========================================================================
/** @name ToolBar
  * @{ */
//===========================================================================

  private :
    QAction* moveAction_;
    QAction* moveSelectionAction_;
    
    QActionGroup* toolBarActions_;
    
    QToolBar* toolbar_;

  private slots:

    /// Called by Toolbar to enable move mode
    void slotSetMoveMode(QAction* _action);

/** @} */

//===========================================================================
/** @name PickToolBar
  * @{ */
//===========================================================================

  private :
    QToolBar* pickToolbar_;

    QAction* placeAction_;
    QAction* rotateTranslateAction_;
    QAction* rotateManipAction_;
    QAction* resizeAction_;

    QAction* biggerManipAction_;
    QAction* smallerManipAction_;

    QAction* fixChildManipAction_;
    QAction* transformRefPoseManipAction_;
    QAction* currentPoseManipAction_;
    
    QAction* placeAndSnapAction_;

    QActionGroup* pickToolBarActions_;

  private slots:

    /// Called by pick Toolbar
    void slotPickToolbarAction(QAction* _action);

/** @} */

//===========================================================================
/** @name Manipulator Handling
  * @{ */
//===========================================================================

  private:
    //object ids of all objects with active Manipulator
    std::vector< int > activeManipulators_;

  private slots:

    /// move the object when its manipulator moves
    void manipulatorMoved( QtTranslationManipulatorNode* _node , QMouseEvent* _event);

    /// update object when its manipulator changes position
    void ManipulatorPositionChanged(QtTranslationManipulatorNode* _node );

  private:

    ///Transform a mesh with the given transformation matrix
    template< typename MeshT >
    void transformMesh(ACG::Matrix4x4d _mat , MeshT& _mesh );

    #ifdef ENABLE_POLYLINE_SUPPORT
    ///Transform a polyline with the given transformation matrix
    template< class PolyLineT >
    void transformPolyLine( ACG::Matrix4x4d _mat , PolyLineT& _polyLine );
    #endif

    #ifdef ENABLE_SKELETON_SUPPORT
    ///Transform a skeleton with the given transformation matrix
    void transformSkeleton( ACG::Matrix4x4d _mat , Skeleton& _skeleton );
    #endif

    /** Get the Matrix of the last active Manipulator ( Identity if not found or hidden Manipulator )
      *
      * @param _reset reset the transformation matrix of the manipulator to identity)
      * @return current transformation matrix of the manipulator
      */
    ACG::Matrix4x4d getLastManipulatorMatrix(bool _reset = true);

    /// Checks if the manipulators should be visible or not
    void showManipulators( );

    /// Place and show the Manipulator
    void placeManip(QMouseEvent * _event, bool _snap = false);

    /// scale mesh to have a boundingboxdiagonal of one
    template< typename MeshT >
    void unifyBBDiag(MeshT& _mesh );

    /// get bounding box diagonal of a mesh
    template< typename MeshT >
    void getBB( MeshT& _mesh, ACG::Vec3d& _bb_min, ACG::Vec3d& _bb_max  );

    /// Scales object such that bounding box diagonal has unit length
    template< typename MeshT >
    void unifyBBDiag( MeshT& _mesh, ACG::Vec3d& _bb_min, ACG::Vec3d& _bb_max  );

    /// Size for the manipulators
    double manip_size_;

    /// Modifier for the Size (changed by Mousewheel Events)
    double manip_size_modifier_;

    /// Stores the last manipulator which has been clicked ( used for the toolbox dialog)
    int lastActiveManipulator_;

    /// Set Descriptions for scriptable functions
    void setDescriptions();

    /// Move an object with given id
    void moveObject(ACG::Matrix4x4d mat, int _id);

    /// Move selection on an object with given id
    void moveSelection(ACG::Matrix4x4d mat, int _id, QEvent::Type _type);

    /// Object marker to dimm Objects during manipulator transformation
    MoveObjectMarker objectMarker_;

  private:

    /// Holds the current manipulator mode
    QtTranslationManipulatorNode::ManipulatorMode manMode_;

    void setManipMode (QtTranslationManipulatorNode::ManipulatorMode _mode);

/** @} */

//===========================================================================
/** @name Selection Plugin Communication
  * @{ */
//===========================================================================

    /// Current SelectionType of SelectionPlugin
    SelectionType selectionType_;

    /// Get current primitive selection
    void updateSelectionType();

  private slots:

    /// Sets whether all targets should be affected or not
    void setAllTargets(bool _state);

/** @} */

  //===========================================================================
  /** @name Context Menu
   * @{ */
  //===========================================================================

    private slots:

    /// Show properties of move manipulator in a dialog ( Called via context for picking. Get the picked id from the Qvariant attached to the connected action)
    void showProps( );
    
    /// Hide the manipulator( Called via context for picking. Get the picked id from the Qvariant attached to the connected action)
    void hideManipulator();

    /// Get properties dialog widget that is attached to BaseDataObject obj
    movePropsWidget* getDialogWidget(BaseObjectData* _obj);

    /// Get parent properties dialog widget of QPushButton but
    movePropsWidget* getDialogFromButton(QPushButton* _but);

    /** Override ContextMenuInterface slot in order to avoid appearance of
     * context menu entry when clicking on other nodes (other than manpipulator nodes)
     */
    void slotUpdateContextMenuNode( int _nodeId );

    private:

    void setPickModeProps(movePropsWidget* _pW, const std::string &_pickmode);

    /// List of properties dialogs (each corresponding to one manipulator)
    QList<movePropsWidget*> propsWindows_;

    /// Context menu entry for showing per manipulator settings
    QAction* contextAction_;
    
    /// Context menu entry to hide a manipulator
    QAction* contextActionHide_;

    /// Checked if transformation should be applied to all target objs
    QAction* toAllTargets_;

/** @} */

//===========================================================================
/** @name Scriptable Functions
  * @{ */
//===========================================================================

public slots :

  /// translate an Object by a given vector
  void translate( int _objectId , Vector _vector );

  /// translate given vertices by a vector
  void translate( int _objectId , IdList _vHandles, Vector _vector );

  /// translate current vertex selection of an Object by a given vector
  void translateVertexSelection( int _objectId , Vector _vector );

  /// translate current face selection of an Object by a given vector
  void translateFaceSelection( int _objectId , Vector _vector );

  /// translate current edge selection of an Object by a given vector
  void translateEdgeSelection( int _objectId , Vector _vector );

  /// transform an Object by a given matrix
  void transform( int _objectId , Matrix4x4 _matrix );

  /// transform given vertices by a matrix
  void transform( int _objectId , IdList _vHandles, Matrix4x4 _matrix );

  /// transform current selection of an Object by a given matrix
  void transformVertexSelection( int _objectId , Matrix4x4 _matrix );

  /// transform current selection of an Object by a given matrix
  void transformFaceSelection( int _objectId , Matrix4x4 _matrix );

  /// transform current selection of an Object by a given matrix
  void transformEdgeSelection( int _objectId , Matrix4x4 _matrix );

  /// Transform handle region using the given transformation matrix
  void transformHandleRegion(int _objectId, Matrix4x4 _matrix);

  /// Set the position of the manipulator
  void setManipulatorPosition( int _objectId , Vector _position );

  /// Get the position of the manipulator
  Vector manipulatorPosition( int _objectId );

  /// Set the direction of the manipulator
  void setManipulatorDirection( int _objectId , Vector _directionX, Vector _directionY );

  /// Get the x-direction of the manipulator
  Vector manipulatorDirectionX( int _objectId );

  /// Get the y-direction of the manipulator
  Vector manipulatorDirectionY( int _objectId );

  /// Get the z-direction of the manipulator
  Vector manipulatorDirectionZ( int _objectId );
/** @} */

  public slots:
    QString version() { return QString("1.2"); };

  private:
    bool hide_;

    bool allTargets_;

    bool placeMode_;
};

#endif //MOVEPLUGIN_HH
