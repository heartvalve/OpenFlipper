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


#include "RulerPlugin.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

//------------------------------------------------------------------------------
RulerPlugin::RulerPlugin()
:
buttonAction_(0),
pickModeName_("MeasureDistance"),
lineDrag_(-1),
dblClickCheck_(false),
optionsWidget_(0),
textSizeSettingName_(name()+QString("/TextSize"))
{

}

//------------------------------------------------------------------------------
RulerPlugin::~RulerPlugin()
{
  reset();
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

//////create or change ruler/////////
//set one of the points, depending on the hit state (first, second or modifying)
  if (_event->type() == QEvent::MouseButtonRelease )
  {
    unsigned int node_idx, target_idx;
    OpenMesh::Vec3d hitPoint;

    // Get picked object's identifier by picking in scenegraph
    if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING ,_event->pos(), node_idx, target_idx, &hitPoint) && !dblClickCheck_)
    {
      if (!currentRuler_)
      {
        //create a new Ruler
        BaseObjectData* object;
        if ( PluginFunctions::getPickedObject(node_idx, object) )
        {
          currentRuler_.reset(new Ruler(object,name(),0));
          connect(currentRuler_.get(),SIGNAL(updateView()),this,SIGNAL(updateView()));
          unsigned textSize = OpenFlipperSettings().value(textSizeSettingName_,16).toUInt();
          currentRuler_->setTextSize(textSize);
          currentRuler_->setPoints(hitPoint,hitPoint);
          enableDragMode(1);
        }

      }
      else
      {
        //Ruler was created -> change position of a point
        if (!dragModeActive())
        {
          //dragmode is disabled ->  update position of nearest point
          float distToStart = (currentRuler_->points()[0] - hitPoint).length();
          float distToEnd = (currentRuler_->points()[1] - hitPoint).length();
          if (distToStart < distToEnd)
            currentRuler_->setStartPoint(hitPoint);
          else
            currentRuler_->setEndPoint(hitPoint);
        }
        else
        {
          //second: drag mode is enabled so we can easily update the position
          if (lineDrag_ == 0)
            currentRuler_->setStartPoint(hitPoint);
          else
            currentRuler_->setEndPoint(hitPoint);
          disableDragMode();
        }
      }
    }
    else // if nothing was picked
    {
      reset();
    }
  }

//////enable drag mode//////
  else if (_event->type() == QEvent::MouseButtonPress)
  {//enable drag mode

    if (currentRuler_)
    {
      //decides which point is the nearest one, so
      //it can be dragged
      unsigned int node_idx, target_idx;
      OpenMesh::Vec3d hitPoint;

      // Get picked object's identifier by picking in scenegraph
      if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING ,_event->pos(), node_idx, target_idx, &hitPoint) )
      {
        float distToStart = (currentRuler_->points()[0] - hitPoint).length();
        float distToEnd = (currentRuler_->points()[1] - hitPoint).length();
        enableDragMode( (distToStart < distToEnd)? 0 : 1);
      }
    }
  }
////////modify ruler of drag mode was enabled/////////
  else if (_event->type() == QEvent::MouseMove && dragModeActive())
  {//mouse moved and drag mode is enabled

    unsigned int node_idx, target_idx;
    OpenMesh::Vec3d hitPoint;
    ACG::Vec3d hitPoints[2];
    std::copy(currentRuler_->points(),currentRuler_->points()+2,hitPoints);

    // Get picked object's identifier by picking in scenegraph
    if ( !PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING ,_event->pos(), node_idx, target_idx, &hitPoint) )
    {
      //if mouse is not over an object, get the unprojected coordinates and the last z-value
      QPoint position = _event->pos();
      ACG::Vec3d viewCoords = ACG::Vec3d(position.x(), PluginFunctions::viewerProperties().glState().context_height() - position.y(), 0.5);
      hitPoint = PluginFunctions::viewerProperties().glState().unproject(viewCoords);
      hitPoints[lineDrag_] = ACG::Vec3d(hitPoint.data()[0], hitPoint.data()[1], hitPoints[lineDrag_].data()[2] );
    }
    else
      hitPoints[lineDrag_] = hitPoint;

    currentRuler_->setPoints(hitPoints[0],hitPoints[1]);
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
  lineDrag_ = -1;
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
  disableDragMode();
}

void RulerPlugin::objectDeleted(int _id)
{
  if (!currentRuler_)
    return;

  disableDragMode();
  if (_id == currentRuler_->getBaseObj()->id())
  {
    disconnect(currentRuler_.get(),SIGNAL(updateView()),this,SIGNAL(updateView()));
    currentRuler_.reset();
  }
}
//------------------------------------------------------------------------------
bool RulerPlugin::initializeOptionsWidget(QWidget*& _widget)
{
  if (!optionsWidget_)
  {
    optionsWidget_ = new RulerOptions();
  }

  unsigned textSize = OpenFlipperSettings().value(textSizeSettingName_,16).toUInt();
  optionsWidget_->textSizeSpinBox->setValue(textSize);
  _widget = optionsWidget_;
  return true;
}
//------------------------------------------------------------------------------
void RulerPlugin::applyOptions()
{
  int textSize = optionsWidget_->textSizeSpinBox->value();
  if (currentRuler_)
    currentRuler_->setTextSize(textSize);
  OpenFlipperSettings().setValue(textSizeSettingName_,textSize);
}

Q_EXPORT_PLUGIN2( rulerPlugin , RulerPlugin );
