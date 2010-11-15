#ifndef SUBDIVIDERPLUGIN_HH
#define SUBDIVIDERPLUGIN_HH

/** Simple Subdivider Plugin based on Subdivider code from Martin Habbeke
 */
#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "subdividerToolbar.hh"

// Subdivider types
//typename OpenMesh::Subdivider::Uniform:LoopT<TriMesh>  LoopT;
//typename OpenMesh::Subdivider::Uniform:Sqrt3T<TriMesh> Sqrt3T;

class SubdividerPlugin : public QObject, BaseInterface , ToolboxInterface, LoggingInterface, LoadSaveInterface, ScriptInterface, BackupInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(LoadSaveInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(BackupInterface)

signals:
  void updateView();
  void updatedObject(int _id, const UpdateType _type);

  void addPickMode( const std::string _mode );

  void log(Logtype _type, QString _message);
  void log(QString _message);

  void addEmptyObject( DataType _type, int& _id );

  void defineViewMode(QString _mode, QStringList _usedWidgets);

  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget );   
  
  // BackupInterface
  void createBackup( int _id , QString _name );
  
  // ScriptInterface
  void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                          QStringList _parameters, QStringList _descriptions);

private slots:

  void initializePlugin();
  
  void pluginsInitialized();
  
  /// Make plugin available in no-GUI mode
  void noguiSupported( ) {} ;
  
  void slotSubdivideUniform();

public slots:

  /// Scripting slot for subdivision
  void subdivide(int _objectId, QString _algorithm , int _steps);
  

public :
  ~SubdividerPlugin() {};

  QString name() { return (QString("Subdivider")); };
  QString description( ) { return (QString("Subdivide a mesh")); };

private :
  /// Widget for Toolbox
  subdividerToolbarWidget* tool_;

private:

  bool test_trimesh_object(int _identifier, BaseObjectData*& _object);


public slots:
   QString version() { return QString("1.0"); };
};

#endif //SUBDIVIDERPLUGIN_HH
