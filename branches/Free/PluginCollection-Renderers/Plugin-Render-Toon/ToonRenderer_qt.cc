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

/**
 * @file Contains definitions that require qt headers which are incompatible
 * with glew.h.
 */

#include <QGLFormat>
#include <QDialog>
#include <QColor>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <ACG/QtWidgets/QtColorChooserButton.hh>
#include "ToonRenderer.hh"


QString ToonRenderer::checkOpenGL() {
  // Get version and check
  QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
  if ( !flags.testFlag(QGLFormat::OpenGL_Version_3_2) )
    return QString("Insufficient OpenGL Version! OpenGL 3.2 or higher required");

  // Check extensions
  QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));
  QString missing("");
  if ( !glExtensions.contains("GL_ARB_vertex_buffer_object") )
    missing += "GL_ARB_vertex_buffer_object extension missing\n";

#ifndef __APPLE__
  if ( !glExtensions.contains("GL_ARB_vertex_program") )
    missing += "GL_ARB_vertex_program extension missing\n";
#endif

  return missing;
}


void ToonRenderer::actionDialog( bool )
{
  //generate widget
  QDialog* optionsDlg = new QDialog();
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setAlignment(Qt::AlignTop);

  QColor curColor;
  curColor.setRgbF(outlineCol_[0],outlineCol_[1],outlineCol_[2]);

  QLabel* label = new QLabel(tr("Palette Size [0, 10]:"));
  layout->addWidget(label);

  QSlider* paletteSizeSlider = new QSlider(Qt::Horizontal);
  paletteSizeSlider->setRange(0, 1000);
  paletteSizeSlider->setValue(int(paletteSize_ * 100.0));
  paletteSizeSlider->setTracking(true);
  layout->addWidget(paletteSizeSlider);

  QtColorChooserButton* outlineColorBtn = new QtColorChooserButton("Outline Color");
  layout->addWidget(outlineColorBtn);

  outlineColorBtn->setColor( curColor );

  optionsDlg->setLayout(layout);


  connect(paletteSizeSlider, SIGNAL(sliderMoved(int)), this, SLOT(paletteSizeChanged(int)));
  connect(outlineColorBtn, SIGNAL(colorChanged(QColor)), this, SLOT(outlineColorChanged(QColor)));


  optionsDlg->show();
}
