
#include <QtGui>

#include "ScriptingPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include "OpenFlipper/common/GlobalOptions.hh"

#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QSyntaxHighlighter>

ScriptingPlugin::ScriptingPlugin() {

}

void ScriptingPlugin::pluginsInitialized() {

  if ( OpenFlipper::Options::nogui() )
    return;

  ///@todo register objectid variable

  // Scriping Menu
  QMenu *scriptingMenu;

  emit getMenubarMenu(tr("&Scripting"), scriptingMenu, true );

  QIcon icon;
  QAction* showWidget = scriptingMenu->addAction( "Show script editor" );
  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"scriptEditor.png");
  showWidget->setIcon(icon);
  connect( showWidget, SIGNAL( triggered() ) ,
           this      , SLOT( showScriptWidget() ));

  scriptWidget_ = new ScriptWidget();

  scriptWidget_->setWindowIcon( OpenFlipper::Options::OpenFlipperIcon() );

  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-open.png");
  scriptWidget_->actionLoad_Script->setIcon(icon);

  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save.png");
  scriptWidget_->actionSave_Script->setIcon(icon);

  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"document-save-as.png");
  scriptWidget_->actionSave_Script_As->setIcon(icon);

  icon.addFile(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"window-close.png");
  scriptWidget_->actionClose->setIcon(icon);

  scriptWidget_->hide();

  scriptWidget_->resize(scriptWidget_->width() , std::min(QApplication::desktop()->screenGeometry().height() - 150 , 800) );

  connect( scriptWidget_->executeButton, SIGNAL(clicked()),
           this                        , SLOT( slotExecuteScriptButton() ));

  connect (scriptWidget_->actionLoad_Script, SIGNAL( triggered() ), this, SLOT( slotLoadScript() ) );
  scriptWidget_->actionLoad_Script->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_O) );
  connect (scriptWidget_->actionSave_Script, SIGNAL( triggered() ), this, SLOT( slotSaveScript() ) );
  scriptWidget_->actionSave_Script->setShortcut( QKeySequence(Qt::CTRL + Qt::Key_S) );
  connect (scriptWidget_->actionSave_Script_As, SIGNAL( triggered() ), this, SLOT( slotSaveScriptAs() ) );
  connect (scriptWidget_->actionClose, SIGNAL( triggered() ), scriptWidget_, SLOT( close() ) );

  connect (scriptWidget_->currentScript, SIGNAL( textChanged() ), this, SLOT( slotEnableSave() ) );

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

  highlighterCurrent_ = new Highlighter( scriptWidget_->currentScript );
  highlighterLive_    = new Highlighter( scriptWidget_->liveEdit );
//   highlighterList_    = new Highlighter( scriptWidget_->functionList  );
  frameTime_.start();
}

void ScriptingPlugin::slotApplyFilter(){
  scriptWidget_->functionList->filter( scriptWidget_->filterEdit->text() );
}

void ScriptingPlugin::slotEnableSave(){
  scriptWidget_->actionSave_Script->setEnabled( true );
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
    if ( ! plugins.contains( plugin ) )
      plugins.push_back( plugin );


    QString function = completeList[i].section('.',1,1);
    function         = function.section('(',0,0);
    if ( ! functions.contains( function ) )
      functions.push_back( function );

    scriptWidget_->functionList->addItem( completeList[i] );

  }

  highlighterCurrent_->pluginPatterns_   = plugins;
  highlighterCurrent_->functionPatterns_ = functions;
  highlighterCurrent_->update();
  highlighterCurrent_->rehighlight();

  highlighterLive_->pluginPatterns_      = plugins;
  highlighterLive_->functionPatterns_    = functions;
  highlighterLive_->update();
  highlighterLive_->rehighlight();

//   highlighterList_->pluginPatterns_      = plugins;
//   highlighterList_->functionPatterns_    = functions;
//   highlighterList_->update();
//   highlighterList_->rehighlight();

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
  QScriptEngine* engine;
  emit getScriptingEngine( engine  );

  /// Switch scripting mode on
  OpenFlipper::Options::scripting(true);


  // Get the filename of the script and set it in the scripting environment
  engine->globalObject().setProperty("ScriptPath",QScriptValue(engine,lastFile_.section(OpenFlipper::Options::dirSeparator(), 0, -2)));

  engine->evaluate( _script );
  if ( engine->hasUncaughtException() ) {
    QScriptValue result = engine->uncaughtException();
    QString exception = result.toString();
    emit log( LOGERR , "Script execution failed with : " + exception );
  }

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
      scriptWidget_->currentScript->setText(script);

    // Set lastfilename to the opened file
    lastFile_ = _filename;

    slotExecuteScript(script);

  } else
    emit log(LOGERR,"Unable to open script file!");
}

void ScriptingPlugin::slotExecuteScriptButton() {
  slotExecuteScript( scriptWidget_->currentScript->toPlainText() );
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

  box.addButton("Continue",QMessageBox::AcceptRole);
  box.setText("Script execution has been interrupted");
  box.setIcon(QMessageBox::Information);
  box.setWindowModality(Qt::NonModal);
  box.setWindowTitle("Continue?");
  box.setWindowFlags( box.windowFlags() | Qt::WindowStaysOnTopHint);
  box.show();

  while ( box.isVisible() )
    QApplication::processEvents();

}

void ScriptingPlugin::waitContinue( QString _msg, int _x, int _y ) {
  if ( OpenFlipper::Options::nogui() )
    return;

  QMessageBox box;


  box.addButton("Continue",QMessageBox::AcceptRole);
  box.setText(_msg);
  box.setIcon(QMessageBox::Information);
  box.setWindowModality(Qt::NonModal);
  box.setWindowTitle("Continue?");
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

  scriptWidget_->currentScript->clear();

  QFile data(filename);
  if (data.open(QFile::ReadOnly)) {
    QTextStream input(&data);
    do {
      scriptWidget_->currentScript->append(input.readLine());
    } while (!input.atEnd());

 }
  lastFile_ = filename;
  OpenFlipper::Options::currentScriptDir( QFileInfo(filename).absolutePath() );

  scriptWidget_->actionSave_Script->setEnabled( false );
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

  showScriptWidget ();

  scriptWidget_->currentScript->setText (_code);
}

Q_EXPORT_PLUGIN2( skriptingplugin , ScriptingPlugin );

