
#include <QtGui>
#include <QFileInfo>
#include <QSettings>

#include "FileScript.hh"

#include <iostream>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

QString FileScriptPlugin::getLoadFilters() {
  return QString( tr("OpenFlipper Scripting Files ( *.ofs )") );
};

QString FileScriptPlugin::getSaveFilters() {
  return QString( "" );
};

DataType  FileScriptPlugin::supportedType() {
  DataType type = DATA_UNKNOWN;
  return type;
}

int FileScriptPlugin::loadObject(QString _filename)
{
  
  QFileInfo file(_filename);
  
  if ( !file.exists() )
    emit log(LOGERR,tr("File %1 does not exist!").arg(_filename));
 
  
  bool ok;
  
  emit functionExists ("scripting", "slotLoadScript(QString)", ok);
  
  if (!ok)
    return -1;
  
  RPC::callFunction ("scripting", "slotLoadScript", _filename);
  
  return 0;
};

Q_EXPORT_PLUGIN2( filescriptplugin , FileScriptPlugin );

