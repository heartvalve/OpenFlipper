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
//   $Revision: 1909 $
//   $Author: wilden $
//   $Date: 2008-06-03 18:45:21 +0200 (Tue, 03 Jun 2008) $
//
//=============================================================================




#include "optionsWidget.hh"
#include <iostream>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <OpenFlipper/ACGHelper/DrawModeConverter.hh>
#include <OpenFlipper/INIFile/INIFile.hh>

#include <QColorDialog>

OptionsWidget::OptionsWidget(std::vector<PluginInfo>& _plugins, std::vector<KeyBinding>& _core, InverseKeyMap& _invKeys, QWidget *parent)
  : QWidget(parent),
    plugins_(_plugins),
    coreKeys_(_core),
    keys_(_invKeys)

{
  setupUi(this);

  connect(applyButton,SIGNAL(clicked()),this,SLOT(slotApply()));
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(slotCancel()));
  connect(checkUpdateButton,SIGNAL(clicked()),this,SLOT(slotCheckUpdates()));
  connect(updateButton,SIGNAL(clicked()),this,SLOT(slotGetUpdates()));
  updateButton->setEnabled(false);
  connect( restrictFPS, SIGNAL(toggled(bool)), FPS, SLOT(setEnabled(bool)) );
  uint mode = 2;
  for (uint i=1; i < 22; i++) {
    std::vector< QString > dm = drawModeToDescriptions( mode );

    if ( !dm.empty() && dm[0].trimmed() != ""){
      QListWidgetItem* item = new QListWidgetItem(dm[0]);

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);

//       if (OpenFlipper::Options::standardDrawMode() )
//         item->setCheckState(Qt::Checked);
//       else
        item->setCheckState(Qt::Unchecked);

      availDrawModes->addItem( item );
    }

    mode = mode<<1;

  }

  http = new QHttp(this);

  // http specific connections
  connect(http, SIGNAL(requestFinished(int, bool)),
        this, SLOT(httpRequestFinished(int, bool)));
  connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
        this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
  connect(http, SIGNAL(dataReadProgress(int, int)),
          this, SLOT(updateDataReadProgress(int, int)));


  progressDialog = new QProgressDialog(this);
  connect(progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));


  //colordialog
  connect(backgroundButton, SIGNAL(clicked()), this, SLOT(getBackgroundColor()) );
}

void OptionsWidget::getBackgroundColor(){
  QColor newColor = QColorDialog::getColor (QColor(OpenFlipper::Options::defaultBackgroundColor()));

  OpenFlipper::Options::defaultBackgroundColor( newColor.rgb() );

  QPixmap color(16,16);
  color.fill(QColor(OpenFlipper::Options::defaultBackgroundColor()));
  backgroundButton->setIcon( QIcon(color) );
}

void OptionsWidget::showEvent ( QShowEvent * /*event*/ ) {

  //general
  fullscreen->setChecked( OpenFlipper::Options::fullScreen() );
  splash->setChecked( OpenFlipper::Options::splash() );
  toolbox->setChecked( !OpenFlipper::Options::hideToolbox() );
  logger->setChecked( !OpenFlipper::Options::hideLogger() );
  enableLogFile->setChecked( OpenFlipper::Options::logFileEnabled() );

  //paths
  logFile->setText( OpenFlipper::Options::logFile() );

  //viewer
  animation->setChecked( OpenFlipper::Options::animation() );
  backfaceCulling->setChecked( OpenFlipper::Options::backfaceCulling() );
  wZoomFactor->setText( QString::number(OpenFlipper::Options::wheelZoomFactor(), 'f') );
  wZoomFactorShift->setText( QString::number(OpenFlipper::Options::wheelZoomFactorShift(), 'f') );

  restrictFPS->setChecked( OpenFlipper::Options::restrictFrameRate() );
  FPS->setValue( OpenFlipper::Options::maxFrameRate() );

  QPixmap color(16,16);
  color.fill(QColor(OpenFlipper::Options::defaultBackgroundColor()));
  backgroundButton->setIcon( QIcon(color) );

  // updates
  updateUser->setText( OpenFlipper::Options::updateUsername() );
  updatePass->setText( OpenFlipper::Options::updatePassword() );
  updateURL->setText( OpenFlipper::Options::updateUrl( ) );

  // debugging
  slotDebugging->setChecked(OpenFlipper::Options::doSlotDebugging());

  renderPicking->setChecked(OpenFlipper::Options::renderPicking());
  int itemIndex = pickingRenderMode->findText( OpenFlipper::Options::pickingRenderMode() );
  if (  itemIndex != -1 )
    pickingRenderMode->setCurrentIndex(itemIndex);

  //keyBindings
  initKeyTree();

  keyTree->disconnect(); //to be sure..disconnect every slot

  connect(keyTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
          this,    SLOT(keyTreeItemChanged( QTreeWidgetItem*, QTreeWidgetItem*)) );

  connect(keyTree, SIGNAL(itemDoubleClicked (QTreeWidgetItem*, int)),
          this,    SLOT(keyTreeDoubleClicked( QTreeWidgetItem*, int)) );


  connect(shortcutButton, SIGNAL(keyChanged()), this, SLOT(updateShortcut()) );


  for (int i = 0 ; i < availDrawModes->count(); ++i )
    availDrawModes->item( i )->setCheckState(Qt::Unchecked) ;

  //Check the drawModes from StandardDrawMode
  std::vector< QString > dm = drawModeToDescriptions( OpenFlipper::Options::standardDrawMode() );

  for (uint i=0; i < dm.size(); i++){
    if ( !dm.empty() ) {
      QList< QListWidgetItem* > found = availDrawModes->findItems(dm[i],Qt::MatchExactly);
      for(int k=0; k < found.count(); k++)
        (found[k])->setCheckState(Qt::Checked);
    }
  }

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

void OptionsWidget::slotApply() {

  //general
  OpenFlipper::Options::fullScreen( fullscreen->isChecked() );
  OpenFlipper::Options::splash( splash->isChecked() );
  OpenFlipper::Options::hideToolbox( !toolbox->isChecked() );
  OpenFlipper::Options::hideLogger( !logger->isChecked() );
  OpenFlipper::Options::logFileEnabled( enableLogFile->isChecked() );

  //paths
  OpenFlipper::Options::logFile( logFile->text() );

  //viewer
  OpenFlipper::Options::animation( animation->isChecked() );
  OpenFlipper::Options::backfaceCulling( backfaceCulling->isChecked() );
  OpenFlipper::Options::wheelZoomFactor( wZoomFactor->text().toDouble() );
  OpenFlipper::Options::wheelZoomFactorShift( wZoomFactorShift->text().toDouble() );

  OpenFlipper::Options::restrictFrameRate( restrictFPS->isChecked() );
  OpenFlipper::Options::maxFrameRate( FPS->value() );

  // updates
  OpenFlipper::Options::updateUrl( updateURL->text() );
  OpenFlipper::Options::updateUsername( updateUser->text() );
  OpenFlipper::Options::updatePassword( updatePass->text() );

  // Debugging
  OpenFlipper::Options::doSlotDebugging(slotDebugging->isChecked());

  OpenFlipper::Options::renderPicking(renderPicking->isChecked());
  OpenFlipper::Options::pickingRenderMode( pickingRenderMode->currentText() );

  //standardDrawMode
  std::vector< QString > mode;

  for (int i=0; i < availDrawModes->count(); i++)
    if (availDrawModes->item(i)->checkState() == Qt::Checked)
      mode.push_back( availDrawModes->item(i)->text() );

  OpenFlipper::Options::standardDrawMode( descriptionsToDrawMode(mode) );

  applyShortcuts();

  emit applyOptions();
  emit saveOptions();

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
      statusLabel->setText("Download failed!");
      slotGetUpdates();
      return;
   } else {
      if ( ! targetFileInfo.exists() ) {
         statusLabel->setText("plugin target does not exist");
         slotGetUpdates();
         return;
      }

      statusLabel->setText("Installing new file");

      // Remove the old plugin
      QFile targetFile(targetName);
      targetFile.remove();

      // copy new to old
      QFile::copy(sourceName,targetName);

      // remove downloaded file
      QFile sourceFile(sourceName);
      sourceFile.remove();

      statusLabel->setText("updated " + currentUpdateName_);
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

  statusLabel->setText("Checking for new versions");

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
    statusLabel->setText("Updates found");
  } else {
    statusLabel->setText("No updates found");
  }

  ini.disconnect();

  updateVersionsTable();
}
