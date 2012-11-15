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

#define OVM_PROPERTY_VISUALIZER_VECTOR_CC

#include "OVMPropertyVisualizerVector.hh"


template <typename MeshT>
OVMPropertyVisualizerVector<MeshT>::OVMPropertyVisualizerVector(MeshT* _mesh, int objectID, PropertyInfo _propertyInfo)
    : OVMPropertyVisualizer<MeshT>(_mesh, objectID, _propertyInfo)
{
    if (PropertyVisualizer::widget) delete PropertyVisualizer::widget;
    VectorWidget* w = new VectorWidget();
    w->paramVector->setTitle(QString("3D Vector Parameters of ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
    PropertyVisualizer::widget = w;

    lineNode = new ACG::SceneGraph::LineNode( ACG::SceneGraph::LineNode::LineSegmentsMode, dynamic_cast < ACG::SceneGraph::SeparatorNode* >( PluginFunctions::getRootNode() ) );

    w->vectors_edges_rb->hide();
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::clear()
{
    lineNode->clear();
    OVMPropertyVisualizer<MeshT>::clear();
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::duplicateProperty()
{
    OVMPropertyVisualizer<MeshT>::template duplicateProperty_stage1<ACG::Vec3d>();
}

template<typename MeshT>
template<typename PropType, typename EntityIterator>
void OVMPropertyVisualizerVector<MeshT>::visualizeVectorAsColorForEntity(PropType prop, EntityIterator e_begin, EntityIterator e_end) {
    if (!prop)
        throw VizException("Getting PropHandle from mesh for selected property failed.");
    VolumeMeshObject<MeshT>* object;
    PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);
    for (EntityIterator e_it = e_begin; e_it != e_end; ++e_it) {
        ACG::Vec3d v = prop[*e_it].normalized() * .5 + ACG::Vec3d(.5, .5, .5);

        object->colors()[*e_it] = ACG::Vec4f(v[0], v[1], v[2], 1.0);
    }
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeCellProp()
{
    VectorWidget* w = (VectorWidget*)PropertyVisualizer::widget;
    if (w->vectors_colors_rb->isChecked())
    {
        OpenVolumeMesh::CellPropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_cell_property<ACG::Vec3d>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        visualizeVectorAsColorForEntity(prop,
                                        OVMPropertyVisualizer<MeshT>::mesh->cells_begin(),
                                        OVMPropertyVisualizer<MeshT>::mesh->cells_end());
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::CELLS_COLORED);
    }
    else visualizeCellPropAsStrokes();
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeFaceProp()
{
    VectorWidget* w = (VectorWidget*)PropertyVisualizer::widget;
    if (w->vectors_colors_rb->isChecked())
    {
        OpenVolumeMesh::FacePropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_face_property<ACG::Vec3d>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        visualizeVectorAsColorForEntity(prop,
                                        OVMPropertyVisualizer<MeshT>::mesh->faces_begin(),
                                        OVMPropertyVisualizer<MeshT>::mesh->faces_end());
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);
    }
    else visualizeFacePropAsStrokes();
}


template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeHalffaceProp()
{
    VectorWidget* w = (VectorWidget*)PropertyVisualizer::widget;
    if (w->vectors_colors_rb->isChecked())
    {
        OpenVolumeMesh::HalfFacePropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_halfface_property<ACG::Vec3d>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        visualizeVectorAsColorForEntity(prop,
                                        OVMPropertyVisualizer<MeshT>::mesh->halffaces_begin(),
                                        OVMPropertyVisualizer<MeshT>::mesh->halffaces_end());
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED); //todo: halffaces draw mode
    }
    else visualizeVertexPropAsStrokes();
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeEdgeProp()
{
    VectorWidget* w = (VectorWidget*)PropertyVisualizer::widget;
    if (w->vectors_colors_rb->isChecked())
    {
        OpenVolumeMesh::EdgePropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_edge_property<ACG::Vec3d>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        visualizeVectorAsColorForEntity(prop,
                                        OVMPropertyVisualizer<MeshT>::mesh->edges_begin(),
                                        OVMPropertyVisualizer<MeshT>::mesh->edges_end());
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::EDGES_COLORED);
    }
    else visualizeEdgePropAsStrokes();
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeHalfedgeProp()
{
    VectorWidget* w = (VectorWidget*)PropertyVisualizer::widget;
    if (w->vectors_colors_rb->isChecked())
    {
        OpenVolumeMesh::HalfEdgePropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_halfedge_property<ACG::Vec3d>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        visualizeVectorAsColorForEntity(prop,
                                        OVMPropertyVisualizer<MeshT>::mesh->halfedges_begin(),
                                        OVMPropertyVisualizer<MeshT>::mesh->halfedges_end());
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::HALFEDGES_COLORED);
    }
    else visualizeHalfedgePropAsStrokes();
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeVertexProp()
{
    VectorWidget* w = (VectorWidget*)PropertyVisualizer::widget;
    if (w->vectors_colors_rb->isChecked())
    {
        OpenVolumeMesh::VertexPropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_vertex_property<ACG::Vec3d>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        visualizeVectorAsColorForEntity(prop,
                                        OVMPropertyVisualizer<MeshT>::mesh->vertices_begin(),
                                        OVMPropertyVisualizer<MeshT>::mesh->vertices_end());
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);
    }
    else visualizeVertexPropAsStrokes();
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeFacePropAsStrokes()
{
    VectorWidget* vectorWidget = static_cast<VectorWidget*>(PropertyVisualizer::widget);

    lineNode->clear();

    ACG::Vec4f color = OVMPropertyVisualizer<MeshT>::convertColor(vectorWidget->lineColor->color());

    OpenVolumeMesh::FacePropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_face_property<ACG::Vec3d>(PropertyVisualizer::propertyInfo.propName());

    if ( !prop )
        return;

    OpenVolumeMesh::FaceIter f_begin(OVMPropertyVisualizer<MeshT>::mesh->faces_begin()), f_end(OVMPropertyVisualizer<MeshT>::mesh->faces_end());
    for (OpenVolumeMesh::FaceIter f_it = f_begin; f_it != f_end; ++f_it){

        ACG::Vec3d center(0.0, 0.0, 0.0);
        int vCount = 0;

        OpenVolumeMesh::HalfFaceHandle hfh = OVMPropertyVisualizer<MeshT>::mesh->halfface_handle(*f_it, 0);
        for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = OVMPropertyVisualizer<MeshT>::mesh->hfv_iter(hfh); hfv_it; ++hfv_it){
            vCount++;
            center += OVMPropertyVisualizer<MeshT>::mesh->vertex(*hfv_it);
        }

        center /= vCount;

        ACG::Vec3d v  = prop[*f_it];

        if (vectorWidget->normalize->isChecked() && v.sqrnorm() > 1e-12)
            v.normalize();

        if(vectorWidget->scale->isChecked())
            v *= vectorWidget->scaleBox->value();

        lineNode->add_line( center, (center+v) );
        lineNode->add_color(color);
    }
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeEdgePropAsStrokes()
{
    VectorWidget* vectorWidget = static_cast<VectorWidget*>(PropertyVisualizer::widget);

    lineNode->clear();

    ACG::Vec4f color = OVMPropertyVisualizer<MeshT>::convertColor(vectorWidget->lineColor->color());

    OpenVolumeMesh::EdgePropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_edge_property<ACG::Vec3d>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
        return;

    OpenVolumeMesh::EdgeIter e_begin(OVMPropertyVisualizer<MeshT>::mesh->edges_begin()), e_end(OVMPropertyVisualizer<MeshT>::mesh->edges_end());
    for (OpenVolumeMesh::EdgeIter e_it = e_begin; e_it != e_end; ++e_it){

        OpenVolumeMesh::OpenVolumeMeshEdge edge = OVMPropertyVisualizer<MeshT>::mesh->edge(*e_it);
        ACG::Vec3d v1 = OVMPropertyVisualizer<MeshT>::mesh->vertex(edge.from_vertex());
        ACG::Vec3d v2 = OVMPropertyVisualizer<MeshT>::mesh->vertex(edge.to_vertex());
        ACG::Vec3d start = 0.5*(v1+v2);

        ACG::Vec3d v  = prop[*e_it];

        if (vectorWidget->normalize->isChecked() && v.sqrnorm() > 1e-12)
            v.normalize();



        if(vectorWidget->scale->isChecked())
        v *= vectorWidget->scaleBox->value();

        lineNode->add_line( start, (start+v) );
        lineNode->add_color(color);
    }
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeHalfedgePropAsStrokes()
{
    VectorWidget* vectorWidget = static_cast<VectorWidget*>(PropertyVisualizer::widget);

    lineNode->clear();

    ACG::Vec4f color = OVMPropertyVisualizer<MeshT>::convertColor(vectorWidget->lineColor->color());

    OpenVolumeMesh::HalfEdgePropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_halfedge_property<ACG::Vec3d>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
        return;

    OpenVolumeMesh::HalfEdgeIter he_begin(OVMPropertyVisualizer<MeshT>::mesh->halfedges_begin()), he_end(OVMPropertyVisualizer<MeshT>::mesh->halfedges_end());
    for (OpenVolumeMesh::HalfEdgeIter he_it = he_begin; he_it != he_end; ++he_it){

        OpenVolumeMesh::OpenVolumeMeshEdge edge = OVMPropertyVisualizer<MeshT>::mesh->halfedge(*he_it);

        ACG::Vec3d v1 = OVMPropertyVisualizer<MeshT>::mesh->vertex(edge.from_vertex());
        ACG::Vec3d v2 = OVMPropertyVisualizer<MeshT>::mesh->vertex(edge.to_vertex());
        ACG::Vec3d start = (2.0*v1+v2)/3.0;

        ACG::Vec3d v  = prop[*he_it];

        if (vectorWidget->normalize->isChecked() && v.sqrnorm() > 1e-12)
            v.normalize();

        if(vectorWidget->scale->isChecked())
        v *= vectorWidget->scaleBox->value();

        lineNode->add_line( start, (start+v) );
        lineNode->add_color(color);
    }
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeVertexPropAsStrokes()
{
    VectorWidget* vectorWidget = static_cast<VectorWidget*>(PropertyVisualizer::widget);

    lineNode->clear();

    ACG::Vec4f color = OVMPropertyVisualizer<MeshT>::convertColor(vectorWidget->lineColor->color());

    OpenVolumeMesh::VertexPropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_vertex_property<ACG::Vec3d>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
        return;

    OpenVolumeMesh::VertexIter v_begin(OVMPropertyVisualizer<MeshT>::mesh->vertices_begin()), v_end(OVMPropertyVisualizer<MeshT>::mesh->vertices_end());
    for (OpenVolumeMesh::VertexIter v_it = v_begin; v_it != v_end; ++v_it){

        ACG::Vec3d start = OVMPropertyVisualizer<MeshT>::mesh->vertex(*v_it);

        ACG::Vec3d v  = prop[*v_it];

        if (vectorWidget->normalize->isChecked() && v.sqrnorm() > 1e-12)
            v.normalize();

        if(vectorWidget->scale->isChecked())
        v *= vectorWidget->scaleBox->value();

        lineNode->add_line( start, (start+v) );
        lineNode->add_color(color);
    }
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeCellPropAsStrokes()
{
    VectorWidget* vectorWidget = static_cast<VectorWidget*>(PropertyVisualizer::widget);

    lineNode->clear();

    ACG::Vec4f color = OVMPropertyVisualizer<MeshT>::convertColor(vectorWidget->lineColor->color());

    OpenVolumeMesh::CellPropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_cell_property<ACG::Vec3d>(PropertyVisualizer::propertyInfo.propName());

    if ( !prop )
        return;

    OpenVolumeMesh::CellIter c_begin(OVMPropertyVisualizer<MeshT>::mesh->cells_begin()), c_end(OVMPropertyVisualizer<MeshT>::mesh->cells_end());
    for (OpenVolumeMesh::CellIter c_it = c_begin; c_it != c_end; ++c_it){

        // Compute cell's center
        ACG::Vec3d center(0.0, 0.0, 0.0);
        unsigned int vCount = OVMPropertyVisualizer<MeshT>::mesh->n_vertices_in_cell(*c_it);
        for(OpenVolumeMesh::CellVertexIter cv_it = OVMPropertyVisualizer<MeshT>::mesh->cv_iter(*c_it); cv_it.valid(); ++cv_it) {
            center += OVMPropertyVisualizer<MeshT>::mesh->vertex(*cv_it) / (double)vCount;
        }

        ACG::Vec3d v  = prop[*c_it];

        if (vectorWidget->normalize->isChecked() && v.sqrnorm() > 1e-12)
            v.normalize();

        if(vectorWidget->scale->isChecked())
            v *= vectorWidget->scaleBox->value();

        lineNode->add_line( center, (center+v) );
        lineNode->add_color(color);
    }
}

template <typename MeshT>
void OVMPropertyVisualizerVector<MeshT>::visualizeHalffacePropAsStrokes()
{
    VectorWidget* vectorWidget = static_cast<VectorWidget*>(PropertyVisualizer::widget);

    lineNode->clear();

    ACG::Vec4f color = OVMPropertyVisualizer<MeshT>::convertColor(vectorWidget->lineColor->color());

    OpenVolumeMesh::HalfFacePropertyT<ACG::Vec3d> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_halfface_property<ACG::Vec3d>(PropertyVisualizer::propertyInfo.propName());

    if ( !prop )
        return;

    OpenVolumeMesh::HalfFaceIter hf_begin(OVMPropertyVisualizer<MeshT>::mesh->halffaces_begin()), hf_end(OVMPropertyVisualizer<MeshT>::mesh->halffaces_end());
    for (OpenVolumeMesh::HalfFaceIter hf_it = hf_begin; hf_it != hf_end; ++hf_it){

        ACG::Vec3d center(0.0, 0.0, 0.0);
        int vCount = 0;

        for (OpenVolumeMesh::HalfFaceVertexIter hfv_it = OVMPropertyVisualizer<MeshT>::mesh->hfv_iter(*hf_it); hfv_it; ++hfv_it){
            vCount++;
            center += OVMPropertyVisualizer<MeshT>::mesh->vertex(*hfv_it);
        }

        center /= vCount;

        ACG::Vec3d v  = prop[*hf_it];

        if (vectorWidget->normalize->isChecked() && v.sqrnorm() > 1e-12)
            v.normalize();

        if(vectorWidget->scale->isChecked())
            v *= vectorWidget->scaleBox->value();

        lineNode->add_line( center, (center+v) );
        lineNode->add_color(color);
    }
}

template <typename MeshT>
QString OVMPropertyVisualizerVector<MeshT>::getPropertyText(unsigned int index)
{
    return OVMPropertyVisualizer<MeshT>::template getPropertyText_<ACG::Vec3d>(index);
}

#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */
