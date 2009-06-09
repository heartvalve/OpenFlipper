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