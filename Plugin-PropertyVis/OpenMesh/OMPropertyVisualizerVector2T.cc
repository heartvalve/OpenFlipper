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
*   $Revision: 18259 $                                                       *
*   $LastChangedBy: ebke $                                                *
*   $Date: 2014-02-25 16:34:42 +0100 (Tue, 25 Feb 2014) $                     *
*                                                                            *
\*===========================================================================*/

#define OM_PROPERTY_VISUALIZER_VECTOR2_CC

#include "OMPropertyVisualizerVector2.hh"
#include <OpenMesh/Core/Utils/Property.hh>

template <typename MeshT, typename VectorType>
OMPropertyVisualizerVector2<MeshT, VectorType>::OMPropertyVisualizerVector2(MeshT* _mesh, PropertyInfo _propertyInfo)
    : OMPropertyVisualizer<MeshT>(_mesh, _propertyInfo)
{
    if (PropertyVisualizer::widget) delete PropertyVisualizer::widget;
    VectorWidget* w = new VectorWidget();
    w->paramVector->setTitle(QString("2D Vector Parameters of ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
    PropertyVisualizer::widget = w;

    lineNode = new ACG::SceneGraph::LineNode( ACG::SceneGraph::LineNode::LineSegmentsMode, dynamic_cast < ACG::SceneGraph::SeparatorNode* >( PluginFunctions::getRootNode() ) );

    if (!_propertyInfo.isFaceProp())
    {
        w->vectors_edges_rb->hide();
    }
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::clear()
{
    lineNode->clear();
    OMPropertyVisualizer<MeshT>::clear();
}

template <typename MeshT, typename VectorType>
QString OMPropertyVisualizerVector2<MeshT, VectorType>::getPropertyText(unsigned int index)
{
    return OMPropertyVisualizer<MeshT>::template getPropertyText_<VectorType>(index);
}


namespace {

template<typename PROPTYPE, typename VectorType, typename MeshT, typename ENTITY_IT, typename PROPINFO_TYPE>
void visualizeVectorAsColorForEntity2(MeshT *mesh, const ENTITY_IT e_begin, const ENTITY_IT e_end,
                                      const PROPINFO_TYPE &propinfo) {
    PROPTYPE prop;
    if (!mesh->get_property_handle(prop, propinfo.propName()))
        throw VizException("Getting PropHandle from mesh for selected property failed.");
    for (ENTITY_IT e_it = e_begin; e_it != e_end; ++e_it) {
        VectorType v = mesh->property(prop, *e_it).normalized() * .5 + VectorType(0.5);
        mesh->set_color(*e_it, typename MeshT::Color(v[0], v[1], 0.0, 1.0));
    }
}

template<typename PROPTYPE, typename MeshT, typename ENTITY_IT, typename PROPINFO_TYPE>
void visualizeVectorLengthAsColorForEntity2(
        MeshT *mesh, const ENTITY_IT e_begin, const ENTITY_IT e_end,
        const PROPINFO_TYPE &propinfo) {
    PROPTYPE prop;
    if (!mesh->get_property_handle(prop, propinfo.propName()))
        throw VizException("Getting PropHandle from mesh for selected "
                "property failed.");

    double min =  std::numeric_limits<double>::infinity();
    double max = -std::numeric_limits<double>::infinity();

    for (ENTITY_IT e_it = e_begin; e_it != e_end; ++e_it) {
        const double val = mesh->property(prop, *e_it).norm();
        min = std::min(min, val);
        max = std::max(max, val);
    }

    ACG::ColorCoder color_coder(min, max);

    for (ENTITY_IT e_it = e_begin; e_it != e_end; ++e_it) {
        mesh->set_color(*e_it, color_coder(mesh->property(prop, *e_it).norm()));
    }
}

template <typename VectorType> ACG::Vec3d generateVec3AtLocation(VectorType uv, ACG::Vec3d normal)
{
    ACG::Vec3d tan;
    if(fabsf(normal[0]) > fabsf(normal[1]))
        tan = ACG::Vec3d(-normal[2], 0, normal[0]);
    else tan = ACG::Vec3d(0, normal[2], -normal[1]);
    ACG::Vec3d bi = normal % tan;
    return double(uv[0]) * tan.normalize() + double(uv[1]) * bi.normalize();
}

}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::visualizeFaceProp(bool _setDrawMode)
{
    VectorWidget* const w = (VectorWidget*)PropertyVisualizer::widget;
    if (w->vectors_edges_rb->isChecked()) {
        visualizeFacePropOnEdges();
    } else if (w->vectors_colors_rb->isChecked() ||
            w->vectors_length_color_rb->isChecked()) {
        if ( !OMPropertyVisualizer<MeshT>::mesh->has_face_colors() )
            OMPropertyVisualizer<MeshT>::mesh->request_face_colors();

        if (w->vectors_colors_rb->isChecked()) {
            visualizeVectorAsColorForEntity2<OpenMesh::FPropHandleT<VectorType>, VectorType >(
                    OMPropertyVisualizer<MeshT>::mesh,
                    OMPropertyVisualizer<MeshT>::mesh->faces_begin(),
                    OMPropertyVisualizer<MeshT>::mesh->faces_end(),
                    PropertyVisualizer::propertyInfo);
        } else {
            visualizeVectorLengthAsColorForEntity2<OpenMesh::FPropHandleT<VectorType> >(
                    OMPropertyVisualizer<MeshT>::mesh,
                    OMPropertyVisualizer<MeshT>::mesh->faces_begin(),
                    OMPropertyVisualizer<MeshT>::mesh->faces_end(),
                    PropertyVisualizer::propertyInfo);
        }
        if (_setDrawMode)
            PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);
    }
    else visualizeFacePropAsStrokes();
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::visualizeEdgeProp(bool _setDrawMode)
{
    VectorWidget* const w = (VectorWidget*)PropertyVisualizer::widget;
    if (w->vectors_colors_rb->isChecked() ||
            w->vectors_length_color_rb->isChecked()) {
        if ( !OMPropertyVisualizer<MeshT>::mesh->has_edge_colors() )
            OMPropertyVisualizer<MeshT>::mesh->request_edge_colors();
        MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;
        if ( !mesh->has_edge_colors() )
            mesh->request_edge_colors();
        if (w->vectors_colors_rb->isChecked()) {
            visualizeVectorAsColorForEntity2<
                OpenMesh::EPropHandleT<VectorType>, VectorType >(
                        mesh,
                        mesh->edges_begin(),
                        mesh->edges_end(),
                        PropertyVisualizer::propertyInfo);
        } else {
            visualizeVectorLengthAsColorForEntity2<
                OpenMesh::EPropHandleT<VectorType> >(
                        mesh,
                        mesh->edges_begin(),
                        mesh->edges_end(),
                        PropertyVisualizer::propertyInfo);
        }
        if (_setDrawMode)
            PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::EDGES_COLORED);
    }
    else visualizeEdgePropAsStrokes();
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::visualizeHalfedgeProp(bool _setDrawMode)
{
    VectorWidget* const w = (VectorWidget*)PropertyVisualizer::widget;
    if (w->vectors_colors_rb->isChecked() ||
            w->vectors_length_color_rb->isChecked()) {
        if ( !OMPropertyVisualizer<MeshT>::mesh->has_halfedge_colors() )
            OMPropertyVisualizer<MeshT>::mesh->request_halfedge_colors();
        MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;
        if ( ! mesh->has_halfedge_colors() )
            mesh->request_halfedge_colors();

        if (w->vectors_colors_rb->isChecked()) {
            visualizeVectorAsColorForEntity2<
                OpenMesh::HPropHandleT<VectorType>, VectorType >(
                        mesh,
                        mesh->halfedges_begin(),
                        mesh->halfedges_end(),
                        PropertyVisualizer::propertyInfo);
        } else {
            visualizeVectorLengthAsColorForEntity2<
                OpenMesh::HPropHandleT<VectorType> >(
                        mesh,
                        mesh->halfedges_begin(),
                        mesh->halfedges_end(),
                        PropertyVisualizer::propertyInfo);
        }

        if (_setDrawMode)
            PluginFunctions::setDrawMode(
                    ACG::SceneGraph::DrawModes::HALFEDGES_COLORED);
    }
    else visualizeHalfedgePropAsStrokes();
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::visualizeVertexProp(bool _setDrawMode)
{
    VectorWidget* const w = (VectorWidget*)PropertyVisualizer::widget;
    if (w->vectors_colors_rb->isChecked() ||
            w->vectors_length_color_rb->isChecked()) {
        if ( !OMPropertyVisualizer<MeshT>::mesh->has_vertex_colors() )
            OMPropertyVisualizer<MeshT>::mesh->request_vertex_colors();

        if (w->vectors_colors_rb->isChecked()) {
            visualizeVectorAsColorForEntity2<
                OpenMesh::VPropHandleT<VectorType>, VectorType >(
                    OMPropertyVisualizer<MeshT>::mesh,
                    OMPropertyVisualizer<MeshT>::mesh->vertices_begin(),
                    OMPropertyVisualizer<MeshT>::mesh->vertices_end(),
                    PropertyVisualizer::propertyInfo);
        } else {
            visualizeVectorLengthAsColorForEntity2<
                OpenMesh::VPropHandleT<VectorType> >(
                    OMPropertyVisualizer<MeshT>::mesh,
                    OMPropertyVisualizer<MeshT>::mesh->vertices_begin(),
                    OMPropertyVisualizer<MeshT>::mesh->vertices_end(),
                    PropertyVisualizer::propertyInfo);
        }
        if (_setDrawMode)
            PluginFunctions::setDrawMode(
                    ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);
    }
    else visualizeVertexPropAsStrokes();
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::visualizeFacePropOnEdges() {
    VectorWidget* w = (VectorWidget*)PropertyVisualizer::widget;
    MeshT* _mesh = OMPropertyVisualizer<MeshT>::mesh;


    const double thresh_1 = w->vectors_edges_alpha->value();
    const double thresh_2 = std::min(thresh_1, w->vectors_edges_alpha->value());

    OpenMesh::FPropHandleT<VectorType> prop;
    if (!_mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName()))
        throw VizException("Getting PropHandle from mesh for selected property failed.");

    if (!_mesh->has_edge_colors())
        _mesh->request_edge_colors();
    const ACG::Vec4f cold(0, 0, 0, 1.0), hot(0, 1, 0, 1.0), degen(1, 1, 0, 1.0);
    for (typename MeshT::EdgeIter e_it = _mesh->edges_begin(), e_end = _mesh->edges_end();
            e_it != e_end; ++e_it) {
        VectorType p1 = _mesh->property(prop, _mesh->face_handle(_mesh->halfedge_handle(*e_it, 0)));
        VectorType p2 = _mesh->property(prop, _mesh->face_handle(_mesh->halfedge_handle(*e_it, 1)));

        ACG::Vec4f color;

        const char degenerate = ((p1.sqrnorm() < 1e-6) ? 1 : 0) | ((p2.sqrnorm() < 1e-6) ? 2 : 0);
        if (degenerate == 3) {
            color = cold;
        } else if (degenerate == 0) {
            p1.normalize(); p2.normalize();
            const double alpha = std::min(1.0, double(std::abs(p1 | p2)));
            if (alpha < thresh_1)
                color = hot;
            else if (alpha > thresh_2)
                color = cold;
            else {
                const double beta = (alpha - thresh_1) / (thresh_2 - thresh_1);
                color = cold * beta + hot * (1.0 - beta);
            }
        } else {
            color = degen;
        }
        _mesh->set_color(*e_it, color);
    }
    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED | ACG::SceneGraph::DrawModes::EDGES_COLORED);
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::visualizeFacePropAsStrokes()
{
    VectorWidget* vectorWidget = static_cast<VectorWidget*>(PropertyVisualizer::widget);

    lineNode->clear();

    typename MeshT::Color color = OMPropertyVisualizer<MeshT>::convertColor(vectorWidget->lineColor->color());

    OpenMesh::FPropHandleT< VectorType > prop;

    if ( !OMPropertyVisualizer<MeshT>::mesh->get_property_handle(prop, OMPropertyVisualizer<MeshT>::propertyInfo.propName() ) )
      return;

    for (typename MeshT::FaceIter f_it = OMPropertyVisualizer<MeshT>::mesh->faces_begin() ; f_it != OMPropertyVisualizer<MeshT>::mesh->faces_end() ; ++f_it){

        typename MeshT::Point center(0.0, 0.0, 0.0);
      int vCount = 0;

      for (typename MeshT::FaceVertexIter fv_it(*(OMPropertyVisualizer<MeshT>::mesh),*f_it); fv_it.is_valid(); ++fv_it){
        vCount++;
        center += OMPropertyVisualizer<MeshT>::mesh->point(*fv_it);
      }

      center /= vCount;

      VectorType v  = (OMPropertyVisualizer<MeshT>::mesh->property(prop, *f_it));
      ACG::Vec3d normal = OMPropertyVisualizer<MeshT>::mesh->normal(*f_it);

      if (vectorWidget->normalize->isChecked() && v.sqrnorm() > 1e-12)
        v.normalize();

      if(vectorWidget->scale->isChecked())
        v *= vectorWidget->scaleBox->value();

      lineNode->add_line( center, (center+generateVec3AtLocation<VectorType>(v, normal)) );
      lineNode->add_color(color);
    }
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::visualizeEdgePropAsStrokes()
{
    VectorWidget* vectorWidget = static_cast<VectorWidget*>(PropertyVisualizer::widget);

    lineNode->clear();

    typename MeshT::Color color = OMPropertyVisualizer<MeshT>::convertColor(vectorWidget->lineColor->color());

    //TODO check if this also works if the property is Vec3f
    OpenMesh::EPropHandleT< VectorType > prop;

    if ( !OMPropertyVisualizer<MeshT>::mesh->get_property_handle(prop, OMPropertyVisualizer<MeshT>::propertyInfo.propName() ) )
      return;

    for (typename MeshT::EdgeIter e_it = OMPropertyVisualizer<MeshT>::mesh->edges_begin() ; e_it != OMPropertyVisualizer<MeshT>::mesh->edges_end() ; ++e_it){

      typename MeshT::HalfedgeHandle hh = OMPropertyVisualizer<MeshT>::mesh->halfedge_handle( *e_it, 0 );

      typename MeshT::VertexHandle vh0 = OMPropertyVisualizer<MeshT>::mesh->from_vertex_handle( hh );
      typename MeshT::VertexHandle vh1 = OMPropertyVisualizer<MeshT>::mesh->to_vertex_handle( hh );

      typename MeshT::Point v1 = OMPropertyVisualizer<MeshT>::mesh->point(vh0) + 0.5 * (OMPropertyVisualizer<MeshT>::mesh->point(vh1) - OMPropertyVisualizer<MeshT>::mesh->point(vh0));
      VectorType v  = OMPropertyVisualizer<MeshT>::mesh->property(prop, *e_it);
      ACG::Vec3d normal = OMPropertyVisualizer<MeshT>::mesh->normal(OMPropertyVisualizer<MeshT>::mesh->halfedge_handle(*e_it, 0));

      if (vectorWidget->normalize->isChecked() && v.sqrnorm() > 1e-12)
        v.normalize();

      if(vectorWidget->scale->isChecked())
        v *= vectorWidget->scaleBox->value();

      lineNode->add_line( v1, (v1+generateVec3AtLocation<VectorType>(v, normal)) );
      lineNode->add_color(color);
    }
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::visualizeHalfedgePropAsStrokes()
{
    VectorWidget* vectorWidget = static_cast<VectorWidget*>(PropertyVisualizer::widget);

    lineNode->clear();

    typename MeshT::Color color = OMPropertyVisualizer<MeshT>::convertColor(vectorWidget->lineColor->color());

    //TODO check if this also works if the property is Vec3f
    OpenMesh::HPropHandleT< VectorType > prop;

    if ( !OMPropertyVisualizer<MeshT>::mesh->get_property_handle(prop, OMPropertyVisualizer<MeshT>::propertyInfo.propName() ) )
      return;

    for (typename MeshT::HalfedgeIter he_it = OMPropertyVisualizer<MeshT>::mesh->halfedges_begin() ; he_it != OMPropertyVisualizer<MeshT>::mesh->halfedges_end() ; ++he_it){

      typename MeshT::VertexHandle vh0 = OMPropertyVisualizer<MeshT>::mesh->from_vertex_handle( *he_it );
      typename MeshT::VertexHandle vh1 = OMPropertyVisualizer<MeshT>::mesh->to_vertex_handle( *he_it );

      typename MeshT::Point v1 = OMPropertyVisualizer<MeshT>::mesh->point(vh0) + 0.5 * (OMPropertyVisualizer<MeshT>::mesh->point(vh1) - OMPropertyVisualizer<MeshT>::mesh->point(vh0));
      VectorType v  = OMPropertyVisualizer<MeshT>::mesh->property(prop, *he_it);
      ACG::Vec3d normal = OMPropertyVisualizer<MeshT>::mesh->normal(*he_it);

      if (vectorWidget->normalize->isChecked() && v.sqrnorm() > 1e-12)
        v.normalize();

      if(vectorWidget->scale->isChecked())
        v *= vectorWidget->scaleBox->value();

      lineNode->add_line( v1, (v1+generateVec3AtLocation<VectorType>(v, normal)) );
      lineNode->add_color(color);
    }
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::visualizeVertexPropAsStrokes()
{
    VectorWidget* vectorWidget = static_cast<VectorWidget*>(PropertyVisualizer::widget);

    lineNode->clear();

    typename MeshT::Color color = OMPropertyVisualizer<MeshT>::convertColor(vectorWidget->lineColor->color());

    //TODO check if this also works if the property is Vec3f
    OpenMesh::VPropHandleT< VectorType > prop;

    if ( !OMPropertyVisualizer<MeshT>::mesh->get_property_handle(prop, OMPropertyVisualizer<MeshT>::propertyInfo.propName() ) )
      return;

    for (typename MeshT::VertexIter v_it = OMPropertyVisualizer<MeshT>::mesh->vertices_begin() ; v_it != OMPropertyVisualizer<MeshT>::mesh->vertices_end() ; ++v_it){

        typename MeshT::Point v1 = OMPropertyVisualizer<MeshT>::mesh->point( *v_it );
      VectorType v  = OMPropertyVisualizer<MeshT>::mesh->property(prop, *v_it);
      ACG::Vec3d normal = OMPropertyVisualizer<MeshT>::mesh->normal(*v_it);

      if (vectorWidget->normalize->isChecked() && v.sqrnorm() > 1e-12)
        v.normalize();

      if(vectorWidget->scale->isChecked())
        v *= vectorWidget->scaleBox->value();

      lineNode->add_line( v1, (v1+generateVec3AtLocation<VectorType>(v, normal)) );
      lineNode->add_color(color);
    }
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::setFacePropertyFromText(unsigned int index, QString text)
{
    OpenMesh::FPropHandleT< VectorType > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));


    typename MeshT::FaceHandle fh = mesh->face_handle(index);

    mesh->property(prop, fh) = this->strToVec2f(text);
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::setEdgePropertyFromText(unsigned int index, QString text)
{
    OpenMesh::EPropHandleT< VectorType > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));


    typename MeshT::EdgeHandle eh = mesh->edge_handle(index);

    mesh->property(prop, eh) = this->strToVec2f(text);
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::setHalfedgePropertyFromText(unsigned int index, QString text)
{
    OpenMesh::HPropHandleT< VectorType > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));


    typename MeshT::HalfedgeHandle heh = mesh->halfedge_handle(index);

    mesh->property(prop, heh) = this->strToVec2f(text);
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::setVertexPropertyFromText(unsigned int index, QString text)
{
    OpenMesh::VPropHandleT< VectorType> prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));


    typename MeshT::VertexHandle vh = mesh->vertex_handle(index);

    mesh->property(prop, vh) = this->strToVec2f(text);
}


template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::removeProperty()
{
    OMPropertyVisualizer<MeshT>::template removeProperty_stage1<VectorType>();
}

template <typename MeshT, typename VectorType>
void OMPropertyVisualizerVector2<MeshT, VectorType>::duplicateProperty()
{
    OMPropertyVisualizer<MeshT>::template duplicateProperty_stage1<VectorType>();
}

