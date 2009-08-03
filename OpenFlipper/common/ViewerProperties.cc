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
// Property storage Class for glWidgets
//
//=============================================================================

#include <OpenFlipper/common/GlobalDefines.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include "ViewerProperties.hh"
#include <iostream>

namespace Viewer {

  ViewerProperties::ViewerProperties():
    currentDrawMode_(ACG::SceneGraph::DrawModes::NONE),
    snapshotName_("snap.png"),
    snapshotCounter_(0),
    wZoomFactor_(1.0),
    wZoomFactorShift_(0.2),
    CCWFront_(true),
    backgroundColor_(0.0f,0.0f,0.0f,1.0f),
    renderPicking_(false),
    pickRendererMode_(ACG::SceneGraph::PICK_ANYTHING),
    locked_(0),
    backFaceCulling_(false),
    twoSidedLighting_(true),
    multisampling_(true),
    animation_(false),
    glState_(0),
    objectMarker_(0)
  {

  }

  ViewerProperties::~ViewerProperties() {

    if ( glState_ != 0 )
      delete glState_;

  }

  void ViewerProperties::snapshotBaseFileName(const QString& _fname) {
    snapshotName_    = _fname;
    snapshotCounter_ = 0;
  }

  std::string ViewerProperties::pickMode(){

    std::string mode;
    emit getPickMode(mode);
    return mode;
  }

  void ViewerProperties::pickMode(const std::string _name){
    emit setPickMode(_name);
  }

  Viewer::ActionMode ViewerProperties::actionMode(){
    Viewer::ActionMode am;
    emit getActionMode(am);
    return am;
  }

  void ViewerProperties::actionMode(const Viewer::ActionMode _am){
    emit setActionMode(_am);
  }

  int ViewerProperties::currentViewingDirection(){
    return currentViewingDirection_;
  }

  void ViewerProperties::currentViewingDirection(int _dir){
    currentViewingDirection_ = _dir;
  }

}

