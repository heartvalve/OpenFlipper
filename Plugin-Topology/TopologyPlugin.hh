#ifndef TOPOLOGYPLUGIN_HH
#define TOPOLOGYPLUGIN_HH

#include <QObject>
#include <QMenuBar>
#include <QGroupBox>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

class TopologyPlugin : public QObject, BaseInterface , MouseInterface, KeyInterface, PickingInterface, BackupInterface, LoggingInterface, ContextMenuInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(ContextMenuInterface)

  signals:
    void updateView();
    void updatedObject(int);

    void addHiddenPickMode( const std::string _mode );

    void createBackup( int _id , QString _name );

    void log(Logtype _type, QString _message);
    void log(QString _message);

    void addContextMenu(QMenu* _menu ,DataType _objectType , ContextMenuType _type );

  private slots:
    void slotMouseEvent( QMouseEvent* _event );

    void slotKeyEvent( QKeyEvent* /*_event*/ ){};

    void pluginsInitialized();

    void contextMenuTriggered(QAction* _action);

    void slotUpdateContextMenu( int _objectId );

  public :

    ~TopologyPlugin() {};


    QString name() { return (QString("Topology")); };
    QString description( ) { return (QString("Allows to control Mesh topology (Edge Flips,...)")); };

  private :

    /// clear the add face vector
    void clearAddFaceVertices();

    /// Add a face
    void add_face(QMouseEvent* _event);

    /// Split a face at the current hit point
    void split_face(QMouseEvent* _event);

    /// Delete a face at the current hit point
    void delete_face(QMouseEvent* _event);

    /// Flip edge
    void flip_edge(QMouseEvent* _event);

    /// Collapse edge
    void collapse_edge(QMouseEvent* _event);

    /// Split Edge
    void split_edge(QMouseEvent* _event);

  private:
    std::vector< std::pair<int,int> > addFaceVertices_;
    QMenu* trimeshMenu_;
    QMenu* polymeshMenu_;


   public slots:
      QString version() { return QString("1.0"); };
};

#endif //TOPOLOGYPLUGIN_HH
