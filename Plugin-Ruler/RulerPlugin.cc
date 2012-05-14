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
*   $Revision: 14395 $                                                       *
*   $LastChangedBy: moeller $                                                *
*   $Date: 2012-04-16 14:46:02 +0200 (Mon, 16 Apr 2012) $                     *
*                                                                            *
\*===========================================================================*/


#include "RulerPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <MeshTools/MeshFunctions.hh>
#include <MeshTools/MeshInfoT.hh>
//------------------------------------------------------------------------------
RulerPlugin::RulerPlugin()
:
buttonAction_(0),
pickModeName_("MeasureDistance"),
lineNodeName_("Ruler Line"),
textNodeName_("Ruler Text"),
textTransformNodeName_("Ruler Text Transform"),
hitStage_(firstClick),
hitPoints_( ),
lineNode_(0),
textNode_(0),
textTransformNode_(0),
lineDrag_(-1)

{

}

//------------------------------------------------------------------------------
RulerPlugin::~RulerPlugin()
{

}
//------------------------------------------------------------------------------
void RulerPlugin::showDistance()
{
  ACG::Vec3d Point1 = hitPoints_[0];
  ACG::Vec3d Point2 = hitPoints_[1];

  lineNode_->clear_points();
  lineNode_->set_color(OpenMesh::Vec4f(1.0f,0.0f,0.0f,1.0f));
  lineNode_->set_line_width(3);
  lineNode_->add_line(Point1,Point2);

  ACG::Vec3d distVec = Point1 - Point2;
  QString distanceStr = QString().number((distVec).length());
  textNode_->setText(distanceStr.toStdString());
  textNode_->multipassNodeSetActive(8, true);
  textNode_->setSize(5);

  textTransformNode_->loadIdentity();
  textTransformNode_->translate(ACG::Vec3d(Point2.data()[0],Point2.data()[1],bbmax_.data()[2]));
  ACG::Vec3d halfDist = distVec/2.f;
  textTransformNode_->translate(ACG::Vec3d(halfDist.data()[0], halfDist.data()[1], 0.0));
  textTransformNode_->scale(std::max(distVec.length()*0.2, 1.0));

  emit updateView();
}

//------------------------------------------------------------------------------
void RulerPlugin::initializePlugin()
{
  QToolBar *button = new QToolBar();

  buttonAction_ = new QAction(tr("<B>Ruler</B><br> Display the distance between two points."),this);

  button->addAction(buttonAction_);
  buttonAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"ruler.png"));
  buttonAction_->setCheckable(true);
  buttonAction_->setWhatsThis(tr("Measures the distance between two points.\n<LeftClick>: define/change the position.\n<DoubleClick>: reset the ruler."));

  buttonAction_->setChecked(false);

  connect(buttonAction_,SIGNAL(triggered()), this , SLOT(slotChangePickMode()) );
  emit addToolbar(button);
}

//------------------------------------------------------------------------------
void RulerPlugin::pluginsInitialized()
{
  emit addPickMode(pickModeName_);
}

//------------------------------------------------------------------------------
void RulerPlugin::slotMouseEvent(QMouseEvent* _event)
{
  if ( PluginFunctions::pickMode() == pickModeName_)
  {//set one of the points, depending on the hit state (first, second or modifying)

    if (_event->type() == QEvent::MouseButtonRelease )
    {
      unsigned int node_idx, target_idx;
      OpenMesh::Vec3d hitPoint;

      // Get picked object's identifier by picking in scenegraph
      if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING ,_event->pos(), node_idx, target_idx, &hitPoint) )
      {
        if (hitStage_ == firstClick)
        {
          //get or create the line
          BaseObjectData* object;
          if ( PluginFunctions::getPickedObject(node_idx, object) )
          {

            //create line, text and transformation nodes
            if (!object->getAdditionalNode(lineNode_,name(),lineNodeName_.c_str()))
            {
              lineNode_ = new ACG::SceneGraph::LineNode(ACG::SceneGraph::LineNode::LineSegmentsMode,
                  object->manipulatorNode(),lineNodeName_);
              object->addAdditionalNode(lineNode_,name(),lineNodeName_.c_str());
            }

            if (!object->getAdditionalNode(textTransformNode_,name(),textTransformNodeName_.c_str()))
            {
              textTransformNode_ = new ACG::SceneGraph::TransformNode(lineNode_,textTransformNodeName_.c_str());
              object->addAdditionalNode(textTransformNode_,name(),textTransformNodeName_.c_str());
            }

            if (!object->getAdditionalNode(textNode_,name(),textNodeName_.c_str()))
            {
              textNode_ = new ACG::SceneGraph::TextNode((OpenFlipper::Options::fontsDirStr() + OpenFlipper::Options::dirSeparator() + "freefont" + OpenFlipper::Options::dirSeparator() + "FreeSans.ttf").toStdString(),
                  textTransformNode_,textNodeName_,ACG::SceneGraph::TextNode::OBJECT_ALIGNED);
              object->addAdditionalNode(textNode_,name(),textNodeName_.c_str());

            }
          }//end creating nodes

          hitPoints_[1] = hitPoints_[0] = hitPoint;
          hitStage_ = secondClick;

          ACG::Vec3d bbmin;
          object->getBoundingBox(bbmin,bbmax_);

          showDistance();
          //enable mouse tracking for line drawing
          lineDrag_ = 1;
          emit setPickModeMouseTracking(pickModeName_,true);
        }
        else if (hitStage_ == secondClick)
        {
          lineDrag_ = -1;
          emit setPickModeMouseTracking(pickModeName_,false);
          //second position was clicked, so distance can be computed and displayed
          hitPoints_[1] = hitPoint;

          // show picked object
          showDistance();

          //after this stage, the points can be modified by a simple click
          hitStage_ = modifyPoint;
        }
        else if ((hitStage_ == modifyPoint))
        {
          //two modes:
          //first: no point was dragged, so we can compute and change the position
          //of the nearest one
          if (lineDrag_ < 0)
          {
            float firstDist = (hitPoints_[0] - hitPoint).length();
            float secondDist = (hitPoints_[1] - hitPoint).length();
            if (firstDist < secondDist)
              hitPoints_[0] = hitPoint;
            else
              hitPoints_[1] = hitPoint;
          }
          else
            //second: drag mode is enabled so we know the point
          {
            hitPoints_[lineDrag_] = hitPoint;
            lineDrag_ = -1;
          }
          showDistance();
        }
      }
      else // if nothing was picked
      {
        reset();
      }
    }
    else if (_event->type() == QEvent::MouseButtonPress)
    {//enable drag mode

      if (hitStage_ == modifyPoint)
      {
        //decides which point is the nearest one, so
        //it can be dragged
        unsigned int node_idx, target_idx;
        OpenMesh::Vec3d hitPoint;

        // Get picked object's identifier by picking in scenegraph
        if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING ,_event->pos(), node_idx, target_idx, &hitPoint) )
        {
          float firstDist = (hitPoints_[0] - hitPoint).length();
          float secondDist = (hitPoints_[1] - hitPoint).length();
          if (firstDist < secondDist)
            lineDrag_ = 0;
          else
            lineDrag_ = 1;

          showDistance();
        }
      }
    }
    else if (_event->type() == QEvent::MouseMove && lineDrag_ >= 0)
    {//mouse moved and drag mode is enabled

      unsigned int node_idx, target_idx;
      OpenMesh::Vec3d hitPoint;

      // Get picked object's identifier by picking in scenegraph
      if ( !PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING ,_event->pos(), node_idx, target_idx, &hitPoint) )
      {
        QPoint position = _event->pos();
        ACG::Vec3d viewCoords = ACG::Vec3d(position.x(), PluginFunctions::viewerProperties().glState().context_height() - position.y(), 0.5);
        hitPoint = PluginFunctions::viewerProperties().glState().unproject(viewCoords);
      }
      hitPoints_[lineDrag_] = ACG::Vec3d(hitPoint.data()[0], hitPoint.data()[1], hitPoints_[lineDrag_].data()[2] );
      showDistance();
    }
    else if (_event->type() == QEvent::MouseButtonDblClick)
    {//reset
      reset();
    }

  }
}

//------------------------------------------------------------------------------
void RulerPlugin::reset()
{
  hitStage_ = firstClick;
  lineNode_->clear_points();
  textNode_->setText("");
}

//------------------------------------------------------------------------------
void RulerPlugin::slotChangePickMode()
{
  PluginFunctions::actionMode( Viewer::PickingMode );
  PluginFunctions::pickMode(pickModeName_);

}

//------------------------------------------------------------------------------
void RulerPlugin::slotPickModeChanged(const std::string& _mode)
{
  buttonAction_->setChecked(_mode == pickModeName_);
}

Q_EXPORT_PLUGIN2( rulerPlugin , RulerPlugin );
