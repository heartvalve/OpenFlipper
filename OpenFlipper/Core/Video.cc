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
//   $Revision: 2160 $
//   $Author: wilden $
//   $Date: 2008-07-08 18:05:21 +0200 (Di, 08. Jul 2008) $
//
//=============================================================================




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
  // set track auf true;
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

  if ( elapsed < 40 ) {
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
}

void Core::viewUpdated() {
  if ( capture_ )
    captureVideo();
}



//=============================================================================
