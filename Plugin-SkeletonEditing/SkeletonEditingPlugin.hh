#ifndef SKELETONEDITINGPLUGIN_HH
#define SKELETONEDITINGPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/Skeleton/Skeleton.hh>
#include "SkeletonMarker.hh"

/** Plugin for creating and editing skeletons
*/
class SkeletonEditingPlugin : public QObject, BaseInterface, MouseInterface, KeyInterface, PickingInterface, ToolboxInterface, LoggingInterface, ScriptInterface,ToolbarInterface, ContextMenuInterface, LoadSaveInterface, BackupInterface, RPCInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(ContextMenuInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(ToolbarInterface)
  Q_INTERFACES(LoadSaveInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(RPCInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-SkeletonEditing")
#endif

  signals:
    // BaseInterface
    void updateView();
    void updatedObject(int, const UpdateType& _type);
    void nodeVisibilityChanged(int);

    void setRenderer(unsigned int _viewer, QString _rendererName);
    void getCurrentRenderer(unsigned int _viewer, QString& _rendererName);

    // PickingInterface
    void addPickMode( const std::string& _mode );
    void addHiddenPickMode( const std::string& _mode );
    void setPickModeMouseTracking (const std::string& _mode, bool _mouseTracking);
    void setPickModeToolbar (const std::string& _mode, QToolBar * _toolbar);

    // ContextMenuInterface
    void addContextMenuItem(QAction* _action , ContextMenuType _type);

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
    void addToolbox( QString _name  , QWidget* _widget );

    // LoadSaveInterface
    void addEmptyObject(DataType _type, int& _id);
    void deleteObject( int _id);

    // BackupInterface
    void createBackup( int _objectid, QString _name, UpdateType _type = UPDATE_ALL );

  private slots :

    // BaseInterface
    void initializePlugin();
    void pluginsInitialized();
    void slotObjectUpdated( int _id, const UpdateType& _type);

    // MouseInterface
    void slotMouseWheelEvent(QWheelEvent * _event, const std::string & _mode);
    void slotMouseEvent( QMouseEvent* _event );

    // KeyInterface
    void slotKeyEvent (QKeyEvent* _event);
    void slotKeyReleaseEvent (QKeyEvent* _event);

    // PickingInterface
    void slotPickModeChanged( const std::string& _mode);

  private:
    void moveJoint(QMouseEvent* _event);
    bool canModify(QMouseEvent* _event);
    void insertJoint(QMouseEvent* _event);
    void cancelJointInsertion();
    void deleteJoint(QMouseEvent* _event);
    void selectJoint(QMouseEvent* _event);

    Skeleton::Pose* activePose(SkeletonObject* _skeletonObj);
    void setJointPosition(SkeletonObject* _skeletonObj, Skeleton::Joint* _joint, ACG::Vec3d& _position);

    OpenMesh::Vec3d getNearestJoint(SkeletonObject* _skeletonObj, OpenMesh::Vec3d &_hitPoint, int& _bestJointID);
  public:

    /// Default Constructor
    SkeletonEditingPlugin();

    /// Destructor
    ~SkeletonEditingPlugin();

    /// Name of the Plugin
    QString name(){ return (QString("Skeleton Editing")); };

    /// Description of the Plugin
    QString description() { return (QString(tr("Create and edit skeletons"))); };


//===========================================================================
/** @name Scriptable Functions
  * @{ */
//===========================================================================

  public slots:
    /// insert a joint in the middle of a bone given by its (unique) tailJoint
    void splitBone( int _objectId, int _tailJoint);

    /// add joint to the skeleton
    void addJoint( int _objectId, int _parent, Vector _position );

    /// delete joint from the skeleton
    void deleteJoint( int _objectId , int _jointId );

    /// transform joint with given matrix
    void transformJoint( int _objectId , int _jointId, Matrix4x4 _matrix );

    /// get global matrix of a joint in the active pose
    Matrix4x4 globalMatrix(int _objectId, int _jointId);

    /// get local matrix of a joint in the active pose
    Matrix4x4 localMatrix(int _objectId, int _jointId);

    /// get global translation of a joint in the active pose
    Vector globalTranslation(int _objectId, int _jointId);

    /// get local translation of a joint in the active pose
    Vector localTranslation(int _objectId, int _jointId);

    /// get the number of animations
    int animationCount(int _objectId);

    /// get the number of frames
    int frameCount(int _objectId, int _animationIndex);

    /// get active animation
    int activeAnimation(int _objectId);

    /// get active frame
    int activeFrame(int _objectId);

    /// set active pose
    void setActivePose(int _objectId, int _animationIndex, int _frame);

    /// add animation
    void addAnimation(int _objectId, QString _name, int _frames);

    /// split selected Joint
    void splitJoint(QMouseEvent* _event);

/** @} */

  public slots:
    QString version() { return QString("1.0"); };


  private slots:
    void slotSetEditingMode(QAction* _action);
    void slotPickToolbarAction(QAction* _action);

    void setDescriptions();

  private:
    QToolBar*     toolbar_;
    QActionGroup* toolBarActions_;
    QAction*      skeletonEditingAction_;

    QToolBar*     pickToolbar_;
    QActionGroup* pickToolBarActions_;
    QAction*      selectJointAction_;
    QAction*      insertJointAction_;
    QAction*			splitJointAction_;
    QAction*      deleteJointAction_;
    QAction*      moveJointAction_;
    QAction*      transformChildManipAction_;
    QAction*      transformAllManipAction_;
    QAction*      rotateManipAction_;
    QAction*      inverseKinematicAction_;

    int currentSkeleton_;
    int currentJoint_;
    bool jointPreview_;
    
    bool transformChildJoints_;
    bool transformAllFrames_;
    bool inverseKinematic_;
    
    bool dblClick_;

    /// Stores the last active renderer before we switched to skeleton editing mode
    QString lastRenderer_;
    /// Remembers, if we changed the renderer
    bool rendererChanged_;

  private:
    // object ids of all objects with active Manipulator
    std::vector< int > activeManipulators_;
    // Size for the manipulators
    double manip_size_;
    // Modifier for the Size (changed by Mousewheel Events)
    double manip_size_modifier_;

    // Object marker to dimm Objects during manipulator transformation
    SkeletonMarker objectMarker_;

    ACG::Matrix4x4d accumMatrix_;

  private:
    /// Place and show the Manipulator
    void placeManip(QMouseEvent* _event);

    /// Show/Hide active manipulators
    void showManipulators();

    /// make sure the manipulator is positioned on a joint
    void updateManipulatorPosition(BaseObjectData* _skeletonObj);

    ///function for computing the position of our joints using inverse Kinematic
    void inverseKinematic(ACG::Vec3d dest,Skeleton::Pose* currentPose,Skeleton::Joint* pickedJoint, std::vector<Skeleton::Joint*> rotatableJoints);

  private slots:
    /// move the object when its manipulator moves
    void manipulatorMoved( QtTranslationManipulatorNode* _node , QMouseEvent* _event);

    /// update object when its manipulator changes position
    void ManipulatorPositionChanged(QtTranslationManipulatorNode* _node );

    void slotRotateManipulator(bool _toggled);
};

#endif //SKELETONEDITINGPLUGIN_HH
