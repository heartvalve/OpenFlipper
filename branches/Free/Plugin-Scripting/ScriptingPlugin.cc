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


#include <QtGui>

#include "ScriptingPlugin.hh"

#include <ACG/GL/GLState.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include "OpenFlipper/common/GlobalOptions.hh"

#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QSyntaxHighlighter>




ScriptingPlugin::ScriptingPlugin() :
   lastProblemLine_(0),
   lastError_(""),
   errorTimer_(0),
   descrLayout_(0),
   scriptWidget_(0),
   statusBar_(0),
   highlighterCurrent_(0),
   highlighterLive_(0),
   highlighterList_(0),
   lastFile_(""),
   debuggerButton_(0)
#ifdef ENABLE_SCRIPT_DEBUGGER
   ,debugger_(0)
#endif
{

}

void ScriptingPlugin::pluginsInitialized() {

  if ( OpenFlipper::Options::nogui() )
    return;

  ///@todo register objectid variable

  // Scriping Menu
  QMenu *scriptingMenu;

  emit getMenubarMenu(tr("&Scripting"), scriptingMenu, true );

  QIcon icon;
  QAction* showWidget = scriptingMenu->addAction( tr("Show script editor") );
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scriptEditor.png");
  showWidget->setIcon(icon);
  connect( showWidget, SIGNAL( triggered() ) ,
           this      , SLOT( showScriptWidget() ));

  scriptWidget_ = new ScriptWidget();


  QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();

  scriptWidget_->setWindowIcon( OpenFlipper::Options::OpenFlipperIcon() );

  icon.addFile(iconPath+"document-open.png");
  scriptWidget_->actionLoad_Script->setIcon(icon);

  icon.addFile(iconPath+"document-save.png");
  scriptWidget_->actionSave_Script->setIcon(icon);

  icon.addFile(iconPath+"document-save-as.png");
  scriptWidget_->actionSave_Script_As->setIcon(icon);

  icon.addFile(iconPath+"window-close.png");
  scriptWidget_->actionClose->setIcon(icon);

  // ==================================================================
  // Add a toolbar
  // ==================================================================

  QToolBar* toolBar = new QToolBar(tr("Scripting Toolbar"));

  QAction* openButton = new QAction(QIcon(iconPath + "document-open.png"), "Open", toolBar);
  toolBar->addAction(openButton);
  connect (openButton, SIGNAL( triggered() ), this, SLOT( slotLoadScript() ) );

  QAction* saveButton = new QAction(QIcon(iconPath + "document-save.png"), "Save", toolBar);
  toolBar->addAction(saveButton);
  connect (saveButton, SIGNAL( triggered() ), this, SLOT( slotSaveScript() ) );

  QAction* saveAsButton = new QAction(QIcon(iconPath + "document-save-as.png"), "Save as", toolBar);
  toolBar->addAction(saveAsButton);
  connect (saveAsButton, SIGNAL( triggered() ), this, SLOT( slotSaveScriptAs() ) );

  toolBar->addSeparator();

  debuggerButton_ = new QAction(QIcon(iconPath + "script-debugger.png"), "Enable Debugger", toolBar);
  debuggerButton_->setCheckable(true);
  toolBar->addAction(debuggerButton_);

#ifdef ENABLE_SCRIPT_DEBUGGER
  if ( OpenFlipperSettings().value("Scripting/QtScriptDebugger",true).toBool() )
    debuggerButton_->setChecked(true);
  else
    debuggerButton_->setChecked(false);

  connect (debuggerButton_, SIGNAL( triggered() ), this, SLOT( slotDebuggerButton() ) );
#else
  debuggerButton_->setEnabled(false);
  debuggerButton_->setToolTip(tr("QtScriptTools library not available. Debugger is not available!"));
#endif

  toolBar->addSeparator();

  QAction* executeButton = new QAction(QIcon(iconPath + "arrow-right.png"), "Execute", toolBar);
  toolBar->addAction(executeButton);
  connect (executeButton, SIGNAL( triggered() ), this, SLOT( slotExecuteScriptButton() ) );

  scriptWidget_->addToolBar(toolBar);

  // ==================================================================
  // Create a status bar
  // ==================================================================

  statusBar_ = new QStatusBar();

  scriptWidget_->setStatusBar( statusBar_ );

  // ==================================================================

  scriptWidget_->hide();

  scriptWidget_->resize(scriptWidget_->width() , std::min(QApplication::desktop()->screenGeometry().height() - 150 , 800) );

  connect (scriptWidget_->actionLoad_Script, SIGNAL( triggered() ), this, SLOT( slotLoadScript() ) );
  scriptWidget_->actionLoad_Script->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_O) );
  connect (scriptWidget_->actionSave_Script, SIGNAL( triggered() ), this, SLOT( slotSaveScript() ) );
  scriptWidget_->actionSave_Script->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_S) );
  connect (scriptWidget_->actionSave_Script_As, SIGNAL( triggered() ), this, SLOT( slotSaveScriptAs() ) );
  connect (scriptWidget_->actionClose, SIGNAL( triggered() ), scriptWidget_, SLOT( close() ) );

  connect (scriptWidget_->currentScript, SIGNAL( textChanged() ), this, SLOT( slotScriptChanged() ) );

  connect (scriptWidget_->functionList, SIGNAL( itemClicked(QListWidgetItem*) ),
           this,                          SLOT( slotFunctionClicked(QListWidgetItem*) ));
  connect (scriptWidget_->functionList, SIGNAL( itemDoubleClicked(QListWidgetItem*) ),
           this,                          SLOT( slotFunctionDoubleClicked(QListWidgetItem*) ));

  //filter
  connect (scriptWidget_->filterButton, SIGNAL( clicked() ),
           this,                          SLOT( slotApplyFilter() ));
  connect (scriptWidget_->resetButton, SIGNAL( clicked() ),
           scriptWidget_->functionList,  SLOT( reset() ));
  connect (scriptWidget_->resetButton, SIGNAL( clicked() ),
           scriptWidget_->filterEdit,  SLOT( clear() ));
  connect (scriptWidget_->functionList, SIGNAL(getDescription(QString,QString&,QStringList&,QStringList&)),
           this                       , SIGNAL(getDescription(QString,QString&,QStringList&,QStringList&)));

  scriptWidget_->description->setVisible( false );

  highlighterCurrent_ = new Highlighter( scriptWidget_->currentScript->document() );
  highlighterLive_    = new Highlighter( scriptWidget_->liveEdit );
//   highlighterList_    = new Highlighter( scriptWidget_->functionList  );
  frameTime_.start();


  // Timer for syntax error while editing. If the Syntax is not correct
  // And the text does not change for a specified time, the line will be highlighted
  // And a message printed to the status bar
  errorTimer_ = new QTimer();
  errorTimer_->setSingleShot(true);
  connect(errorTimer_,SIGNAL(timeout()),this,SLOT(slotHighlightError()));

  // ==================================================================
  // Setup scripting debugger if available
  // ==================================================================

#ifdef ENABLE_SCRIPT_DEBUGGER
  QScriptEngine* engine;
  emit getScriptingEngine( engine  );
  debugger_ = new QScriptEngineDebugger;

  if ( OpenFlipperSettings().value("Scripting/QtScriptDebugger",false).toBool() )
    debugger_->attachTo(engine);
#endif
}

void ScriptingPlugin::slotApplyFilter(){
  scriptWidget_->functionList->filter( scriptWidget_->filterEdit->text() );
}

void ScriptingPlugin::slotScriptChanged(){
  scriptWidget_->actionSave_Script->setEnabled( true );

  // Stop timers, as the text changed!
  errorTimer_->stop();

  // Check the current script for syntax
  const QString script = scriptWidget_->currentScript->toPlainText();
  QScriptSyntaxCheckResult syntaxCheck = QScriptEngine::checkSyntax ( script );

  switch (syntaxCheck.state() ) {
    case QScriptSyntaxCheckResult::Error :
      lastProblemLine_ = syntaxCheck.errorLineNumber();
      lastError_       = syntaxCheck.errorMessage();
      errorTimer_->start(500);
      break;
    case QScriptSyntaxCheckResult::Valid :
      break;
    default :
      break;
  }
}

void ScriptingPlugin::slotHighlightError() {
  scriptWidget_->currentScript->highLightErrorLine(lastProblemLine_);
  statusBar_->showMessage(lastError_,5000);
}

void ScriptingPlugin::showScriptWidget( ) {
  if ( OpenFlipper::Options::nogui() )
    return;

  scriptWidget_->show();

  // Update list of available functions
  QStringList completeList;
  emit getAvailableFunctions( completeList  );

  QStringList plugins;
  QStringList functions;

  scriptWidget_->functionList->clear( );

  //Update Highlighters
  for ( int i = 0  ; i <  completeList.size() ; ++i) {

    QString plugin   = completeList[i].section('.',0,0);

    // Global functions start with - and are not added as plugins!
    if (plugin != "-") {
      if ( ! plugins.contains( plugin ) )
        plugins.push_back( plugin );
    }


    QString function = completeList[i].section('.',1,1);
    function         = function.section('(',0,0);
    if ( ! functions.contains( function ) )
      functions.push_back( function );

    // Either write the whole string or cut the "-." for global functions
    if ( plugin != "-")
      scriptWidget_->functionList->addItem( completeList[i] );
    else
      scriptWidget_->functionList->addItem( completeList[i].right(completeList[i].size() - 2) );

  }

  // Sort the available functions
  scriptWidget_->functionList->sortItems ( );

  highlighterCurrent_->pluginPatterns_   = plugins;
  highlighterCurrent_->functionPatterns_ = functions;
  highlighterCurrent_->update();
  highlighterCurrent_->rehighlight();

  highlighterLive_->pluginPatterns_      = plugins;
  highlighterLive_->functionPatterns_    = functions;
  highlighterLive_->update();
  highlighterLive_->rehighlight();

  // Bring it to foreground
  scriptWidget_->raise();

}

void ScriptingPlugin::hideScriptWidget( ) {
  if ( OpenFlipper::Options::nogui() )
    return;

  scriptWidget_->hide();
}

void ScriptingPlugin::slotScriptInfo( QString _pluginName , QString _functionName  ) {

  if ( OpenFlipper::Options::scripting() || OpenFlipper::Options::nogui()  )
    return;

  scriptWidget_->liveEdit->append( _pluginName + "." + _functionName );

  QScrollBar* bar = scriptWidget_->liveEdit->verticalScrollBar();
  bar->setValue(bar->maximum());
}

void ScriptingPlugin::slotExecuteScript( QString _script ) {

  if ( OpenFlipper::Options::gui())
    statusBar_->showMessage(tr("Executing Script"));

  QScriptEngine* engine;
  emit getScriptingEngine( engine  );

  /// Switch scripting mode on
  OpenFlipper::Options::scripting(true);


  // Get the filename of the script and set it in the scripting environment
  engine->globalObject().setProperty("ScriptPath",QScriptValue(engine,lastFile_.section(OpenFlipper::Options::dirSeparator(), 0, -2)));

  // Execute the script
  engine->evaluate( _script );

  // Catch errors and print some reasonable error message to log and statusbar
  bool error = false;
  if ( engine->hasUncaughtException() ) {
    error = true;
    QScriptValue result = engine->uncaughtException();
    QString exception = result.toString();
    int lineNumber = engine->uncaughtExceptionLineNumber();
    emit log( LOGERR , tr("Script execution failed at line %1, with : %2 ").arg(lineNumber).arg(exception) );

    if ( OpenFlipper::Options::gui()) {
        statusBar_->showMessage(tr("Script execution failed at line %1, with : %2 ").arg(lineNumber).arg(exception));

        // Get cursor and move it to the line containing the error
        QTextCursor cursor = scriptWidget_->currentScript->textCursor();
        cursor.setPosition(0);
        cursor.movePosition ( QTextCursor::Down, QTextCursor::MoveAnchor, lineNumber - 1 );
        scriptWidget_->currentScript->setTextCursor(cursor);

        scriptWidget_->currentScript->highLightErrorLine(lineNumber);

        lastProblemLine_ = lineNumber;
        lastError_       = exception;

    }
  }

  if ( OpenFlipper::Options::gui() && !error)
    statusBar_->clearMessage();

  /// Switch scripting mode off
  OpenFlipper::Options::scripting(false);
}

void ScriptingPlugin::slotExecuteFileScript( QString _filename ) {
  QString script;

  QFile data(_filename);
  if (data.open(QFile::ReadOnly)) {
    QTextStream input(&data);
    do {
      script.append(input.readLine() + "\n");
    } while (!input.atEnd());

    if ( OpenFlipper::Options::gui() )
      scriptWidget_->currentScript->setPlainText(script);

    slotExecuteScript(script);

  } else
    emit log(LOGERR,tr("Unable to open script file!"));
}

void ScriptingPlugin::slotExecuteScriptButton() {
  slotExecuteScript( scriptWidget_->currentScript->toPlainText() );
}

void ScriptingPlugin::slotDebuggerButton() {

#ifdef ENABLE_SCRIPT_DEBUGGER
  QScriptEngine* engine;
  emit getScriptingEngine( engine  );

  if ( debuggerButton_->isChecked() ) {
    debugger_->attachTo(engine);
  } else {
    debugger_->detach();
  }

  OpenFlipperSettings().setValue("Scripting/QtScriptDebugger",debuggerButton_->isChecked());

#endif

}

QString ScriptingPlugin::mangleScript(QString _input ) {

  // Update list of available functions
  QStringList functions;
  emit getAvailableFunctions( functions  );

  std::cerr << "Todo : mangle script " << std::endl;
  return _input;

}

void ScriptingPlugin::sleep( int _seconds ) {

  if ( OpenFlipper::Options::nogui() )
    return;

  QTimer timer;

  timer.setSingleShot(true);
  timer.start( _seconds * 1000 );

  while (timer.isActive() )
    QApplication::processEvents();

}

void ScriptingPlugin::sleepmsecs( int _mseconds ) {

  if ( OpenFlipper::Options::nogui() )
    return;

  QTimer timer;

  timer.setSingleShot(true);
  timer.start( _mseconds );

  while (timer.isActive() )
    QApplication::processEvents();

}

void ScriptingPlugin::frameStart( ) {
  frameTime_.restart();
}

void ScriptingPlugin::waitFrameEnd( int _mseconds ) {
  int elapsed = frameTime_.elapsed();

  // Wait remaining time
  if ( elapsed < _mseconds ) {
    sleepmsecs( _mseconds - elapsed );
  }

  // restart timer
  frameTime_.restart();

}


void ScriptingPlugin::waitContinue( ) {
  if ( OpenFlipper::Options::nogui() )
    return;

  QMessageBox box;

  box.addButton(tr("Continue"),QMessageBox::AcceptRole);
  box.setText(tr("Script execution has been interrupted"));
  box.setIcon(QMessageBox::Information);
  box.setWindowModality(Qt::NonModal);
  box.setWindowTitle(tr("Continue?"));
  box.setWindowFlags( box.windowFlags() | Qt::WindowStaysOnTopHint);
  box.show();

  while ( box.isVisible() )
    QApplication::processEvents();

}

void ScriptingPlugin::waitContinue( QString _msg, int _x, int _y ) {
  if ( OpenFlipper::Options::nogui() )
    return;

  QMessageBox box;


  box.addButton(tr("Continue"),QMessageBox::AcceptRole);
  box.setText(_msg);
  box.setIcon(QMessageBox::Information);
  box.setWindowModality(Qt::NonModal);
  box.setWindowTitle(tr("Continue?"));
  box.setWindowFlags( box.windowFlags() | Qt::WindowStaysOnTopHint);
  if(_x!=-1 && _y!=-1)
    box.move(_x,_y);
  box.show();

  while ( box.isVisible() )
    QApplication::processEvents();

}


void ScriptingPlugin::slotLoadScript(){
  QString filename = QFileDialog::getOpenFileName(0,
     tr("Load Script"),OpenFlipper::Options::currentScriptDirStr(), tr("Script Files (*.ofs)"));

  if (filename == "")
    return;
  
  slotLoadScript(filename);
}

void ScriptingPlugin::slotLoadScript( QString _filename ) {
  
  if (_filename == "")
      return;
    
  // Check if we are in gui mode. Otherwise just ignore this call
  if ( OpenFlipper::Options::gui() ) {
    scriptWidget_->currentScript->clear();
  
    QFile data(_filename);
    
    if (data.open(QFile::ReadOnly)) {
      QTextStream input(&data);
      do {
        scriptWidget_->currentScript->appendPlainText(input.readLine());
      } while (!input.atEnd());
      
      lastFile_ = _filename;
      OpenFlipper::Options::currentScriptDir( QFileInfo(_filename).absolutePath() );
      
      scriptWidget_->actionSave_Script->setEnabled( false );
      
      scriptWidget_->show();
    }
  }
  
}

void ScriptingPlugin::slotSaveScript(){

  QFile file(lastFile_);

  if ( !file.exists())
    slotSaveScriptAs();
  else{
    //write script to file
    if (file.open(QFile::WriteOnly)) {
      QTextStream output(&file);
      output << scriptWidget_->currentScript->toPlainText();
    }
    scriptWidget_->actionSave_Script->setEnabled( false );
  }
}

void ScriptingPlugin::slotSaveScriptAs(){
  QString filename = QFileDialog::getSaveFileName(scriptWidget_,
     tr("Save Script"),OpenFlipper::Options::currentScriptDirStr(), tr("Script Files (*.ofs)"));

  if (filename == "") return;

  QFile data(filename);

  //perhaps add an extension
  if (!data.exists()){
    QFileInfo fi(filename);
    if (fi.completeSuffix() == ""){
      filename = filename + ".ofs";
      data.setFileName(filename);
    }
  }

  //write script to file
  if (data.open(QFile::WriteOnly)) {
    QTextStream output(&data);
    output << scriptWidget_->currentScript->toPlainText();
 }

 lastFile_ = filename;

  OpenFlipper::Options::currentScriptDir( QFileInfo(filename).absolutePath() );
  scriptWidget_->actionSave_Script->setEnabled( false );
}

void ScriptingPlugin::slotFunctionClicked(QListWidgetItem * _item)
{

  QString slotDescription;
  QStringList params;
  QStringList descriptions;

  emit getDescription(_item->text(), slotDescription, params, descriptions);

  if ( !slotDescription.isEmpty() ){

    if (descriptionLabels_.count() > 0){
      //first remove old stuff
      for (int i = 0; i < descriptionLabels_.count(); i++){
        descrLayout_->removeWidget( descriptionLabels_[i] );
        delete descriptionLabels_[i];
      }
      descriptionLabels_.clear();
    }else
      descrLayout_ = new QVBoxLayout();

    QLabel* lSlotName = new QLabel("<B>" + _item->text() + "</B>");
    QLabel* lDescription = new QLabel(slotDescription);
    lDescription->setWordWrap(true);

    descrLayout_->addWidget(lSlotName);
    descrLayout_->addWidget(lDescription);

    descriptionLabels_.append(lSlotName);
    descriptionLabels_.append(lDescription);

    if ( params.count() == descriptions.count() ){

      //get parameter-types from function-name
      QString typeStr = _item->text().section("(",1,1).section(")",0,0);
      QStringList types = typeStr.split(",");

      if (types.count() == params.count()){

        for(int p=0; p < params.count(); p++ ){
          QLabel* param = new QLabel("<B>" + types[p] + " " + params[p] + ":</B>" );
          QLabel* descr = new QLabel(descriptions[p]);
          descr->setWordWrap(true);
          descrLayout_->addWidget(param);
          descrLayout_->addWidget(descr);

          descriptionLabels_.append(param);
          descriptionLabels_.append(descr);
        }

      }

    }


    scriptWidget_->description->setLayout( descrLayout_ );
  }

  scriptWidget_->description->setVisible( !slotDescription.isEmpty() );
}

void ScriptingPlugin::slotFunctionDoubleClicked(QListWidgetItem * _item)
{
  scriptWidget_->currentScript->insertPlainText( _item->text() );
}

void ScriptingPlugin::showScriptInEditor(QString _code)
{
  if ( OpenFlipper::Options::nogui() )
    return;

  /*
   * this is called from the VSI plugin
   * we do not want to overwrite any previously opened scripts
   */
  lastFile_ = "";

  showScriptWidget ();

  scriptWidget_->currentScript->setPlainText(_code);
}

void ScriptingPlugin::clearEditor() {
  if ( OpenFlipper::Options::nogui() )
     return;

  scriptWidget_->currentScript->clear();
}

Q_EXPORT_PLUGIN2( skriptingplugin , ScriptingPlugin );

