//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2009 by Computer Graphics Group, RWTH Aachen
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
//   $Revision: $
//   $Author: $
//   $Date: $
//
//=============================================================================

//== INCLUDES =================================================================

#ifndef VSIPLUGIN_HH
#define VSIPLUGIN_HH

#include <QObject>
#include <QScriptValue>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MenuInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>

//== FORWARDDECLARATIONS ======================================================

namespace VSI {
  class Context;
  class BaseWidget;
}

//== CLASS DEFINITION =========================================================

/** Plugin that provides a visual scripting interface
*/
class VsiPlugin : public QObject, BaseInterface, MenuInterface, ScriptInterface, RPCInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MenuInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(RPCInterface)

  signals:

    // MenuInterface
    void getMenubarMenu (QString _name, QMenu *& _menu, bool _create);

    // ScriptInterface
    void getScriptingEngine (QScriptEngine*& _engine);

    // RPC Interface
    void pluginExists (QString _pluginName, bool& _exists) ;
    void functionExists (QString _pluginName, QString _functionName, bool& _exists);

  public:

    /// Constructor
    VsiPlugin ();

    /// Destructor
    ~VsiPlugin ();

    /// Name of the Plugin
    QString name () { return QString ("Visual Scripting"); };

    /// Description of the Plugin
    QString description () { return QString ("Visual Scripting interface for OpenFlipper"); };

  public slots:

    /// Scripting function, that allows to ask the user for inputs during script execution
    QScriptValue askForInputs (QString _element, QString _inputs);

    /// Scripting function, that displays a message box
    void messageBox (QString _message);

    /// Scripting function, that displays a Yes/No message box
    bool questionBox (QString _message);

    QString version () { return QString("1.0"); };

  private slots:
    void pluginsInitialized ();

    void noguiSupported () {};

    /// Shows visual script editor
    void showScriptEditor ();

    /// Opens the text based script editor with the given script
    void showInScriptEditor (QString _script);

  private:

    /// initalisation
    void initContext ();

  private:

    VSI::Context *context_;
    VSI::BaseWidget *baseWidget_;
};


//=============================================================================
//=============================================================================

#endif