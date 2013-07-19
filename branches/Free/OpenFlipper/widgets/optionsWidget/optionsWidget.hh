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




#ifndef OPTIONSWIDGET_HH
#define OPTIONSWIDGET_HH

#include "ui_optionsWidget.hh"
#include <QtGui>
#include <QStringList>
#include <QProgressDialog>
#include <QtNetwork>

#include <OpenFlipper/Core/PluginInfo.hh>
#include <ACG/Scenegraph/DrawModes.hh>

//map for keyBindings
typedef std::multimap< std::pair< QObject*, int >, std::pair< int, Qt::KeyboardModifiers > > InverseKeyMap;


class OptionsWidget : public QWidget, public Ui::OptionsWidget
{

Q_OBJECT

public:
   OptionsWidget(std::vector<PluginInfo>& _plugins, std::vector<KeyBinding>& _core, InverseKeyMap& _invKeys, QWidget* parent =0 );

signals:
   void applyOptions();
   void saveOptions();

   void addKeyMapping(int _key, Qt::KeyboardModifiers _modifiers, QObject* _plugin, int _keyBindingID);

private slots:
   /// call slotApply and hide widget
   void slotOk();
   /// Update Options and tell others about changed Options
   void slotApply();

   /// Only hide widget
   void slotCancel();

   /// Checks for updates
   void slotCheckUpdates();

   /// Download updates
   void slotGetUpdates();

   /// open a dialog to determine the color
   void getBackgroundColor();
   void getDefaultColor();

   /// keyBinding TreeWidget-Item changed
   void keyTreeItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous );

   void keyTreeDoubleClicked(QTreeWidgetItem* _item, int col);

   void updateShortcut();

   void slotShowPluginOptions(const QString& _pluginName );

   ///restore keyBinding Presets
   void restoreKeyPresets();

   /// update the dialog if the selected viewer changes
   void updateViewerSettings(int _row);

   /// remember user changes to viewer properties
   void viewerSettingsChanged(QListWidgetItem* _item);
   void viewerSettingsChanged(int _index);

   /// Switch between the stacked widgets of the stereo settings
   void switchStackedWidget();

   /// This function is called to update stereo settings for real-time preview
   /// Note: The parameter only makes sure that the signals can be connected
   /// to this slot.
   void slotPreviewStereoSettings(int _tmpParam = 0);
   
   /// Clear all OpenFlipper settings
   void slotClearSettings();
   
   /// Clear OpenFlipper INI
   void slotClearINI();

protected:
   void showEvent ( QShowEvent * event );

private:

   // plugin Options
   void initPluginOptions();

   QVBoxLayout* pluginOptionsLayout;

   //key-bindings
   std::vector<PluginInfo>& plugins_;
   std::vector<KeyBinding>& coreKeys_;

   InverseKeyMap& keys_;

   int getPluginInfo(QString pluginName);

   void initKeyTree();

   void applyShortcuts();

   // flag indicating if something went wrong and the request has to be aborted
   bool httpRequestAborted;


   // Request variable
   QNetworkAccessManager *networkMan_;
   // current request
   QNetworkReply* downloadRep_;

   // File for http downloads
   QFile *file;

   // ProgressDialog for Downloads
   QProgressDialog *progressDialog;

   // What type of download is currently active
   enum DOWNLOAD {
      NONE,
      VERSIONS_FILE,
      PLUGIN,
      WINDOWS_SETUP
   } downloadType;

  // remember changes to viewer properties
  std::vector< ACG::SceneGraph::DrawModes::DrawMode > defaultDrawModes_;
  std::vector< bool >  defaultProjectionMode_;
  std::vector< int >   defaultViewingDirections_;
  std::vector< bool >  defaultRotationLocks_;

  bool updatingViewerSettings_;
  
  // Store current background color
  QColor backgroundColor_;

private:
   /// Starts the download of the given file
   void startDownload( QString _url );

   /// Compares the versions from the downloaded Versions file with the current versions
   void compareVersions();

   /// Compares two version strings and returns if a newer Version is available
   bool isNewer(QString _current, QString _latest);

   /// Redraws the version table
   void updateVersionsTable();

private slots:

   // This slot is called when a http request has been finished
   void httpRequestFinished(QNetworkReply *_qnr);

   // Updates the progress Dialog while downloading
   void updateDataReadProgress(qint64 _bytesReceived, qint64 _bytesTotal);

   /// error occured while downloading
   void showError(QNetworkReply::NetworkError _error);

   // Progress dialog callback for cancel
   void cancelDownload();

   // Update component of OpenFlipper
   void updateComponent();
   
   // Tracks whether another language has been selected
   void slotTranslationIndexChanged(int);

   /// authentication
   void authentication  ( QNetworkReply* _reply, QAuthenticator* _authenticator );

private:
   /** After checking for updates this variable will contain a list of filenames for which updates are available
    */
   QStringList updatedPlugins_;

   /** Here the architecture specific path for plugins will be created
    */
   QString pluginPath_;

   /** Current filename of the plugin to be updated
    */
   QString currentUpdateName_;
   
   /** Used to track whether another language has been selected
   */
   bool restartRequired_;
   bool exitOnClose_;


};

#endif //OPTIONSWIDGET_HH
