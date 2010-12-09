/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
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
    snapshotFileType_("png"),
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
    mipmapping_(true),
    animation_(false),
    glState_(0),
    objectMarker_(0),
    rotationLocked_(false)
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
  
  void ViewerProperties::snapshotFileType(const QString& _type) {
    snapshotFileType_  = _type;
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
  
  bool ViewerProperties::rotationLocked(){
    return rotationLocked_;
  }

  void ViewerProperties::rotationLocked(bool _locked){
    rotationLocked_ = _locked;
  }

}

