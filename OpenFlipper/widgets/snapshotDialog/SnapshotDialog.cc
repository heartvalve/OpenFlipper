#include "SnapshotDialog.hh"
#include <QtGui>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <iostream>
#include <OpenFlipper/common/GlobalOptions.hh>

SnapshotDialog::SnapshotDialog(QString _suggest, bool _captureViewers, QWidget *parent)
 : QDialog(parent),
   captureViewers_(_captureViewers)
{
  setupUi(this);

  filename->setText( _suggest );

  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
  connect(findButton, SIGNAL(clicked()), this, SLOT(findFile()) );
  connect(resButton,  SIGNAL(clicked()), this, SLOT(slotChangeResolution()) );
  connect(okButton,  SIGNAL(clicked()), this, SLOT(slotOk()) );
}

void SnapshotDialog::slotChangeResolution()
{
  if ( captureViewers_ )
    emit resizeViewers(snapWidth->value(), snapHeight->value());
  else
    emit resizeApplication(snapWidth->value(), snapHeight->value());
}

void SnapshotDialog::slotOk()
{

  if (filename->text() == ""){
    QMessageBox msgBox;
    msgBox.setText("The Filename is empty!");
    msgBox.exec();
    return;
  }

  if ( captureViewers_ )
    emit resizeViewers(snapWidth->value(), snapHeight->value());
  else
    emit resizeApplication(snapWidth->value(), snapHeight->value());

  accept();
}

void SnapshotDialog::findFile()
{

  QFileInfo fi( filename->text() );

  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDefaultSuffix("png");
  dialog.setNameFilter("Images (*.png *.ppm *.jpg)");
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setConfirmOverwrite(true);
  dialog.setDirectory( fi.path() );
  dialog.selectFile( filename->text() );
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle("Save Snapshot");

  bool ok = dialog.exec();

  if (ok)
    filename->setText( dialog.selectedFiles()[0] );
}