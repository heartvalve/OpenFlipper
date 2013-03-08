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

//=============================================================================
//
//  CLASS VsiPlugin - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include <QAction>
#include <QIcon>
#include <QScriptEngine>
#include <QMessageBox>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <OpenFlipper/common/GlobalOptions.hh>

#include "vsiPlugin.hh"

#include "baseWidget.hh"
#include "parser/context.hh"
#include "parser/element.hh"
#include "parser/input.hh"
#include "config/dynamicDialog.hh"

Q_EXPORT_PLUGIN2( VsiPlugin , VsiPlugin );

//------------------------------------------------------------------------------

/// Constructor
VsiPlugin::VsiPlugin () :
  context_ (0),
  baseWidget_ (0)
{ 
}

//------------------------------------------------------------------------------

/// Destructor
VsiPlugin::~ VsiPlugin()
{
  if (context_)
    delete context_;
  if (baseWidget_)
    delete baseWidget_;
}

//------------------------------------------------------------------------------

/// Register in menubar
void VsiPlugin::pluginsInitialized ()
{
  if (OpenFlipper::Options::nogui ())
    return;

  QMenu *scriptingMenu;

  emit emit getMenubarMenu(tr("&Scripting"), scriptingMenu, true );

  QAction* showEditor = scriptingMenu->addAction ("Visual script editor");
  showEditor->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"vsi_vsiEditor.png") );

  connect (showEditor, SIGNAL(triggered()) ,
           this      , SLOT(showScriptEditor()));
  
  emit addMenubarAction(showEditor, TOOLSMENU );

}

//------------------------------------------------------------------------------

/// Show visual script editor
void VsiPlugin::showScriptEditor()
{
  initContext ();
  if (!baseWidget_)
  {
    baseWidget_ = VSI::BaseWidget::createBaseWidget (context_);
    connect (baseWidget_, SIGNAL (codeToScriptEditor (QString)),
             this, SLOT (showInScriptEditor(QString)));
  }

  baseWidget_->show ();
}

/// initalize context
void VsiPlugin::initContext()
{
  if (context_)
    return;

  QScriptEngine *engine;
  emit getScriptingEngine (engine);

  // empty context
  context_ = new VSI::Context (engine);

  // parse all metadata xml files
  QDir dir = OpenFlipper::Options::dataDir ();

  dir.cd ("VsiMetadata");

  if (!dir.exists ())
    return;

  foreach (QString sub, dir.entryList(QDir::Dirs))
  {
    if (sub == "..")
      continue;
    
    QDir subdir = dir;
    subdir.cd (sub);
    subdir.makeAbsolute();
    
    foreach (QString file, subdir.entryList (QStringList("*.xml"), QDir::Files))
    {
      QFile f (subdir.filePath (file));
      if (!f.open (QIODevice::ReadOnly))
	continue;

      context_->parse (f.readAll ());
    }
  }
  
}

//------------------------------------------------------------------------------

/// Gererates a dialog for the given element inputs
QScriptValue VsiPlugin::askForInputs(QString _element, QString _inputs)
{
  initContext ();

  VSI::Element *e = context_->element (_element);

  if (!e)
    return QScriptValue ();

  QVector<VSI::Input *> inputs;

  foreach (QString s, _inputs.split (",", QString::SkipEmptyParts))
    foreach (VSI::Input *i, e->inputs ())
    {
      if (i->name () == s)
      {
        inputs.append (i);
        break;
      }
    }

  if (inputs.isEmpty ())
    return QScriptValue ();

  VSI::DynamicDialog d (inputs);
  d.setWindowTitle (e->shortDescription () + " Input Configuration");
  d.exec ();

  QMap<QString, QString> results = d.getResults ();

  QString script = "inputs = { ";

  foreach (QString s, _inputs.split (",", QString::SkipEmptyParts))
    script += s + ": " + results[s] + ",";

  script.remove (script.length () - 1, 1);

  script += "};";

  context_->scriptEngine ()->pushContext ();
  QScriptValue rv = context_->scriptEngine ()->evaluate (script);
  context_->scriptEngine ()->globalObject ().setProperty ("inputs", rv);
  //rv = context_->scriptEngine ()->globalObject ().property ("inputs");
  context_->scriptEngine ()->popContext ();

  return rv;
}

//------------------------------------------------------------------------------

/// shows the given script in the textual script editor
void VsiPlugin::showInScriptEditor(QString _script)
{
  bool ok;

  emit functionExists ("scripting", "showScriptInEditor(QString)", ok);

  if (!ok)
    return;

  RPC::callFunction ("scripting", "showScriptInEditor", _script);
}

//------------------------------------------------------------------------------

/// Scripting function, that displays a message box
void VsiPlugin::messageBox (QString _message)
{
  QMessageBox msgBox;
  msgBox.setText(_message);
  msgBox.exec();
}

//------------------------------------------------------------------------------

/// Scripting function, that displays a Yes/No message box
bool VsiPlugin::questionBox (QString _message)
{
  QMessageBox msgBox;
  msgBox.setText(_message);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::Yes);
  int ret = msgBox.exec();
  if (ret == QMessageBox::Yes) 
    return true;
  return false;
}

/// Non blocking box which can be used inside scripting loops to stop on clicks
bool VsiPlugin::continueBox (QString _message)
{
  static QContinueBox* msgBox = NULL;

  if ( !msgBox ) {
    msgBox = new QContinueBox(_message);
    msgBox->show();
  } else {

    if ( msgBox->continueBox() ) {
      return true;
    } else {
      delete msgBox;
      msgBox = NULL;
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
