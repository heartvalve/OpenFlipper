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

#define OM_PROPERTY_VISUALIZER_BOOLEAN_CC

#include "OMPropertyVisualizerBoolean.hh"

template <typename MeshT>
OMPropertyVisualizerBoolean<MeshT>::OMPropertyVisualizerBoolean(MeshT* _mesh, PropertyInfo _propertyInfo)
    : OMPropertyVisualizer<MeshT>(_mesh, _propertyInfo)
{
    if (PropertyVisualizer::widget) delete PropertyVisualizer::widget;
    BooleanWidget* w = new BooleanWidget();
    w->paramBool->setTitle(QString("Boolean Parameters of ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
    PropertyVisualizer::widget = w;
}

template <typename MeshT>
QString OMPropertyVisualizerBoolean<MeshT>::getPropertyText(unsigned int index)
{
    return OMPropertyVisualizer<MeshT>::template getPropertyText_<bool>(index);
}

template <typename MeshT>
void OMPropertyVisualizerBoolean<MeshT>::visualizeFaceProp()
{
    BooleanWidget* booleanWidget = static_cast<BooleanWidget*>(PropertyVisualizer::widget);
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;
    typename MeshT::Color colorTrue, colorFalse;

    colorTrue  = OMPropertyVisualizer<MeshT>::convertColor(booleanWidget->colorTrue->color());
    colorFalse = OMPropertyVisualizer<MeshT>::convertColor(booleanWidget->colorFalse->color());

    OpenMesh::FPropHandleT< bool > prop;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        return;

    if ( !mesh->has_face_colors() )
        OMPropertyVisualizer<MeshT>::mesh->request_face_colors();

    for (typename MeshT::FaceIter f_it = mesh->faces_begin() ; f_it != mesh->faces_end() ; ++f_it)
        if ( mesh->property(prop, f_it) )
            mesh->set_color(f_it, colorTrue);
        else
            mesh->set_color(f_it, colorFalse);

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);

}

template <typename MeshT>
void OMPropertyVisualizerBoolean<MeshT>::visualizeEdgeProp()
{
    BooleanWidget* booleanWidget = static_cast<BooleanWidget*>(PropertyVisualizer::widget);
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;
    typename MeshT::Color colorTrue, colorFalse;

    colorTrue  = OMPropertyVisualizer<MeshT>::convertColor(booleanWidget->colorTrue->color());
    colorFalse = OMPropertyVisualizer<MeshT>::convertColor(booleanWidget->colorFalse->color());

    OpenMesh::EPropHandleT< bool > prop;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        return;

    if ( !mesh->has_edge_colors() )
        mesh->request_edge_colors();

    for (typename MeshT::EdgeIter e_it = mesh->edges_begin() ; e_it != mesh->edges_end() ; ++e_it)
        if ( mesh->property(prop, e_it) )
            mesh->set_color(e_it, colorTrue);
        else
            mesh->set_color(e_it, colorFalse);

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::EDGES_COLORED);
}

template <typename MeshT>
void OMPropertyVisualizerBoolean<MeshT>::visualizeHalfedgeProp()
{
    BooleanWidget* booleanWidget = static_cast<BooleanWidget*>(PropertyVisualizer::widget);
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;
    typename MeshT::Color colorTrue, colorFalse;

    colorTrue  = OMPropertyVisualizer<MeshT>::convertColor(booleanWidget->colorTrue->color());
    colorFalse = OMPropertyVisualizer<MeshT>::convertColor(booleanWidget->colorFalse->color());

    OpenMesh::HPropHandleT< bool > prop;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        return;

    if ( ! mesh->has_halfedge_colors() )
        mesh->request_halfedge_colors();

    for (typename MeshT::HalfedgeIter he_it = mesh->halfedges_begin() ; he_it != mesh->halfedges_end() ; ++he_it)
        if ( mesh->property(prop, he_it) )
            mesh->set_color(he_it, colorTrue);
        else
            mesh->set_color(he_it, colorFalse);

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::HALFEDGES_COLORED);

}

template <typename MeshT>
void OMPropertyVisualizerBoolean<MeshT>::visualizeVertexProp()
{
    BooleanWidget* booleanWidget = static_cast<BooleanWidget*>(PropertyVisualizer::widget);
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;
    typename MeshT::Color colorTrue, colorFalse;

    colorTrue  = OMPropertyVisualizer<MeshT>::convertColor(booleanWidget->colorTrue->color());
    colorFalse = OMPropertyVisualizer<MeshT>::convertColor(booleanWidget->colorFalse->color());

    OpenMesh::VPropHandleT< bool > prop;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        return;

    if ( ! mesh->has_vertex_colors() )
        mesh->request_vertex_colors();

    for (typename MeshT::VertexIter v_it = mesh->vertices_begin() ; v_it != mesh->vertices_end() ; ++v_it)
        if ( mesh->property(prop, v_it) )
            mesh->set_color(v_it, colorTrue);
        else
            mesh->set_color(v_it, colorFalse);

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);

}

template <typename MeshT>
void OMPropertyVisualizerBoolean<MeshT>::setFacePropertyFromText(unsigned int index, QString text)
{
    OpenMesh::FPropHandleT< bool > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        std::cerr << "Error: No property with name " <<  PropertyVisualizer::propertyInfo.propName().c_str() << std::endl;


    typename MeshT::FaceHandle fh = mesh->face_handle(index);

    mesh->property(prop, fh) = this->strToBool(text);
}

template <typename MeshT>
void OMPropertyVisualizerBoolean<MeshT>::setEdgePropertyFromText(unsigned int index, QString text)
{
    OpenMesh::EPropHandleT< bool > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        std::cerr << "Error: No property with name " <<  PropertyVisualizer::propertyInfo.propName().c_str() << std::endl;


    typename MeshT::EdgeHandle eh = mesh->edge_handle(index);

    mesh->property(prop, eh) = this->strToBool(text);
}

template <typename MeshT>
void OMPropertyVisualizerBoolean<MeshT>::setHalfedgePropertyFromText(unsigned int index, QString text)
{
    OpenMesh::HPropHandleT< bool > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        std::cerr << "Error: No property with name " <<  PropertyVisualizer::propertyInfo.propName().c_str() << std::endl;


    typename MeshT::HalfedgeHandle heh = mesh->halfedge_handle(index);

    mesh->property(prop, heh) = this->strToBool(text);
}

template <typename MeshT>
void OMPropertyVisualizerBoolean<MeshT>::setVertexPropertyFromText(unsigned int index, QString text)
{
    OpenMesh::VPropHandleT< bool > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        std::cerr << "Error: No property with name " <<  PropertyVisualizer::propertyInfo.propName().c_str() << std::endl;


    typename MeshT::VertexHandle vh = mesh->vertex_handle(index);

    mesh->property(prop, vh) = this->strToBool(text);
}

template<typename MeshT>
void OMPropertyVisualizerBoolean<MeshT>::removeProperty()
{
    OMPropertyVisualizer<MeshT>::template removeProperty_stage1<bool>();
}

template<typename MeshT>
void OMPropertyVisualizerBoolean<MeshT>::duplicateProperty()
{
    OMPropertyVisualizer<MeshT>::template duplicateProperty_stage1<bool>();
}
