#ifndef FILESKELETONPLUGIN_HH
#define FILESKELETONPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <ObjectTypes/Skeleton/Skeleton.hh>

class FileSkeletonPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface, LoggingInterface, ScriptInterface, INIInterface
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

    void loadIniFile( INIFile& _ini ,int _id );
    void saveIniFile( INIFile& _ini ,int _id);

    void noguiSupported( ) {} ;
    
    void initializePlugin();

  public :

     ~FileSkeletonPlugin() {};

     QString name() { return (QString("FileSkeleton")); };
     QString description( ) { return (QString("Load/Save Skeletons")); };

     int addEmpty( );

     QString typeName() { return (QString("Skeleton")); };

     DataType supportedType();

     QString getSaveFilters();
     QString getLoadFilters();

     QWidget* saveOptionsWidget(QString /*_currentFilter*/) { return 0; };
     QWidget* loadOptionsWidget(QString /*_currentFilter*/) { return 0; };

  public slots:

    int loadObject(QString _filename);

    bool saveObject(int _id, QString _filename);

    int addNewSkeleton();

  private :
	  template<typename Skeleton> bool LoadSkeleton(Skeleton *_pSkeleton, QString _filename);
	  template<typename Skeleton> bool SaveSkeleton(Skeleton *_pSkeleton, QString _filename);

  QString get_unique_name(SkeletonObject* _object);
};

#endif //FILESKELETONPLUGIN_HH
