#include "PolyLinePlugin.hh"

//------------------------------------------------------------------------------

/** \brief Generates a polyLine of a plane intersection
*
*
* @param _objectId id of the target object
* @param _planePoint a point on the cut plane
* @param _planeNormal the normal of the cut plane
* @param _polyLineId Specify this if you don't want a new polyline to be generated but to reuse an existing one
* @return returns the id of the polyLine
*/
int PolyLinePlugin::generatePolyLineFromCut( int _objectId, Vector _planePoint, Vector _planeNormal, int _polyLineId ) {
    
    // get object
    BaseObjectData *obj;
    PluginFunctions::getObject(_objectId, obj);
    
    if (obj == 0){
        emit log(LOGERR,tr("Unable to get object"));
        return -1;
    }
    
    //get the intersection points
    
    std::vector< ACG::Vec3d > linePoints;
    bool closed = false;
    if ( obj->dataType(DATA_TRIANGLE_MESH) ) {
        
        TriMesh* mesh = PluginFunctions::triMesh(obj);
        
        if ( mesh == 0 ) {
            emit log(LOGERR,tr("Unable to get mesh"));
            return -1;
        }
        
        //get an edge of the mesh that is cut by the plane
        TriMesh::EdgeHandle eh = getCuttedEdge( *mesh, _planeNormal, _planePoint );
        
        if ( !eh.is_valid() ) {
            emit log(LOGERR,tr("Unable to find initial cut edge."));
            return -1;
        }
        
        TriMesh::FaceHandle fh = mesh->face_handle( mesh->halfedge_handle(eh, 0) );
        
        if (!fh.is_valid())
            fh = mesh->face_handle( mesh->halfedge_handle(eh, 1) );
        
        // get all intersection points
        linePoints = getIntersectionPoints( mesh, fh.idx() , _planeNormal , _planePoint, closed);
        
    } else {
        
        PolyMesh* mesh = PluginFunctions::polyMesh(obj);
        
        if ( mesh == 0 ) {
            emit log(LOGERR,tr("Unable to get mesh"));
            return -1;
        }
        
        //get a edge of the mesh that is cut by the plane
        PolyMesh::EdgeHandle eh = getCuttedEdge( *mesh, _planeNormal, _planePoint );
        
        if ( !eh.is_valid() ) {
            emit log(LOGERR,tr("Unable to find initial cut edge."));
            return -1;
        }
        
        PolyMesh::FaceHandle fh = mesh->face_handle( mesh->halfedge_handle(eh, 0) );
        
        if (!fh.is_valid())
            fh = mesh->face_handle( mesh->halfedge_handle(eh, 1) );
        
        // get all intersection points
        linePoints = getIntersectionPoints( mesh, fh.idx() , _planeNormal , _planePoint, closed);
    }
    
    if ( linePoints.empty() )  {
        emit log(LOGERR,tr("No points from cut found."));
        return -1;
    }
    
    //generate a polyLine from the intersection Points
    int polyLineId = -1;
    
    // add new polyline
    if (_polyLineId == -1)
      emit addEmptyObject(DATA_POLY_LINE,polyLineId);
    else
      polyLineId = _polyLineId;
    
    // get current polylineobject
    BaseObjectData *polyLineObj;
    PluginFunctions::getObject(polyLineId, polyLineObj);
    
    // get polyline object
    PolyLineObject* currentPolyLine = PluginFunctions::polyLineObject(polyLineObj);
    
    currentPolyLine->line()->clear();
    
    for ( uint i = 0 ; i < linePoints.size(); ++i )
        currentPolyLine->line()->add_point( (PolyLine::Point) linePoints[i] );
    
    currentPolyLine->line()->set_closed(closed);
    currentPolyLine->target(true);

    return polyLineId;
}

