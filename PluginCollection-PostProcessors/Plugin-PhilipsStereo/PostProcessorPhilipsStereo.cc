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
*   $Revision: 12293 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-09-08 11:00:06 +0200 (Do, 08 Sep 2011) $                     *
*                                                                            *
\*===========================================================================*/


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif


#include "PostProcessorPhilipsStereo.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <ACG/GL/ScreenQuad.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include "CRC/crc32.hh"

#include <QCursor>

PostProcessorPhilipsStereoPlugin::PostProcessorPhilipsStereoPlugin():
  pProgram_(0),
  settingsWidget_(0)
{

}

QString PostProcessorPhilipsStereoPlugin::postProcessorName() {
  return QString("Philips Stereo Display Output");
}

QAction* PostProcessorPhilipsStereoPlugin::optionsAction() {
 QAction* action = new QAction("Philips Stereo Options",0);
 connect(action,SIGNAL(triggered()),this,SLOT(slotShowOptionsMenu()));
 return action;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void PostProcessorPhilipsStereoPlugin::postProcess(ACG::GLState* _glstate, const PostProcessorInput& _input, GLuint _targetFBO) {

  if ( !ACG::checkExtensionSupported("GL_ARB_texture_rectangle") ) {
    std::cerr << "GL_ARB_texture_rectangle not supported! " << std::endl;
    return;
  }

  // load shader if needed
  if (!pProgram_)
    pProgram_ = GLSL::loadProgram("Philips/screenquad.glsl", "Philips/Fragment42.glsl");

  if (!pProgram_)
      return;

  int vp_l, vp_b, vp_w, vp_h;
  _glstate->get_viewport(vp_l, vp_b, vp_w, vp_h);


  // Turn on shader program
  pProgram_->use();

  // ======================================================================================================
  // Bind textures to different texture units and tell shader where to find them
  // ======================================================================================================
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, _input.depthTex_);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _input.colorTex_);

  pProgram_->setUniform("ColorTexture", 0);
  pProgram_->setUniform("DepthStencil", 1);

  // ======================================================================================================
  // Setup render states
  // ======================================================================================================

  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);

  glDepthMask(1);
  glColorMask(1,1,1,1);

  // ======================================================================================================
  // Setup orthogonal projection
  // ======================================================================================================
  _glstate->push_projection_matrix();
  _glstate->push_modelview_matrix();

  _glstate->reset_projection();
  _glstate->reset_modelview();

  _glstate->ortho(0, vp_w, 0, vp_h, 0, 1);

  // ======================================================================================================
  // Bind textures to different texture units and tell shader where to find them
  // ======================================================================================================
  glColor3f(1.0, 1.0, 1.0);


  // ======================================================================================================
  // Bind output FBO
  // ======================================================================================================

  glBindFramebuffer(GL_FRAMEBUFFER, _targetFBO);

  // ======================================================================================================
  // Clear buffers
  // ======================================================================================================
  glClearColor(.0, .0, .0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ======================================================================================================
  // Execute
  // ======================================================================================================
  ACG::ScreenQuad::draw(pProgram_);


  pProgram_->disable();

  glBindTexture(GL_TEXTURE_2D, 0);

  // ======================================================================================================
  // Compute the header required for the display
  // ======================================================================================================
  uchar *header = new uchar[6];

  //   Header ID
  //   Basic identifier used by the display to verify the header
  header[0] = 241; // Header_ID1 = 11110001

  //   Header content type
  //   This entry controls the displays internal rendering based on the input data specified here.
  //   There is no info about how this changes the rendering
  //   Possible values:
  //   0 No Depth
  //   1 Signage
  //   2 Movie
  //   3 Game
  //   4 CGI
  //   5 Still
  header[1] = OpenFlipperSettings().value("Core/Stereo/Philips/Content",3).toInt(); // Hdr_Content_type (Game) = 00000011 (Gaming Mode)

  //   Header Factor
  //   Each 3D Display has a 'Display recommended depth value', which corresponds to an
  //   acceptable maximum depth factor value for that specific type of display. This value strongly
  //   depends on the lens design. The factor field in the header contains the percentage to be
  //   used from the display recommended depth value. The value of 64 corresponds with the 100%
  //   of the display recommended depth value. It is allowed to use values higher than 64. The factor
  //   works on a linear scale and is multiplied with the factor controlled by the user in the Display
  //   Control Tool.
  //   Value range: 0-255 (default 64)
  header[2] = OpenFlipperSettings().value("Core/Stereo/Philips/Factor",64).toInt(); // Hdr_Factor

  //   Header Offset CC
  //   Values in the Depth map equal to the header-offset value will be located on the plane of the
  //   display. All values in the disparity map with a higher value will de displayed in front of the
  //   display.
  //   Offset_CC is the offset controlled by the Content Creator. In the system there is also an
  //   Offset_user present, which is controlled by the user using the Display Control Tool.
  //   Value Range: 0-255 (default 128)
  header[3] = OpenFlipperSettings().value("Core/Stereo/Philips/Offset",128).toInt(); // Hdr_Offset_CC

  //   Header select
  //   When all select signals are low the rendering settings are set to optimal settings for the content
  //   type denoted by Hdr_content_type. By making select signals high the settings for Factor and
  //   Offset_cc can be controlled individually by the header.
  //   Possible Values:
  //   0 Use Displays defaults and automatic optimizations
  //   1 Use Header provided factor
  //   2 Use Header provided offset
  //   3 Use both factor and offset
  header[4] = OpenFlipperSettings().value("Core/Stereo/Philips/Select",0).toInt(); // Hdr_Factor_select(1) + Hdr_Offset_select(1) + reserved(6)

  //   Unused Header entry (leave at 0 !)
  header[5] = 0; // Reserved

  //   Header checksum.
  //   The 4-byte EDC field H(6) − H(9) contains an Error Detection Code computed over the first 6
  //   header bytes. This EDC uses the standard CRC-32 polynomial as defined in IEEE 802.3 and ITU-T
  //   V.42. The initial value and final XOR value are both 0.
  //   unsigned long has 32bit = 4Byte
  unsigned long checksum = CalcCRC32(&header[0], 6);

  //   Store the complete header in a bit vector
  std::vector<uchar> bitVector;

  // For all bytes of the header
  for (int i = 0; i < 6; i++) {

      // For each bit of a headers byte
      for (int j = 7; j >= 0; --j) {

          // Red and Green component have to be 0
          bitVector.push_back(0);
          bitVector.push_back(0);

          // If bit is set, the full component will be set to one otherwise zero
          // And the order of the bits has to be reversed!
          if (header[i] & (1 << j)) {
              bitVector.push_back(255);
          } else {
              bitVector.push_back(0);
          }

          // Only every second pixel is used for the header
          // Skip every odd one by filling in 0 for RGB
          bitVector.push_back(0);
          bitVector.push_back(0);
          bitVector.push_back(0);
      }
  }

  // Append checksum to header.
  // Reversed bit order!
  for (int i = 31; i >= 0; i--) {

      // Red and Green component have to be 0
      bitVector.push_back(0);
      bitVector.push_back(0);

      if (checksum & (1 << i))
          bitVector.push_back(255);
      else
          bitVector.push_back(0);

      // Only every second pixel is used for the header
      // Skip every odd one by filling in 0 for RGB
      bitVector.push_back(0);
      bitVector.push_back(0);
      bitVector.push_back(0);
  }

  // Select the top left of the renderbuffer and
  // write complete header into these bits
  glRasterPos2i(0, _glstate->context_height() - 1);
  glDrawPixels(bitVector.size() / 3, 1, GL_RGB, GL_UNSIGNED_BYTE, &bitVector[0]);

  // ======================================================================================================
  // Reset projection and modelview
  // ======================================================================================================
  _glstate->pop_projection_matrix();
  _glstate->pop_modelview_matrix();


}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( postprocessorphilipsstereoplugin , PostProcessorPhilipsStereoPlugin );
#endif

