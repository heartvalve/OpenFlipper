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


#include "SlicePlugin.hh"

#include <iostream>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <OpenFlipper/common/GlobalOptions.hh>

SlicePlugin::SlicePlugin() :
        tool_(0),
        toolIcon_(0),
        node_(0)
{

}

void SlicePlugin::initializePlugin(){
  //init the slice node
  node_ = new ACG::SceneGraph::ClippingNode(0,"Clipping Node");

 
  PluginFunctions::addObjectRenderingNode( node_ );

  node_->set_status( ACG::SceneGraph::BaseNode::HideNode );
  node_->setMultipassStatus(ACG::SceneGraph::BaseNode::NOPASS);
  
  tool_ = new SliceToolBox();
  
  QSize size(300, 300);
  tool_->resize(size);
  
  QButtonGroup* bbGroup = new QButtonGroup();
  bbGroup->setExclusive( true );
  bbGroup->addButton( tool_->radioAll );
  bbGroup->addButton( tool_->radioTarget );
  
  QButtonGroup* axisGroup = new QButtonGroup();
  axisGroup->setExclusive( true );
  axisGroup->addButton( tool_->radioX );
  axisGroup->addButton( tool_->radioY );
  axisGroup->addButton( tool_->radioZ );
  
  tool_->radioAll->setChecked( true );
  tool_->radioX->setChecked( true );
  
  connect(tool_->radioAll, SIGNAL( released() ), this, SLOT( updateSlice() ) );
  connect(tool_->radioTarget, SIGNAL( released() ), this, SLOT( updateSlice() ) );
  connect(tool_->resetButton, SIGNAL( released() ), this, SLOT( resetParameters() ) );
  connect(tool_->enabled, SIGNAL( released() ), this, SLOT( updateSlice() ) );
  connect(tool_->radioX, SIGNAL( released() ), this, SLOT( updateSlice() ) );
  connect(tool_->radioY, SIGNAL( released() ), this, SLOT( updateSlice() ) );
  connect(tool_->radioZ, SIGNAL( released() ), this, SLOT( updateSlice() ) );
  connect(tool_->posSlider, SIGNAL( sliderMoved(int) ), this, SLOT( updateSlice(int) ) );
  connect(tool_->sizeSlider, SIGNAL( sliderMoved(int) ), this, SLOT( updateSlice(int) ) );

  toolIcon_ = new QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"slice.png");
  emit addToolbox( tr("Slice") , tool_, toolIcon_);
}

void SlicePlugin::resetParameters(){
  tool_->posSlider->setValue(0);
  tool_->sizeSlider->setValue(102);
  updateSlice();
}

void SlicePlugin::updateSlice(int /*bla*/){
 updateSlice();
}

void SlicePlugin::updateSlice(){

  if ( tool_->enabled->isChecked() ) {
    node_->set_status( ACG::SceneGraph::BaseNode::Active );
    node_->setMultipassStatus(BaseNode::ALLPASSES);
  } else {
    node_->set_status( ACG::SceneGraph::BaseNode::HideNode );
    node_->setMultipassStatus(ACG::SceneGraph::BaseNode::NOPASS);
  }

  if ( tool_->enabled->isChecked() ){

    ACG::Vec3d bbmin;
    ACG::Vec3d bbmax;

    getBoundingBox( bbmin, bbmax);

    ACG::Vec3d center = (bbmin + bbmax) * 0.5;
    ACG::Vec3f pos (center[0], center[1], center[2]);

    //get the normal
    ACG::Vec3f normal(1.0, 0.0, 0.0);
    float eps;
    float offset = 0.0;

    eps = tool_->sizeSlider->value() / 100.0;

    if (eps == 0.0)
      eps = 0.01f;

    if (tool_->radioX->isChecked()){
      normal = ACG::Vec3f(1.0, 0.0, 0.0);
      eps *= (bbmax[0] - bbmin[0]);
      offset = (bbmax[0] - bbmin[0]) * 0.5;
    }else
    if (tool_->radioY->isChecked()){
      normal = ACG::Vec3f(0.0, 1.0, 0.0);
      eps *= (bbmax[1] - bbmin[1]);
      offset = (bbmax[1] - bbmin[1]) * 0.5;
    }else
    if (tool_->radioZ->isChecked()){
      normal = ACG::Vec3f(0.0, 0.0, 1.0);
      eps *= (bbmax[2] - bbmin[2]);
      offset = (bbmax[2] - bbmin[2]) * 0.5;
    }

    pos += normal * ( (float)tool_->posSlider->value() / 100.0 ) * (offset + 0.1); //0.1 is just a little workarround

    node_->set_plane(pos, normal, eps);
  }
  emit updateView();
}

void SlicePlugin::getBoundingBox(ACG::Vec3d& bbmin, ACG::Vec3d& bbmax){

  bool firstRound = true;

  PluginFunctions::IteratorRestriction restriction;

  if (tool_->radioTarget->isChecked())
    restriction = PluginFunctions::TARGET_OBJECTS;
  else
    restriction = PluginFunctions::ALL_OBJECTS;

  for ( PluginFunctions::ObjectIterator o_it(restriction,DataType(DATA_ALL)) ;
                                        o_it != PluginFunctions::objectsEnd(); ++o_it)  {
    // get scene size
    ACG::Vec3d cur_min;
    ACG::Vec3d cur_max;

    o_it->getBoundingBox(cur_min, cur_max);

    if (firstRound){
      bbmin = cur_min;
      bbmax = cur_max;
      firstRound = false;
    }else{
      bbmin[0] = std::min( bbmin[0], cur_min[0]);
      bbmin[1] = std::min( bbmin[1], cur_min[1]);
      bbmin[2] = std::min( bbmin[2], cur_min[2]);
      bbmax[0] = std::max( bbmax[0], cur_max[0]);
      bbmax[1] = std::max( bbmax[1], cur_max[1]);
      bbmax[2] = std::max( bbmax[2], cur_max[2]);
    }
  }

  if ((bbmin[0] > bbmax[0]) || (bbmin[1] > bbmax[1]) || (bbmin[2] > bbmax[2]))
    std::cerr << "Error while computing bounding box!";

}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( slicePlugin , SlicePlugin );
#endif

