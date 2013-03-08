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

    /// Shows a non blocking stop box for use inside loops
    bool continueBox(QString _message);

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

class QContinueBox : public QWidget {
  Q_OBJECT

public:
  QContinueBox(QString _message,QWidget* _parent = 0) :
    QWidget(_parent),
    continue_(true)
  {
    QPushButton* stopButton = new QPushButton("Stop",this);
    QHBoxLayout* layout = new QHBoxLayout(this);

    this->setWindowTitle(_message);

    layout->addWidget(stopButton);
    this->setLayout(layout);

    connect(stopButton,SIGNAL(clicked()), this, SLOT(clicked()));
  }

public slots:
  void clicked( ) {
    continue_ = false;
  }

public:
  bool continueBox() { return continue_; };

private:
  bool continue_;
};


//=============================================================================
//=============================================================================

#endif
