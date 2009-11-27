#ifndef FILESCRIPTPLUGIN_HH
#define FILESCRIPTPLUGIN_HH

#include <QObject>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>

class FileScriptPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface, LoggingInterface, ScriptInterface, INIInterface, RPCInterface
{
   Q_OBJECT
   Q_INTERFACES(FileInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(ScriptInterface)
   Q_INTERFACES(INIInterface)
   Q_INTERFACES(RPCInterface)

  signals:
    // log Interface
    void log(Logtype _type, QString _message);
    void log(QString _message);
    
    // RPC Interface
    void pluginExists (QString _pluginName, bool& _exists) ;
    void functionExists (QString _pluginName, QString _functionName, bool& _exists);    

  private slots:

    void noguiSupported( ) {} ;

  public :

     ~FileScriptPlugin() {};

     QString name() { return (QString("FileScript")); };
     QString description( ) { return (QString(tr("Load Scripts"))); };

     QString typeName() { return (QString("Script")); };

     DataType supportedType();

     QString getSaveFilters();
     QString getLoadFilters();
     


     QWidget* saveOptionsWidget(QString /*_currentFilter*/) { return 0; };
     QWidget* loadOptionsWidget(QString /*_currentFilter*/) { return 0; };

  public slots:

    int loadObject(QString _filename);
    
    /// No objects are added by this Plugin so ignore the addEmpty call
    int addEmpty(){ return -1; };
    
    /// No objects are saved by this Plugin so ignore the saveObject call
    bool saveObject(int /*_id*/, QString /*_filename*/) { return false;}

    QString version() { return QString("1.0"); };

};

#endif //FILESCRIPTPLUGIN_HH
