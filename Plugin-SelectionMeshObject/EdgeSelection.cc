/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/

#include "MeshObjectSelectionPlugin.hh"

#include <iostream>

#include <MeshTools/MeshSelectionT.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//=========================================================
//==== Edge selections
//=========================================================

void MeshObjectSelectionPlugin::selectEdges( int objectId , IdList _edgeList ) {
    
    if(_edgeList.empty() ) return;
    
    BaseObjectData* object = 0;
    if ( ! PluginFunctions::getObject(objectId,object) ) {
        emit log(LOGERR,tr("selectEdges : unable to get object") );
        return;
    }

    if ( object->dataType() == DATA_TRIANGLE_MESH )
        MeshSelection::selectEdges(PluginFunctions::triMesh(object), _edgeList);
    else if ( object->dataType() == DATA_POLY_MESH )
        MeshSelection::selectEdges(PluginFunctions::polyMesh(object), _edgeList);
    else {
        emit log(LOGERR,tr("selectEdges : Unsupported object Type") );
        return;
    }

    QString selection = "selectEdges( ObjectId , [ " + QString::number(_edgeList[0]);

    for ( uint i = 1 ; i < _edgeList.size(); ++i) {
        selection +=  " , " + QString::number(_edgeList[i]);
    }

    selection += " ] )";

    emit updatedObject(object->id(), UPDATE_SELECTION_EDGES );
    emit scriptInfo( selection );
}

//=========================================================

void MeshObjectSelectionPlugin::unselectEdges( int objectId , IdList _edgeList ) {
    
    if(_edgeList.empty()) return;
    
    BaseObjectData* object;
    if ( ! PluginFunctions::getObject(objectId,object) ) {
        emit log(LOGERR,tr("unselectEdges : unable to get object") );
        return;
    }

    if ( object->dataType() == DATA_TRIANGLE_MESH )
        MeshSelection::unselectEdges(PluginFunctions::triMesh(object), _edgeList);
    else if ( object->dataType() == DATA_POLY_MESH )
        MeshSelection::unselectEdges(PluginFunctions::polyMesh(object), _edgeList);
    else {
        emit log(LOGERR,tr("unselectEdges : Unsupported object Type") );
        return;
    }

    QString selection = "unselectVertices( ObjectId , [ " + QString::number(_edgeList[0]);

    for ( uint i = 1 ; i < _edgeList.size(); ++i) {
        selection +=  " , " + QString::number(_edgeList[i]);
    }

    selection += " ] )";

    emit updatedObject(object->id(), UPDATE_SELECTION_EDGES);
    emit scriptInfo( selection );
}

//=========================================================

void MeshObjectSelectionPlugin::selectAllEdges( int objectId ) {
    
    BaseObjectData* object;
    if ( ! PluginFunctions::getObject(objectId,object) ) {
        emit log(LOGERR,tr("selectAllVertices : unable to get object") );
        return;
    }

    if ( object->dataType() == DATA_TRIANGLE_MESH )
        MeshSelection::selectAllEdges(PluginFunctions::triMesh(object));
    else if ( object->dataType() == DATA_POLY_MESH )
        MeshSelection::selectAllEdges(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("selectAllEdges : Unsupported object Type") );
        return;
    }

    emit updatedObject(object->id(), UPDATE_SELECTION_EDGES);
    emit scriptInfo( "selectAllEdges( ObjectId )" );
}

//=========================================================

void MeshObjectSelectionPlugin::clearEdgeSelection( int objectId ) {
    
    BaseObjectData* object;
    if ( ! PluginFunctions::getObject(objectId,object) ) {
        emit log(LOGERR,tr("clearEdgeSelection : unable to get object") );
        return;
    }

    if ( object->dataType() == DATA_TRIANGLE_MESH )
        MeshSelection::clearEdgeSelection(PluginFunctions::triMesh(object));
    else if ( object->dataType() == DATA_POLY_MESH )
        MeshSelection::clearEdgeSelection(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("clearEdgeSelection : Unsupported object Type") );
        return;
    }

    emit updatedObject(object->id(), UPDATE_SELECTION_EDGES);
    emit scriptInfo( "clearEdgeSelection( ObjectId )" );
}

//=========================================================

void MeshObjectSelectionPlugin::invertEdgeSelection( int objectId ) {
    
    BaseObjectData* object;
    if ( ! PluginFunctions::getObject(objectId,object) ) {
        emit log(LOGERR,tr("invertEdgeSelection : unable to get object") );
        return;
    }

    if ( object->dataType() == DATA_TRIANGLE_MESH )
        MeshSelection::invertEdgeSelection(PluginFunctions::triMesh(object));
    else if ( object->dataType() == DATA_POLY_MESH )
        MeshSelection::invertEdgeSelection(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("invertEdgeSelection : Unsupported object Type") );
        return;
    }

    emit updatedObject(object->id(), UPDATE_SELECTION_EDGES);
    emit scriptInfo( "invertEdgeSelection( ObjectId )" );
}

//=========================================================

void MeshObjectSelectionPlugin::selectBoundaryEdges( int objectId ) {
    
    BaseObjectData* object;
    if ( ! PluginFunctions::getObject(objectId,object) ) {
        emit log(LOGERR,tr("selectBoundaryEdges : unable to get object") );
        return;
    }

    if ( object->dataType() == DATA_TRIANGLE_MESH )
        MeshSelection::selectBoundaryEdges(PluginFunctions::triMesh(object));
    else if ( object->dataType() == DATA_POLY_MESH )
        MeshSelection::selectBoundaryEdges(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("selectBoundaryEdges : Unsupported object Type") );
        return;
    }

    emit updatedObject(object->id(), UPDATE_SELECTION_EDGES);
    emit scriptInfo( "selectBoundaryEdges( ObjectId )" );
}

//=========================================================

void MeshObjectSelectionPlugin::deleteEdgeSelection(int _objectId) {

    BaseObjectData* object = 0;
    if (!PluginFunctions::getObject(_objectId,object)) {
        emit log(LOGERR,tr("deleteEdgeSelection: unable to get object"));
        return;
    }

    if (object->dataType() == DATA_TRIANGLE_MESH)
        deleteSelection(PluginFunctions::triMesh(object), edgeType_);
    else if (object->dataType() == DATA_POLY_MESH)
        deleteSelection(PluginFunctions::polyMesh(object), edgeType_);
    else {
        emit log(LOGERR,tr("deleteEdgeSelection: Unsupported object Type"));
        return;
    }

    emit updatedObject(object->id(), UPDATE_ALL);
    emit scriptInfo("deleteEdgeSelection(ObjectId)");
}

//=========================================================

int MeshObjectSelectionPlugin::createMeshFromEdgeSelection( int _objectId) {
  return createMeshFromSelection(_objectId, edgeType_ );
}

//=========================================================

IdList MeshObjectSelectionPlugin::getEdgeSelection( int objectId ) {
    
    BaseObjectData* object;
    if ( ! PluginFunctions::getObject(objectId,object) ) {
        emit log(LOGERR,tr("getEdgeSelection : unable to get object") );
        return IdList(0);
    }

    emit scriptInfo( "getEdgeSelection( ObjectId )" );

    if ( object->dataType() == DATA_TRIANGLE_MESH )
        return MeshSelection::getEdgeSelection(PluginFunctions::triMesh(object));
    else if ( object->dataType() == DATA_POLY_MESH )
        return MeshSelection::getEdgeSelection(PluginFunctions::polyMesh(object));
    else {
        emit log(LOGERR,tr("getEdgeSelection : Unsupported object Type") );
        return IdList(0);
    }

    return IdList(0);

}

//=========================================================

IdList MeshObjectSelectionPlugin::convertEdgesToVertexPairs(int _id, const IdList& _edges) {

    IdList vertex_pairs;

    BaseObjectData* object = 0;
    if ( !PluginFunctions::getObject(_id,object) ) {
        emit log(LOGERR,tr("Cannot find object for id ") + QString::number(_id));
        return IdList(0);
    }

    if(object->dataType() == DATA_TRIANGLE_MESH) {

        TriMeshObject* obj = 0;
        if(!PluginFunctions::getObject(_id, obj)) {
            emit log(LOGERR, "Could not get mesh object!");
            return IdList(0);
        }

        TriMesh* mesh = obj->mesh();

        for(IdList::const_iterator it = _edges.begin(); it != _edges.end(); ++it) {
            vertex_pairs.push_back(mesh->from_vertex_handle(mesh->halfedge_handle(TriMesh::EdgeHandle(*it), 0)).idx());
            vertex_pairs.push_back(mesh->to_vertex_handle(mesh->halfedge_handle(TriMesh::EdgeHandle(*it), 0)).idx());
        }

    } else if(object->dataType() == DATA_POLY_MESH) {

        PolyMeshObject* obj = 0;
        if(!PluginFunctions::getObject(_id, obj)) {
            emit log(LOGERR, "Could not get mesh object!");
            return IdList(0);
        }

        PolyMesh* mesh = obj->mesh();

        for(IdList::const_iterator it = _edges.begin(); it != _edges.end(); ++it) {
            vertex_pairs.push_back(mesh->from_vertex_handle(mesh->halfedge_handle(TriMesh::EdgeHandle(*it), 0)).idx());
            vertex_pairs.push_back(mesh->to_vertex_handle(mesh->halfedge_handle(TriMesh::EdgeHandle(*it), 0)).idx());
        }
    }

    return vertex_pairs;
}

//=========================================================

IdList MeshObjectSelectionPlugin::convertVertexPairsToEdges(int _id, const IdList& _vertices) {

    if(_vertices.size() % 2 != 0) {
        emit log(LOGERR, "Number of vertices is not even!");
        return IdList(0);
    }

    IdList edges;

    BaseObjectData* object = 0;
    if ( !PluginFunctions::getObject(_id,object) ) {
        emit log(LOGERR,tr("Cannot find object for id ") + QString::number(_id));
        return IdList(0);
    }

    if(object->dataType() == DATA_TRIANGLE_MESH) {

        TriMeshObject* obj = 0;
        if(!PluginFunctions::getObject(_id, obj)) {
            emit log(LOGERR, "Could not get mesh object!");
            return IdList(0);
        }

        TriMesh* mesh = obj->mesh();

        for(IdList::const_iterator it = _vertices.begin(); it != _vertices.end(); it+=2) {
            TriMesh::VertexHandle vh = TriMesh::VertexHandle(*it);
            if(!vh.is_valid()) continue;
            for(TriMesh::VertexOHalfedgeIter voh_it = mesh->voh_iter(vh);
                    voh_it; ++voh_it) {
                if(mesh->to_vertex_handle(voh_it.handle()).idx() == *(it+1)) {
                    edges.push_back(mesh->edge_handle(voh_it.handle()).idx());
                    continue;
                }
            }
        }

    } else if(object->dataType() == DATA_POLY_MESH) {

        PolyMeshObject* obj = 0;
        if(!PluginFunctions::getObject(_id, obj)) {
            emit log(LOGERR, "Could not get mesh object!");
            return IdList(0);
        }

        PolyMesh* mesh = obj->mesh();

        for(IdList::const_iterator it = _vertices.begin(); it != _vertices.end(); it+=2) {
            PolyMesh::VertexHandle vh = PolyMesh::VertexHandle(*it);
            if(!vh.is_valid()) continue;
            for(PolyMesh::VertexOHalfedgeIter voh_it = mesh->voh_iter(vh);
                    voh_it; ++voh_it) {
                if(mesh->to_vertex_handle(voh_it.handle()).idx() == *(it+1)) {
                    edges.push_back(mesh->edge_handle(voh_it.handle()).idx());
                    continue;
                }
            }
        }
    }

    return edges;
}

//=========================================================

void MeshObjectSelectionPlugin::colorizeEdgeSelection(int objectId, int r, int g, int b, int a ) {
    
    BaseObjectData* object;
    if ( ! PluginFunctions::getObject(objectId,object) ) {
        emit log(LOGERR,"colorizeEdgeSelection : unable to get object" );
        return;
    }

    if ( object->dataType() == DATA_TRIANGLE_MESH ) {
        colorizeSelection(PluginFunctions::triMesh(object), edgeType_, r, g, b, a);
    } else if ( object->dataType() == DATA_POLY_MESH ) {
        colorizeSelection(PluginFunctions::polyMesh(object), edgeType_, r, g, b, a);
    } else {
        emit log(LOGERR,"colorizeEdgeSelection : Unsupported object Type" );
        return;
    }

    emit scriptInfo( "colorizeEdgeSelection( ObjectId, "
                     + QString::number(r) + ", " + QString::number(g) + ", " + QString::number(b) + " )" );

    emit updatedObject(object->id(), UPDATE_COLOR);
}
