#include <QObject>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/TextureInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <ACG/QtWidgets/QtExaminerViewer.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include "AnimationToolbox.hh"
#include "ObjectTypes/Skeleton/SkeletonObjectDataT.hh"
#include "ObjectTypes/Skeleton/Skeleton.hh"
#include "ObjectTypes/Skeleton/PoseT.hh"
#include "ObjectTypes/Skeleton/BaseSkin.hh"

/**
 * @brief The skeletal animation plugin is used to interact with the skeleton
 *
 * this plugin controls the playback of skeletal animations
 *
 */
class SkeletalAnimationPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, LoadSaveInterface, RPCInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(LoadSaveInterface)
  Q_INTERFACES(RPCInterface)

private:

  /// Pose typedef
  typedef PoseT<Skeleton::Point>  Pose;

signals:
  void updateView();

  void log(Logtype _type, QString _message);
  void log(QString _message);
  void addEmptyObject(DataType _type, int& _id);
  void updatedObject(int _id, const UpdateType _type);
    
  void pluginExists(QString _pluginName, bool &_exists);
  void functionExists(QString _pluginName, QString _functionName, bool &_exists);

  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );   

public slots:
  // BaseInterface
  void slotAllCleared();
  void slotObjectUpdated( int _id, const UpdateType _type);
  void slotObjectSelectionChanged(int _id);
  // LoadSaveInterface
  void fileOpened(int _id);
  void addedEmptyObject(int _id);
  void objectDeleted(int _id);

public slots:
  /**
    * @name Scripting interface
    * Use these methods in the script editor.
    */
  //@{
  int  getNumberOfFrames();
  bool attachSkin(int skeletonId, int skinId);
  bool detachSkin(int skeletonId);

  void setFrame(int _iFrame);
  int  getFrame();
  void changeFPS(int _fps);

  void playAnimation();
  void pauseAnimation();
  void stopAnimation();
  void nextFrame();
  void prevFrame();

  void updateSkin();
  //@}

  void UpdateUI();
  void checkObjectSelection();

private slots:
  void setDescriptions();
  
  void initializePlugin();
  void pluginsInitialized();
  
  void exit();

  void slotBindMesh();
  void slotUnbindMesh();
  void slotMethodChanged(int _index);
  void slotAnimationIndexChanged(int /*_index*/);
  void slotFrameChanged(int /*_index*/);
  void slotSkipFramesChanged(int _state);

  void animate();

public:
  SkeletalAnimationPlugin() : pToolbox_(0) {};
  QString name();
  QString description();

protected:

  AnimationHandle currentAnimationHandle();

  void UpdateSkin(BaseObjectData *_pSkeletonObject, AnimationHandle &_hAni);

private:
  /**
  * @name Rigging
  */
  //@{
  void rig(BaseObjectData *pSkin, BaseObjectData *pSkeleton);
  void unrig(BaseObjectData *_pSkeleton);
  //@}

protected:
  AnimationToolboxWidget *pToolbox_;         ///< A pointer to the toolbox widget
  QIcon* toolIcon_;

private:
  BaseSkin::Method method_; ///< The current blending method for the skin

  bool bGuiUpdating_;       ///< Used to drop a few messages while the gui is being updated
  QTimer animationTimer_;   ///< Timer used to control animations
  QTime animationTime_;     ///< Time since the animation was started, used to meet the given fps
  int animationOffset_;     ///< This frame was selected as the animation was started
  
  std::vector< int > activeSkeletons_;
  std::vector< int > activeMeshes_;
};
