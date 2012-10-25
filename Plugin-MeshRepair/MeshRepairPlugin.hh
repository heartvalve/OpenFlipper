/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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

#ifndef MESHREPAIRPLUGIN_HH
#define MESHREPAIRPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "MeshRepairToolbar.hh"

class MeshRepairPlugin : public QObject, BaseInterface , ToolboxInterface, BackupInterface, LoggingInterface, ScriptInterface, RPCInterface
{
    Q_OBJECT
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(ToolboxInterface)
    Q_INTERFACES(BackupInterface)
    Q_INTERFACES(LoggingInterface)
    Q_INTERFACES(ScriptInterface)
    Q_INTERFACES(RPCInterface)

signals:

    void updateView();

    // Base Interface
    void updatedObject(int _identifier, const UpdateType& _type);
    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

    // Logging interface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    // ToolboxInterface
    void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );

    // Backup Interface
    void createBackup( int _objectid, QString _name, UpdateType _type = UPDATE_ALL);

    // RPC Interface
    void pluginExists( QString _pluginName , bool& _exists  ) ;
    void functionExists( QString _pluginName , QString _functionName , bool& _exists  );

    // ScriptInterface
    void scriptInfo( QString _functionName );

private slots:

    void initializePlugin();
    void pluginsInitialized();

public :

    MeshRepairPlugin();

    ~MeshRepairPlugin() {};

    QString name() {
        return (QString("Mesh Repair"));
    };
    QString description( ) {
        return (QString("Functionality to repair meshes"));
    };

private :

    /// Widget for Toolbox
    MeshRepairToolbarWidget* tool_;
    QIcon* toolIcon_;

private slots:

    /// Button slot
    void slotRemoveSelectedVal3Vertices();

    /// Button slot
    void slotRemoveSelectedEdges();

    /// Button slot
    void slotDetectSkinnyTriangleByAngle();

    /// Button slot
    void slotRemoveSkinnyTriangleByAngle();

    /// Button slot
    void slotDetectFoldover();

    /// Button slot
    void slotDetectTriangleAspect();

    /// Button slot
    void slotFlipOrientation();

    /// Button slot
    void slotFixMesh();

    /// Button slot
    void slotUpdateVertexNormals();

    /// Button slot
    void slotUpdateFaceNormals();

    /// Button slot
    void slotUpdateHalfedgeNormals();

    /// Button slot
    void slotUpdateNormals();

    /// Button Slot
    void slotDetectEdgesLonger();

    /// Button Slot
    void slotDetectEdgesShorter();

    /// Button slot
    void slotDetectFlatValence3Vertices();

    /// Button slot
    void slotSnapBoundary();

    /// Button slot
    void slotFixNonManifoldVertices();


    //===========================================================================
    /** @name Scripting functions
    * @{ */
    //===========================================================================

public slots:

    /// Remove all selected valence 3 vertices
    void removeSelectedVal3Vertices(int _objectId);

    /// Removes all selected edges
    void removeSelectedEdges(int _objectId);

    /// Detect/Remove edges where neighboring faces form angle > _angle degrees
    void detectSkinnyTriangleByAngle(int _objectId, double _angle, bool _remove);

    /// Detect folded-over configurations
    void detectFoldover(int _objectId, float _angle);

    /// Selects all edges of an object which are shorter than the given length
    void selectEdgesShorterThan(int _objectId,double _length);

    /// Selects all edges of an object which are larger than the given length
    void selectEdgesLongerThan(int _objectId,double _length);

    /// Detect valence 3 vertices with faces that lie in the plane of their adjacent triangles
    void detectFlatValence3Vertices(int _objectId, double _angle);

    // ==================================================
    // Face operations
    // ==================================================

    /** \brief Detect triangles with aspect ratio greater than _aspect and select them
     *
     * @param _objectId Id of the object
     * @param _aspect   Minimal aspect ratio to select
     */
    void detectTriangleAspect(int _objectId, float _aspect);

    /** \brief Flips the normals of all selected faces by changing the vertex order
     *
     * @param _objectId Id of the object
     */
    void flipOrientation(int _objectId);

    // ==================================================
    // Normal recomputations
    // ==================================================

    /** \brief  Recomputes the face normals of an object
     *
     * @param _objectId Id of the object
     */
    void updateFaceNormals(int _objectId);

    /** \brief Recomputes the halfedge normals of an object
     *
     * @param _objectId Id of the object
     */
    void updateHalfedgeNormals(int _objectId);

    /** \brief Recomputes the vertex normals of an object
     *
     * @param _objectId Id of the object
     */
    void updateVertexNormals(int _objectId);

    /** \brief  Recomputes the face and vertex normals of an object
     *
     * @param _objectId Id of the object
     */
    void updateNormals(int _objectId);

    // ==================================================
    // General
    // ==================================================

    /** \brief Snaps selected vertices at boundaries
     *
     * Snaps selected boundary vertices together if they are closer than the given
     * distance. No new vertices will be introduced on either edge, so they are just
     * snapped to existing ones.
     *
     * @param _objectId Id of the object
     * @param _eps      Distance of vertices to snap
     */
    void snapBoundary(int _objectId, double _eps);

    /** \brief remove non-manifold vertices by duplicating them
     *
     * @param _objectId Id of the mesh to fix
     */
    void fixNonManifoldVertices(int _objectId);

    /** \brief Fix a mesh
     *
     * Degenerated faces will be removed and all vertices which are closer than the given distance
     * will be collapsed. Non-manifold configurations at vertices will be removed and all faces of
     * each component will be updated to have the same orientation.
     *
     * @param _objectId Id of the object to fix
     * @param _epsilon  Snapping distance
     */
    void fixMesh(int _objectId, double _epsilon);

    /** @} */

private:
    /** \brief select edges based on length
    *
    * Selects edges if they are larger or shorter than a given length,
    * @param _objectId Id of the object to select
    * @param _length Length
    * @param _larger true : select larger edges, false select smaller edges
    */
    void selectionEdgeLength(int _objectId, double _length, bool _larger);

    /** \brief flip orientation of selected faces.
     *
     * flips the orientation of selected faces.
     * for correct topology, some vertices are added
     * @param _mesh target mesh
     *
     */
    template<typename MeshT>
    void flipOrientationSelected(MeshT *_mesh);


public slots:
    QString version() { return QString("1.3"); };
};

#if defined(INCLUDE_TEMPLATES) && !defined(MESHREPAIRPLUGINT_CC)
#define MESHREPAIRPLUGIN_TEMPLATES
#include "MeshRepairPluginT.cc"
#endif

#endif //MESHREPAIRPLUGIN_HH
