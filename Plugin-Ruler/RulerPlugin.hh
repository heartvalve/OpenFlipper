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
*   $Revision: 14388 $                                                       *
*   $LastChangedBy: moeller $                                                *
*   $Date: 2012-04-16 12:55:37 +0200 (Mon, 16 Apr 2012) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef RULERPLUGIN_HH
#define RULERPLUGIN_HH

#include <QObject>
#include <QString>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>

#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include <ACG/Scenegraph/LineNode.hh>
#include <ACG/Scenegraph/TextNode.hh>

class RulerPlugin : public QObject, BaseInterface, MouseInterface, PickingInterface, ToolbarInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(ToolbarInterface)

signals:
  void addToolbar(QToolBar *_toolbar);

  void addPickMode(const std::string &_mode);

  void updateView();

  void setPickModeMouseTracking  ( const std::string &_mode, bool  _mouseTracking);

public slots:

  void slotPickModeChanged(const std::string& _mode);

  void slotMouseEvent(QMouseEvent* _event);


public:

  RulerPlugin();
  ~RulerPlugin();

  QString name(){return QString("RulerPlugin");}
  QString description(){return QString("Measure the distance between two points.");}

private:

  void enableDragMode(const int _point);

  void disableDragMode();

  void reset();

  QAction *buttonAction_;


  const std::string pickModeName_;
  const std::string lineNodeName_;
  const std::string textNodeName_;
  const std::string textTransformNodeName_;

  enum HitStage
  {
    firstClick = 0,
    secondClick,
    modifyPoint
  } hitStage_;

  OpenMesh::Vec3d hitPoints_[2];

  ACG::SceneGraph::LineNode* lineNode_;
  ACG::SceneGraph::TextNode* textNode_;
  ACG::SceneGraph::TransformNode* textTransformNode_;

  //saves the index of the dragged point, if no point was dragged, it is -1
  int lineDrag_;

  //checks if a double click was provided for resetting
  bool dblClickCheck_;

private slots:

  void initializePlugin();

  void pluginsInitialized();

  void showDistance();

  void slotChangePickMode();



public slots:
  QString version(){ return QString("1.0"); }
};


#endif //RULERPLUGIN_HH
