/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright(C)2001-2010 by Computer Graphics Group, RWTH Aachen        *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or(at your option)any later version with the                *
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
 *   $Revision$                                                      *
 *   $Author$                                                       *
 *   $Date: 2010-09-24 14:05:45 +0200(Fri, 24 Sep 2010)$                     *
 *                                                                           *
\*===========================================================================*/

#include "MeshObjectSelectionPlugin.hh"

#include <MeshTools/MeshNavigationT.hh>
#include <OpenMesh/Core/Geometry/MathDefs.hh>

#include <set>

/** \brief Delete all selected elements of a mesh
 *
 * @param _mesh a mesh
 * @param _primitiveType bitmask of active primitive types
 * @return returns true if the mesh was changed(i.e there was something selected that was deleted)
 */
template<typename MeshT>
bool MeshObjectSelectionPlugin::deleteSelection(MeshT* _mesh, PrimitiveType _primitiveType) {
  
    bool changed = false;

    if(_primitiveType & vertexType_) {
        typename MeshT::VertexIter v_it, v_end=_mesh->vertices_end();

        for(v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it) {
            if(_mesh->status(*v_it).selected()) {
                _mesh->delete_vertex(*v_it);
                changed = true;
            }
        }
    }

    if(_primitiveType & edgeType_) {
        typename MeshT::EdgeIter e_it, e_end(_mesh->edges_end());

        for(e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it) {
            if(_mesh->status(*e_it).selected()) {
                _mesh->delete_edge(*e_it);
                changed = true;
            }
        }
    }

    if(_primitiveType & faceType_) {
        typename MeshT::FaceIter f_it, f_end(_mesh->faces_end());

        for(f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it) {
            if(_mesh->status(*f_it).selected()) {
                _mesh->delete_face(*f_it);
                changed = true;
            }
        }
    }

    _mesh->garbage_collection();

    return changed;
}

//***********************************************************************************

/** \brief Update face selection to correspond to the vertex selection
 *
 * @param _mesh a mesh
 */
template<typename MeshType>
void MeshObjectSelectionPlugin::update_regions(MeshType* _mesh) {

    // Set face status
    typename MeshType::FaceIter                 f_it, f_end=_mesh->faces_end();
    typename MeshType::FaceVertexIter           fv_it;
    typename MeshType::VertexHandle             v0, v1, v2;

    for(f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it) {
        fv_it = _mesh->fv_iter(*f_it);
        v0  = *(  fv_it);
        v1  = *(++fv_it);
        v2  = *(++fv_it);

        const bool a =(_mesh->status(v0).is_bit_set(AREA)|| _mesh->status(v1).is_bit_set(AREA)|| _mesh->status(v2).is_bit_set(AREA));
        bool h =(_mesh->status(v0).is_bit_set(HANDLEAREA)&&
                 _mesh->status(v1).is_bit_set(HANDLEAREA)&&
                 _mesh->status(v2).is_bit_set(HANDLEAREA));

        if(!(a || h))
            if(_mesh->status(v0).is_bit_set(HANDLEAREA)||
                    _mesh->status(v1).is_bit_set(HANDLEAREA)||
                    _mesh->status(v2).is_bit_set(HANDLEAREA))
                h = true;

        _mesh->status(*f_it).change_bit(AREA    ,   a);
        _mesh->status(*f_it).change_bit(HANDLEAREA, h);
    }
}

//***********************************************************************************

/** \brief Toggle the selection state of mesh primitives
 *
 * @param _objectId id of the object that encapsulates the mesh
 * @param _mesh a mesh
 * @param _fh handle of the face that was picked
 * @param _hit_point point that was picked
 * @param _primitiveType primitive types to be selected
 */
template<class MeshT>
void MeshObjectSelectionPlugin::toggleMeshSelection(int _objectId, MeshT* _mesh, uint _fh, ACG::Vec3d& _hit_point, PrimitiveType _primitiveType) {

    typename MeshT::FaceHandle fh = _mesh->face_handle(_fh);

    if(!fh.is_valid())
        return;

    //Vertex Selection
    if(_primitiveType & vertexType_) {

        typename MeshT::FaceVertexIter fv_it(*_mesh, fh);
        typename MeshT::VertexHandle closest = *fv_it;
        typename MeshT::Scalar shortest_distance =(_mesh->point(closest)- _hit_point).sqrnorm();

        do {
            if((_mesh->point(*fv_it)- _hit_point).sqrnorm()<shortest_distance) {
                shortest_distance =(_mesh->point(*fv_it)- _hit_point).sqrnorm();
                closest = *fv_it;
            }

            ++fv_it;

        } while( fv_it.is_valid() );

        _mesh->status(closest).set_selected(!_mesh->status(closest).selected());

        if(_mesh->status(closest).selected())
            emit scriptInfo("selectVertices(ObjectId(" + QString::number(_objectId) + ") , [" + QString::number(closest.idx())+ "])");
        else
            emit scriptInfo("unselectVertices(ObjectId(" + QString::number(_objectId) + ") , [" + QString::number(closest.idx())+ "])");
        emit updatedObject(_objectId, UPDATE_SELECTION_VERTICES);
    }

    //Edge Selection
    if( (_primitiveType & edgeType_) || (_primitiveType & halfedgeType_) ) {

        typename MeshT::FaceHalfedgeIter fhe_it(*_mesh, fh);

        typename MeshT::HalfedgeHandle closest(-1);
        typename MeshT::Scalar         closest_dist(-1);

        typename MeshT::Point pp =(typename MeshT::Point)_hit_point;

        for(; fhe_it.is_valid(); ++fhe_it) {

            // typename MeshT::HalfedgeHandle heh0 = _mesh->halfedge_handle(*fe_it, 0);
            // typename MeshT::HalfedgeHandle heh1 = _mesh->halfedge_handle(*fe_it, 1);

            typename MeshT::Point lp0 = _mesh->point(_mesh->to_vertex_handle (*fhe_it));
            typename MeshT::Point lp1 = _mesh->point(_mesh->from_vertex_handle(*fhe_it));

            double dist_new = ACG::Geometry::distPointLineSquared(pp, lp0, lp1);

            if(dist_new <closest_dist || closest_dist == -1) {

                // save closest Edge
                closest_dist = dist_new;
                closest = *fhe_it;
            }
        }

        typename MeshT::EdgeHandle closest_eh = _mesh->edge_handle(closest);

        if(_primitiveType & edgeType_) {

            _mesh->status(closest_eh).set_selected(!_mesh->status(closest_eh).selected());

            if(_mesh->status(closest_eh).selected())
                emit scriptInfo("selectEdges(ObjectId(" + QString::number(_objectId) + ") , [" + QString::number(closest_eh.idx())+ "])");
            else
                emit scriptInfo("unselectEdges(ObjectId(" + QString::number(_objectId) + ") , [" + QString::number(closest_eh.idx())+ "])");
            emit updatedObject(_objectId, UPDATE_SELECTION_EDGES);
        } else {
            _mesh->status(closest).set_selected(!_mesh->status(closest).selected());

            if(_mesh->status(closest).selected())
                emit scriptInfo("selectHalfedges(ObjectId(" + QString::number(_objectId) + ") , [" + QString::number(closest.idx())+ "])");
            else
                emit scriptInfo("unselectHalfedges(ObjectId(" + QString::number(_objectId) + ") , [" + QString::number(closest.idx())+ "])");
            emit updatedObject(_objectId, UPDATE_SELECTION_HALFEDGES);
        }
    }

    //Face Selection
    if(_primitiveType & faceType_) {
        _mesh->status(fh).set_selected(!_mesh->status(fh).selected());

        if(_mesh->status(fh).selected())
            emit scriptInfo("selectFaces(ObjectId(" + QString::number(_objectId) + ") , [" + QString::number(fh.idx())+ "])");
        else
            emit scriptInfo("unselectFaces(ObjectId(" + QString::number(_objectId) + ") , [" + QString::number(fh.idx())+ "])");
        emit updatedObject(_objectId, UPDATE_SELECTION_FACES);
    }
}

//***********************************************************************************

/** \brief paint selection with a sphere
 *
 * @param _mesh          a mesh
 * @param _target_idx    handle of the face that was hit by the mouse picking
 * @param _hitpoint      point where the mouse picking hit
 * @param _radius        Radius of the selection sphere
 * @param _primitiveType primitive types to be selected
 * @param _deselection   true, if entities should be deselected
 */
template <class MeshT>
void MeshObjectSelectionPlugin::paintSphereSelection(MeshT*                _mesh ,
                                                     int                   _objectId ,
                                                     int                   _target_idx ,
                                                     typename MeshT::Point _hitpoint,
                                                     double                _radius,
                                                     PrimitiveType         _primitiveType,
                                                     bool                  _deselection) {

    float sqr_radius = _radius * _radius;
    typename MeshT::FaceHandle hitface = _mesh->face_handle(_target_idx);

    if(!hitface.is_valid())
        return;
    
    // initialize everything

    if(_primitiveType & vertexType_) {
        //reset tagged status
        typename MeshT::VertexIter v_it, v_end(_mesh->vertices_end());
        for(v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
            _mesh->status(*v_it).set_tagged(false);

    }

    if( (_primitiveType & edgeType_) || ( _primitiveType & halfedgeType_ )) {
        //reset tagged status
        typename MeshT::EdgeIter e_it, e_end(_mesh->edges_end());
        for(e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it)
            _mesh->status(*e_it).set_tagged(false);

    }

    // reset tagged status
    typename MeshT::FaceIter f_it, f_end(_mesh->faces_end());
    for(f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
        _mesh->status(*f_it).set_tagged(false);

    _mesh->status(hitface).set_tagged(true);

    //add an additional property
    OpenMesh::FPropHandleT<bool> checkedProp;
    if(!_mesh->get_property_handle(checkedProp,"checkedProp"))
        _mesh->add_property(checkedProp, "checkedProp");

    for(f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
        _mesh->property(checkedProp, *f_it)= false;


    std::vector<typename MeshT::FaceHandle>   face_handles;
    face_handles.reserve(50);
    face_handles.push_back(hitface);


    // find primitives to be selected

    while(!face_handles.empty()) {
        typename MeshT::FaceHandle fh = face_handles.back();
        face_handles.pop_back();

        for(typename MeshT::FaceFaceIter ff_it(*_mesh,fh); ff_it.is_valid() ; ++ff_it) {

            // Check if already tagged
            if(_mesh->status(*ff_it).tagged()|| _mesh->property(checkedProp, *ff_it))
                continue;

            // Check which points of the new face lie inside the sphere
            uint fVertices = 0;

            std::vector<typename MeshT::VertexHandle>   vertex_handles;
            std::vector<typename MeshT::EdgeHandle>     edge_handles;

            for(typename MeshT::FaceHalfedgeIter fh_it(*_mesh,*ff_it); fh_it.is_valid(); ++fh_it) {

                typename MeshT::VertexHandle vh = _mesh->from_vertex_handle(*fh_it);

                if((_mesh->point(vh)- _hitpoint).sqrnorm()<= sqr_radius) {
                    vertex_handles.push_back(vh);

                    if((_mesh->point(_mesh->to_vertex_handle(*fh_it))-  _hitpoint).sqrnorm()<= sqr_radius)
                        edge_handles.push_back(_mesh->edge_handle(*fh_it));

                }

                fVertices++;
            }

            //check what has to be tagged
            bool tagged = false;

            if(_primitiveType & vertexType_) {
                for(uint i=0; i <vertex_handles.size(); i++)
                    _mesh->status(vertex_handles[i]).set_tagged(true);

                if( ! vertex_handles.empty() )
                  tagged = true;
            }
            if( (_primitiveType & edgeType_) || (_primitiveType & halfedgeType_)) {
                for(uint i=0; i <edge_handles.size(); i++)
                    _mesh->status(edge_handles[i]).set_tagged(true);

                if( !edge_handles.empty() )
                  tagged = true;
            }
            if(_primitiveType & faceType_) {
                if(vertex_handles.size()== fVertices) {

                    _mesh->status(*ff_it).set_tagged(true);
                    tagged = true;
                }
            }

            //if something was tagged also check the 1-ring
            if(tagged) {
                _mesh->property(checkedProp, *ff_it)= true;
                face_handles.push_back(*ff_it);
            }
        }
    }

    // select all tagged primitives
    bool sel = !_deselection;

    if(_primitiveType & vertexType_) {
        typename MeshT::VertexIter v_it, v_end(_mesh->vertices_end());
        for(v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
            if(_mesh->status(*v_it).tagged())
                _mesh->status(*v_it).set_selected(sel);
        emit updatedObject(_objectId, UPDATE_SELECTION_VERTICES);
    }
    if(_primitiveType & edgeType_) {
        typename MeshT::EdgeIter e_it, e_end(_mesh->edges_end());

        for(e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it)
            if(_mesh->status(*e_it).tagged())
                _mesh->status(*e_it).set_selected(sel);
        emit updatedObject(_objectId, UPDATE_SELECTION_EDGES);
    }
    if(_primitiveType & halfedgeType_) {
        typename MeshT::HalfedgeIter he_it, he_end(_mesh->halfedges_end());

        for(he_it=_mesh->halfedges_begin(); he_it!=he_end; ++he_it)
            if(_mesh->status(_mesh->edge_handle(*he_it)).tagged())
                _mesh->status(*he_it).set_selected(sel);
        emit updatedObject(_objectId, UPDATE_SELECTION_HALFEDGES);
    }
    if(_primitiveType & faceType_) {
        typename MeshT::FaceIter f_it, f_end(_mesh->faces_end());

        for(f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
            if(_mesh->status(*f_it).tagged())
                _mesh->status(*f_it).set_selected(sel);
        emit updatedObject(_objectId, UPDATE_SELECTION_FACES);
    }

    _mesh->remove_property(checkedProp);
}

//***********************************************************************************

/** \brief Select all primitves that are projected to the given region
 *
 * @param _mesh a mesh
 * @param _state current gl state
 * @param _region region
 * @param _primitiveType primitive types to be selected
 * @param _deselection true, if entities should be deselected
 * @return true, if something was selected
 */
template<class MeshT>
bool MeshObjectSelectionPlugin::volumeSelection(MeshT* _mesh, int _objectId, ACG::GLState& _state, QRegion *_region,
                                                PrimitiveType _primitiveType, bool _deselection) {
    ACG::Vec3d proj;
    bool rv = false;

    //reset tagged status
    typename MeshT::VertexIter v_it, v_end(_mesh->vertices_end());
    for(v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
        _mesh->status(*v_it).set_tagged(false);

    //tag all vertices that are projected into region
    for(v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it) {

        proj = _state.project(_mesh->point(*v_it));

        if(_region->contains(QPoint((int)proj[0], _state.context_height()- (int)proj[1]))) {
            
            _mesh->status(*v_it).set_tagged(true);
            rv = true;
            if(_primitiveType & vertexType_) {
                _mesh->status(*v_it).set_selected(!_deselection);
            }
        }
    }
    
    if( (_primitiveType & edgeType_) || (_primitiveType & halfedgeType_) ) {
        typename MeshT::EdgeIter e_it, e_end(_mesh->edges_end());
        for(e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it) {
            
            if(_mesh->status(_mesh->to_vertex_handle(_mesh->halfedge_handle(*e_it, 0))).tagged()||
                    _mesh->status(_mesh->to_vertex_handle(_mesh->halfedge_handle(*e_it, 1))).tagged()) {
                
                if(_primitiveType & edgeType_)
                    _mesh->status(*e_it).set_selected(!_deselection);

                if(_primitiveType & halfedgeType_) {
                    
                    _mesh->status(_mesh->halfedge_handle(*e_it,0)).set_selected(!_deselection);
                    _mesh->status(_mesh->halfedge_handle(*e_it,1)).set_selected(!_deselection);
                }
            }
        }
    }
    
    if(_primitiveType & faceType_) {
        typename MeshT::FaceIter f_it, f_end(_mesh->faces_end());
        for(f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it) {
            
            bool select = false;
            for(typename MeshT::FaceVertexIter fv_it(*_mesh,*f_it); fv_it.is_valid(); ++fv_it)
                if(_mesh->status(*fv_it).tagged())
                    select = true;

            if(select)
                _mesh->status(*f_it).set_selected(!_deselection);
        }
    }

    if(_primitiveType & vertexType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_VERTICES);
    if(_primitiveType & edgeType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_EDGES);
    if(_primitiveType & halfedgeType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_HALFEDGES);
    if(_primitiveType & faceType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_FACES);

    return rv;
}

//***********************************************************************************

/** \brief select primitives of the closest boundary
 *
 * @param _mesh a mesh
 * @param _vh handle of the vertex that was picked
 * @param _primitiveTypes the currently active primitive types
 * @param _deselection true if primitives should be deselected
 */
template<class MeshT>
void MeshObjectSelectionPlugin::closestBoundarySelection(MeshT* _mesh, int _vh, PrimitiveType _primitiveTypes, bool _deselection) {
    
    typename MeshT::VertexHandle vh = _mesh->vertex_handle(_vh);

    if(vh.is_valid()) {
        //get boundary vertex
        typename MeshT::VertexHandle vhBound = MeshNavigation::findClosestBoundary(_mesh , vh);
        if(vhBound.is_valid()) {

            //walk around the boundary and select primitves
            OpenMesh::VPropHandleT<bool> visited;
            if(!_mesh->get_property_handle(visited,"Visited Vertices"))
                _mesh->add_property(visited, "Visited Vertices");

            typename MeshT::VertexIter v_it, v_end = _mesh->vertices_end();
            for(v_it = _mesh->vertices_begin(); v_it != v_end; ++v_it)
                _mesh->property(visited, *v_it)= false;

            std::stack<typename MeshT::VertexHandle> stack;
            stack.push(vhBound);

            while(!stack.empty()) {

                typename MeshT::VertexHandle vh = stack.top();
                stack.pop();

                if(_mesh->property(visited,vh))
                    continue;

                //find outgoing boundary-edges
                for(typename MeshT::VertexOHalfedgeIter voh_it(*_mesh,vh); voh_it.is_valid(); ++voh_it) {
                    
                    if(_mesh->is_boundary(_mesh->edge_handle(*voh_it))) {
                        
                        stack.push(_mesh->to_vertex_handle(*voh_it));

                        if(_primitiveTypes & edgeType_)
                            _mesh->status(_mesh->edge_handle(*voh_it)).set_selected(!_deselection);

                        if(_primitiveTypes & halfedgeType_) {
                            typename MeshT::HalfedgeHandle heh = *voh_it;
                            if(!_mesh->is_boundary(heh))heh = _mesh->opposite_halfedge_handle(heh);
                            _mesh->status(heh).set_selected(!_deselection);
                        }
                    }

                    if(_primitiveTypes & faceType_) {
                        typename MeshT::FaceHandle f1 = _mesh->face_handle(*voh_it);
                        typename MeshT::FaceHandle f2 = _mesh->face_handle(_mesh->opposite_halfedge_handle(*voh_it));
                        if(f1.is_valid())_mesh->status(f1).set_selected(!_deselection);
                        if(f2.is_valid())_mesh->status(f2).set_selected(!_deselection);
                    }
                }

                _mesh->property(visited,vh)= true;

                if(_primitiveTypes & vertexType_)
                    _mesh->status(vh).set_selected(!_deselection);
            }
            _mesh->remove_property(visited);


        } else {
            emit log(LOGERR, tr("Unable to find boundary."));
        }
    } else {
        emit log(LOGERR, tr("Invalid vertex handle."));
    }
}

//***********************************************************************************

/** \brief Select all primitves of a planar region surrounding the faceHandle
 *
 * @param _mesh a mesh
 * @param _fh handle of the face that was picked
 * @param _maxAngle the maximum dihedral angle over which the flood fill selection passes
 * @param _primitiveTypes the currently active primitive types
 * @param _deselection true if primitives should be deselected
 */
template<class MeshT>
void MeshObjectSelectionPlugin::floodFillSelection(
        MeshT* _mesh, int _objectId, uint _fh, double _maxAngle,
        PrimitiveType _primitiveTypes, bool _deselection) {

    // reset tagged status
    typename MeshT::FaceIter f_it, f_end(_mesh->faces_end());
    for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
        _mesh->status(*f_it).set_tagged(false);

    std::vector<typename MeshT::FaceHandle> face_handles;


    typename MeshT::FaceHandle hitFace = typename MeshT::FaceHandle(_fh);
    face_handles.reserve(50);
    face_handles.push_back(hitFace);
    _mesh->status(hitFace).set_tagged(true);

    typename MeshT::Point n1 = _mesh->normal(hitFace);

    double maxAngle = OpenMesh::deg_to_rad(_maxAngle);

    while (!face_handles.empty()) {
        typename MeshT::FaceHandle fh = face_handles.back();
        face_handles.pop_back();

        for (typename MeshT::FaceFaceIter ff_it(*_mesh,fh) ; ff_it.is_valid() ; ++ff_it) {

            // Check if already tagged
            if (_mesh->status(*ff_it).tagged())
                continue;

            typename MeshT::Point n2 = _mesh->normal(*ff_it);

            double angle = acos(n1 | n2);

            if (angle <= maxAngle) {
                _mesh->status(*ff_it).set_tagged(true);
                face_handles.push_back(*ff_it);
            }
        }
    }

    // now select all tagged primitives
    for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it) {
        if (_mesh->status(*f_it).tagged()) {

            if(_primitiveTypes & vertexType_) {
                for (typename MeshT::FaceVertexIter fv_it(*_mesh,*f_it) ; fv_it.is_valid(); ++fv_it)
                    _mesh->status(*fv_it).set_selected(!_deselection);
            }

            if(_primitiveTypes & edgeType_) {
                for (typename MeshT::FaceEdgeIter fe_it(*_mesh,*f_it) ; fe_it.is_valid(); ++fe_it)
                    _mesh->status(*fe_it).set_selected(!_deselection);
            }

            if(_primitiveTypes & halfedgeType_) {
                for (typename MeshT::FaceHalfedgeIter fhe_it(*_mesh,*f_it) ; fhe_it.is_valid(); ++fhe_it)
                    _mesh->status(*fhe_it).set_selected(!_deselection);
            }

            if(_primitiveTypes & faceType_) {
                _mesh->status(*f_it).set_selected(!_deselection);
            }
        }
    }


    if(_primitiveTypes & vertexType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_VERTICES);
    if(_primitiveTypes & edgeType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_EDGES);
    if(_primitiveTypes & halfedgeType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_HALFEDGES);
    if(_primitiveTypes & faceType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_FACES);
}

//***********************************************************************************

/** \brief Select a complete connected component
 *
 * @param _mesh a mesh
 * @param _fh handle of the face that was picked
 * @param _hit_point point that was picked
 * @param _primitiveType primitive types to be selected
 */
template<class MeshT>
void MeshObjectSelectionPlugin::componentsMeshSelection(
        MeshT* _mesh, int _objectId, uint _fh, ACG::Vec3d& _hit_point,
        PrimitiveType _primitiveType) {

    typename MeshT::FaceHandle fh = _mesh->face_handle(_fh);

    if(!fh.is_valid())
        return;

    //Vertex Selection
    if(_primitiveType & vertexType_) {

        OpenMesh::VPropHandleT<bool> visited;
        _mesh->add_property(visited);

        typename MeshT::VertexIter v_it;
        typename MeshT::VertexIter v_end = _mesh->vertices_end();

        // Initialize vertex tag
        for (v_it = _mesh->vertices_begin(); v_it != v_end; ++v_it)
            _mesh->property(visited, *v_it) = false;

        // Get some vertex incident to the clicked face
        typename MeshT::VertexHandle current = *(_mesh->fv_iter(fh));
        if(!current.is_valid())
            return;

        std::set<typename MeshT::VertexHandle> unprocessed;
        unprocessed.insert(current);

        while( !unprocessed.empty() ) {

            // Select current vertex
            current = *unprocessed.begin();
            _mesh->status(current).set_selected(!(_mesh->status(current)).selected());
            _mesh->property(visited, current) = true;
            unprocessed.erase(current);

            // Go over all neighbors
            for(typename MeshT::VertexVertexIter vv_it = _mesh->vv_iter(current); vv_it.is_valid(); ++vv_it) {
                if(_mesh->property(visited, *vv_it) == true) continue;
                unprocessed.insert(*vv_it);
            }

        }

        _mesh->remove_property(visited);
    }

    //Edge Selection
    if( (_primitiveType & edgeType_) || (_primitiveType & halfedgeType_ ) ) {

        OpenMesh::FPropHandleT<bool> visited;
        _mesh->add_property(visited);

        typename MeshT::FaceIter f_it;
        typename MeshT::FaceIter f_end = _mesh->faces_end();

        // Initialize face tag
        for (f_it = _mesh->faces_begin(); f_it != f_end; ++f_it)
            _mesh->property(visited, *f_it) = false;


        typename MeshT::FaceHandle current = fh;

        std::set<typename MeshT::FaceHandle> unprocessed;
        unprocessed.insert(current);

        typename MeshT::EdgeHandle firstEdge = *(_mesh->fe_iter(fh));
        if(!firstEdge.is_valid()) return;
        bool selected = _mesh->status(firstEdge).selected();

        while( !unprocessed.empty() ) {

            // Select all edges incident to current face
            current = *unprocessed.begin();
            for(typename MeshT::FaceHalfedgeIter fh_it = _mesh->fh_iter(current); fh_it.is_valid(); ++fh_it) {
                if(_primitiveType & halfedgeType_) {
                    _mesh->status(*fh_it).set_selected(!(_mesh->status(*fh_it)).selected());
                }
                if(_primitiveType & edgeType_) {
                    _mesh->status(_mesh->edge_handle(*fh_it)).set_selected(!selected);
                }
            }

            _mesh->property(visited, current) = true;
            unprocessed.erase(current);

            // Go over all neighbors
            for(typename MeshT::FaceFaceIter ff_it = _mesh->ff_iter(current); ff_it.is_valid(); ++ff_it) {
                if(_mesh->property(visited, *ff_it) == true) continue;
                unprocessed.insert(*ff_it);
            }

        }

        _mesh->remove_property(visited);
    }

    //Face Selection
    if(_primitiveType & faceType_) {

        OpenMesh::FPropHandleT<bool> visited;
        _mesh->add_property(visited);

        typename MeshT::FaceIter f_it;
        typename MeshT::FaceIter f_end = _mesh->faces_end();

        // Initialize face tag
        for (f_it = _mesh->faces_begin(); f_it != f_end; ++f_it)
            _mesh->property(visited, *f_it) = false;


        typename MeshT::FaceHandle current = fh;

        std::set<typename MeshT::FaceHandle> unprocessed;
        unprocessed.insert(current);

        while( !unprocessed.empty() ) {

            // Select all edges incident to current face
            current = *unprocessed.begin();
            _mesh->status(current).set_selected(!(_mesh->status(current)).selected());
            _mesh->property(visited, current) = true;
            unprocessed.erase(current);

            // Go over all neighbors
            for(typename MeshT::FaceFaceIter ff_it = _mesh->ff_iter(current); ff_it.is_valid(); ++ff_it) {
                if(_mesh->property(visited, *ff_it) == true) continue;
                unprocessed.insert(*ff_it);
            }

        }

        _mesh->remove_property(visited);
    }
    if(_primitiveType & vertexType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_VERTICES);
    if(_primitiveType & edgeType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_EDGES);
    if(_primitiveType & halfedgeType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_HALFEDGES);
    if(_primitiveType & faceType_)
        emit updatedObject(_objectId, UPDATE_SELECTION_FACES);
}

//***********************************************************************************

/** \brief Colorize a selection
 *
 * @param _mesh           a mesh
 * @param _primitiveTypes selection types
 * @param _red            rgba color
 * @param _green          rgba color
 * @param _blue           rgba color
 * @param _alpha          rgba color
 */
template< typename MeshT >
void MeshObjectSelectionPlugin::colorizeSelection(MeshT* _mesh,
                                                  PrimitiveType _primitiveTypes,
                                                  int _red,
                                                  int _green,
                                                  int _blue,
                                                  int _alpha) {

    typename MeshT::Color color = OpenMesh::color_cast<OpenMesh::Vec4f,OpenMesh::Vec4uc>(OpenMesh::Vec4uc(_red,_green,_blue,_alpha));
    /*color[0] = _red;
    color[1] = _green;
    color[2] = _blue;
    color[3] = _alpha;
    */

    if (_primitiveTypes & vertexType_) {
        typename MeshT::VertexIter v_it, v_end=_mesh->vertices_end();

        // Request vertex color attribute
        if ( !_mesh->has_vertex_colors() )
            _mesh->request_vertex_colors();

        for (v_it=_mesh->vertices_begin(); v_it!=v_end; ++v_it)
            if ( _mesh->status(*v_it).selected() )
                _mesh->set_color(*v_it, color);
    }

    if (_primitiveTypes & faceType_) {
        typename MeshT::FaceIter f_it, f_end( _mesh->faces_end() );

        // Request face color attribute
        if ( !_mesh->has_face_colors() )
            _mesh->request_face_colors();

        for (f_it=_mesh->faces_begin(); f_it!=f_end; ++f_it)
            if ( _mesh->status(*f_it).selected() )
                _mesh->set_color(*f_it, color);
    }

    if (_primitiveTypes & edgeType_) {
        typename MeshT::EdgeIter e_it, e_end( _mesh->edges_end() );

        // Request edge color attribute
        if ( !_mesh->has_edge_colors() )
            _mesh->request_edge_colors();

        for (e_it=_mesh->edges_begin(); e_it!=e_end; ++e_it)
            if ( _mesh->status(*e_it).selected() )
                _mesh->set_color(*e_it, color);
    }
    
    if (_primitiveTypes & halfedgeType_) {
        typename MeshT::HalfedgeIter h_it, h_end( _mesh->halfedges_end() );

        // Request halfedge color attribute
        if ( !_mesh->has_halfedge_colors() )
            _mesh->request_halfedge_colors();

        for (h_it=_mesh->halfedges_begin(); h_it!=h_end; ++h_it)
            if ( _mesh->status(*h_it).selected() )
                _mesh->set_color(*h_it, color);
    }
}

//***********************************************************************************


/** \brief Create a new mesh from the selection
 *
 * @param _mesh          A mesh
 * @param _newMesh       The resulting mesh containing only the selected elements
 * @param _primitiveType Which primitive should be used for selection (vertices,faces,edges,...)
 */

template<class MeshT>
void MeshObjectSelectionPlugin::createMeshFromSelection(MeshT& _mesh, MeshT& _newMesh, PrimitiveType _primitiveType)
{

  // Tracking during copy action
  OpenMesh::VPropHandleT<typename MeshT::VertexHandle> copyHandle;
  _mesh.add_property(copyHandle, "copyHandle Property");

  //first copy vertices
  typename MeshT::VertexIter v_it, v_end = _mesh.vertices_end();
  for (v_it = _mesh.vertices_begin(); v_it != v_end; ++v_it) {

    bool copy = false;

    //if the vertex belongs to the selection copy it
    if (_primitiveType & vertexType_)
      copy = _mesh.status(*v_it).selected();
    else if (_primitiveType & edgeType_) {

      for (typename MeshT::VertexOHalfedgeIter voh_it(_mesh, *v_it); voh_it.is_valid(); ++voh_it)
        if (_mesh.status(_mesh.edge_handle(*voh_it)).selected()) {
          copy = true;
          break;
        }

    } else if (_primitiveType & faceType_) {
      for (typename MeshT::VertexFaceIter vf_it(_mesh, *v_it); vf_it.is_valid(); ++vf_it)
        if (_mesh.status(*vf_it).selected()) {
          copy = true;
          break;
        }
    }

    //copy it
    if (copy) {
      _mesh.property(copyHandle, *v_it) = _newMesh.add_vertex(_mesh.point(*v_it));
    } else {
      _mesh.property(copyHandle, *v_it) = typename MeshT::VertexHandle(-1);
    }
  }

  //now check all faces
  //if all vertices of the face exist in the new mesh -> copy it
  typename MeshT::FaceIter f_it, f_end = _mesh.faces_end();
  for (f_it = _mesh.faces_begin(); f_it != f_end; ++f_it) {

    std::vector<typename MeshT::VertexHandle> v;

    bool skip = false;

    for (typename MeshT::FaceVertexIter fv_it(_mesh, *f_it); fv_it.is_valid(); ++fv_it)
      if (_mesh.property(copyHandle, *fv_it).is_valid())
        v.push_back(_mesh.property(copyHandle, *fv_it));
      else {
        skip = true;
        break;
      }

    if (!skip)
      _newMesh.add_face(v);
  }

  _newMesh.update_normals();

  _mesh.remove_property(copyHandle);

}

template<class MeshT>
void MeshObjectSelectionPlugin::selectVerticesByValue(MeshT* _mesh, QString _component, bool _greater, double _value)
{

  //first copy vertices
  typename MeshT::VertexIter v_it, v_end = _mesh->vertices_end();
  for (v_it = _mesh->vertices_begin(); v_it != v_end; ++v_it) {
    const typename MeshT::Point p = _mesh->point(*v_it);

    bool select = false;

    if (_component.contains("x",Qt::CaseInsensitive) ) {
      select = (p[0] > _value);
    } else  if (_component.contains("y",Qt::CaseInsensitive) ) {
      select = (p[1] > _value);
    } else if (_component.contains("z",Qt::CaseInsensitive) ) {
      select = (p[2] > _value);
    } else {
      emit log(LOGERR,tr("selectVerticesByValue, undefined component ") + _component );
    }


    // invert if requested
    if (! _greater )
      select = !select;

    // set selection status only if the vertex was previously unselected
    if ( ! _mesh->status(*v_it).selected() )
      _mesh->status(*v_it).set_selected(select);

  }

}

