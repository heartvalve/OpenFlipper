/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
\*===========================================================================*/

#include "VideoCaptureDialog.hh"
#include <QtGui>
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <OpenFlipper/common/GlobalOptions.hh>

VideoCaptureDialog::VideoCaptureDialog(QWidget *parent) : QDialog(parent)
{
  setupUi(this);

  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
  connect(findButton, SIGNAL(clicked()), this, SLOT(findFile()) );
  connect(resButton,  SIGNAL(clicked()), this, SLOT(slotChangeResolution()) );
  connect(startButton,  SIGNAL(clicked()), this, SLOT(slotStartVideoCapture()) );
}

void VideoCaptureDialog::slotChangeResolution()
{
  if ( captureViewers->isChecked() )
    emit resizeViewers(videoWidth->value(), videoHeight->value());
  else
    emit resizeApplication(videoWidth->value(), videoHeight->value());
}

void VideoCaptureDialog::slotStartVideoCapture()
{

  if (filename->text() == ""){
    QMessageBox msgBox;
    msgBox.setText("The Filename is empty!");
    msgBox.exec();
    return;
  }

  if ( captureViewers->isChecked() )
    emit resizeViewers(videoWidth->value(), videoHeight->value());
  else
    emit resizeApplication(videoWidth->value(), videoHeight->value());

  emit startVideoCapture( filename->text(), fps->value(), captureViewers->isChecked() );

  accept();
}

void VideoCaptureDialog::findFile()
{

  QString file = QFileDialog::getSaveFileName(this, tr("Output File"),
                 OpenFlipper::Options::applicationDirStr() + OpenFlipper::Options::dirSeparator() + "untitled.jpg",
                 tr("Images (*.jpg)"));

  if (file != "")
    filename->setText( file );
}