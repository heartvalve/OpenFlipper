#ifndef FILEPOLYLINEPLUGIN_HH
#define FILEPOLYLINEPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <ObjectTypes/PolyLine/PolyLine.hh>

class FilePolyLinePlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface, LoggingInterface, ScriptInterface, INIInterface
{
   Q_OBJECT
   Q_INTERFACES(FileInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(ScriptInterface)
   Q_INTERFACES(INIInterface)

  signals:
    void openedFile( int _id );
    void addEmptyObject( DataType _type, int& _id);
    void load(QString _filename, DataType _type, int& _id);
    void save(int _id , QString _filename );
    void log(Logtype _type, QString _message);
    void log(QString _message);

    void emptyObjectAdded( int _id );

  private slots:

    void fileOpened( int /*_id*/ ){};

    void loadIniFileLast( INIFile& _ini ,int _id );
    void saveIniFile( INIFile& _ini ,int _id);

    void noguiSupported( ) {} ;

    void initializePlugin();

  public slots:
    
    // Deprecated
    int addEmpty() { return -1; };
    
  public :

     ~FilePolyLinePlugin() {};

     QString name() { return (QString("FilePolyLine")); };
     QString description( ) { return (QString(tr("Load/Save Poly Lines"))); };

     DataType supportedType();

     QString getSaveFilters();
     QString getLoadFilters();

     QWidget* saveOptionsWidget(QString /*_currentFilter*/) { return 0; };
     QWidget* loadOptionsWidget(QString /*_currentFilter*/) { return 0; };

  public slots:

    int loadObject(QString _filename);

    bool saveObject(int _id, QString _filename);

    int addNewPolyLine();

    QString version() { return QString("1.01"); };

  private :

  QString get_unique_name(PolyLineObject* _object);
};

#endif //FILEPOLYLINEPLUGIN_HH
