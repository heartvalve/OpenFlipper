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

OptionsWidget::OptionsWidget(std::vector<PluginInfo>& _plugins, std::vector<KeyBinding>& _core, QWidget *parent)
  : QWidget(parent),
    plugins_(_plugins),
    coreKeys_(_core)

{
  setupUi(this);

  connect(applyButton,SIGNAL(clicked()),this,SLOT(slotApply()));
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(slotCancel()));
  connect(checkUpdateButton,SIGNAL(clicked()),this,SLOT(slotCheckUpdates()));

  uint mode = 2;
  for (uint i=1; i < 22; i++) {
    std::vector< QString > dm = drawModeToList( mode );

    if ( !dm.empty() && dm[0].trimmed() != ""){
      QListWidgetItem* item = new QListWidgetItem(dm[0]);

      item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
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
}

void OptionsWidget::showEvent ( QShowEvent * event ) {

  //general
  fullscreen->setChecked( OpenFlipper::Options::fullScreen() );
  splash->setChecked( OpenFlipper::Options::splash() );
  toolbox->setChecked( !OpenFlipper::Options::hideToolbox() );
  logger->setChecked( !OpenFlipper::Options::hideLogger() );
  enableLogFile->setChecked( OpenFlipper::Options::logFileEnabled() );

  //paths
  logFile->setText( OpenFlipper::Options::logFile() );

  //viewer
  backfaceCulling->setChecked( OpenFlipper::Options::backfaceCulling() );
  wZoomFactor->setText( QString::number(OpenFlipper::Options::wheelZoomFactor(), 'f') );
  wZoomFactorShift->setText( QString::number(OpenFlipper::Options::wheelZoomFactorShift(), 'f') );

  restrictFPS->setChecked( OpenFlipper::Options::restrictFrameRate() );
  FPS->setValue( OpenFlipper::Options::maxFrameRate() );

  // debugging
  slotDebugging->setChecked(OpenFlipper::Options::doSlotDebugging());

  //set drawmodes


  //keyBindings
  keyTree->clear();

  keyTree->setColumnCount ( 2 );

  QStringList headerdata;
  headerdata << "Action" << "Shortcut";
  keyTree->setHeaderLabels(headerdata);

  //add Core Keys
  QTreeWidgetItem * core = new QTreeWidgetItem(keyTree, QStringList("CoreWidget"));

  QList<QTreeWidgetItem *> keys;

  for (uint i=0; i < coreKeys_.size(); i++){
    QStringList row;
    QKeySequence keySequence( coreKeys_[i].key + coreKeys_[i].modifiers );
    row << coreKeys_[i].description << keySequence.toString();
    keys.append(new QTreeWidgetItem(core, row));
  }

  core->addChildren(keys);
  keyTree->addTopLevelItem( core );



  QList<QTreeWidgetItem *> plugins;

  int off = 0;

  for (uint i=0; i < plugins_.size(); i++){
//      if (( (plugins_[i]).keys).size() == 0)
//        continue;

    plugins.append(new QTreeWidgetItem(keyTree, QStringList( plugins_[i].name )));

    QList<QTreeWidgetItem *> keys;

    for (int k=0; k < plugins_[i].keys.count(); k++){
      QStringList row;
      QKeySequence keySequence( plugins_[i].keys[k].key + plugins_[i].keys[k].modifiers );
      row << plugins_[i].keys[k].description << keySequence.toString();
      keys.append(new QTreeWidgetItem(plugins[i-off], row));
    }

    if (keys.count() > 0)
      plugins[i-off]->addChildren(keys);
    else{
      delete plugins[i-off];
      plugins.removeLast();
      off++;
    }
  }

  if (plugins.count() > 0)
    keyTree->addTopLevelItems( plugins );

  keyTree->setColumnWidth(0,350);


  for (int i = 0 ; i < availDrawModes->count(); ++i )
    availDrawModes->item( i )->setCheckState(Qt::Unchecked) ;

  //Check the drawModes from StandardDrawMode
  std::vector< QString > dm = drawModeToList( OpenFlipper::Options::standardDrawMode() );

  for (uint i=0; i < dm.size(); i++){
    if ( !dm.empty() ) {
      QList< QListWidgetItem* > found = availDrawModes->findItems(dm[i],Qt::MatchExactly);
      for(int k=0; k < found.count(); k++)
        (found[k])->setCheckState(Qt::Checked);
    }
  }
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
  OpenFlipper::Options::backfaceCulling( backfaceCulling->isChecked() );
  OpenFlipper::Options::wheelZoomFactor( wZoomFactor->text().toDouble() );
  OpenFlipper::Options::wheelZoomFactorShift( wZoomFactorShift->text().toDouble() );

  OpenFlipper::Options::restrictFrameRate( restrictFPS->isChecked() );
  OpenFlipper::Options::maxFrameRate( FPS->value() );

  // Debugging
  OpenFlipper::Options::doSlotDebugging(slotDebugging->isChecked());

  //standardDrawMode
  std::vector< QString > mode;

  for (int i=0; i < availDrawModes->count(); i++)
    if (availDrawModes->item(i)->checkState() == Qt::Checked)
      mode.push_back( availDrawModes->item(i)->text() );

  OpenFlipper::Options::standardDrawMode( ListToDrawMode(mode) );

  emit applyOptions();
  emit saveOptions();

  hide();
}

void OptionsWidget::slotCancel() {
  hide();
}

void OptionsWidget::startDownload( QString _url ) {
   QUrl url(_url);

   // If username or passowrd are supplied, use them
   if ( ! updateUser->text().isEmpty() )
    url.setUserName(updateUser->text());

  if ( ! updatePass->text().isEmpty() )
    url.setPassword(updatePass->text());


  QFileInfo urlInfo(_url);

  // Download the file to the Home Directory
  QFileInfo fileInfo( QDir::home().absolutePath() + OpenFlipper::Options::dirSeparator() +
                      ".OpenFlipper" + OpenFlipper::Options::dirSeparator() + urlInfo.fileName() );

  QString fileName = fileInfo.filePath();

  if (QFile::exists(fileName)) {
    QFile::remove(fileName);
  }

  file = new QFile(fileName);
  if (!file->open(QIODevice::WriteOnly)) {
    std::cerr << "Unable to Open local file for writing" << std::endl;
    delete file;
    file = 0;
  } else {
    QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
    http->setHost(url.host(), mode, url.port() == -1 ? 0 : url.port());

    if (!url.userName().isEmpty())
        http->setUser(url.userName(), url.password());

    httpRequestAborted = false;
    QByteArray path = QUrl::toPercentEncoding(url.path(), "!$&'()*+,;=:@/");
    if (path.isEmpty())
      path = "/";

    httpGetId = http->get(path, file);

    statusLabel->setText("Getting Versions file from " + _url);

    progressDialog->setWindowTitle(tr("HTTP"));
    progressDialog->setLabelText(tr("Downloading %1.").arg(fileName));
    checkUpdateButton->setEnabled(false);
  }

}

void OptionsWidget::slotCheckUpdates() {

   // http://www.graphics.rwth-aachen.de/restricted/OpenFlipper-SIL/

  downloadType = VERSIONS_FILE;
  QString ServerMainURL = updateURL->text() + "Versions.txt";

  startDownload(ServerMainURL);

}

void OptionsWidget::slotGetUpdates() {
}

void OptionsWidget::httpRequestFinished(int requestId, bool error)
{
    if (requestId != httpGetId)
        return;
    if (httpRequestAborted) {
        if (file) {
            file->close();
            file->remove();
            delete file;
            file = 0;
        }

        progressDialog->hide();
        return;
    }

    if (requestId != httpGetId)
        return;

    progressDialog->hide();
    file->close();

    if (error) {
        file->remove();
        statusLabel->setText(tr("Download failed: %1.").arg(http->errorString()));
        QMessageBox::information(this, tr("HTTP"),
                                  tr("Download failed: %1.")
                                  .arg(http->errorString()));
    } else {
        QString fileName = QFileInfo(QUrl(updateURL->text()).path()).fileName();
        statusLabel->setText(tr("Downloaded %1").arg(file->fileName() ));
    }

    checkUpdateButton->setEnabled(true);
    delete file;
    file = 0;
}

void OptionsWidget::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
  switch (responseHeader.statusCode()) {
  case 200:                   // Ok
  case 301:                   // Moved Permanently
  case 302:                   // Found
  case 303:                   // See Other
  case 307:                   // Temporary Redirect
    // these are not error conditions
    break;

  default:
    QMessageBox::information(this, tr("HTTP"),
                              tr("Download failed: %1.")
                              .arg(responseHeader.reasonPhrase()));
    statusLabel->setText("Download failed: " + responseHeader.reasonPhrase());
    httpRequestAborted = true;
    progressDialog->hide();
    http->abort();
  }
 }

void OptionsWidget::cancelDownload()
{
  statusLabel->setText(tr("download canceled."));
  httpRequestAborted = true;
  http->abort();
  checkUpdateButton->setEnabled(true);
}

void OptionsWidget::updateDataReadProgress(int bytesRead, int totalBytes)
{
  if (httpRequestAborted)
    return;

  progressDialog->setMaximum(totalBytes);
  progressDialog->setValue(bytesRead);
}





