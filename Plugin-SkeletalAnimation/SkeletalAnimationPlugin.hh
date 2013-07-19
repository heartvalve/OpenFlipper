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
*   $LastChangedBy$                                                 *
*   $Date$                    *
*                                                                            *
\*===========================================================================*/

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

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-SkeletalAnimation")
#endif

private:

  /// Pose typedef
  typedef PoseT<Skeleton::Point>  Pose;

signals:
  void updateView();

  void log(Logtype _type, QString _message);
  void log(QString _message);
  void addEmptyObject(DataType _type, int& _id);
  void updatedObject(int _id, const UpdateType& _type);
    
  void pluginExists(QString _pluginName, bool &_exists);
  void functionExists(QString _pluginName, QString _functionName, bool &_exists);

  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );   

public slots:
  // BaseInterface
  void slotAllCleared();
  void slotObjectUpdated( int _id, const UpdateType& _type);
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
  bool detachSkin(int skeletonId, int skinId);
  bool clearSkins(int skeletonId);

  void setFrame(int _iFrame);
  int  getFrame();
  void changeFPS(int _fps);

  void playAnimation();
  void pauseAnimation();
  void stopAnimation();
  void nextFrame();
  void prevFrame();

  void updateSkins();
  //@}

  void UpdateUI();
  void setComboBoxPosition(unsigned int _animationIndex);
  void checkObjectSelection(const int _objectId);

  void slotAnimationNameChanged();

private slots:
  void setDescriptions();
  
  void initializePlugin();
  void pluginsInitialized();
  
  void exit();

  void slotAttachSkin();
  void slotClearSkins();
  void slotMethodChanged(int _index);
  void slotAnimationIndexChanged(int /*_index*/);
  void slotFrameChanged(int /*_index*/);
  void slotSkipFramesChanged(int _state);
  void slotAddAnimation();
  void slotDeleteAnimation();

  void animate();

public:
  SkeletalAnimationPlugin();
  QString name();
  QString description();

protected:

  AnimationHandle currentAnimationHandle();

  void UpdateSkins(BaseObjectData *_pSkeletonObject, AnimationHandle &_hAni);

private:
  /**
  * @name Skin management
  */
  //@{
  void attachSkin(BaseObjectData *pSkin, BaseObjectData *pSkeleton);
  void detachSkin(BaseObjectData *pSkin, BaseObjectData *pSkeleton);
  void clearSkins(BaseObjectData *_pSkeleton);
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
};
