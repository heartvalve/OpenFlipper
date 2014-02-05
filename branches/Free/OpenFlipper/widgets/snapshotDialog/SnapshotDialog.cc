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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include "SnapshotDialog.hh"
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <iostream>
#include <OpenFlipper/common/GlobalOptions.hh>

SnapshotDialog::SnapshotDialog(QString _suggest, bool _captureViewers, int _w, int _h, QWidget *parent)
 : QDialog(parent),
   captureViewers_(_captureViewers),
   aspect_((double)_w / (double)_h),
   blockSpinBox_(false)
{
  setupUi(this);

  filename->setText( _suggest );
  
  // Disable 'change resolution' button if
  // in viewer snapshot mode
  multisampling->setChecked(captureViewers_);
  resButton->setDisabled(captureViewers_);
  transparent->setDisabled(!captureViewers_);
  hideCoordsys->setDisabled(!captureViewers_);
  multisampling->setDisabled(!captureViewers_);
  num_samples->setDisabled(!captureViewers_);
  
  snapWidth->setValue(_w);
  snapHeight->setValue(_h);
  
  // Load button states
  loadStates();
  
  connect(snapWidth,  SIGNAL(valueChanged(int)), this, SLOT(snapWidthChanged(int)) );
  connect(snapHeight, SIGNAL(valueChanged(int)), this, SLOT(snapHeightChanged(int)) );
  connect(keepAspect, SIGNAL(stateChanged(int)), this, SLOT(keepAspectChanged()) );
  connect(multisampling, SIGNAL(stateChanged(int)), this, SLOT(multisampleChanged()) );

  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
  connect(findButton, SIGNAL(clicked()), this, SLOT(findFile()) );
  connect(resButton,  SIGNAL(clicked()), this, SLOT(slotChangeResolution()) );
  connect(okButton,  SIGNAL(clicked()), this, SLOT(slotOk()) );
}

void SnapshotDialog::saveStates() {
    
    OpenFlipperSettings().setValue( "Viewer/SnapshotDialog/SnapWidth",      snapWidth->value());
    OpenFlipperSettings().setValue( "Viewer/SnapshotDialog/SnapHeight",     snapHeight->value());
    OpenFlipperSettings().setValue( "Viewer/SnapshotDialog/KeepAspect",     keepAspect->isChecked());
    OpenFlipperSettings().setValue( "Viewer/SnapshotDialog/Transparent",    transparent->isChecked());
    OpenFlipperSettings().setValue( "Viewer/SnapshotDialog/HideCoordsys",   hideCoordsys->isChecked());
    OpenFlipperSettings().setValue( "Viewer/SnapshotDialog/Multisampling",  multisampling->isChecked());
    OpenFlipperSettings().setValue( "Viewer/SnapshotDialog/NumSamples",     num_samples->value());
}

void SnapshotDialog::loadStates() {
    
    snapWidth->setValue(        OpenFlipperSettings().value( "Viewer/SnapshotDialog/SnapWidth",     snapWidth->value()).toInt());
    snapHeight->setValue(       OpenFlipperSettings().value( "Viewer/SnapshotDialog/SnapHeight",    snapHeight->value()).toInt());
    keepAspect->setChecked(     OpenFlipperSettings().value( "Viewer/SnapshotDialog/KeepAspect",    false).toBool());
    transparent->setChecked(    OpenFlipperSettings().value( "Viewer/SnapshotDialog/Transparent",   false).toBool());
    hideCoordsys->setChecked(   OpenFlipperSettings().value( "Viewer/SnapshotDialog/HideCoordsys",  false).toBool());
    multisampling->setChecked(  OpenFlipperSettings().value( "Viewer/SnapshotDialog/Multisampling", true).toBool());
    num_samples->setValue(      OpenFlipperSettings().value( "Viewer/SnapshotDialog/NumSamples",    16).toInt());
}

void SnapshotDialog::snapWidthChanged(int _w) {
    
    if(blockSpinBox_) return;
    
    if(keepAspect->isChecked()) {
        blockSpinBox_ = true;
        snapHeight->setValue((int)((double)_w / aspect_));
        blockSpinBox_ = false;
    }
}

void SnapshotDialog::snapHeightChanged(int _h) {
    
    if(blockSpinBox_) return;
    
    if(keepAspect->isChecked()) {
        blockSpinBox_ = true;
        snapWidth->setValue((int)((double)_h * aspect_));
        blockSpinBox_ = false;
    }
}

void SnapshotDialog::keepAspectChanged() {

    if(keepAspect->isChecked()) {
        blockSpinBox_ = true;
	snapHeight->setValue((int)((double)snapWidth->value() / aspect_));
        blockSpinBox_ = false;
    }
}

void SnapshotDialog::multisampleChanged() {
    num_samples->setDisabled (!multisampling->isChecked());
}
void SnapshotDialog::slotChangeResolution()
{
  if ( !captureViewers_ )
    emit resizeApplication(snapWidth->value(), snapHeight->value());
}

void SnapshotDialog::slotOk()
{

  if (filename->text() == ""){
    QMessageBox msgBox;
    msgBox.setText(tr("The Filename is empty!"));
    msgBox.exec();
    return;
  }

  if ( !captureViewers_ )
    emit resizeApplication(snapWidth->value(), snapHeight->value());

  // Remember button states for next time...
  saveStates();
  
  accept();
}

void SnapshotDialog::findFile()
{

  QFileInfo fi( filename->text() );

  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDefaultSuffix("png");
  dialog.setNameFilter(tr("Images (*.png *.ppm *.jpg)"));
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setConfirmOverwrite(true);
  dialog.setDirectory( fi.path() );
  dialog.selectFile( filename->text() );
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle(tr("Save Snapshot"));

  bool ok = dialog.exec();

  if (ok)
    filename->setText( dialog.selectedFiles()[0] );
}
