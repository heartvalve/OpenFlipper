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
    std::cerr << "Unable to Open local file " + fileName.toStdString() + " for writing" << std::endl;
    delete file;
    file = 0;
    checkUpdateButton->setEnabled(true);
  } else {
    QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
    http->setHost(url.host(), mode, url.port() == -1 ? 0 : url.port());

    if (!url.userName().isEmpty())
        http->setUser(url.userName(), url.password());

    httpRequestAborted = false;
    QByteArray path = QUrl::toPercentEncoding(url.path(), "!$&'()*+,;=:@/");
    if (path.isEmpty())
      path = "/";

    statusLabel->setText("Getting Versions file from Server");

    progressDialog->setWindowTitle(tr("HTTP"));
    progressDialog->setLabelText(tr("Downloading %1.").arg(fileName));
    progressDialog->show();

    httpGetId = http->get(path, file);

    checkUpdateButton->setEnabled(false);
  }

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
        checkUpdateButton->setEnabled(true);
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
                                  .arg(http->errorString()) + file->fileName() );
    } else {
        QString fileName = QFileInfo(QUrl(updateURL->text()).path()).fileName();
        statusLabel->setText(tr("Downloaded %1").arg(file->fileName() ));
    }

    checkUpdateButton->setEnabled(true);
    delete file;
    file = 0;

    if ( !error ) {
      if ( downloadType == VERSIONS_FILE )
         compareVersions();
      if ( downloadType == PLUGIN )
         updateComponent();
    }
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





