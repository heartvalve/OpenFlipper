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

//------------------------------------------------------------------------------
RulerPlugin::RulerPlugin()
:
buttonAction_(0),
pickModeName_("MeasureDistance"),
lineNodeName_("Ruler-Plugin LineNode"),
textNodeName_("Ruler-Plugin TextNode"),
textTransformNodeName_("Ruler-Plugin TransformNode"),
hitStage_(firstClick),
hitPoints_( ),
lineNode_(0),
textNode_(0),
textTransformNode_(0),
lineDrag_(-1),
dblClickCheck_(false)

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

  //creates the line
  lineNode_->clear_points();
  lineNode_->set_color(OpenMesh::Vec4f(1.0f,0.0f,0.0f,1.0f));
  lineNode_->set_line_width(3);
  lineNode_->add_line(Point1,Point2);

  //set params for the text
  ACG::Vec3d distVec = Point1 - Point2;
  QString distanceStr = QString().number((distVec).length());
  textNode_->setText(distanceStr.toStdString());
  textNode_->multipassNodeSetActive(8, true);
  textNode_->setSize(1);

  //translate and scale text
  textTransformNode_->loadIdentity();
  textTransformNode_->translate(Point1);
  ACG::Vec3d halfDist = distVec/2.f;
  textTransformNode_->translate(-halfDist);
  textTransformNode_->scale(distVec.length()*0.125);

  emit updateView();
}

//------------------------------------------------------------------------------
void RulerPlugin::initializePlugin()
{
  QToolBar *button = new QToolBar("Ruler");

  buttonAction_ = new QAction(tr("<B>Ruler</B><br> Display the distance between two points."),this);

  button->addAction(buttonAction_);
  buttonAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"ruler.png"));
  buttonAction_->setCheckable(true);
  WhatsThisGenerator whatsThisGen("Ruler");
  whatsThisGen.setWhatsThis(buttonAction_,tr("Measures the distance between two points.<br><b>LeftClick</b>: define/change the position.<br><b>DoubleClick</b>: reset the ruler.<br>"));

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
  if ( PluginFunctions::pickMode() != pickModeName_)
    return;

  //set one of the points, depending on the hit state (first, second or modifying)

  if (_event->type() == QEvent::MouseButtonRelease )
  {
    unsigned int node_idx, target_idx;
    OpenMesh::Vec3d hitPoint;

    // Get picked object's identifier by picking in scenegraph
    if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING ,_event->pos(), node_idx, target_idx, &hitPoint) && !dblClickCheck_)
    {
      if (hitStage_ == firstClick)
      {
        //get or create the line
        BaseObjectData* object;
        if ( PluginFunctions::getPickedObject(node_idx, object) )
        {

          //create line, point, text and transformation nodes
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
            textNode_ = new ACG::SceneGraph::TextNode(textTransformNode_,textNodeName_,ACG::SceneGraph::TextNode::SCREEN_ALIGNED,true);
            object->addAdditionalNode(textNode_,name(),textNodeName_.c_str());

          }

        }//end creating nodes

        hitPoints_[1] = hitPoints_[0] = hitPoint;
        hitStage_ = secondClick;

        enableDragMode(1);
        showDistance();
      }
      else if (hitStage_ == secondClick)
      {
        disableDragMode();
        //second position was clicked, so distance can be computed and displayed
        hitPoints_[1] = hitPoint;

        // show picked object
        showDistance();

        //after this stage, the points can be modified by a simple click
        hitStage_ = modifyPoint;
      }
      else if (hitStage_ == modifyPoint)
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
          //second: drag mode is enabled so we can easily update the position
        {
          hitPoints_[lineDrag_] = hitPoint;
          disableDragMode();
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
        enableDragMode( (firstDist < secondDist)? 0 : 1);
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
      //if mouse is not over an object, get the unprojected coordinates and the last z-value
      QPoint position = _event->pos();
      ACG::Vec3d viewCoords = ACG::Vec3d(position.x(), PluginFunctions::viewerProperties().glState().context_height() - position.y(), 0.5);
      hitPoint = PluginFunctions::viewerProperties().glState().unproject(viewCoords);
      hitPoints_[lineDrag_] = ACG::Vec3d(hitPoint.data()[0], hitPoint.data()[1], hitPoints_[lineDrag_].data()[2] );
    }
    else
      hitPoints_[lineDrag_] = hitPoint;
    showDistance();
  }
  else if (_event->type() == QEvent::MouseButtonDblClick)
  {//reset
    reset();
    dblClickCheck_ = true;
  }
  if (dblClickCheck_ && _event->type() == QEvent::MouseButtonRelease)
    dblClickCheck_ = false;
}

//------------------------------------------------------------------------------
void RulerPlugin::reset()
{
  hitStage_ = firstClick;
  lineDrag_ = -1;
  if (lineNode_)
    lineNode_->clear_points();
  if (textNode_)
    textNode_->setText("");
}
//------------------------------------------------------------------------------
void RulerPlugin::enableDragMode(const int _point)
{
  lineDrag_ = _point;
  emit setPickModeMouseTracking(pickModeName_,true);
}

//------------------------------------------------------------------------------
void RulerPlugin::disableDragMode()
{
  lineDrag_ = -1;
  emit setPickModeMouseTracking(pickModeName_,false);
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

//------------------------------------------------------------------------------
void RulerPlugin::slotAllCleared()
{
  hitStage_ = firstClick;
  lineDrag_ = -1;

  lineNode_ = 0;
  textNode_ = 0;
  textTransformNode_ = 0;
}

Q_EXPORT_PLUGIN2( rulerPlugin , RulerPlugin );
