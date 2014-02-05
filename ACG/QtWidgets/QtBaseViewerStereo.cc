/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



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
    ACG::GLState::drawBuffer(GL_BACK);
  }

  updateGL();
}

//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
