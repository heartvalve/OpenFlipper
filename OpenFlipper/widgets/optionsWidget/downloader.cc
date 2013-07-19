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

void OptionsWidget::startDownload( QString _url ) {
   QUrl url(_url);

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
    QNetworkRequest req;
    req.setUrl(url);

    httpRequestAborted = false;
    QByteArray path = QUrl::toPercentEncoding(url.path(), "!$&'()*+,;=:@/");
    if (path.isEmpty())
      path = "/";

    statusLabel->setText(tr("Getting Versions file from Server"));

    progressDialog->setWindowTitle(tr("HTTP"));
    progressDialog->setLabelText(tr("Downloading %1.").arg(fileName));
    progressDialog->show();

    downloadRep_ = networkMan_->get(req);

    connect(downloadRep_, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(showError(QNetworkReply::NetworkError)));
    connect(downloadRep_,SIGNAL(downloadProgress(qint64 , qint64 )),
            this,SLOT(updateDataReadProgress(qint64 , qint64 )));

    checkUpdateButton->setEnabled(false);
  }

}

void OptionsWidget::authentication  ( QNetworkReply* _reply, QAuthenticator* _authenticator )
{
  if ( ! updateUser->text().isEmpty() )
    _authenticator->setUser(updateUser->text());

  if ( ! updatePass->text().isEmpty() )
    _authenticator->setPassword(updatePass->text());
}

void OptionsWidget::httpRequestFinished(QNetworkReply* _qnr)
{
  if (_qnr != downloadRep_)
    return;

  QNetworkReply::NetworkError error = _qnr->error();

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

  progressDialog->hide();
  file->close();

  if (error != QNetworkReply::NoError) {
    file->remove();
  } else {
    QString fileName = QFileInfo(QUrl(updateURL->text()).path()).fileName();
    statusLabel->setText(tr("Downloaded %1").arg(file->fileName() ));
  }

  checkUpdateButton->setEnabled(true);
  delete file;
  file = 0;

  if ( error == QNetworkReply::NoError ) {
    if ( downloadType == VERSIONS_FILE )
      compareVersions();
    if ( downloadType == PLUGIN )
      updateComponent();
  }
}

void OptionsWidget::cancelDownload()
{
  statusLabel->setText(tr("download canceled."));
  httpRequestAborted = true;
  if (downloadRep_)
    downloadRep_->abort();
  checkUpdateButton->setEnabled(true);
}

void OptionsWidget::updateDataReadProgress(qint64 _bytesReceived, qint64 _bytesTotal)
{
  if (httpRequestAborted)
    return;

  progressDialog->setMaximum(_bytesTotal);
  progressDialog->setValue(_bytesReceived);
}

void OptionsWidget::showError(QNetworkReply::NetworkError _error)
{
  if (_error == QNetworkReply::NoError)
    return;
  statusLabel->setText(tr("Download failed: %1.").arg(downloadRep_->errorString()));
  QMessageBox::information(this, tr("HTTP Error"),
      tr("Download failed: %1.")
      .arg(downloadRep_->errorString()) + file->fileName() );
}





