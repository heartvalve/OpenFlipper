/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
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
  void slotEnableSave();

  void slotFunctionClicked(QListWidgetItem* _item);
  void slotFunctionDoubleClicked(QListWidgetItem* _item);

  void slotApplyFilter();

private:
  QList< QLabel* > descriptionLabels_;
  QVBoxLayout* descrLayout_;

//===========================================================================
/** @name Gui control from script
* @{ */
//===========================================================================

public slots:
  void showScriptWidget();
  void hideScriptWidget();

  void showScriptInEditor (QString _code);

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

  /// wait until _mseconds have passed since frameStart (if more time has passed, it will return immediatly)
  void waitFrameEnd( int _mseconds );

/** @} */


private :
  QString mangleScript( QString _input );

  /// Counts mseconds since a frame start has occured
  QTime frameTime_;

private :
  ScriptWidget* scriptWidget_;

  Highlighter* highlighterCurrent_;
  Highlighter* highlighterLive_;
  Highlighter* highlighterList_;

  QString lastFile_;

public slots:
  QString version() { return QString("1.0"); };
};

#endif //SCRIPTINGPLUGIN_HH

