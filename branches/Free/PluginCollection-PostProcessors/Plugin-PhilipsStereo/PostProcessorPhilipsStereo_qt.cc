/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $Revision: 18127 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2014-02-05 10:12:54 +0100 (Wed, 05 Feb 2014) $                     *
*                                                                            *
\*===========================================================================*/

/** @file
 *
 *  Contains definitions of the PostProcessorPhilipsStereoPlugin that require qt headers
 *  which are incompatible with glew.h.
 */

#include "PostProcessorPhilipsStereo.hh"
#include "widgets/philipsStereoSettingsWidget.hh"
#include <OpenFlipper/common/GlobalOptions.hh>
#include <QGLFormat>

QString PostProcessorPhilipsStereoPlugin::checkOpenGL() {
  QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
  if ( ! flags.testFlag(QGLFormat::OpenGL_Version_3_0) )
    return QString("Insufficient OpenGL Version! OpenGL 3.0 or higher required");

  // Check extensions
  QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));
  QString missing("");
  if ( !glExtensions.contains("GL_ARB_texture_rectangle") )
    missing += "GL_ARB_texture_rectangle extension missing\n";

  return missing;
}

void  PostProcessorPhilipsStereoPlugin::slotShowOptionsMenu() {

  // Create widget if it does not exist
  if ( settingsWidget_ == 0) {
    settingsWidget_ = new PhilipsStereoSettingsWidget(0);
    connect(settingsWidget_,SIGNAL(updateView()),this,SIGNAL(updateView()));
  }

  // Set values Philips stereo mode

  // Block signals such that slotApplyStereoSettings
  // won't be called when setting the initial values here...
  settingsWidget_->headerContentType->blockSignals(true);
  settingsWidget_->headerSelect->blockSignals(true);
  settingsWidget_->offsetCounter->blockSignals(true);
  settingsWidget_->headerFactor->blockSignals(true);
  settingsWidget_->factorCounter->blockSignals(true);
  settingsWidget_->headerOffsetCC->blockSignals(true);

  settingsWidget_->headerContentType->setCurrentIndex(OpenFlipperSettings().value("Core/Stereo/Philips/Content",3).toInt());
  settingsWidget_->headerFactor->setValue(OpenFlipperSettings().value("Core/Stereo/Philips/Factor",64).toInt());
  settingsWidget_->headerOffsetCC->setValue(OpenFlipperSettings().value("Core/Stereo/Philips/Offset",128).toInt());
  settingsWidget_->factorCounter->setNum(OpenFlipperSettings().value("Core/Stereo/Philips/Factor",64).toInt());
  settingsWidget_->offsetCounter->setNum(OpenFlipperSettings().value("Core/Stereo/Philips/Offset",128).toInt());
  settingsWidget_->headerSelect->setCurrentIndex(OpenFlipperSettings().value("Core/Stereo/Philips/Select",0).toInt());

  // Unblock signals
  settingsWidget_->headerContentType->blockSignals(false);
  settingsWidget_->headerSelect->blockSignals(false);
  settingsWidget_->offsetCounter->blockSignals(false);
  settingsWidget_->headerFactor->blockSignals(false);
  settingsWidget_->factorCounter->blockSignals(false);
  settingsWidget_->headerOffsetCC->blockSignals(false);



  // Move widget to the position of the cursor
  settingsWidget_->move( QCursor::pos() - QPoint((int)(settingsWidget_->width()/2), 0));

  settingsWidget_->show();

}
