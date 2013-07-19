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

#ifndef SCRIPTINGPLUGIN_HH
#define SCRIPTINGPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

// Include the scripttools for qt script debugging if available
#ifdef QT_SCRIPTTOOLS_LIB
  #include <QtScriptTools/QScriptEngineDebugger>
#endif

#include "highLighter.hh"
#include "scriptingWidget.hh"



class ScriptingPlugin : public QObject, BaseInterface, MenuInterface, ScriptInterface, RPCInterface, LoggingInterface
{
Q_OBJECT
Q_INTERFACES(BaseInterface)
Q_INTERFACES(MenuInterface)
Q_INTERFACES(ScriptInterface)
Q_INTERFACES(RPCInterface)
Q_INTERFACES(LoggingInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Scripting")
#endif

signals:
  void updateView();

  void getScriptingEngine( QScriptEngine*& _engine  );
  void getAvailableFunctions( QStringList& _functions  );

  void log(Logtype _type, QString _message);
  void log(QString _message);

  void getMenubarMenu (QString _name, QMenu *& _menu, bool _create);

  void getDescription(QString      _function,   QString&     _description,
                      QStringList& _parameters, QStringList& _descriptions);

private slots:

  void slotScriptInfo( QString _pluginName , QString _functionName  );
  void slotExecuteScript( QString _script );
  void slotExecuteFileScript( QString _filename );

  void slotLoadScript();
  void slotSaveScript();
  void slotSaveScriptAs();
  void pluginsInitialized();

  void noguiSupported( ) {} ;

public :

  ScriptingPlugin();

  QString name() { return (QString(tr("Scripting"))); };
  QString description( ) { return (QString(tr("Provides Scripting for OpenFlipper"))); };

private slots :

  void slotExecuteScriptButton();

  /** \brief Called everytime the text in the scriptingwidget is changed by the user
   *
   * This slot also handles syntax checking!
   */
  void slotScriptChanged();

  void slotFunctionClicked(QListWidgetItem* _item);
  void slotFunctionDoubleClicked(QListWidgetItem* _item);

  void slotApplyFilter();

  /// Called when an error is detected when checking the syntax
  void slotHighlightError();

  /// Triggered by the debugger button
  void slotDebuggerButton();

private:

  /// Store the last line that contained an error
  int lastProblemLine_;

  /// Store the last error message
  QString lastError_;

  /// Timer to wait until the user stopped typing before showing an error
  QTimer* errorTimer_;

  QList< QLabel* > descriptionLabels_;
  QVBoxLayout* descrLayout_;

//===========================================================================
/** @name Gui control from script
* @{ */
//===========================================================================

public slots:

  /** \brief Show the script editor widget
   *
   */
  void showScriptWidget();

  /** \brief Hide the script editor widget
   *
   */
  void hideScriptWidget();

  /** \brief Show the given Code in the script editor
   *
   * @param _code Code to show
   */
  void showScriptInEditor (QString _code);

  /** \brief Load Script into gui from file
  *
  * Loads a script from the given file and loads it into the widget.
  *
  * @param _filename Filename of script to load, including full path.
  */
  void slotLoadScript( QString _filename );
  
  /** \brief Clear the editor window
   * Clears the script editor window
   */
  void clearEditor();

/** @} */

//===========================================================================
/** @name Interrupt script execution
* @{ */
//===========================================================================

public slots:

  /** Stops script execution and opens a message windows.
  *  Execution continues, when the user clicks on a button
  */
  void waitContinue( );

  /** Stops script execution and opens a message windows.
  *  Execution continues, when the user clicks on a button
  *  Also allowing the user to set a message
  */
  void waitContinue( QString _msg, int _x=-1, int _y=-1);


  /// Sleeps for some seconds in script execution ( Gui will remain functional)
  void sleep( int _seconds );

  /// Sleeps for some mseconds in script execution ( Gui will remain functional)
  void sleepmsecs( int _mseconds );

  /// Marks the current time as the frame start ( Use wait sleepFrameLength to wait until _mseconds have passed since frameStart )
  void frameStart( );

  /// wait until _mseconds have passed since frameStart (if more time has passed, it will return immediately)
  void waitFrameEnd( int _mseconds );

/** @} */


private :
  QString mangleScript( QString _input );

  /// Counts mseconds since a frame start has occured
  QTime frameTime_;

private :
  ScriptWidget* scriptWidget_;

  QStatusBar*   statusBar_;

  Highlighter*  highlighterCurrent_;
  Highlighter*  highlighterLive_;
  Highlighter*  highlighterList_;

  QString       lastFile_;

  /** Path to the directory where the current script is executed.
   *  This might be empty, if a script is not saved!
   */
  QString       scriptPath_;

  QAction*      debuggerButton_;

#ifdef ENABLE_SCRIPT_DEBUGGER
 #ifdef QT_SCRIPTTOOLS_LIB
  QScriptEngineDebugger* debugger_;
 #endif
#endif



public slots:
  QString version() { return QString("1.0"); };
};

#endif //SCRIPTINGPLUGIN_HH

