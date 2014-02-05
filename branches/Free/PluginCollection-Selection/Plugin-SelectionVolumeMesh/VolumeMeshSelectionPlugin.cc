/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
 \*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision: 11942 $                                                      *
 *   $Author: kremer $                                                       *
 *   $Date: 2011-07-06 15:10:17 +0200 (Wed, 06 Jul 2011) $                   *
 *                                                                           *
 \*===========================================================================*/

#include "VolumeMeshSelectionPlugin.hh"

#include <OpenFlipper/common/GlobalOptions.hh>

// Primitive type icons
#define VERTEX_TYPE         "selection_vertex.png"
#define EDGE_TYPE           "selection_edge.png"
#define FACE_TYPE           "selection_face.png"
#define CELL_TYPE           "datacontrol-boundingBox.png"
// Custom selection mode
#define COLUMN_SELECTION    "column-selection.png"
#define SHEET_SELECTION     "sheet-selection.png"
// =======================================
// Define operations
// =======================================
// Vertices:
#define V_SELECT_ALL      "Select All Vertices"
#define V_DESELECT_ALL    "Deselect All Vertices"
#define V_INVERT          "Invert Vertex Selection"
#define V_DELETE          "Delete Selected Vertices"
// Edges:
#define E_SELECT_ALL      "Select All Edges"
#define E_DESELECT_ALL    "Deselect All Edges"
#define E_INVERT          "Invert Edge Selection"
#define E_DELETE          "Delete Selected Edges"
// Faces:
#define F_SELECT_ALL      "Select All Faces"
#define F_DESELECT_ALL    "Deselect All Faces"
#define F_INVERT          "Invert Face Selection"
#define F_DELETE          "Delete Selected Faces"
// Cells:
#define C_SELECT_ALL      "Select All Cells"
#define C_DESELECT_ALL    "Deselect All Cells"
#define C_INVERT          "Invert Cell Selection"
#define C_DELETE          "Delete Selected Cells"

/// Default constructor
VolumeMeshSelectionPlugin::VolumeMeshSelectionPlugin() :
    vertexType_(0), edgeType_(0), allSupportedTypes_(0), lastPickedCell_(HexahedralMesh::InvalidCellHandle),
            lastPickedOrientation_(0) {
}

//==============================================================================================

VolumeMeshSelectionPlugin::~VolumeMeshSelectionPlugin() {
}

//==============================================================================================

void VolumeMeshSelectionPlugin::initializePlugin() {

    // Tell core about all scriptable slots
    updateSlotDescriptions();
}

//==============================================================================================

void VolumeMeshSelectionPlugin::pluginsInitialized() {
    // Create new selection environment for volumemeshs
    // and register volumemesh data type for the environment.

    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();

    emit
    addSelectionEnvironment("VolumeMesh Selections", "Select volume mesh primitives.",
                            iconPath + "datacontrol-boundingBox.png", environmentHandle_);

    // Register mesh object types
    emit
    registerType(environmentHandle_, DATA_POLYHEDRAL_MESH);
    emit
    registerType(environmentHandle_, DATA_HEXAHEDRAL_MESH);

    emit addPrimitiveType(environmentHandle_, "Select Volume Mesh Vertices", iconPath + VERTEX_TYPE, vertexType_);
    emit addPrimitiveType(environmentHandle_, "Select Volume Mesh Edges", iconPath + EDGE_TYPE, edgeType_);
    emit addPrimitiveType(environmentHandle_, "Select Volume Mesh Faces", iconPath + FACE_TYPE, faceType_);
    emit addPrimitiveType(environmentHandle_, "Select Volume Mesh Cells", iconPath + CELL_TYPE, cellType_);

    emit addCustomSelectionMode(environmentHandle_, "Column Selection", "Select entire column of cells",
                           iconPath + COLUMN_SELECTION, cellType_, columnSelectionHandle_);

    emit
    addCustomSelectionMode(environmentHandle_, "Sheet Selection", "Select entire sheet of cells",
                           iconPath + SHEET_SELECTION, cellType_, sheetSelectionHandle_);

    allSupportedTypes_ = vertexType_ | edgeType_ | faceType_ | cellType_;
    floodFillSupportedTypes_ = vertexType_ | edgeType_ | faceType_;

    // Determine, which selection modes are requested
    emit showToggleSelectionMode(environmentHandle_, true, allSupportedTypes_);

    emit showVolumeLassoSelectionMode(environmentHandle_, true, allSupportedTypes_);
    emit showFloodFillSelectionMode(environmentHandle_, true, floodFillSupportedTypes_);

    // Define vertex operations
    QStringList vertexOperations;
    vertexOperations.append(V_SELECT_ALL);
    vertexOperations.append(V_DESELECT_ALL);
    vertexOperations.append(V_INVERT);
    vertexOperations.append(V_DELETE);

    QStringList edgeOperations;
    edgeOperations.append(E_SELECT_ALL);
    edgeOperations.append(E_DESELECT_ALL);
    edgeOperations.append(E_INVERT);
    edgeOperations.append(E_DELETE);

    QStringList faceOperations;
    faceOperations.append(F_SELECT_ALL);
    faceOperations.append(F_DESELECT_ALL);
    faceOperations.append(F_INVERT);
    faceOperations.append(F_DELETE);

    QStringList cellOperations;
    cellOperations.append(C_SELECT_ALL);
    cellOperations.append(C_DESELECT_ALL);
    cellOperations.append(C_INVERT);
    cellOperations.append(C_DELETE);

    emit
    addSelectionOperations(environmentHandle_, vertexOperations, "Vertex Operations", vertexType_);
    emit
    addSelectionOperations(environmentHandle_, edgeOperations, "Edge Operations", edgeType_);
    emit
    addSelectionOperations(environmentHandle_, faceOperations, "Face Operations", faceType_);
    emit
    addSelectionOperations(environmentHandle_, cellOperations, "Cell Operations", cellType_);

    // Register key shortcuts:

    // Select (a)ll
    emit
    registerKeyShortcut(Qt::Key_A, Qt::ControlModifier);
    // (C)lear selection
    emit
    registerKeyShortcut(Qt::Key_C, Qt::NoModifier);
    // (I)nvert selection
    emit
    registerKeyShortcut(Qt::Key_I, Qt::NoModifier);
    // Delete selected entities
    emit registerKeyShortcut(Qt::Key_Delete, Qt::NoModifier);
}

//==============================================================================================

void VolumeMeshSelectionPlugin::updateSlotDescriptions() {

    /*
     * TODO: Complete this function for all supported types
     */
}

//==============================================================================================

bool VolumeMeshSelectionPlugin::vertexTypeActive() {
  SelectionInterface::PrimitiveType t = 0u;
  emit getActivePrimitiveType(t);
  return (t & vertexType_) > 0;
}

bool VolumeMeshSelectionPlugin::edgeTypeActive() {
  SelectionInterface::PrimitiveType t = 0u;
  emit getActivePrimitiveType(t);
  return (t & edgeType_) > 0;
}

bool VolumeMeshSelectionPlugin::faceTypeActive() {
  SelectionInterface::PrimitiveType t = 0u;
  emit getActivePrimitiveType(t);
  return (t & faceType_) > 0;
}

bool VolumeMeshSelectionPlugin::cellTypeActive() {
  SelectionInterface::PrimitiveType t = 0u;
  emit getActivePrimitiveType(t);
  return (t & cellType_) > 0;
}

//==============================================================================================

void VolumeMeshSelectionPlugin::slotSelectionOperation(QString _operation) {

    SelectionInterface::PrimitiveType type = 0u;
    emit getActivePrimitiveType(type);

    if((type & allSupportedTypes_) == 0)
        return;

    // Test if operation should be applied to target objects only
    bool targetsOnly = false;
    emit targetObjectsOnly(targetsOnly);
    PluginFunctions::IteratorRestriction restriction = (targetsOnly ? PluginFunctions::TARGET_OBJECTS
                                                                    : PluginFunctions::ALL_OBJECTS);

    if (_operation == V_SELECT_ALL) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectAllVertices(o_it->id());
        }
    } else if (_operation == V_DESELECT_ALL) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                deselectAllVertices(o_it->id());
        }
    } else if (_operation == V_INVERT) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                invertVertexSelection(o_it->id());
        }
    } else if (_operation == V_DELETE) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                deleteSelectedVertices(o_it->id());
        }
    } else if (_operation == E_SELECT_ALL) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectAllEdges(o_it->id());
        }
    } else if (_operation == E_DESELECT_ALL) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                deselectAllEdges(o_it->id());
        }
    } else if (_operation == E_INVERT) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                invertEdgeSelection(o_it->id());
        }
    } else if (_operation == E_DELETE) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                deleteSelectedEdges(o_it->id());
        }
    } else if (_operation == F_SELECT_ALL) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectAllFaces(o_it->id());
        }
    } else if (_operation == F_DESELECT_ALL) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                deselectAllFaces(o_it->id());
        }
    } else if (_operation == F_INVERT) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                invertFaceSelection(o_it->id());
        }
    } else if (_operation == F_DELETE) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                deleteSelectedFaces(o_it->id());
        }
    } else if (_operation == C_SELECT_ALL) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                selectAllCells(o_it->id());
        }
    } else if (_operation == C_DESELECT_ALL) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                deselectAllCells(o_it->id());
        }
    } else if (_operation == C_INVERT) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                invertCellSelection(o_it->id());
        }
    } else if (_operation == C_DELETE) {
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible())
                deleteSelectedCells(o_it->id());
        }
    }
}

//==============================================================================================

void VolumeMeshSelectionPlugin::slotToggleSelection(QMouseEvent* _event,
                                                    SelectionInterface::PrimitiveType _currentType, bool _deselect) {

    // Return if none of the currently active types is handled by this plugin
    if((_currentType & allSupportedTypes_) == 0)
        return;

    // Return if mouse event is not a left-button click
    if(_event->button() != Qt::LeftButton)
        return;

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    BaseObjectData* object = 0;

    if(_currentType & vertexType_) {
        // Perform picking
        bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(),
                                                                  node_idx, target_idx, &hit_point)
                && PluginFunctions::getPickedObject(node_idx, object);

        if(successfullyPicked) {

            PolyhedralMesh* polyMesh = PluginFunctions::polyhedralMesh(object);
            HexahedralMesh* hexMesh = PluginFunctions::hexahedralMesh(object);
            if(!polyMesh && !hexMesh) {
                emit log(LOGERR, tr("Neither polyhedral nor hexahedral mesh!"));
                return;
            }

            OpenVolumeMesh::StatusAttrib& status = (PluginFunctions::polyhedralMeshObject(object) != 0 ?
                                                    PluginFunctions::polyhedralMeshObject(object)->status() :
                                                    PluginFunctions::hexahedralMeshObject(object)->status());

            if(status[OpenVolumeMesh::VertexHandle(target_idx)].selected() || _deselect)
                status[OpenVolumeMesh::VertexHandle(target_idx)].set_selected(false);
            else
                status[OpenVolumeMesh::VertexHandle(target_idx)].set_selected(true);

            emit updatedObject(object->id(), UPDATE_SELECTION);
        }
    }

    if(_currentType & edgeType_) {
        // Perform picking
        bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_EDGE, _event->pos(), node_idx,
                                                                  target_idx, &hit_point)
                && PluginFunctions::getPickedObject(node_idx, object);

        if(successfullyPicked) {

            PolyhedralMesh* polyMesh = PluginFunctions::polyhedralMesh(object);
            HexahedralMesh* hexMesh = PluginFunctions::hexahedralMesh(object);
            if(!polyMesh && !hexMesh) {
                emit log(LOGERR, tr("Neither polyhedral nor hexahedral mesh!"));
                return;
            }

            OpenVolumeMesh::StatusAttrib& status = (PluginFunctions::polyhedralMeshObject(object) != 0 ?
                                                    PluginFunctions::polyhedralMeshObject(object)->status() :
                                                    PluginFunctions::hexahedralMeshObject(object)->status());

            if(status[OpenVolumeMesh::EdgeHandle(target_idx)].selected() || _deselect)
                status[OpenVolumeMesh::EdgeHandle(target_idx)].set_selected(false);
            else
                status[OpenVolumeMesh::EdgeHandle(target_idx)].set_selected(true);

            emit updatedObject(object->id(), UPDATE_SELECTION);
        }
    }

    if(_currentType & faceType_) {
        // Perform picking
        bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(), node_idx,
                                                                  target_idx, &hit_point)
                && PluginFunctions::getPickedObject(node_idx, object);

        if(successfullyPicked) {

            PolyhedralMesh* polyMesh = PluginFunctions::polyhedralMesh(object);
            HexahedralMesh* hexMesh = PluginFunctions::hexahedralMesh(object);
            if(!polyMesh && !hexMesh) {
                emit log(LOGERR, tr("Neither polyhedral nor hexahedral mesh!"));
                return;
            }

            OpenVolumeMesh::StatusAttrib& status = (PluginFunctions::polyhedralMeshObject(object) != 0 ?
                                                    PluginFunctions::polyhedralMeshObject(object)->status() :
                                                    PluginFunctions::hexahedralMeshObject(object)->status());

            if(status[OpenVolumeMesh::FaceHandle(target_idx)].selected() || _deselect)
                status[OpenVolumeMesh::FaceHandle(target_idx)].set_selected(false);
            else
                status[OpenVolumeMesh::FaceHandle(target_idx)].set_selected(true);

            emit updatedObject(object->id(), UPDATE_SELECTION);
        }
    }

    if(_currentType & cellType_) {

        // Perform picking
        bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_CELL, _event->pos(), node_idx,
                                                                  target_idx, &hit_point)
                && PluginFunctions::getPickedObject(node_idx, object);

        if(successfullyPicked) {

            PolyhedralMesh* polyMesh = PluginFunctions::polyhedralMesh(object);
            HexahedralMesh* hexMesh = PluginFunctions::hexahedralMesh(object);
            if(!polyMesh && !hexMesh) {
                emit log(LOGERR, tr("Neither polyhedral nor hexahedral mesh!"));
                return;
            }

            OpenVolumeMesh::StatusAttrib& status = (PluginFunctions::polyhedralMeshObject(object) != 0 ?
                                                    PluginFunctions::polyhedralMeshObject(object)->status() :
                                                    PluginFunctions::hexahedralMeshObject(object)->status());

            if(status[OpenVolumeMesh::CellHandle(target_idx)].selected() || _deselect) {
                status[OpenVolumeMesh::CellHandle(target_idx)].set_selected(false);
            } else {
                status[OpenVolumeMesh::CellHandle(target_idx)].set_selected(true);
            }

            emit updatedObject(object->id(), UPDATE_SELECTION);
        }
    }
}

//==============================================================================================

void VolumeMeshSelectionPlugin::slotVolumeLassoSelection(QMouseEvent* _event,
        PrimitiveType _currentType, bool _deselect)
{
    if ((_currentType & allSupportedTypes_) == 0) return;

    if (_event->type() == QEvent::MouseButtonPress)
    {
        volumeLassoPoints_.append(_event->pos());
        return;
    }
    else if (_event->type() == QEvent::MouseButtonDblClick)
    {
        ACG::GLState &state = PluginFunctions::viewerProperties().glState();
        bool updateGL = state.updateGL();
        state.set_updateGL (false);

        QPolygon p(volumeLassoPoints_);
        QRegion region = QRegion(p);

        SelectVolumeAction action(region, this, _currentType, _deselect, state);
        ACG::SceneGraph::traverse (PluginFunctions::getRootNode(), action);

        state.set_updateGL(updateGL);

        // Clear lasso points
        volumeLassoPoints_.clear();
    }
}

void VolumeMeshSelectionPlugin::slotFloodFillSelection(QMouseEvent* _event,
        double _maxAngle, PrimitiveType _currentType, bool _deselect)
{
    // Return if none of the currently active types is handled by this plugin
    if ((_currentType & floodFillSupportedTypes_) == 0)
        return;

    unsigned int node_idx, target_idx;
    ACG::Vec3d hit_point;

    // pick Anything to find all possible objects
    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,
                                        _event->pos(), node_idx, target_idx, &hit_point))
    {
        BaseObjectData* object = 0;

        if (PluginFunctions::getPickedObject(node_idx, object))
        {
            if (object->dataType() == DATA_POLYHEDRAL_MESH)
            {
                if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE,
                            _event->pos(), node_idx, target_idx, &hit_point))
                {
                    if (PluginFunctions::getPickedObject(node_idx, object))
                    {
                        if (object->dataType(DATA_POLYHEDRAL_MESH))
                        {
                            floodFillSelection(PluginFunctions::polyhedralMesh(object),
                                    target_idx, _maxAngle, _currentType, _deselect);

                            emit updatedObject(object->id(), UPDATE_SELECTION);
                        }
                    }
                }
            }
            else if(object->dataType() == DATA_HEXAHEDRAL_MESH)
            {
                if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE,
                            _event->pos(), node_idx, target_idx, &hit_point))
                {
                    if(PluginFunctions::getPickedObject(node_idx, object) )
                    {
                        if(object->dataType(DATA_HEXAHEDRAL_MESH))
                        {
                            floodFillSelection(PluginFunctions::hexahedralMesh(object),
                                    target_idx, _maxAngle, _currentType, _deselect);

                            emit updatedObject(object->id(), UPDATE_SELECTION);
                        }
                    }
                }
            }
            else
            {
                emit log(LOGERR, tr("floodFillSelection: Unsupported dataType"));
            }
        }
    }
}

/// Traverse the scenegraph and call the selection function for all mesh nodes
bool SelectVolumeAction::operator()(BaseNode* _node)
{
    BaseObjectData* object = 0;
    if (PluginFunctions::getPickedObject(_node->id(), object))
    {
        bool selected = false;
        if (object->dataType(DATA_POLYHEDRAL_MESH))
        {
            PolyhedralMesh* m = PluginFunctions::polyhedralMesh(object);
            selected = plugin_->volumeSelection(m, state_, &region_, type_, deselection_);

        } else if(object->dataType(DATA_HEXAHEDRAL_MESH)) {

            HexahedralMesh* m = PluginFunctions::hexahedralMesh(object);
            selected = plugin_->volumeSelection(m, state_, &region_, type_, deselection_);
        }

        if (selected){
            emit plugin_->updatedObject(object->id(), UPDATE_SELECTION);
        }
    }
    return true;
}

//==============================================================================================

void VolumeMeshSelectionPlugin::slotCustomSelection(QMouseEvent *_event, PrimitiveType _currentType,
                                                    QString _customIdentifier, bool _deselect) {

    if(_customIdentifier != columnSelectionHandle_ && _customIdentifier != sheetSelectionHandle_)
        return;

    // Return if mouse event is not a left-button click
    if(_event->button() != Qt::LeftButton || _event->type() != QEvent::MouseButtonPress)
        return;

    if(_customIdentifier == columnSelectionHandle_) {

        ACG::Vec3d hit_point;

        if(_currentType & cellType_) {

            BaseObjectData* object = 0;

            // Perform picking
            unsigned int node_idx, target_idx;
            bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),
                                                                      node_idx, target_idx, &hit_point)
                    && PluginFunctions::getPickedObject(node_idx, object);

            if(successfullyPicked) {

                HexahedralMeshObject* hexMeshObject = PluginFunctions::hexahedralMeshObject(object);
                if(!hexMeshObject) {
                    emit log(LOGERR, "Could not get hexahedral mesh object!");
                    return;
                }
                OpenVolumeMesh::StatusAttrib& status = hexMeshObject->status();

                HexahedralMesh* hexMesh = hexMeshObject->mesh();

                // Get first inside halfface
                OpenVolumeMesh::FaceHandle fh = OpenVolumeMesh::FaceHandle(target_idx);
                OpenVolumeMesh::HalfFaceHandle cif =
                        (hexMesh->is_boundary(hexMesh->halfface_handle(fh, 0)) ? hexMesh->halfface_handle(fh, 1)
                                                                               : hexMesh->halfface_handle(fh, 0));

                while(!hexMesh->is_boundary(cif)) {
                    // Get associated cell
                    OpenVolumeMesh::CellHandle ch = hexMesh->incident_cell(cif);

                    if(status[ch].selected() || _deselect)
                        status[ch].set_selected(false);
                    else
                        status[ch].set_selected(true);

                    cif = hexMesh->opposite_halfface_handle_in_cell(cif, ch);
                    cif = hexMesh->opposite_halfface_handle(cif);
                }

                emit updatedObject(object->id(), UPDATE_SELECTION);
            }
        }
    }

    if(_customIdentifier == sheetSelectionHandle_) {

        if(_currentType & cellType_) {

            BaseObjectData* object = 0;
            ACG::Vec3d hit_point;

            // Perform picking
            unsigned int node_idx, target_idx;
            bool successfullyPicked = PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),
                                                                      node_idx, target_idx, &hit_point)
                    && PluginFunctions::getPickedObject(node_idx, object);

            if(successfullyPicked) {

                HexahedralMeshObject* hexMeshObject = PluginFunctions::hexahedralMeshObject(object);
                if(!hexMeshObject) {
                    emit log(LOGERR, "Could not get hexahedral mesh object!");
                    return;
                }
                OpenVolumeMesh::StatusAttrib& status = hexMeshObject->status();

                HexahedralMesh* hexMesh = hexMeshObject->mesh();

                // Get first inside halfface
                OpenVolumeMesh::FaceHandle fh = OpenVolumeMesh::FaceHandle(target_idx);
                OpenVolumeMesh::HalfFaceHandle cif =
                        (hexMesh->is_boundary(hexMesh->halfface_handle(fh, 0)) ? hexMesh->halfface_handle(fh, 1)
                                                                               : hexMesh->halfface_handle(fh, 0));

                // Get picked cell
                OpenVolumeMesh::CellHandle ch = hexMesh->incident_cell(cif);

                if(lastPickedCell_ == HexahedralMesh::InvalidCellHandle) {

                    lastPickedCell_ = ch;
                    lastPickedOrientation_ = hexMesh->orientation(cif, ch);
                    status[lastPickedCell_].set_selected(!status[lastPickedCell_].selected());
                    emit updatedObject(object->id(), UPDATE_SELECTION);
                } else {

                    // Select whole sheet
                    // Get orientation of common face
                    HFPair pair = getCommonFace(lastPickedCell_, ch, hexMesh);

                    unsigned char secondDir = hexMesh->orientation(pair.first, lastPickedCell_);
                    unsigned char orthDir = hexMesh->orthogonal_orientation(lastPickedOrientation_, secondDir);

                    orientationMap_.clear();
                    status[lastPickedCell_].set_selected(!status[lastPickedCell_].selected());

                    // Start with last picked cell
                    std::set<OpenVolumeMesh::CellHandle> unprocessed;
                    unprocessed.insert(lastPickedCell_);
                    std::set<OpenVolumeMesh::CellHandle> processed;

                    orientationMap_.insert(
                                           std::pair<OpenVolumeMesh::CellHandle, unsigned char>(lastPickedCell_,
                                                                                                orthDir));

                    // Flood-fill cell sheet
                    while(!unprocessed.empty() ) {

                        OpenVolumeMesh::CellHandle cur_c = *unprocessed.begin();
                        unprocessed.erase(cur_c);
                        status[cur_c].set_selected(!status[cur_c].selected());
                        processed.insert(cur_c);

                        std::map<OpenVolumeMesh::CellHandle, unsigned char>::iterator f = orientationMap_.find(cur_c);
                        if(f == orientationMap_.end()) {
                            emit log(LOGERR, "Could not get orientation of current cell in sheet!");
                            return;
                        }
                        unsigned char od = f->second;

                        for(OpenVolumeMesh::CellSheetCellIter csc_it = hexMesh->csc_iter(cur_c, od); csc_it.valid(); ++csc_it) {
                            if(processed.count(*csc_it) > 0)
                                continue;

                            unsigned char new_o = getOrthogonalOrientationOfNeighborCell(cur_c, *csc_it, od, hexMesh);
                            orientationMap_.insert(std::pair<OpenVolumeMesh::CellHandle, unsigned char>(*csc_it, new_o));
                            unprocessed.insert(*csc_it);
                        }
                    }

                    lastPickedCell_ = HexahedralMesh::InvalidCellHandle;
                    emit updatedObject(object->id(), UPDATE_SELECTION);
                }
            }
        }
    }
}

//==============================================================================================

unsigned char VolumeMeshSelectionPlugin::getOrthogonalOrientationOfNeighborCell(const OpenVolumeMesh::CellHandle& _ch1,
                                                                                const OpenVolumeMesh::CellHandle& _ch2,
                                                                                unsigned char _firstOrthDirection,
                                                                                const HexahedralMesh* _mesh) const {

    // Return orientation of halfface in _ch2 that corresponds to
    // _firstOrthDirection in the first cell

    OpenVolumeMesh::HalfFaceHandle firstOrthHF = _mesh->get_oriented_halfface(_firstOrthDirection, _ch1);
    HFPair commonHF = getCommonFace(_ch1, _ch2, _mesh);

    // Get edge that's shared on the orth side
    std::vector<OpenVolumeMesh::HalfEdgeHandle> hes1 = _mesh->halfface(firstOrthHF).halfedges();
    std::vector<OpenVolumeMesh::HalfEdgeHandle> hes2 = _mesh->halfface(commonHF.first).halfedges();
    OpenVolumeMesh::HalfEdgeHandle sharedHE = HexahedralMesh::InvalidHalfEdgeHandle;
    for(std::vector<OpenVolumeMesh::HalfEdgeHandle>::const_iterator he_it1 = hes1.begin(); he_it1 != hes1.end(); ++he_it1) {
        for(std::vector<OpenVolumeMesh::HalfEdgeHandle>::const_iterator he_it2 = hes2.begin(); he_it2 != hes2.end(); ++he_it2) {
            if(_mesh->edge_handle(*he_it1) == _mesh->edge_handle(*he_it2)) {
                sharedHE = _mesh->opposite_halfedge_handle(*he_it2);
                break;
            }
            if(sharedHE != HexahedralMesh::InvalidHalfEdgeHandle)
                break;
        }
    }

    return _mesh->orientation(_mesh->adjacent_halfface_in_cell(commonHF.second, sharedHE), _ch2);
}

//==============================================================================================

VolumeMeshSelectionPlugin::HFPair VolumeMeshSelectionPlugin::getCommonFace(const OpenVolumeMesh::CellHandle& _ch1,
                                                                           const OpenVolumeMesh::CellHandle& _ch2,
                                                                           const HexahedralMesh* _mesh) const {

    std::vector<OpenVolumeMesh::HalfFaceHandle> hfs1 = _mesh->cell(_ch1).halffaces();
    std::vector<OpenVolumeMesh::HalfFaceHandle> hfs2 = _mesh->cell(_ch2).halffaces();

    for(std::vector<OpenVolumeMesh::HalfFaceHandle>::const_iterator hf_it1 = hfs1.begin(); hf_it1 != hfs1.end(); ++hf_it1) {

        for(std::vector<OpenVolumeMesh::HalfFaceHandle>::const_iterator hf_it2 = hfs2.begin(); hf_it2 != hfs2.end(); ++hf_it2) {

            if(_mesh->face_handle(*hf_it1) == _mesh->face_handle(*hf_it2)) {
                return HFPair(*hf_it1, *hf_it2);
            }
        }
    }

    return HFPair(HexahedralMesh::InvalidHalfFaceHandle, HexahedralMesh::InvalidHalfFaceHandle);
}

//==============================================================================================

void VolumeMeshSelectionPlugin::loadSelection(int _objId, const QString& _filename) {

//    // Load ini file
//    INIFile file;
//
//    if(!file.connect(_filename, false)) {
//        emit log(LOGERR, QString("Could not read file '%1'!").arg(_filename));
//        return;
//    }
//
//    // Load selection from file
//    loadIniFile(file, _objId);
}

//==============================================================================================

void VolumeMeshSelectionPlugin::loadIniFile(INIFile& _ini, int _id) {
    // From INI Interface
    // Load plugin specific settings

    BaseObjectData* bod = NULL;
    PluginFunctions::getObject(_id, bod);
    if(!bod) {
        emit log(LOGERR, "Could not get base object data!");
        return;
    }

    QString section = QString("PolyhedralMeshSelection") + "//" + bod->name();
    if(!_ini.section_exists(section)) {
        return;
    }

    std::vector<int> ids;
    // Load vertex selection:
    _ini.get_entry(ids, section, "VertexSelection");
    selectVertices(_id, ids);
    ids.clear();
    // Load edge selection:
    _ini.get_entry(ids, section, "EdgeSelection");
    selectEdges(_id, ids);
    ids.clear();
    // Load half-edge selection:
    _ini.get_entry(ids, section, "HalfEdgeSelection");
    selectHalfEdges(_id, ids);
    ids.clear();
    // Load face selection:
    _ini.get_entry(ids, section, "FaceSelection");
    selectFaces(_id, ids);
    ids.clear();
    // Load half-face selection:
    _ini.get_entry(ids, section, "HalfFaceSelection");
    selectHalfFaces(_id, ids);
    ids.clear();
    // Load cell selection:
    _ini.get_entry(ids, section, "CellSelection");
    selectCells(_id, ids);
    ids.clear();
}

//==============================================================================================

void VolumeMeshSelectionPlugin::saveIniFile(INIFile& _ini, int _id) {
    // From INI Interface
    // Save plugin specific settings

    BaseObjectData* bod = NULL;
    PluginFunctions::getObject(_id, bod);
    if(!bod) {
        emit log(LOGERR, "Could not get base object data!");
        return;
    }

    QString section = QString("PolyhedralMeshSelection") + "//" + bod->name();

    _ini.add_entry(section, "VertexSelection",     getVertexSelection(_id));
    _ini.add_entry(section, "EdgeSelection",       getEdgeSelection(_id));
    _ini.add_entry(section, "HalfEdgeSelection",   getHalfEdgeSelection(_id));
    _ini.add_entry(section, "FaceSelection",       getFaceSelection(_id));
    _ini.add_entry(section, "HalfFaceSelection",   getHalfFaceSelection(_id));
    _ini.add_entry(section, "CellSelection",       getCellSelection(_id));
}

//==============================================================================================

void VolumeMeshSelectionPlugin::slotLoadSelection(const INIFile& _file) {

    // Iterate over all polyhedral mesh objects in the scene and save
    // the selections for all supported entity types
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
         o_it != PluginFunctions::objectsEnd(); ++o_it) {

        // Read section for each object
        // Append object name to section identifier
        QString section = QString("PolyhedralMeshSelection") + "//" + o_it->name();
        if(!_file.section_exists(section)) {
            continue;
        }

        std::vector<int> ids;
        // Load vertex selection:
        _file.get_entry(ids, section, "VertexSelection");
        selectVertices(o_it->id(), ids);
        ids.clear();
        // Load edge selection:
        _file.get_entry(ids, section, "EdgeSelection");
        selectEdges(o_it->id(), ids);
        ids.clear();
        // Load half-edge selection:
        _file.get_entry(ids, section, "HalfEdgeSelection");
        selectHalfEdges(o_it->id(), ids);
        ids.clear();
        // Load face selection:
        _file.get_entry(ids, section, "FaceSelection");
        selectFaces(o_it->id(), ids);
        ids.clear();
        // Load half-face selection:
        _file.get_entry(ids, section, "HalfFaceSelection");
        selectHalfFaces(o_it->id(), ids);
        ids.clear();
        // Load cell selection:
        _file.get_entry(ids, section, "CellSelection");
        selectCells(o_it->id(), ids);
        ids.clear();
    }
}

//==============================================================================================

void VolumeMeshSelectionPlugin::slotSaveSelection(INIFile& _file) {

    // Iterate over all volumemesh objects in the scene and save
    // the selections for all vertices
    for(PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS,
            DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {

        // Create section for each object
        // Append object name to section identifier
        QString section = QString("PolyhedralMeshSelection") + "//" + o_it->name();

        // Store vertex selection:
        _file.add_entry(section, "VertexSelection",     getVertexSelection(o_it->id()));
        _file.add_entry(section, "EdgeSelection",       getEdgeSelection(o_it->id()));
        _file.add_entry(section, "HalfEdgeSelection",   getHalfEdgeSelection(o_it->id()));
        _file.add_entry(section, "FaceSelection",       getFaceSelection(o_it->id()));
        _file.add_entry(section, "HalfFaceSelection",   getHalfFaceSelection(o_it->id()));
        _file.add_entry(section, "CellSelection",       getCellSelection(o_it->id()));
    }
}

//==============================================================================================

void VolumeMeshSelectionPlugin::slotKeyShortcutEvent(int _key, Qt::KeyboardModifiers _modifiers) {

    SelectionInterface::PrimitiveType type = 0u;
    emit
    getActivePrimitiveType(type);

    if((type & allSupportedTypes_) == 0) {
        // No supported type is active
        return;
    }

    bool targetsOnly = false;
    emit
    targetObjectsOnly(targetsOnly);
    PluginFunctions::IteratorRestriction restriction = (targetsOnly ? PluginFunctions::TARGET_OBJECTS
                                                                    : PluginFunctions::ALL_OBJECTS);

    if(_key == Qt::Key_A && _modifiers == Qt::ControlModifier) {
        // Select all entities
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & vertexType_)
                    selectAllVertices(o_it->id());
                if(type & edgeType_)
                    selectAllEdges(o_it->id());
                if(type & faceType_)
                    selectAllFaces(o_it->id());
                if(type & cellType_)
                    selectAllCells(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
        }
    } else if(_key == Qt::Key_C && _modifiers == Qt::NoModifier) {
        // Deselect all entities
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & vertexType_)
                    deselectAllVertices(o_it->id());
                if(type & edgeType_)
                    deselectAllEdges(o_it->id());
                if(type & faceType_)
                    deselectAllFaces(o_it->id());
                if(type & cellType_)
                    deselectAllCells(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
        }
    } else if(_key == Qt::Key_I && _modifiers == Qt::NoModifier) {
        // Invert entity selection
        for (PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
            o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->visible()) {
                if(type & vertexType_)
                    invertVertexSelection(o_it->id());
                if(type & edgeType_)
                    invertEdgeSelection(o_it->id());
                if(type & faceType_)
                    invertFaceSelection(o_it->id());
                if(type & cellType_)
                    invertCellSelection(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_SELECTION);
        }

    } else if(_key == Qt::Key_Delete && _modifiers == Qt::NoModifier) {
        // Delete selected entities and its children
        for(PluginFunctions::ObjectIterator o_it(restriction, DataType(DATA_POLYHEDRAL_MESH | DATA_HEXAHEDRAL_MESH));
                o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if(o_it->visible()) {
                if(type & vertexType_)
                    deleteSelectedVertices(o_it->id());
                if(type & edgeType_)
                    deleteSelectedEdges(o_it->id());
                if(type & faceType_)
                    deleteSelectedFaces(o_it->id());
                if(type & cellType_)
                    deleteSelectedCells(o_it->id());
            }
            emit updatedObject(o_it->id(), UPDATE_ALL);
        }
    }
}

//==============================================================================================

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(volumemeshselectionplugin, VolumeMeshSelectionPlugin);
#endif


