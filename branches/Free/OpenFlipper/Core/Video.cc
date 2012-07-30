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




//=============================================================================
//
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

// -------------------- mview
#include "Core.hh"

//== IMPLEMENTATION ==========================================================

//-----------------------------------------------------------------------------

void Core::startVideoCapture(QString _baseName, int _fps, bool _captureViewers) {
  
    connect( &videoTimer_ , SIGNAL(timeout()), this, SLOT( viewUpdated() ) ,Qt::DirectConnection );
  // set track to true;
  videoTimer_.setSingleShot(false);

  // 25 fps
  videoTimer_.start( 1000 / _fps);

  lastVideoTime_.start();

  QString name = _baseName;

  if (_captureViewers){
    applicationSnapshotName(name);
    captureType_ = 1;
  } else {
    applicationSnapshotName(name);
    captureType_ = 0;
  }

  capture_ = true;
}

void Core::captureVideo() {
  int elapsed = lastVideoTime_.elapsed();

  if ( elapsed < videoTimer_.interval() ) {
//     std::cerr << "Too early to capture" << std::endl;
    return;
  }

  lastVideoTime_.restart();

  if (captureType_ == 1)
    viewerSnapshot();
  else
    applicationSnapshot();
}


void Core::stopVideoCapture() {
  videoTimer_.stop();
  capture_ = false;

  if ( OpenFlipper::Options::gui() ){

    if (captureType_ == 1){
      coreWidget_->glView_->resize(lastWidth_, lastHeight_);
    } else {
      coreWidget_->resize(lastWidth_, lastHeight_);
    }
  }
}

void Core::viewUpdated( ) {
  
  if ( capture_ )
    captureVideo();

  // Only call the view updates, if the scenegraph updates are not currently locked!
  if ( ! OpenFlipper::Options::sceneGraphUpdatesBlocked() )
    emit pluginViewChanged();
}



//=============================================================================
