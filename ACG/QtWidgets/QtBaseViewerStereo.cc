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
//   $Revision: 3663 $
//   $Author: moebius $
//   $Date: 2008-11-06 14:55:30 +0100 (Do, 06. Nov 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS QtBaseViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtBaseViewer.hh"
#include <QStatusBar>
#include <QToolButton>

#include "mono.xpm"
#include "stereo.xpm"

#define monoIcon         mono_xpm
#define stereoIcon       stereo_xpm

//== NAMESPACES ===============================================================

namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ==========================================================

void QtBaseViewer::setEyeDistance(double _distance) {
  eyeDist_ = _distance;
  updateGL();
}

double QtBaseViewer::eyeDistance( ) {
  return eyeDist_;
}

void QtBaseViewer::setFocalDistance(double _distance) {
  focalDist_ = _distance;
  updateGL();
}

double QtBaseViewer::focalDistance( ) {
  return focalDist_;
}

void QtBaseViewer::toggleStereoMode() {
  QtBaseViewer::setStereoMode(!stereo_);
}

//-----------------------------------------------------------------------------


void
QtBaseViewer::setStereoMode(bool _b)
{
  stereo_ = _b;

  if (stereo_)
  {
    statusbar_->showMessage("Stereo enabled");
    stereoButton_->setIcon( QPixmap(stereoIcon) );
  }
  else
  {
    statusbar_->showMessage("Stereo disabled");
    stereoButton_->setIcon( QPixmap(monoIcon) );
    makeCurrent();
    glDrawBuffer(GL_BACK);
  }

  updateGL();
}

//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
