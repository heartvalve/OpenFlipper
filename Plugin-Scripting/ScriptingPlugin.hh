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

  void addMenubarAction(QAction* _action, MenuActionType _type );

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

  QString name() { return (QString("Scripting")); };
  QString description( ) { return (QString("Provides Scripting for OpenFlipper")); };

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

