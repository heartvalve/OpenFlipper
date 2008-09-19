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




#ifndef OPTIONSWIDGET_HH
#define OPTIONSWIDGET_HH

#include "ui_optionsWidget.hh"
#include <QtGui>
#include <QStringList>
#include <QtNetwork>

#include <OpenFlipper/Core/PluginInfo.hh>

class OptionsWidget : public QWidget, public Ui::OptionsWidget
{

  Q_OBJECT
  public:
    OptionsWidget(std::vector<PluginInfo>& _plugins, std::vector<KeyBinding>& _core, QWidget *parent = 0 );

  signals:
    void applyOptions();
    void saveOptions();

  private slots:
    /// Hide widget, Update Options and tell others about changed Options
    void slotApply();

    /// Only hide widget
    void slotCancel();

    /// Checks for updates
    void slotCheckUpdates();

    /// Download updates
    void slotGetUpdates();

  protected:
    void showEvent ( QShowEvent * event );

  private:
    //key-bindings
    std::vector<PluginInfo>& plugins_;
    std::vector<KeyBinding>& coreKeys_;

    // flag indicating if something went wrong and the request has to be aborted
    bool httpRequestAborted;

    // Id of the current request
    int httpGetId;

    // Request variable
    QHttp *http;

    // File for http downloads
    QFile *file;

    // ProgressDialog for Downloads
    QProgressDialog *progressDialog;

    // What type of download is currently active
    enum DOWNLOAD {
      NONE,
      VERSIONS_FILE,
      WINDOWS_SETUP
    } downloadType;

  private:
    /// Starts the download of the given file
    void startDownload( QString _url );

    /// Compares the versions from the downloaded Versions file with the current versions
    void compareVersions();

    /// Compares two version strings and returns if a newer Version is available
    bool isNewer(QString _current, QString _latest);

  private slots:

    // This slot is called when a http request has been finished
    void httpRequestFinished(int requestId, bool error);

    // Parses the response and gives feedback
    void readResponseHeader(const QHttpResponseHeader &responseHeader);

    // Updates the progress Dialog while downloading
    void updateDataReadProgress(int bytesRead, int totalBytes);

    // Progress dialog callback for cancel
    void cancelDownload();

};

#endif //OPTIONSWIDGET_HH
