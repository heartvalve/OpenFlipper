#ifndef FILEOFFPLUGIN_HH
#define FILEOFFPLUGIN_HH

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
#include <ObjectTypes/Plane/Plane.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

class FileOFFPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface,
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

  public :
    
     FileOFFPlugin();

     ~FileOFFPlugin() {};

     QString name() { return (QString("FileOFF")); };
     QString description( ) { return (QString(tr("Load/Save OFF-Files"))); };

     DataType supportedType();

     QString getSaveFilters();
     QString getLoadFilters();

     QWidget* saveOptionsWidget(QString /*_currentFilter*/);
     QWidget* loadOptionsWidget(QString /*_currentFilter*/);

  public slots:

    /// Loads Object and converts it to a triangle mesh if possible
    int loadObject(QString _filename);
    
    /// Always loads mesh as polymesh
    int loadPolyMeshObject(QString _filename);
    
    /// Loads a triangle mesh
    int loadTriMeshObject(QString _filename);

    bool saveObject(int _id, QString _filename);

    QString version() { return QString("1.0"); };

  private :
    
    //Option Widgets
    QWidget*   loadOptions_;
    QComboBox* triMeshHandling_;
    
    QWidget* saveOptions_;
    QCheckBox* saveBinary_;
    QCheckBox* saveVertexColor_;
    QCheckBox* saveFaceColor_;
    QCheckBox* saveAlpha_;
    QCheckBox* saveNormals_;
    QCheckBox* saveTexCoords_;

    QCheckBox* loadVertexColor_;
    QCheckBox* loadFaceColor_;
    QCheckBox* loadAlpha_;
    QCheckBox* loadNormals_;
    QCheckBox* loadTexCoords_;
};

#endif //FILEOFFPLUGIN_HH
