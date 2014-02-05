/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

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

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Topology")
#endif

  signals:
    // BaseInterface
    void updateView();
    void updatedObject(int _id, const UpdateType& _type);

    // PickingInterface
    void addHiddenPickMode( const std::string& _mode );

    // BackupInterface
    void createBackup( int _objectid, QString _name, UpdateType _type = UPDATE_ALL);

    // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    // ToolbarInterface
    void addToolbar(QToolBar* _toolbar);

  private slots:

    // BaseInterface
    void pluginsInitialized();

    //PickingInterface
    void slotPickModeChanged( const std::string& _mode);

    // MouseInterface
    void slotMouseEvent( QMouseEvent* _event );

  private slots:

    /// called when an action on the toolbar was triggered
    void toolBarTriggered(QAction* _action);

  public :

    ///Constructor
    TopologyPlugin();

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

    QAction* edgeFlipAction_;
    QAction* edgeSplitAction_;
    QAction* edgeCollapseAction_;

    QAction* faceAddAction_;
    QAction* faceDeleteAction_;
    QAction* faceSplitAction_;


   public slots:
      QString version() { return QString("1.0"); };
};

#endif //TOPOLOGYPLUGIN_HH
