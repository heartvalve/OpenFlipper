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
#include "ViewerProperties.hh"
#include <iostream>

namespace Viewer {

  ViewerProperties::ViewerProperties():
    actionMode_(Viewer::PickingMode),
    lastActionMode_(Viewer::PickingMode),
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
    animation_(false),
    glState_(0)
  {

  }

  ViewerProperties::~ViewerProperties() {

    if ( glState_ != 0 )
      delete glState_;

  }

  void ViewerProperties::actionMode(Viewer::ActionMode _am) {
    if (_am != actionMode_) {
      lastActionMode_ = actionMode_;
      actionMode_ = _am;
      emit actionModeChanged(_am);
    }

  }

  void ViewerProperties::snapshotBaseFileName(const QString& _fname) {
    snapshotName_    = _fname;
    snapshotCounter_ = 0;
  }

}

