#ifndef FILESTLPLUGIN_HH
#define FILESTLPLUGIN_HH

#include <QObject>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

class FileSTLPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface,
    LoggingInterface, ScriptInterface, INIInterface
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
    void updateView();

    void emptyObjectAdded( int _id );
    void deleteObject( int _id );

  private slots:

    void fileOpened( int /*_id*/ ){};

    void noguiSupported( ) {} ;

    void initializePlugin();
    
    // Deprecated!
    int addEmpty() { return -1; };
    
    /// Slot called when user wants to save the given Load options as default
    void slotLoadDefault();
    
    /// Slot called when user wants to save the given Save options as default
    void slotSaveDefault();

  public :
    
     FileSTLPlugin();

     ~FileSTLPlugin() {};

     QString name() { return (QString("FileSTL")); };
     QString description( ) { return (QString(tr("Load/Save STL-Files"))); };

     DataType supportedType();

     QString getSaveFilters();
     QString getLoadFilters();

     QWidget* saveOptionsWidget(QString /*_currentFilter*/);
     QWidget* loadOptionsWidget(QString /*_currentFilter*/);

  public slots:

    /// Loads Object as triangle mesh
    int loadObject(QString _filename);
    
    bool saveObject(int _id, QString _filename);

    QString version() { return QString("1.0"); };

  private :
    
    //Option Widgets
    QWidget* saveOptions_;
    QWidget* loadOptions_;
    
    QCheckBox* saveBinary_;
    
    QPushButton* saveDefaultButton_;
    QPushButton* loadDefaultButton_;
};

#endif //FILESTLPLUGIN_HH
