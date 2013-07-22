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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/




#include "optionsWidget.hh"
#include <iostream>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>
#include <OpenFlipper/INIFile/INIFile.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <QColorDialog>
#include <QMessageBox>

OptionsWidget::OptionsWidget(std::vector<PluginInfo>& _plugins, std::vector<KeyBinding>& _core, InverseKeyMap& _invKeys, QWidget *parent)
  : QWidget(parent),
    plugins_(_plugins),
    coreKeys_(_core),
    keys_(_invKeys),
    restartRequired_(false),
    exitOnClose_(false)
{
  setupUi(this);

  connect(okButton,SIGNAL(clicked()),this,SLOT(slotOk()));
  connect(applyButton,SIGNAL(clicked()),this,SLOT(slotApply()));
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(slotCancel()));
  connect(checkUpdateButton,SIGNAL(clicked()),this,SLOT(slotCheckUpdates()));
  connect(updateButton,SIGNAL(clicked()),this,SLOT(slotGetUpdates()));
  updateButton->setEnabled(false);

  // Viewer Settings
  connect( restrictFPS, SIGNAL(toggled(bool)), FPS, SLOT(setEnabled(bool)) );
  connect( viewerList, SIGNAL(currentRowChanged(int)), this, SLOT(updateViewerSettings(int)) );
  connect( availDrawModes, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(viewerSettingsChanged(QListWidgetItem*)) );
  connect( projectionBox, SIGNAL(currentIndexChanged(int)), this, SLOT(viewerSettingsChanged(int)));
  connect( directionBox, SIGNAL(currentIndexChanged(int)), this, SLOT(viewerSettingsChanged(int)));
  connect( lockRotationBox, SIGNAL(stateChanged(int)), this, SLOT(viewerSettingsChanged(int)));

  // Switch stacked widget of stereo settings
  connect(stereoOpengl, SIGNAL(clicked()),
          this, SLOT(switchStackedWidget()));
  connect(stereoAnaglyph, SIGNAL(clicked()),
          this, SLOT(switchStackedWidget()));
  connect(stereoCustomAnaglyph, SIGNAL(clicked()),
          this, SLOT(switchStackedWidget()));
  
  connect(focalDistance, SIGNAL(sliderReleased()),
          this, SLOT(slotPreviewStereoSettings()));
  connect(eyeDistance, SIGNAL(editingFinished()),
          this, SLOT(slotPreviewStereoSettings()));

  ACG::SceneGraph::DrawModes::ModeFlagSet mode(2);
  for (uint i=1; i < 22; i++) {
    std::vector< QString > dm = drawModeToDescriptions( mode );

    if ( !dm.empty() && dm[0].trimmed() != ""){
      QListWidgetItem* item = new QListWidgetItem(dm[0]);

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);

      item->setCheckState(Qt::Unchecked);

      availDrawModes->addItem( item );
    }

    mode = mode<<1;
  }

  defaultProjectionMode_.resize(4);
  defaultViewingDirections_.resize(4);
  defaultRotationLocks_.resize(4);

  for ( int i=0; i < PluginFunctions::viewers(); i++ ){
    viewerList->addItem("Viewer " + QString::number(i+1) );
    defaultDrawModes_.push_back( ACG::SceneGraph::DrawModes::DEFAULT );
  }

  pluginOptionsLayout = new QVBoxLayout;
  pluginOptions->setLayout( pluginOptionsLayout );

  networkMan_ = new QNetworkAccessManager(this);

  // http specific connections
  connect(networkMan_, SIGNAL(finished(QNetworkReply *)),
        this, SLOT(httpRequestFinished(QNetworkReply *)));
  connect(networkMan_,SIGNAL(authenticationRequired(QNetworkReply* , QAuthenticator *)),
        this,SLOT(authentication(QNetworkReply *, QAuthenticator*)));


  progressDialog = new QProgressDialog(this);
  connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));

  //colordialog
  connect(backgroundButton, SIGNAL(clicked()), this, SLOT(getBackgroundColor()) );
  connect(defaultColorButton, SIGNAL(clicked()), this, SLOT(getDefaultColor()) );
  
  // Clear settings and ini buttons
  connect(clearSettingsButton, SIGNAL(clicked()), this, SLOT(slotClearSettings()));
  connect(clearINIButton,      SIGNAL(clicked()), this, SLOT(slotClearINI()));

  // Disable update tab if app store compliant build
  #ifdef OPENFLIPPER_BUILD_APP_STORE_COMPLIANT
    //tabWidget->setTabEnabled(tabWidget->count()-1,false);
    tabWidget->removeTab(tabWidget->count()-1);
  #endif
}

void OptionsWidget::getBackgroundColor(){
  QColor newColor = QColorDialog::getColor ( OpenFlipperSettings().value("Core/Gui/glViewer/defaultBackgroundColor").value<QColor>() );

  backgroundColor_ = newColor;
  
  QPixmap color(16,16);
  color.fill( newColor );
  backgroundButton->setIcon( QIcon(color) );
}

void OptionsWidget::getDefaultColor(){
  QColor newColor = QColorDialog::getColor ( OpenFlipper::Options::defaultColor() );

  OpenFlipper::Options::defaultColor( newColor) ;

  QPixmap color(16,16);
  color.fill( OpenFlipper::Options::defaultColor() );
  defaultColorButton->setIcon( QIcon(color) );
}

void OptionsWidget::viewerSettingsChanged(QListWidgetItem* /*_item*/){
  viewerSettingsChanged(0);
};

void OptionsWidget::viewerSettingsChanged(int /*_index*/){

  if ( !updatingViewerSettings_ ){
    //viewer defaults
    std::vector< QString > mode;

    for (int i=0; i < availDrawModes->count(); i++)
      if (availDrawModes->item(i)->checkState() == Qt::Checked)
        mode.push_back( availDrawModes->item(i)->text() );

    defaultDrawModes_[         viewerList->currentRow() ] = descriptionsToDrawMode(mode);
    defaultProjectionMode_[    viewerList->currentRow() ] = projectionBox->currentIndex() ;
    defaultViewingDirections_[ viewerList->currentRow() ] = directionBox->currentIndex();
    defaultRotationLocks_[ viewerList->currentRow() ]     = lockRotationBox->isChecked();

    if ( lockRotationBox->isChecked() )
      std::cerr << "locked" << std::endl;
    else
      std::cerr << "unlocked" << std::endl;
  }
};

void OptionsWidget::switchStackedWidget() {

    // Show right stacked widget of stereo settings
    if (stereoCustomAnaglyph->isChecked()) {
        stackedWidget->setCurrentIndex(0);
        customAnaGlyphSettings->setVisible(true);
    } else {
        stackedWidget->setCurrentIndex(0);
        customAnaGlyphSettings->setVisible(false);
    }
    
    // Preview new settings
    slotPreviewStereoSettings();
}

void OptionsWidget::slotTranslationIndexChanged(int /*_index*/) {
  restartRequired_ = true;
}

void OptionsWidget::updateViewerSettings(int _row){

  updatingViewerSettings_ = true;

  for (int i = 0 ; i < availDrawModes->count(); ++i )
    availDrawModes->item( i )->setCheckState(Qt::Unchecked) ;

  //Check the drawModes from StandardDrawMode
  std::vector< QString > dm = drawModeToDescriptions( defaultDrawModes_[_row] );

  for (uint i=0; i < dm.size(); i++){
    if ( !dm.empty() ) {
      QList< QListWidgetItem* > found = availDrawModes->findItems(dm[i],Qt::MatchExactly);
      for(int k=0; k < found.count(); k++)
        (found[k])->setCheckState(Qt::Checked);
    }
  }

  if ( defaultProjectionMode_[_row] )
    projectionBox->setCurrentIndex( 1 );
  else
    projectionBox->setCurrentIndex( 0 );

  directionBox->setCurrentIndex(  defaultViewingDirections_[_row] );

  lockRotationBox->setChecked( defaultRotationLocks_[_row] );

  updatingViewerSettings_ = false;
}

void OptionsWidget::showEvent ( QShowEvent * /*event*/ ) {

  //general
  fullscreen->setChecked( OpenFlipperSettings().value("Core/Gui/fullscreen",false).toBool() );
  splash->setChecked( OpenFlipperSettings().value("Core/Gui/splash",true).toBool() );
  toolbox->setChecked( !OpenFlipperSettings().value("Core/Gui/ToolBoxes/hidden",false).toBool() );
  
  iconSmall->setChecked(false);
  iconNormal->setChecked(false);
  iconDefault->setChecked(false);
  
  switch ( OpenFlipperSettings().value("Core/Toolbar/iconSize",0).toInt() ){
    case 1  : iconSmall->setChecked(true); break;
    case 2  : iconNormal->setChecked(true); break;
    default : iconDefault->setChecked(true); break;
  }
  
  loggerHidden->setChecked( OpenFlipper::Options::loggerState() == OpenFlipper::Options::Hidden);
  loggerNormal->setChecked( OpenFlipper::Options::loggerState() == OpenFlipper::Options::Normal);
  loggerInScene->setChecked( OpenFlipper::Options::loggerState() == OpenFlipper::Options::InScene);
  enableLogFile->setChecked( OpenFlipperSettings().value("Core/Log/logFileEnabled",true).toBool() );

  //Files
  logFile->setText( OpenFlipperSettings().value("Core/Log/logFile").toString()  );
  maxRecentBox->setValue( OpenFlipperSettings().value("Core/File/MaxRecent",15).toInt() );

  // UI settings
  toolBoxOrientation->setCurrentIndex((OpenFlipperSettings().value("Core/Gui/ToolBoxes/ToolBoxOnTheRight",true).toBool() ? 0 : 1));
  
  pickToolbarInScene->setChecked(OpenFlipperSettings().value("Core/Gui/ToolBars/PickToolbarInScene",true).toBool());

  //stereo

  // Grey out OpenGL stereo mode option if not available
  if(!OpenFlipper::Options::glStereo()) {
      stereoOpengl->setDisabled(true);
  } else {
      stereoOpengl->setChecked(OpenFlipper::Options::stereoMode() == OpenFlipper::Options::OpenGL);
  }

  stereoAnaglyph->setChecked (OpenFlipper::Options::stereoMode() == OpenFlipper::Options::AnaglyphRedCyan);
  stereoCustomAnaglyph->setChecked (OpenFlipper::Options::stereoMode() == OpenFlipper::Options::AnaglyphCustom);

  eyeDistance->setValue ( OpenFlipperSettings().value("Core/Stereo/EyeDistance").toDouble() );
  focalDistance->setValue ( OpenFlipperSettings().value("Core/Stereo/FocalDistance").toDouble()  * 1000);

  // Show right stacked widget
  customAnaGlyphSettings->setVisible(false);

  stackedWidget->setCurrentIndex(0);

  if (stereoCustomAnaglyph->isChecked())
    customAnaGlyphSettings->setVisible(true);

  std::vector<float> mat = OpenFlipper::Options::anaglyphLeftEyeColorMatrix ();
  lcm0->setValue (mat[0]);
  lcm1->setValue (mat[1]);
  lcm2->setValue (mat[2]);
  lcm3->setValue (mat[3]);
  lcm4->setValue (mat[4]);
  lcm5->setValue (mat[5]);
  lcm6->setValue (mat[6]);
  lcm7->setValue (mat[7]);
  lcm8->setValue (mat[8]);

  mat = OpenFlipper::Options::anaglyphRightEyeColorMatrix ();
  rcm0->setValue (mat[0]);
  rcm1->setValue (mat[1]);
  rcm2->setValue (mat[2]);
  rcm3->setValue (mat[3]);
  rcm4->setValue (mat[4]);
  rcm5->setValue (mat[5]);
  rcm6->setValue (mat[6]);
  rcm7->setValue (mat[7]);
  rcm8->setValue (mat[8]);

  noMousePick->setChecked ( !OpenFlipperSettings().value("Core/Gui/glViewer/stereoMousePick",true).toBool() );
  nativeMouse->setChecked ( OpenFlipperSettings().value("Core/Gui/glViewer/nativeMouse",false).toBool() );

  // plugin options
  initPluginOptions();

  // updates
  updateUser->setText( OpenFlipperSettings().value("Core/Update/UserName","anonymous").toString() );
  updatePass->setText( OpenFlipperSettings().value("Core/Update/Pass","anonymous").toString() );
  updateURL->setText( OpenFlipperSettings().value("Core/Update/URL","http://").toString() );

  // debugging
  slotDebugging->setChecked(OpenFlipper::Options::doSlotDebugging());

  //keyBindings
  initKeyTree();

  keyTree->disconnect(); //to be sure..disconnect every slot

  connect(keyTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
          this,    SLOT(keyTreeItemChanged( QTreeWidgetItem*, QTreeWidgetItem*)) );

  connect(keyTree, SIGNAL(itemDoubleClicked (QTreeWidgetItem*, int)),
          this,    SLOT(keyTreeDoubleClicked( QTreeWidgetItem*, int)) );


  connect(shortcutButton, SIGNAL(keyChanged()), this, SLOT(updateShortcut()) );
  connect(presetsButton, SIGNAL(clicked()), this, SLOT(restoreKeyPresets()) );


  //Init Viewer Settings
  wZoomFactor->setValue( OpenFlipperSettings().value("Core/Mouse/Wheel/ZoomFactor").toDouble() );
  wZoomFactorShift->setValue( OpenFlipperSettings().value("Core/Mouse/Wheel/ZoomFactorShift").toDouble() );
  invertMouseWheelBox->setChecked( OpenFlipperSettings().value("Core/Mouse/Wheel/Invert").toBool() );

  wheelBox->setChecked( OpenFlipperSettings().value("Core/Gui/glViewer/showControlWheels").toBool() );
  
  // Projection settings
  fieldOfView->setValue(OpenFlipperSettings().value("Core/Projection/FOVY", 45.0).toDouble() );

  restrictFPS->setChecked( OpenFlipperSettings().value("Core/Gui/glViewer/restrictFrameRate",false).toBool() );
  FPS->setValue( OpenFlipperSettings().value("Core/Gui/glViewer/maxFrameRate",35).toInt() );
  
  minimalSceneSize->setValue( OpenFlipperSettings().value("Core/Gui/glViewer/minimalSceneSize",0.1).toDouble());

  QPixmap color(16,16);
  color.fill( OpenFlipperSettings().value("Core/Gui/glViewer/defaultBackgroundColor").value<QColor>() );
  backgroundColor_ = OpenFlipperSettings().value("Core/Gui/glViewer/defaultBackgroundColor").value<QColor>();
  backgroundButton->setIcon( QIcon(color) );

  color.fill( OpenFlipper::Options::defaultColor() );
  defaultColorButton->setIcon( QIcon(color) );

  randomDefaultColor->setChecked( OpenFlipper::Options::randomDefaultColor() );

  viewerList->setCurrentRow(0);

  for ( int i=0; i < PluginFunctions::viewers(); i++ ){
    defaultDrawModes_[i] = OpenFlipper::Options::defaultDrawMode(i);
    defaultProjectionMode_[i] = OpenFlipper::Options::defaultPerspectiveProjectionMode(i);
    defaultViewingDirections_[i] = OpenFlipper::Options::defaultViewingDirection(i);
    defaultRotationLocks_[i] = OpenFlipper::Options::defaultLockRotation(i);
  }

  updateViewerSettings(0);

  viewerLayout->setCurrentIndex( OpenFlipper::Options::defaultViewerLayout() );

  if (OpenFlipperSettings().value("Core/Language/Translation","en_US").toString() == "en_US")
    translation->setCurrentIndex(0);
  else if (OpenFlipperSettings().value("Core/Language/Translation","en_US").toString() == "de_DE")
    translation->setCurrentIndex(1);
  else
    translation->setCurrentIndex(2);
  
  // Listen to changes...
  connect(translation, SIGNAL(currentIndexChanged(int)), this, SLOT(slotTranslationIndexChanged(int)));

  updateVersionsTable();

}

void OptionsWidget::updateVersionsTable() {

  updatedPlugins_.clear();

  QString fileName = QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() +
                     ".OpenFlipper" + OpenFlipper::Options::dirSeparator() + "Versions.ini" ;

  INIFile ini;

  ini.connect(fileName,false);

  QString systemString = "";

  if ( OpenFlipper::Options::isWindows() ) {
    systemString = "VersionWindows";
  } else if (OpenFlipper::Options::isLinux()) {
    systemString = "VersionLinux";
  } else {
    std::cerr << "Unsupported platform for update" << std::endl;
    return;
  }

  updateList->clear();
  updateList->setRowCount( 1 + plugins_.size() );
  updateList->setColumnCount(3);

  QStringList header;

  header << "Component" << "current Version" << "latest Version" ;
  updateList->setHorizontalHeaderLabels(header);

  QBrush currentBrush(Qt::white);

  QTableWidgetItem * newItem = 0;

  QString coreVersion;

  if ( ini.is_connected() && ini.get_entry(coreVersion, "Core" , systemString ) ) {

    // Newer Version available
    if ( isNewer(OpenFlipper::Options::coreVersion(),coreVersion) ) {
      currentBrush.setColor(Qt::red);
      updatedPlugins_ << "Core";
    } else if ( isNewer(coreVersion , OpenFlipper::Options::coreVersion()) )
      currentBrush.setColor(Qt::blue);
    else
      currentBrush.setColor(Qt::green);

    newItem = new QTableWidgetItem( coreVersion );

  } else {

    // No local Information available
    currentBrush.setColor(Qt::yellow);

    newItem = new QTableWidgetItem( "Not Available" );
  }

  newItem->setBackground(currentBrush);
  updateList->setItem(0, 2, newItem);

  newItem = new QTableWidgetItem( OpenFlipper::Options::coreVersion() );
  newItem->setBackground(currentBrush);
  updateList->setItem(0, 1, newItem);

  newItem = new QTableWidgetItem( "Core" );
  newItem->setBackground(currentBrush);
  updateList->setItem(0, 0, newItem);


  for ( uint i = 0 ; i < plugins_.size(); ++i ) {
    QString latestVersion;

    if ( ini.is_connected() && ini.get_entry(latestVersion, plugins_[i].name , systemString ) ) {


      // Newer Version available
      if ( isNewer(plugins_[i].version,latestVersion) ) {
         currentBrush.setColor(Qt::red);
         QFileInfo pluginFile(plugins_[i].path );
         updatedPlugins_ << pluginFile.fileName();
      } else if ( isNewer(latestVersion,plugins_[i].version) )
         currentBrush.setColor(Qt::blue);
      else
         currentBrush.setColor(Qt::green);

      newItem = new QTableWidgetItem( latestVersion );

    } else {

      // No local Information available
      currentBrush.setColor(Qt::yellow);

      newItem = new QTableWidgetItem( "Not Available" );
    }

    newItem->setBackground(currentBrush);
    updateList->setItem( i + 1 , 2, newItem);

    newItem = new QTableWidgetItem( plugins_[i].version );
    newItem->setBackground(currentBrush);
    updateList->setItem( i + 1 , 1, newItem);

    newItem = new QTableWidgetItem( plugins_[i].name );
    newItem->setBackground(currentBrush);
    updateList->setItem( i + 1 , 0, newItem);
  }

  updateList->resizeColumnsToContents();

  updateButton->setEnabled(!updatedPlugins_.empty());

}

void OptionsWidget::initPluginOptions(){

  pluginList->disconnect();

  connect(pluginList, SIGNAL( currentTextChanged(const QString&) ), this, SLOT( slotShowPluginOptions(const QString&) ) );

  //init list of plugins
  pluginList->clear();

  for ( uint i = 0 ; i < plugins_.size(); ++i )
    if (plugins_[i].optionsWidget != 0){

      pluginList->addItem( plugins_[i].name );
    }

  if ( pluginList->count() > 0)
    pluginList->setCurrentRow(0);
}

void OptionsWidget::slotShowPluginOptions(const QString& _pluginName ){

  //remove old children
  for (int i = 0; i < pluginOptionsLayout->count(); ++i){
    QWidget* w = pluginOptionsLayout->itemAt(i)->widget();

    if (w != 0)
      w->setParent(0);

    pluginOptionsLayout->removeItem( pluginOptionsLayout->itemAt(i) );
  }

  //find the new optionsWidget
  for ( uint i = 0 ; i < plugins_.size(); ++i )
    if (plugins_[i].optionsWidget != 0 && plugins_[i].name == _pluginName){

      pluginOptionsLayout->addWidget( plugins_[i].optionsWidget );
      pluginOptionsLayout->addStretch();
      return;
    }
}

void OptionsWidget::slotApply() {

  //general
  OpenFlipperSettings().setValue("Core/Gui/fullscreen",fullscreen->isChecked());
  OpenFlipperSettings().setValue("Core/Gui/splash",splash->isChecked());
  OpenFlipperSettings().setValue("Core/Gui/ToolBoxes/hidden",!toolbox->isChecked());
  
  if (loggerHidden->isChecked())
    OpenFlipper::Options::loggerState( OpenFlipper::Options::Hidden );
  else if (loggerInScene->isChecked())
    OpenFlipper::Options::loggerState( OpenFlipper::Options::InScene );
  else
    OpenFlipper::Options::loggerState( OpenFlipper::Options::Normal );
  
  OpenFlipperSettings().setValue("Core/Log/logFileEnabled",enableLogFile->isChecked());
  OpenFlipperSettings().setValue("Core/Log/logFile",logFile->text());

  OpenFlipperSettings().setValue("Core/File/MaxRecent",maxRecentBox->value() ) ;

  
  // Toolbox orientation
  OpenFlipperSettings().setValue("Core/Gui/ToolBoxes/ToolBoxOnTheRight", (toolBoxOrientation->currentIndex() == 0));
  
  // Render picking toolbar into scene
  if ( OpenFlipperSettings().value("Core/Gui/ToolBars/PickToolbarInScene",true).toBool() != pickToolbarInScene->isChecked() )  {
    OpenFlipperSettings().setValue("Core/Gui/ToolBars/PickToolbarInScene", pickToolbarInScene->isChecked());
    restartRequired_ = true;
  }

  if ( iconDefault->isChecked() )
    OpenFlipperSettings().setValue("Core/Toolbar/iconSize", 0);
  else if ( iconSmall->isChecked() )
    OpenFlipperSettings().setValue("Core/Toolbar/iconSize", 1);
  else if ( iconNormal->isChecked() )
    OpenFlipperSettings().setValue("Core/Toolbar/iconSize", 2);
  
  //viewer
  OpenFlipperSettings().setValue("Core/Mouse/Wheel/ZoomFactor", wZoomFactor->text().toDouble());
  OpenFlipperSettings().setValue("Core/Mouse/Wheel/ZoomFactorShift", wZoomFactorShift->text().toDouble());
  OpenFlipperSettings().setValue("Core/Mouse/Wheel/Invert", invertMouseWheelBox->isChecked());

  // Projection settings
  OpenFlipperSettings().setValue("Core/Projection/FOVY", fieldOfView->value());
  OpenFlipperSettings().setValue("Core/Gui/glViewer/minimalSceneSize",minimalSceneSize->value());

  OpenFlipperSettings().setValue("Core/Gui/glViewer/showControlWheels", wheelBox->isChecked() );

  OpenFlipperSettings().setValue("Core/Gui/glViewer/restrictFrameRate", restrictFPS->isChecked());
  OpenFlipperSettings().setValue("Core/Gui/glViewer/maxFrameRate", FPS->value() );

  OpenFlipper::Options::randomDefaultColor( randomDefaultColor->isChecked() );

  //stereo
  if (stereoCustomAnaglyph->isChecked ())
     OpenFlipper::Options::stereoMode(OpenFlipper::Options::AnaglyphCustom);
  else if (stereoAnaglyph->isChecked ())
    OpenFlipper::Options::stereoMode(OpenFlipper::Options::AnaglyphRedCyan);
  else
    OpenFlipper::Options::stereoMode(OpenFlipper::Options::OpenGL);

  OpenFlipperSettings().setValue("Core/Stereo/EyeDistance",eyeDistance->value ());
  OpenFlipperSettings().setValue("Core/Stereo/FocalDistance",double(focalDistance->value() / 1000.0));

  std::vector<float> mat (9, 0);
  mat[0] = lcm0->value ();
  mat[1] = lcm1->value ();
  mat[2] = lcm2->value ();
  mat[3] = lcm3->value ();
  mat[4] = lcm4->value ();
  mat[5] = lcm5->value ();
  mat[6] = lcm6->value ();
  mat[7] = lcm7->value ();
  mat[8] = lcm8->value ();

  OpenFlipper::Options::anaglyphLeftEyeColorMatrix (mat);

  mat[0] = rcm0->value ();
  mat[1] = rcm1->value ();
  mat[2] = rcm2->value ();
  mat[3] = rcm3->value ();
  mat[4] = rcm4->value ();
  mat[5] = rcm5->value ();
  mat[6] = rcm6->value ();
  mat[7] = rcm7->value ();
  mat[8] = rcm8->value ();

  OpenFlipper::Options::anaglyphRightEyeColorMatrix (mat);
  
  OpenFlipperSettings().setValue("Core/Gui/glViewer/stereoMousePick",!noMousePick->isChecked ());
  OpenFlipperSettings().setValue("Core/Gui/glViewer/nativeMouse",nativeMouse->isChecked ());

  // updates
  OpenFlipperSettings().setValue("Core/Update/UserName", updateUser->text());
  OpenFlipperSettings().setValue("Core/Update/Pass",updatePass->text());
  OpenFlipperSettings().setValue("Core/Update/URL",updateURL->text());

  // Debugging
  OpenFlipper::Options::doSlotDebugging(slotDebugging->isChecked());

  //viewer defaults
  for (int i=0; i < PluginFunctions::viewers(); i++){
    OpenFlipper::Options::defaultDrawMode(         defaultDrawModes_[i],         i );
    OpenFlipper::Options::defaultPerspectiveProjectionMode(   defaultProjectionMode_[i],    i );
    OpenFlipper::Options::defaultViewingDirection( defaultViewingDirections_[i], i );
    OpenFlipper::Options::defaultLockRotation( defaultRotationLocks_[i], i );
  }

  OpenFlipper::Options::defaultViewerLayout( viewerLayout->currentIndex() );
  
  // Restrict fps if desired
  if(restrictFPS->isChecked()) {
      OpenFlipperSettings().setValue("Core/Gui/glViewer/restrictFrameRate", true);
      OpenFlipperSettings().setValue("Core/Gui/glViewer/maxFrameRate",FPS->value());
  } else {
      OpenFlipperSettings().setValue("Core/Gui/glViewer/restrictFrameRate", false);
  }
  
  // Set background color
  OpenFlipperSettings().setValue("Core/Gui/glViewer/defaultBackgroundColor", backgroundColor_);
  
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i )
      PluginFunctions::viewerProperties(i).backgroundColor(ACG::Vec4f(((double) backgroundColor_.redF()),
                                                                      ((double) backgroundColor_.greenF()),
                                                                      ((double) backgroundColor_.blueF()),
                                                                        1.0));

  switch ( translation->currentIndex() ){
    case 0 : OpenFlipperSettings().setValue("Core/Language/Translation","en_US");  break;
    case 1 : OpenFlipperSettings().setValue("Core/Language/Translation","de_DE");  break;
    default: OpenFlipperSettings().setValue("Core/Language/Translation","locale"); break;
  }

  // Show warning message that restart is required if language or in scene toolbar has been changed...
  if(restartRequired_) {
    int restart = QMessageBox::information(this, tr("Restart required!"),
                      tr("The changes will take effect after next restart. Do you want to close OpenFlipper now?"),
                      QMessageBox::Yes | QMessageBox::No);
                    
    if(restart == QMessageBox::Yes) exitOnClose_ = true;
  }

  applyShortcuts();

  emit applyOptions();
  emit saveOptions();

  // Make sure all settings are written before we quit!
  OpenFlipperSettings().sync();

  // Close OpenFlipper if demanded
  if(exitOnClose_)  QCoreApplication::quit();
}

void OptionsWidget::slotOk(){
  slotApply();
  hide();
}

void OptionsWidget::slotCancel() {
  hide();
}

void OptionsWidget::slotCheckUpdates() {

   // http://www.graphics.rwth-aachen.de/restricted/OpenFlipper-SIL/

  downloadType = VERSIONS_FILE;
  QString ServerMainURL = updateURL->text();
  if ( !ServerMainURL.endsWith("/") )
     ServerMainURL += "/";

  ServerMainURL += "Versions.ini";

  startDownload(ServerMainURL);
}

void OptionsWidget::slotGetUpdates() {
   std::cerr << "Not implemented yet: Get updates" << std::endl;
   QString url = updateURL->text();

   if ( !url.endsWith("/") )
     url += "/";


   pluginPath_.clear();
   pluginPath_ = "Plugins/";

   if ( OpenFlipper::Options::isWindows() ) {
      pluginPath_ += "Windows/";
   } else if ( OpenFlipper::Options::isLinux() ) {
      pluginPath_ += "Linux/";
   } else {
      std::cerr << "Unknown operating system type, aborting update" << std::endl;
      return;
   }

   if ( OpenFlipper::Options::is64bit() ) {
      pluginPath_ += "64/";
   } else if ( OpenFlipper::Options::is32bit() ) {
      pluginPath_ += "32/";
   } else {
      std::cerr << "Unknown architecture type, aborting update" << std::endl;
      return;
   }

   pluginPath_ += "Release/";

   if ( !updatedPlugins_.empty() && updatedPlugins_[0] == "Core" ) {
      std::cerr << "Core update not supported!" << std::endl;
      return;
   }

   if ( !updatedPlugins_.empty() ) {
      currentUpdateName_ = updatedPlugins_[0];
      std::cerr << "Downloading " << (url + pluginPath_ + currentUpdateName_).toStdString() << std::endl;
      updatedPlugins_.pop_front();

      downloadType = PLUGIN;

      startDownload(url + pluginPath_ + currentUpdateName_);
   }

}

void OptionsWidget::updateComponent() {
   std::cerr << "Todo : Update component" << std::endl;



   QString sourceName = QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() +
                                     ".OpenFlipper" + OpenFlipper::Options::dirSeparator() + currentUpdateName_ ;

   QString targetName = OpenFlipper::Options::applicationDirStr() + "/" + pluginPath_ + currentUpdateName_;

   QFileInfo sourceFileInfo(sourceName);
   QFileInfo targetFileInfo(targetName);

   std::cerr << sourceName.toStdString() << std::endl;
   std::cerr << targetName.toStdString() << std::endl;

   if ( ! sourceFileInfo.exists() ) {
      statusLabel->setText(tr("Download failed!"));
      slotGetUpdates();
      return;
   } else {
      if ( ! targetFileInfo.exists() ) {
         statusLabel->setText(tr("plugin target does not exist"));
         slotGetUpdates();
         return;
      }

      statusLabel->setText(tr("Installing new file"));

      // Remove the old plugin
      QFile targetFile(targetName);
      targetFile.remove();

      // copy new to old
      QFile::copy(sourceName,targetName);

      // remove downloaded file
      QFile sourceFile(sourceName);
      sourceFile.remove();

      statusLabel->setText(tr("updated ") + currentUpdateName_);
   }

   slotGetUpdates();
}

bool OptionsWidget::isNewer(QString _current, QString _latest) {

  QStringList latestVersionParts  = _latest.split('.');
  QStringList currentVersionParts = _current.split('.');

  bool newer = false;

  for ( int i = 0 ; i < latestVersionParts.size(); ++i ) {

    if ( i+1 > currentVersionParts.size() ) {
      // Versions are identical up to now. But latest version has additional version => updated
      newer = true;
      break;
    }

    bool ok = false;
    double latest  = latestVersionParts[i].toInt(&ok);
    double current = currentVersionParts[i].toInt(&ok);

    if ( !ok )
      std::cerr << "Error when parsing version strings!" << std::endl;

    if ( latest > current ) {
      newer = true;
      break;
    }
  }

  return newer;

}

void OptionsWidget::compareVersions() {

  QString fileName = QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() +
                     ".OpenFlipper" + OpenFlipper::Options::dirSeparator() + "Versions.ini" ;

  INIFile ini;
  if ( ! ini.connect(fileName,false) ) {
    std::cerr << "Failed to connect to Versions ini file" << std::endl;
    return;
  }

  statusLabel->setText(tr("Checking for new versions"));

  bool newerVersionsAvailable = false;

  QString systemString = "";
  if ( OpenFlipper::Options::isWindows() ) {
    systemString = "VersionWindows";
  } else if (OpenFlipper::Options::isLinux()) {
    systemString = "VersionLinux";
  } else {
    std::cerr << "Unsupported platform for update" << std::endl;
    return;
  }

  QString coreVersion;

  if ( ini.get_entry(coreVersion, "Core" , systemString )) {
    if ( isNewer( OpenFlipper::Options::coreVersion(), coreVersion ) ) {
      newerVersionsAvailable = true;
    }
  }

  for ( uint i = 0 ; i < plugins_.size(); ++i ) {
    QString latestVersion;
    if ( ini.get_entry(latestVersion, plugins_[i].name , systemString )) {
      if ( isNewer(  plugins_[i].version, latestVersion ) )
        newerVersionsAvailable = true;
    }
  }

  if ( newerVersionsAvailable ) {
    statusLabel->setText(tr("Updates found"));
  } else {
    statusLabel->setText(tr("No updates found"));
  }

  ini.disconnect();

  updateVersionsTable();
}

void OptionsWidget::slotPreviewStereoSettings(int /*_tmpParam*/) {
    
  if (stereoCustomAnaglyph->isChecked ())
     OpenFlipper::Options::stereoMode(OpenFlipper::Options::AnaglyphCustom);
  else if (stereoAnaglyph->isChecked ())
    OpenFlipper::Options::stereoMode(OpenFlipper::Options::AnaglyphRedCyan);
  else
    OpenFlipper::Options::stereoMode(OpenFlipper::Options::OpenGL);

  OpenFlipperSettings().setValue("Core/Stereo/EyeDistance", eyeDistance->value());
  OpenFlipperSettings().setValue("Core/Stereo/FocalDistance", double(focalDistance->value() / 1000.0));

  
  // Update all views
 emit applyOptions();
}

void OptionsWidget::slotClearSettings() {
    
    OpenFlipperSettings().clear();
}

void OptionsWidget::slotClearINI() {
    
    QMessageBox::information(this, tr("Restart required!"),
                             tr("The changes will take effect after next restart."));
    
    OpenFlipper::Options::deleteIniFile(true);
}
