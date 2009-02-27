//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
// 
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================

#ifndef TOPOLOGYPLUGIN_HH
#define TOPOLOGYPLUGIN_HH

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

class TopologyPlugin : public QObject, BaseInterface , MouseInterface, KeyInterface, PickingInterface, BackupInterface, LoggingInterface, ToolbarInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(ToolbarInterface)

  signals:
    // BaseInterface
    void updateView();
    void updatedObject(int);

    // PickingInterface
    void addHiddenPickMode( const std::string _mode );

    // BackupInterface
    void createBackup( int _id , QString _name );

    // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    // ToolbarInterface
    void addToolbar(QToolBar* _toolbar);

  private slots:

    // BaseInterface
    void pluginsInitialized();

    // MouseInterface
    void slotMouseEvent( QMouseEvent* _event );

  private slots:

    /// called when an action on the toolbar was triggered
    void toolBarTriggered(QAction* _action);

  public :

    /// Destructor
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

    QToolBar* toolbar_;


   public slots:
      QString version() { return QString("1.0"); };
};

#endif //TOPOLOGYPLUGIN_HH
