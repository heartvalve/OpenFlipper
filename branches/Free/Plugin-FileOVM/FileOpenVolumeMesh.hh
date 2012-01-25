#ifndef FILEOPENVOLUMEMESH_HH
#define FILEOPENVOLUMEMESH_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <ObjectTypes/Plane/Plane.hh>

#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
#ifndef FILEOPENVOLUMEMESHT_CC
    #include <OpenVolumeMesh/FileManager/FileManager.hh>
#endif

class FileOpenVolumeMeshPlugin: public QObject,
        BaseInterface,
        FileInterface,
        LoadSaveInterface,
        LoggingInterface,
        ScriptInterface,
        INIInterface {
Q_OBJECT
Q_INTERFACES(FileInterface)
Q_INTERFACES(LoadSaveInterface)
Q_INTERFACES(LoggingInterface)
Q_INTERFACES(BaseInterface)
Q_INTERFACES(ScriptInterface)
Q_INTERFACES(INIInterface)

public:
    // Default constructor
    FileOpenVolumeMeshPlugin();

signals:
    void openedFile(int _id);
    void addEmptyObject(DataType _type, int& _id);
    void load(QString _filename, DataType _type, int& _id);
    void save(int _id, QString _filename);
    void log(Logtype _type, QString _message);
    void log(QString _message);

    void emptyObjectAdded(int _id);

    void updateView();
    void updatedObject(int _id, const UpdateType& _type);

    void registerKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse = false);

private slots:

    void fileOpened(int /*_id*/) {}

    void loadIniFileLast(INIFile& _ini, int _id);
    void saveIniFile(INIFile& _ini, int _id);

    void initializePlugin();

public:

    ~FileOpenVolumeMeshPlugin() {}

    QString name() {
        return (QString("FileOpenVolumeMesh"));
    }

    QString description() {
        return (QString(tr("Load/Save OpenVolumeMeshes")));
    }

    DataType supportedType();

    QString getSaveFilters();
    QString getLoadFilters();

    QWidget* saveOptionsWidget(QString _currentFilter);
    QWidget* loadOptionsWidget(QString _currentFilter);

public slots:

    int loadObject(QString _filename);

    bool saveObject(int _id, QString _filename);

    QString version() {
        return QString("1.0");
    }

private:

    template <typename MeshT>
    void loadMesh(const char* _filename, MeshT& _mesh, bool _comp, bool _topCheck, bool _correctOrder);

    template <typename MeshT>
    void saveMesh(const char* _filename, MeshT& _mesh, bool _comp);

private:

    QWidget* loadOptions_;
    QWidget* saveOptions_;

    QComboBox* typeCheck_;

    QCheckBox* loadCompMode_;
    QCheckBox* loadCorrectOrder_;
    QCheckBox* loadTopCheck_;
    QCheckBox* saveCompMode_;

    OpenVolumeMesh::IO::FileManager fileManager_;
};

#if defined(INCLUDE_TEMPLATES) && !defined(FILEOPENVOLUMEMESHT_CC)
#include "FileOpenVolumeMeshT.cc"
#endif

#endif //FILEOPENVOLUMEMESH_HH
