#include "VolumeMeshSelectionPlugin.hh"

//void VolumeMeshSelectionPlugin::selectAllVertices(int _objectId) {
//
//    BaseObjectData* object = 0;
//
//    if(!PluginFunctions::getObject(_objectId, object)) {
//        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//        return;
//    }
//
//    PolyhedralMesh* volumemesh = PluginFunctions::polyhedralMesh(object);
//    if(!volumemesh) volumemesh = dynamic_cast<PolyhedralMesh*>(PluginFunctions::hexahedralMesh(object));
//
//    if(volumemesh) {
//
//        for(unsigned int i = 0; i < volumemesh->n_vertices(); ++i) {
//            volumemesh->select_vertex(i);
//        }
//    }
//
//    emit scriptInfo("selectAllVertices(ObjectId)");
//}
//
//void VolumeMeshSelectionPlugin::deselectAllVertices(int _objectId) {
//
//    BaseObjectData* object = 0;
//
//    if(!PluginFunctions::getObject(_objectId, object)) {
//        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//        return;
//    }
//
//    PolyhedralMesh* volumemesh = PluginFunctions::polyhedralMesh(object);
//    if(!volumemesh) volumemesh = dynamic_cast<PolyhedralMesh*>(PluginFunctions::hexahedralMesh(object));
//
//    if(volumemesh) {
//
//        for(unsigned int i = 0; i < volumemesh->n_vertices(); ++i) {
//            volumemesh->deselect_vertex(i);
//        }
//    }
//
//    emit scriptInfo("deselectAllVertices(ObjectId)");
//}
//
//void VolumeMeshSelectionPlugin::invertVertexSelection(int _objectId) {
//
//    BaseObjectData* object = 0;
//
//    if(!PluginFunctions::getObject(_objectId, object)) {
//        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//        return;
//    }
//
//    PolyhedralMesh* volumemesh = PluginFunctions::polyhedralMesh(object);
//    if(!volumemesh) volumemesh = dynamic_cast<PolyhedralMesh*>(PluginFunctions::hexahedralMesh(object));
//
//    if(volumemesh) {
//
//        for(unsigned int i = 0; i < volumemesh->n_vertices(); ++i) {
//
//            if(volumemesh->vertex_selected(i))
//                volumemesh->deselect_vertex(i);
//            else
//                volumemesh->select_vertex(i);
//        }
//    }
//
//    emit scriptInfo("invertVertexSelection(ObjectId)");
//}
//
//void VolumeMeshSelectionPlugin::selectVertices(int _objectId, const IdList _ids, bool _deselect) {
//
//    if(_ids.size() == 0) return;
//
//    BaseObjectData* object = 0;
//
//    if(!PluginFunctions::getObject(_objectId, object)) {
//        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//        return;
//    }
//
//    PolyhedralMesh* volumemesh = PluginFunctions::polyhedralMesh(object);
//    if(!volumemesh) volumemesh = dynamic_cast<PolyhedralMesh*>(PluginFunctions::hexahedralMesh(object));
//
//    if(volumemesh) {
//        for(IdList::const_iterator it = _ids.begin(); it != _ids.end(); ++it) {
//            if(*it < (int)volumemesh->n_vertices()) {
//                if(_deselect) volumemesh->deselect_vertex(*it);
//                else          volumemesh->select_vertex(*it);
//            }
//        }
//    }
//
//    QString selection = "selectVertices(ObjectId, [ " + QString::number(_ids[0]);
//
//    for (uint i = 1 ; i < _ids.size(); ++i) {
//        selection +=  ", " + QString::number(_ids[i]);
//    }
//
//    selection += " ])";
//
//    emit scriptInfo(selection);
//}
//
//IdList VolumeMeshSelectionPlugin::getVertexSelection(int _objectId) {
//
//    BaseObjectData* object = 0;
//
//    IdList list;
//
//    if(!PluginFunctions::getObject(_objectId, object)) {
//        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//        return list;
//    }
//
//    PolyhedralMesh* volumemesh = PluginFunctions::polyhedralMesh(object);
//    if(!volumemesh) volumemesh = dynamic_cast<PolyhedralMesh*>(PluginFunctions::hexahedralMesh(object));
//
//    if(volumemesh) {
//
//        for(unsigned int i = 0; i < volumemesh->n_vertices(); ++i) {
//            if(volumemesh->vertex_selected(i))
//                list.push_back(i);
//        }
//    }
//
//    return list;
//}
//
////=======================================================================================
//
//void VolumeMeshSelectionPlugin::selectAllEdges(int _objectId) {
//
//    BaseObjectData* object = 0;
//
//    if (!PluginFunctions::getObject(_objectId, object)) {
//        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//        return;
//    }
//
//    PolyhedralMesh* volumemesh = PluginFunctions::polyhedralMesh(object);
//    if(!volumemesh) volumemesh = dynamic_cast<PolyhedralMesh*>(PluginFunctions::hexahedralMesh(object));
//
//    if (volumemesh) {
//
//        for(unsigned int i = 0; i < volumemesh->n_edges(); ++i) {
//            volumemesh->select_edge(i);
//        }
//    }
//
//    emit scriptInfo("selectAllEdges(ObjectId)");
//}
//
//void VolumeMeshSelectionPlugin::deselectAllEdges(int _objectId) {
//
//    BaseObjectData* object = 0;
//
//    if (!PluginFunctions::getObject(_objectId, object)) {
//        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//        return;
//    }
//
//    PolyhedralMesh* volumemesh = PluginFunctions::polyhedralMesh(object);
//    if(!volumemesh) volumemesh = dynamic_cast<PolyhedralMesh*>(PluginFunctions::hexahedralMesh(object));
//
//    if (volumemesh) {
//
//        for(unsigned int i = 0; i < volumemesh->n_edges(); ++i) {
//            volumemesh->deselect_edge(i);
//        }
//    }
//
//    emit scriptInfo("deselectAllEdges(ObjectId)");
//}
//
//void VolumeMeshSelectionPlugin::invertEdgeSelection(int _objectId) {
//
//    BaseObjectData* object = 0;
//
//    if (!PluginFunctions::getObject(_objectId, object)) {
//        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//        return;
//    }
//
//    PolyhedralMesh* volumemesh = PluginFunctions::polyhedralMesh(object);
//    if(!volumemesh) volumemesh = dynamic_cast<PolyhedralMesh*>(PluginFunctions::hexahedralMesh(object));
//
//    if (volumemesh) {
//
//        for(unsigned int i = 0; i < volumemesh->n_edges(); ++i) {
//            if(volumemesh->edge_selected(i))
//                volumemesh->deselect_edge(i);
//            else
//                volumemesh->select_edge(i);
//        }
//    }
//
//    emit scriptInfo("deselectAllEdges(ObjectId)");
//}
//
//void VolumeMeshSelectionPlugin::selectEdges(int _objectId, const IdList _ids, bool _deselect) {
//
//    if(_ids.size() == 0) return;
//
//    BaseObjectData* object = 0;
//
//    if (!PluginFunctions::getObject(_objectId, object)) {
//        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//        return;
//    }
//
//    PolyhedralMesh* volumemesh = PluginFunctions::polyhedralMesh(object);
//    if (volumemesh) {
//        for (IdList::const_iterator it = _ids.begin(); it != _ids.end(); ++it) {
//
//            if(*it < (int)volumemesh->n_edges()) {
//                if(_deselect) volumemesh->deselect_edge(*it);
//                else          volumemesh->select_edge(*it);
//            }
//        }
//    }
//
//    QString selection = "selectEdges(ObjectId, [ " + QString::number(_ids[0]);
//
//    for (uint i = 1 ; i < _ids.size(); ++i) {
//        selection +=  ", " + QString::number(_ids[i]);
//    }
//
//    selection += " ])";
//
//    emit scriptInfo(selection);
//}
//
//IdList VolumeMeshSelectionPlugin::getEdgeSelection(int _objectId) {
//
//    BaseObjectData* object = 0;
//
//    IdList list;
//
//    if(!PluginFunctions::getObject(_objectId, object)) {
//        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
//        return list;
//    }
//
//    PolyhedralMesh* volumemesh = PluginFunctions::polyhedralMesh(object);
//    if(volumemesh) {
//
//        for(unsigned int i = 0; i < volumemesh->n_edges(); ++i) {
//            if(volumemesh->edge_selected(i))
//                list.push_back(i);
//        }
//    }
//
//    return list;
//}

void VolumeMeshSelectionPlugin::deleteSelectedCells(int _objectId) {

    BaseObjectData* object = 0;

    if(!PluginFunctions::getObject(_objectId, object)) {
        emit log(LOGERR, tr("Could not get object with id %1").arg(_objectId));
        return;
    }

    PolyhedralMeshObject* polyMeshObject = PluginFunctions::polyhedralMeshObject(object);
    if(!polyMeshObject) {
        emit log(LOGERR, tr("Could not cast object to VolumeMeshObject"));
        return;
    }

    PolyhedralMesh* volumemesh = PluginFunctions::polyhedralMesh(object);
    if(volumemesh) {

        for(OpenVolumeMesh::CellIter c_it = volumemesh->cells_begin();
                c_it != volumemesh->cells_end(); ++c_it) {

            if(polyMeshObject->status()[*c_it].selected()) {
                polyMeshObject->status()[*c_it].set_deleted(true);
            }
        }

        polyMeshObject->status().garbage_collection();

        return;
    }

    HexahedralMeshObject* hexMeshObject = PluginFunctions::hexahedralMeshObject(object);

    HexahedralMesh* hexmesh = PluginFunctions::hexahedralMesh(object);
    if(hexmesh) {

        for(OpenVolumeMesh::CellIter c_it = hexmesh->cells_begin();
                c_it != hexmesh->cells_end(); ++c_it) {

            if(hexMeshObject->status()[*c_it].selected()) {
                hexMeshObject->status()[*c_it].set_deleted(true);
            }
        }

        hexMeshObject->status().garbage_collection();

        return;
    }
}
