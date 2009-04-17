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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




//=============================================================================
//
//  CLASS CoreWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "CoreWidget.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>



//== IMPLEMENTATION ==========================================================

//=============================================================================

void CoreWidget::slotToggleStereoMode()
{
  stereoActive_ = !stereoActive_;

  if ( stereoActive_ ) {
    statusBar_->showMessage("Stereo enabled");
    stereoButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"stereo.png") );
  } else {
    statusBar_->showMessage("Stereo disabled");
    stereoButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"mono.png") );
  }

  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    examiner_widgets_[i]->setStereoMode(stereoActive_);
}

void CoreWidget::slotSetGlobalBackgroundColor() {
  ACG::Vec4f bc = PluginFunctions::viewerProperties().backgroundColor() * 255.0;

  QColor backCol((int)bc[0], (int)bc[1], (int)bc[2], (int)bc[3]);
  QColor c = QColorDialog::getColor(backCol,this);

  if (c != backCol && c.isValid())
    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i )
      PluginFunctions::viewerProperties(i).backgroundColor(ACG::Vec4f(((double) c.redF())   ,
                                                                      ((double) c.greenF()) ,
                                                                      ((double) c.blueF())  ,
                                                                        1.0));

  OpenFlipper::Options::defaultBackgroundColor( c );

}

void CoreWidget::slotSetContextBackgroundColor() {
  ACG::Vec4f bc = PluginFunctions::viewerProperties().backgroundColor() * 255.0;

  QColor backCol((int)bc[0], (int)bc[1], (int)bc[2], (int)bc[3]);
  QColor c = QColorDialog::getColor(backCol,this);

  if (c != backCol && c.isValid())
    PluginFunctions::viewerProperties().backgroundColor(ACG::Vec4f(((double) c.redF())   ,
                                                                    ((double) c.greenF()) ,
                                                                    ((double) c.blueF())  ,
                                                                     1.0));
}


/// Set the viewer to home position
void CoreWidget::slotContextHomeView() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->home();
}


/// Set the viewer to home position
void CoreWidget::slotGlobalHomeView() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->home();
}

/// Set the viewers home position
void CoreWidget::slotContextSetHomeView() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->setHome();
}


/// Set the home position for all viewers
void CoreWidget::slotGlobalSetHomeView() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->setHome();
}

/// Change view on active viewer to view complete scene
void CoreWidget::slotContextViewAll() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->viewAll();
}


/// Change view on all viewers to view complete scene
void CoreWidget::slotGlobalViewAll() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->viewAll();
}

/// Toggle projection Mode of the active viewer
void CoreWidget::slotContextSwitchProjection() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->toggleProjectionMode();
}

/// Toggle projection Mode of all viewers to perspective projection
void CoreWidget::slotGlobalPerspectiveProjection() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->perspectiveProjection();
}

/// Toggle projection Mode of all viewers to orthogonal projection
void CoreWidget::slotGlobalOrthographicProjection() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->orthographicProjection();
}

/// Set the animation Mode for all viewers
void CoreWidget::slotGlobalChangeAnimation(bool _animation){
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    PluginFunctions::viewerProperties(i).animation(_animation);
}

/// Set the animation Mode for active viewer
void CoreWidget::slotLocalChangeAnimation(bool _animation){
  PluginFunctions::viewerProperties().animation(_animation);
}

/// Set Backface culling for all viewers
void CoreWidget::slotGlobalChangeBackFaceCulling(bool _backFaceCulling){
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    PluginFunctions::viewerProperties(i).backFaceCulling(_backFaceCulling);

}

/// Set Backface culling for active viewer
void CoreWidget::slotLocalChangeBackFaceCulling(bool _backFaceCulling){
  PluginFunctions::viewerProperties().backFaceCulling(_backFaceCulling);
}


/// Set two sided lighting for all viewers
void CoreWidget::slotGlobalChangeTwoSidedLighting(bool _lighting) {
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    PluginFunctions::viewerProperties(i).twoSidedLighting(_lighting);
}

/// Set two sided lighting for active viewer
void CoreWidget::slotLocalChangeTwoSidedLighting(bool _lighting) {
  PluginFunctions::viewerProperties().twoSidedLighting(_lighting);
}

/// Take a snapshot of the current Viewer
void CoreWidget::slotSnapshot() {

  QFileInfo fi(PluginFunctions::viewerProperties().snapshotName());
  int counter = PluginFunctions::viewerProperties().snapshotCounter();

  QString suggest = fi.baseName() + "." + QString::number(counter) + ".";

  QString format="png";

  if (fi.completeSuffix() == "ppm")
    format="ppmraw";

  if (fi.completeSuffix() == "jpg")
    format="jpg";

  suggest += format;

  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDefaultSuffix("png");
  dialog.setNameFilter("Images (*.png *.ppm *.jpg)");
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setConfirmOverwrite(true);
  dialog.setDirectory( fi.path() );
  dialog.selectFile( suggest );
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle("Save Snapshot");

 if (dialog.exec()){
     QString newName = dialog.selectedFiles()[0];

      if (newName != fi.path() + OpenFlipper::Options::dirSeparator() + suggest)
        PluginFunctions::viewerProperties().snapshotBaseFileName(newName);

      QImage image;
      examiner_widgets_[PluginFunctions::activeExaminer()]->snapshot(image);

      image.save(newName);
  }
}

///Take a snapshot of the whole application
void CoreWidget::applicationSnapshotDialog() {

  QFileInfo fi(snapshotName_);

  QString suggest = fi.baseName() + "." + QString::number(snapshotCounter_) + ".";

  QString format="png";

  if (fi.completeSuffix() == "ppm")
    format="ppmraw";

  if (fi.completeSuffix() == "jpg")
    format="jpg";

  suggest += format;

  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDefaultSuffix("png");
  dialog.setNameFilter("Images (*.png *.ppm *.jpg)");
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setConfirmOverwrite(true);
  dialog.setDirectory( fi.path() );
  dialog.selectFile( suggest );
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle("Save Snapshot");

  if (dialog.exec()){
    QString newName = dialog.selectedFiles()[0];

    if (newName != fi.path() + OpenFlipper::Options::dirSeparator() + suggest){
      snapshotName_ = newName;
      snapshotCounter_ = 0;
    }else
      snapshotCounter_++;

    QApplication::processEvents();

    QPixmap pic = QPixmap::grabWindow( winId() );

    pic.save(newName);
  }
}

///Take a snapshot of the whole application
void CoreWidget::applicationSnapshot() {

  QFileInfo fi(snapshotName_);

  QString suggest = fi.path() + QDir::separator() +fi.baseName() + "." + QString::number(snapshotCounter_++) + ".";

  QString format="png";

  if (fi.completeSuffix() == "ppm")
    format="ppmraw";

  if (fi.completeSuffix() == "jpg")
    format="jpg";

  suggest += format;

  QApplication::processEvents();

  QPixmap pic = QPixmap::grabWindow( winId() );

  pic.save(suggest);
}


///Take a snapshot of all viewers
void CoreWidget::viewerSnapshotDialog() {

  QFileInfo fi(snapshotName_);

  QString suggest = fi.baseName() + "." + QString::number(snapshotCounter_) + ".";

  QString format="png";

  if (fi.completeSuffix() == "ppm")
    format="ppmraw";

  if (fi.completeSuffix() == "jpg")
    format="jpg";

  suggest += format;

  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setDefaultSuffix("png");
  dialog.setNameFilter("Images (*.png *.ppm *.jpg)");
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setConfirmOverwrite(true);
  dialog.setDirectory( fi.path() );
  dialog.selectFile( suggest );
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setWindowTitle("Save Snapshot");

  if (dialog.exec()){
    QString newName = dialog.selectedFiles()[0];

    if (newName != fi.path() + OpenFlipper::Options::dirSeparator() + suggest){
      snapshotName_ = newName;
      snapshotCounter_ = 0;
    }else
      snapshotCounter_++;

    //now take the snapshot
    switch ( baseLayout_->mode() ){

      case QtMultiViewLayout::SingleView:
      {
        QImage finalImage;

        examiner_widgets_[PluginFunctions::activeExaminer()]->snapshot(finalImage);

        finalImage.save(newName);

        break;
      }
      case QtMultiViewLayout::Grid:
      {
        QImage img0,img1,img2,img3;

        examiner_widgets_[0]->snapshot(img0);
        examiner_widgets_[1]->snapshot(img1);
        examiner_widgets_[2]->snapshot(img2);
        examiner_widgets_[3]->snapshot(img3);

        QImage finalImage(img0.width() + img1.width(), img0.height() + img2.height(), QImage::Format_ARGB32_Premultiplied);

        QPainter painter(&finalImage);
        painter.drawImage(QRectF(           0,             0, img0.width(), img0.height()),img0,
                          QRectF(           0,             0, img0.width(), img0.height()) );
        painter.drawImage(QRectF(img0.width(),             0, img1.width(), img1.height()),img1,
                          QRectF(           0,             0, img1.width(), img1.height()) );
        painter.drawImage(QRectF(           0, img0.height(), img2.width(), img2.height()),img2,
                          QRectF(           0,             0, img2.width(), img2.height()) );
        painter.drawImage(QRectF(img0.width(), img0.height(), img3.width(), img3.height()),img3,
                          QRectF(           0,             0, img3.width(), img3.height()) );

        finalImage.save(newName);

        break;
      }
      case QtMultiViewLayout::HSplit:
      {
        QImage img0,img1,img2,img3;

        examiner_widgets_[0]->snapshot(img0);
        examiner_widgets_[1]->snapshot(img1);
        examiner_widgets_[2]->snapshot(img2);
        examiner_widgets_[3]->snapshot(img3);

        QImage finalImage(img0.width() + img1.width(), img0.height(), QImage::Format_ARGB32_Premultiplied);

        QPainter painter(&finalImage);
        painter.drawImage(QRectF(           0,             0, img0.width(), img0.height()),img0,
                          QRectF(           0,             0, img0.width(), img0.height()) );
        painter.drawImage(QRectF(img0.width(),             0, img1.width(), img1.height()),img1,
                          QRectF(           0,             0, img1.width(), img1.height()) );
        painter.drawImage(QRectF(img0.width(), img1.height(), img2.width(), img2.height()),img2,
                          QRectF(           0,             0, img2.width(), img2.height()) );
        painter.drawImage(QRectF(img0.width(), img1.height()+img2.height(), img3.width(),img3.height()),img3,
                          QRectF(           0,             0, img3.width(), img3.height()) );

        finalImage.save(newName);

        break;
      }
      default: break;

    }
  }
}

///Take a snapshot of all viewers
void CoreWidget::viewerSnapshot() {

  QFileInfo fi(snapshotName_);

  QString suggest = fi.path() + QDir::separator() +fi.baseName() + "." + QString::number(snapshotCounter_++) + ".";

  QString format="png";

  if (fi.completeSuffix() == "ppm")
    format="ppmraw";

  if (fi.completeSuffix() == "jpg")
    format="jpg";

  suggest += format;


  switch ( baseLayout_->mode() ){

    case QtMultiViewLayout::SingleView:
    {
      QImage finalImage;

      examiner_widgets_[PluginFunctions::activeExaminer()]->snapshot(finalImage);

      finalImage.save(suggest);

      break;
    }
    case QtMultiViewLayout::Grid:
    {
      QImage img0,img1,img2,img3;

      examiner_widgets_[0]->snapshot(img0);
      examiner_widgets_[1]->snapshot(img1);
      examiner_widgets_[2]->snapshot(img2);
      examiner_widgets_[3]->snapshot(img3);

      QImage finalImage(img0.width() + img1.width(), img0.height() + img2.height(), QImage::Format_ARGB32_Premultiplied);

      QPainter painter(&finalImage);
      painter.drawImage(QRectF(           0,             0, img0.width(), img0.height()),img0,
                        QRectF(           0,             0, img0.width(), img0.height()) );
      painter.drawImage(QRectF(img0.width(),             0, img1.width(), img1.height()),img1,
                        QRectF(           0,             0, img1.width(), img1.height()) );
      painter.drawImage(QRectF(           0, img0.height(), img2.width(), img2.height()),img2,
                        QRectF(           0,             0, img2.width(), img2.height()) );
      painter.drawImage(QRectF(img0.width(), img0.height(), img3.width(), img3.height()),img3,
                        QRectF(           0,             0, img3.width(), img3.height()) );

      finalImage.save(suggest);

      break;
    }
    case QtMultiViewLayout::HSplit:
    {
      QImage img0,img1,img2,img3;

      examiner_widgets_[0]->snapshot(img0);
      examiner_widgets_[1]->snapshot(img1);
      examiner_widgets_[2]->snapshot(img2);
      examiner_widgets_[3]->snapshot(img3);

      QImage finalImage(img0.width() + img1.width(), img0.height(), QImage::Format_ARGB32_Premultiplied);

      QPainter painter(&finalImage);
      painter.drawImage(QRectF(           0,             0, img0.width(), img0.height()),img0,
                        QRectF(           0,             0, img0.width(), img0.height()) );
      painter.drawImage(QRectF(img0.width(),             0, img1.width(), img1.height()),img1,
                        QRectF(           0,             0, img1.width(), img1.height()) );
      painter.drawImage(QRectF(img0.width(), img1.height(), img2.width(), img2.height()),img2,
                        QRectF(           0,             0, img2.width(), img2.height()) );
      painter.drawImage(QRectF(img0.width(), img1.height()+img2.height(), img3.width(),img3.height()),img3,
                        QRectF(           0,             0, img3.width(), img3.height()) );

      finalImage.save(suggest);

      break;
    }
    default: break;

  }
}

void CoreWidget::applicationSnapshotName(QString _name) {

  snapshotName_ = _name;
  snapshotCounter_ = 0;
}


void CoreWidget::slotPasteView( ) {
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionPasteView();
}

void CoreWidget::slotCopyView( ) {
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionCopyView();
}




//=============================================================================
