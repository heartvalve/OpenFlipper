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

#ifdef ENABLE_OPENVOLUMEMESH_SUPPORT

#define OVM_PROPERTY_VISZUALIZTER_VECTOR_FIELD_DIFFERENCE_CC

#include "OVMPropertyVisualizerVectorFieldDifference.hh"

template <typename MeshT>
OVMPropertyVisualizerVectorFieldDifference<MeshT>::OVMPropertyVisualizerVectorFieldDifference(MeshT* _mesh, int objectID, PropertyInfo _propertyInfo1, PropertyInfo _propertyInfo2)
    : OVMPropertyVisualizer<MeshT>(_mesh, objectID, _propertyInfo1),
      propertyInfo2(_propertyInfo2)
{
    if (PropertyVisualizer::widget) delete PropertyVisualizer::widget;
    VectorFieldDifferenceWidget* w = new VectorFieldDifferenceWidget();
    w->paramVectorFieldDifference->setTitle(QString("3D Vector Field Difference Parameters of ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
    PropertyVisualizer::widget = w;
}

namespace OVMPVVFD{ //OVMPVVFD = OpenMeshProeprtyVisualizerVectorFieldDifference

template <typename MeshT>
float scalarFn_norm_of_diff(const ACG::Vec3d &a, const ACG::Vec3d &b) {
    return (a - b).norm();
}

template <typename MeshT>
float scalarFn_diff_of_norms(const ACG::Vec3d &a, const ACG::Vec3d &b) {
    return std::fabs(a.norm() - b.norm());
}

template <typename MeshT>
float scalarFn_4_symm_diff(const ACG::Vec3d &a, const ACG::Vec3d &b) {
    double alpha = std::acos((a|b) / a.norm() / b.norm());
    alpha -= std::floor((alpha + M_PI_4) / M_PI_2) * M_PI_2;
    return std::fabs(alpha);
}


template<typename Prop1, typename Prop2, float (*ScalarFn)(const typename Prop1::value_type &, const typename Prop2::value_type &)>
class ScalarAssigner {
    public:
        ScalarAssigner(const Prop1 &prop1, const Prop2 &prop2) :
            prop1(prop1), prop2(prop2) { }

        template<typename Handle>
        float operator() (const Handle &handle) const {
            return ScalarFn(prop1[handle], prop2[handle]);
        }

    protected:
        const Prop1 &prop1;
        const Prop2 &prop2;
};

template<typename MeshT, typename IteratorT, typename PropHandle, float (*ScalarFn)(const ACG::Vec3d &, const ACG::Vec3d &)>
void colorElements(int objectID,
                   PropHandle prop1,
                   PropHandle prop2,
                   IteratorT primitivesBegin,
                   IteratorT primitivesEnd)
{

    std::vector<float> scalars;
    std::transform(primitivesBegin, primitivesEnd, std::back_inserter(scalars),
                   ScalarAssigner<PropHandle, PropHandle, ScalarFn >(prop1, prop2));

    const float min = *std::min_element(scalars.begin(), scalars.end());
    const float max = *std::max_element(scalars.begin(), scalars.end());

    VolumeMeshObject<MeshT>* object;
    PluginFunctions::getObject(objectID, object);

    ACG::ColorCoder colCod(min,max,false);
    for (std::vector<float>::iterator i = scalars.begin(); i != scalars.end(); ++i)
        object->colors()[*(primitivesBegin++)] = colCod(*i);
}

}


template <typename MeshT>
void OVMPropertyVisualizerVectorFieldDifference<MeshT>::visualizeCellProp(bool _setDrawMode)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::CellPropertyT<ACG::Vec3d> prop1 = mesh->template request_cell_property<ACG::Vec3d>(PropertyVisualizer::propertyInfo.propName());
    OpenVolumeMesh::CellPropertyT<ACG::Vec3d> prop2 = mesh->template request_cell_property<ACG::Vec3d>(propertyInfo2.propName());

    VectorFieldDifferenceWidget* w = static_cast<VectorFieldDifferenceWidget*>(PropertyVisualizer::widget);

    if (w->vecFieldDiff_4symm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::CellIter, OpenVolumeMesh::CellPropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_4_symm_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->cells_begin(), mesh->cells_end());
    if (w->vecFieldDiff_diff_norm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::CellIter, OpenVolumeMesh::CellPropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_diff_of_norms<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->cells_begin(), mesh->cells_end());
    if (w->vecFieldDiff_norm_diff_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::CellIter, OpenVolumeMesh::CellPropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_norm_of_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->cells_begin(), mesh->cells_end());
    if (_setDrawMode)
    {
        VolumeMeshObject<MeshT>* object;
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.cellsColoredPerCell);
    }
}

template <typename MeshT>
void OVMPropertyVisualizerVectorFieldDifference<MeshT>::visualizeFaceProp(bool _setDrawMode)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::FacePropertyT<ACG::Vec3d> prop1 = mesh->template request_face_property<ACG::Vec3d>(PropertyVisualizer::propertyInfo.propName());
    OpenVolumeMesh::FacePropertyT<ACG::Vec3d> prop2 = mesh->template request_face_property<ACG::Vec3d>(propertyInfo2.propName());

    VectorFieldDifferenceWidget* w = static_cast<VectorFieldDifferenceWidget*>(PropertyVisualizer::widget);

    if (w->vecFieldDiff_4symm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::FaceIter, OpenVolumeMesh::FacePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_4_symm_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->faces_begin(), mesh->faces_end());
    if (w->vecFieldDiff_diff_norm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::FaceIter, OpenVolumeMesh::FacePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_diff_of_norms<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->faces_begin(), mesh->faces_end());
    if (w->vecFieldDiff_norm_diff_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::FaceIter, OpenVolumeMesh::FacePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_norm_of_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->faces_begin(), mesh->faces_end());
    if (_setDrawMode)
    {
        VolumeMeshObject<MeshT>* object;
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.facesColoredPerFace);
    }
}

template <typename MeshT>
void OVMPropertyVisualizerVectorFieldDifference<MeshT>::visualizeHalffaceProp(bool _setDrawMode)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::HalfFacePropertyT<ACG::Vec3d> prop1 = mesh->template request_halfface_property<ACG::Vec3d>(PropertyVisualizer::propertyInfo.propName());
    OpenVolumeMesh::HalfFacePropertyT<ACG::Vec3d> prop2 = mesh->template request_halfface_property<ACG::Vec3d>(propertyInfo2.propName());

    VectorFieldDifferenceWidget* w = static_cast<VectorFieldDifferenceWidget*>(PropertyVisualizer::widget);

    if (w->vecFieldDiff_4symm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::HalfFaceIter, OpenVolumeMesh::HalfFacePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_4_symm_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->halffaces_begin(), mesh->halffaces_end());
    if (w->vecFieldDiff_diff_norm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::HalfFaceIter, OpenVolumeMesh::HalfFacePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_diff_of_norms<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->halffaces_begin(), mesh->halffaces_end());
    if (w->vecFieldDiff_norm_diff_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::HalfFaceIter, OpenVolumeMesh::HalfFacePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_norm_of_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->halffaces_begin(), mesh->halffaces_end());
    if (_setDrawMode)
    {
        VolumeMeshObject<MeshT>* object;
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.halffacesColoredPerHalfface);
    }
}

template <typename MeshT>
void OVMPropertyVisualizerVectorFieldDifference<MeshT>::visualizeEdgeProp(bool _setDrawMode)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::EdgePropertyT<ACG::Vec3d> prop1 = mesh->template request_edge_property<ACG::Vec3d>(PropertyVisualizer::propertyInfo.propName());
    OpenVolumeMesh::EdgePropertyT<ACG::Vec3d> prop2 = mesh->template request_edge_property<ACG::Vec3d>(propertyInfo2.propName());

    VectorFieldDifferenceWidget* w = static_cast<VectorFieldDifferenceWidget*>(PropertyVisualizer::widget);

    if (w->vecFieldDiff_4symm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::EdgeIter, OpenVolumeMesh::EdgePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_4_symm_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->edges_begin(), mesh->edges_end());
    if (w->vecFieldDiff_diff_norm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::EdgeIter, OpenVolumeMesh::EdgePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_diff_of_norms<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->edges_begin(), mesh->edges_end());
    if (w->vecFieldDiff_norm_diff_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::EdgeIter, OpenVolumeMesh::EdgePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_norm_of_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->edges_begin(), mesh->edges_end());
    if (_setDrawMode)
    {
        VolumeMeshObject<MeshT>* object;
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.edgesColoredPerEdge);
    }
}

template <typename MeshT>
void OVMPropertyVisualizerVectorFieldDifference<MeshT>::visualizeHalfedgeProp(bool _setDrawMode)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::HalfEdgePropertyT<ACG::Vec3d> prop1 = mesh->template request_halfedge_property<ACG::Vec3d>(PropertyVisualizer::propertyInfo.propName());
    OpenVolumeMesh::HalfEdgePropertyT<ACG::Vec3d> prop2 = mesh->template request_halfedge_property<ACG::Vec3d>(propertyInfo2.propName());

    VectorFieldDifferenceWidget* w = static_cast<VectorFieldDifferenceWidget*>(PropertyVisualizer::widget);

    if (w->vecFieldDiff_4symm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::HalfEdgeIter, OpenVolumeMesh::HalfEdgePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_4_symm_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->halfedges_begin(), mesh->halfedges_end());
    if (w->vecFieldDiff_diff_norm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::HalfEdgeIter, OpenVolumeMesh::HalfEdgePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_diff_of_norms<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->halfedges_begin(), mesh->halfedges_end());
    if (w->vecFieldDiff_norm_diff_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::HalfEdgeIter, OpenVolumeMesh::HalfEdgePropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_norm_of_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->halfedges_begin(), mesh->halfedges_end());
    if (_setDrawMode)
    {
        VolumeMeshObject<MeshT>* object;
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.halfedgesColoredPerHalfedge);
    }
}

template <typename MeshT>
void OVMPropertyVisualizerVectorFieldDifference<MeshT>::visualizeVertexProp(bool _setDrawMode)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::VertexPropertyT<ACG::Vec3d> prop1 = mesh->template request_vertex_property<ACG::Vec3d>(PropertyVisualizer::propertyInfo.propName());
    OpenVolumeMesh::VertexPropertyT<ACG::Vec3d> prop2 = mesh->template request_vertex_property<ACG::Vec3d>(propertyInfo2.propName());

    VectorFieldDifferenceWidget* w = static_cast<VectorFieldDifferenceWidget*>(PropertyVisualizer::widget);

    if (w->vecFieldDiff_4symm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::VertexIter, OpenVolumeMesh::VertexPropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_4_symm_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->vertices_begin(), mesh->vertices_end());
    if (w->vecFieldDiff_diff_norm_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::VertexIter, OpenVolumeMesh::VertexPropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_diff_of_norms<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->vertices_begin(), mesh->vertices_end());
    if (w->vecFieldDiff_norm_diff_rb->isChecked())
        OVMPVVFD::colorElements< MeshT, OpenVolumeMesh::VertexIter, OpenVolumeMesh::VertexPropertyT<ACG::Vec3d>, OVMPVVFD::scalarFn_norm_of_diff<MeshT> >(OVMPropertyVisualizer<MeshT>::mObjectID, prop1, prop2, mesh->vertices_begin(), mesh->vertices_end());
    if (_setDrawMode)
    {
        VolumeMeshObject<MeshT>* object;
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.verticesColored);
    }
}

#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */
