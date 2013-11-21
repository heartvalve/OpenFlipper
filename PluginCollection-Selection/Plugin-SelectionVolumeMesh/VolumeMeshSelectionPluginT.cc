

#include "VolumeMeshSelectionPlugin.hh"
#include <stack>
#include <vector>
#include <cmath>

//***********************************************************************************

/** \brief Select all primitives that are entirely projected to the given region
 *
 * @param _mesh a mesh
 * @param _state current gl state
 * @param _region region
 * @param _primitiveType primitive types to be selected
 * @param _deselection true, if entities should be deselected
 * @return true, if something was selected
 */
template<class MeshT>
bool VolumeMeshSelectionPlugin::volumeSelection(MeshT* _mesh, ACG::GLState& _state, QRegion *_region,
                                                PrimitiveType _primitiveType, bool _deselection) {
    using namespace OpenVolumeMesh;

    ACG::Vec3d proj;
    bool rv = false;

    //reset tagged status
    StatusAttrib status(*_mesh);
    VertexIter v_it(_mesh->vertices_begin()), v_end(_mesh->vertices_end());
    for (; v_it != v_end; ++v_it)
        status[*v_it].set_tagged(false);

    //tag all vertices that are projected into region
    for (v_it = _mesh->vertices_begin(); v_it != v_end; ++v_it) {

        proj = _state.project(_mesh->vertex(*v_it));

        if (_region->contains(QPoint((int)proj[0], _state.context_height()- (int)proj[1]))) {
            
            status[*v_it].set_tagged(true);
            if (_primitiveType & vertexType_) {
                rv = true;
                status[*v_it].set_selected(!_deselection);
            }
        }
    }

    if (_primitiveType & edgeType_) {
        EdgeIter e_it(_mesh->edges_begin()), e_end(_mesh->edges_end());

        for (; e_it != e_end; ++e_it)
        {
            VertexHandle v1 = _mesh->halfedge(_mesh->halfedge_handle(*e_it, 0)).to_vertex();
            VertexHandle v2 = _mesh->halfedge(_mesh->halfedge_handle(*e_it, 1)).to_vertex();

            // select only edges whose both vertices are within the selection region
            if (status[v1].tagged() && status[v2].tagged()) {
                rv = true;
                status[*e_it].set_selected(!_deselection);
            }
        }
    }

    if (_primitiveType & faceType_) {
        FaceIter f_it(_mesh->faces_begin()), f_end(_mesh->faces_end());

        for (; f_it != f_end; ++f_it) {
            const std::vector<HalfEdgeHandle>& halfedges = _mesh->face(*f_it).halfedges();
            bool all_fv_tagged = true;

            for (std::size_t i = 0; i < halfedges.size(); ++i) {
                VertexHandle v = _mesh->halfedge(halfedges[i]).to_vertex();
                all_fv_tagged &= status[v].tagged();
            }

            // select only faces that are completely within the selection region
            if (all_fv_tagged) {
                rv = true;
                status[*f_it].set_selected(!_deselection);
            }
        }
    }

    if (_primitiveType & cellType_) {
        CellIter c_it(_mesh->cells_begin()), c_end(_mesh->cells_end());

        for (; c_it != c_end; ++c_it) {
            bool all_cv_tagged = true;
            for (CellVertexIter cv_it(_mesh->cv_iter(*c_it)); cv_it.valid(); ++cv_it) {
                all_cv_tagged &= status[*cv_it].tagged();
            }

            if (all_cv_tagged) {
                rv = true;
                status[*c_it].set_selected(!_deselection);
            }
        }
    }

    return rv;
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
void VolumeMeshSelectionPlugin::floodFillSelection(MeshT* _mesh, uint _fh, double _maxAngle,
                                                   PrimitiveType _primitiveTypes, bool _deselection)
{
    using namespace OpenVolumeMesh;

    FaceHandle hitFace(_fh);

    if (!_mesh->is_boundary(hitFace))
        return;

    HalfFaceHandle hitHalfFace = _mesh->halfface_handle(hitFace, 0);

    if (!_mesh->is_boundary(hitHalfFace))
        hitHalfFace = _mesh->halfface_handle(hitFace, 1);

    StatusAttrib status(*_mesh);

    // reset tagged status
    HalfFaceIter hf_it(_mesh->halffaces_begin()), hf_end(_mesh->halffaces_end());
    for (; hf_it != hf_end; ++hf_it)
    {
        status[*hf_it].set_tagged(false);
    }

    status[hitHalfFace].set_tagged(true);

    std::stack<HalfFaceHandle> hf_handles;
    hf_handles.push(hitHalfFace);

    NormalAttrib<MeshT> normals(*_mesh);
    typename MeshT::PointT n1 = normals[hitHalfFace];
    double maxAngle = _maxAngle / 180.0 * M_PI;

    // tag all half-faces whose normal does not deviate too much from the
    // initial face
    while (!hf_handles.empty())
    {
        HalfFaceHandle hf = hf_handles.top();
        hf_handles.pop();

        for (BoundaryHalfFaceHalfFaceIter bhfhf_it(hf, _mesh) ; bhfhf_it.valid() ; ++bhfhf_it)
        {
            if (status[*bhfhf_it].tagged())
                continue;

            typename MeshT::PointT n2 = normals[*bhfhf_it];

            double angle = std::acos(n1 | n2);

            if (angle <= maxAngle) {
                status[*bhfhf_it].set_tagged(true);
                hf_handles.push(*bhfhf_it);
            }
        }
    }

    // now select all tagged primitives
    for (hf_it = _mesh->halffaces_begin(); hf_it != hf_end; ++hf_it)
    {
        if (status[*hf_it].tagged())
        {
            FaceHandle fh = _mesh->face_handle(*hf_it);

            if (_primitiveTypes & vertexType_)
                for (HalfFaceVertexIter hfv_it(*hf_it, _mesh); hfv_it.valid(); ++hfv_it)
                    status[*hfv_it].set_selected(!_deselection);

            if (_primitiveTypes & edgeType_)
            {
                std::vector<HalfEdgeHandle> const& halfedges = _mesh->face(fh).halfedges();

                for (std::size_t i = 0; i < halfedges.size(); ++i)
                    status[_mesh->edge_handle(halfedges[i])].set_selected(!_deselection);
            }

            if (_primitiveTypes & faceType_)
                status[fh].set_selected(!_deselection);
        }
    }
}

