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

#define OM_PROPERTY_VISUALIZER_DOUBLE_CC

#include "OMPropertyVisualizerDouble.hh"

template <typename MeshT>
OMPropertyVisualizerDouble<MeshT>::OMPropertyVisualizerDouble(MeshT* _mesh, PropertyInfo _propertyInfo)
    : OMPropertyVisualizer<MeshT>(_mesh, _propertyInfo)
{
    if (PropertyVisualizer::widget) delete PropertyVisualizer::widget;
    DoubleWidget* w = new DoubleWidget();
    w->paramDouble->setTitle(QString("Double Parameters of ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
    PropertyVisualizer::widget = w;
}

template <typename MeshT>
void OMPropertyVisualizerDouble<MeshT>::visualizeFaceProp(bool _setDrawMode)
{
    DoubleWidget* doubleWidget = static_cast<DoubleWidget*>(PropertyVisualizer::widget);
    typename MeshT::Color colorMin, colorMax;

    colorMin = OMPropertyVisualizer<MeshT>::convertColor(doubleWidget->doubleMin->color());
    colorMax = OMPropertyVisualizer<MeshT>::convertColor(doubleWidget->doubleMax->color());

    // color coder in [0,1]
    ACG::ColorCoder cc;

    OpenMesh::FPropHandleT< double > prop;

    if ( !OMPropertyVisualizer<MeshT>::mesh->get_property_handle(prop, OMPropertyVisualizer<MeshT>::propertyInfo.propName() ) )
        return;

    double min, max;

    if ( doubleWidget->doubleAbsolute->isChecked() ){
        min = FLT_MAX;
        max = 0.0;
    } else {
        min = FLT_MAX;
        max = FLT_MIN;
    }

    for (typename MeshT::FaceIter f_it = OMPropertyVisualizer<MeshT>::mesh->faces_begin() ; f_it != OMPropertyVisualizer<MeshT>::mesh->faces_end() ; ++f_it){
        if ( doubleWidget->doubleAbsolute->isChecked() ){
            min = std::min( min, fabs(OMPropertyVisualizer<MeshT>::mesh->property(prop, f_it)));
            max = std::max( max, fabs(OMPropertyVisualizer<MeshT>::mesh->property(prop, f_it)));
        } else {
            min = std::min( min, OMPropertyVisualizer<MeshT>::mesh->property(prop, f_it));
            max = std::max( max, OMPropertyVisualizer<MeshT>::mesh->property(prop, f_it));
        }
    }

    // fixed range?
    if( doubleWidget->doubleFixedRange->isChecked())
    {
        min = doubleWidget->doubleFixedRangeMin->value();
        max = doubleWidget->doubleFixedRangeMax->value();
    }
    else
    {
        doubleWidget->doubleFixedRangeMin->setValue(min);
        doubleWidget->doubleFixedRangeMax->setValue(max);
    }

    double range = max - min;

    if ( ! OMPropertyVisualizer<MeshT>::mesh->has_face_colors() )
        OMPropertyVisualizer<MeshT>::mesh->request_face_colors();

    for (typename MeshT::FaceIter f_it = OMPropertyVisualizer<MeshT>::mesh->faces_begin() ; f_it != OMPropertyVisualizer<MeshT>::mesh->faces_end() ; ++f_it){

        if (range == 0.0)
          OMPropertyVisualizer<MeshT>::mesh->set_color(f_it, colorMin);
        else
        {
            double v = OMPropertyVisualizer<MeshT>::mesh->property(prop, f_it);

            // absolut value?
            if ( doubleWidget->doubleAbsolute->isChecked())
                v = fabs(v);

            // clamping
            v = std::max(min,v);
            v = std::min(max,v);

            double t = (v-min)/range;

            typename MeshT::Color color;

            if( doubleWidget->doubleColorCoder->isChecked())
                color = cc.color_float4(t);
            else
                color = (colorMin)*(1.0-t) + (colorMax)*t;

              // set color
            OMPropertyVisualizer<MeshT>::mesh->set_color(f_it, color);
        }
    }

    if (_setDrawMode)
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);
}

template <typename MeshT>
void OMPropertyVisualizerDouble<MeshT>::visualizeEdgeProp(bool _setDrawMode)
{
    DoubleWidget* doubleWidget = static_cast<DoubleWidget*>(PropertyVisualizer::widget);
    typename MeshT::Color colorMin, colorMax;

    colorMin = OMPropertyVisualizer<MeshT>::convertColor(doubleWidget->doubleMin->color());
    colorMax = OMPropertyVisualizer<MeshT>::convertColor(doubleWidget->doubleMax->color());

    // color coder in [0,1]
    ACG::ColorCoder cc;



    //TODO check if this also works if the property is Vec3d
    OpenMesh::EPropHandleT< double > prop;

    if ( !OMPropertyVisualizer<MeshT>::mesh->get_property_handle(prop, OMPropertyVisualizer<MeshT>::propertyInfo.propName() ) )
        return;

    double min, max;

    if ( doubleWidget->doubleAbsolute->isChecked() ){
        min = FLT_MAX;
        max = 0.0;
    } else {
        min = FLT_MAX;
        max = FLT_MIN;
    }

    for (typename MeshT::EdgeIter e_it = OMPropertyVisualizer<MeshT>::mesh->edges_begin() ; e_it != OMPropertyVisualizer<MeshT>::mesh->edges_end() ; ++e_it){
        if ( doubleWidget->doubleAbsolute->isChecked() ){
            min = std::min( min, fabs(OMPropertyVisualizer<MeshT>::mesh->property(prop, e_it)));
            max = std::max( max, fabs(OMPropertyVisualizer<MeshT>::mesh->property(prop, e_it)));
        } else {
            min = std::min( min, OMPropertyVisualizer<MeshT>::mesh->property(prop, e_it));
            max = std::max( max, OMPropertyVisualizer<MeshT>::mesh->property(prop, e_it));
        }
    }

    // fixed range?
    if( doubleWidget->doubleFixedRange->isChecked())
    {
        min = doubleWidget->doubleFixedRangeMin->value();
        max = doubleWidget->doubleFixedRangeMax->value();
    }
    else
    {
        doubleWidget->doubleFixedRangeMin->setValue(min);
        doubleWidget->doubleFixedRangeMax->setValue(max);
    }


    double range = max - min;

    if ( ! OMPropertyVisualizer<MeshT>::mesh->has_edge_colors() )
        OMPropertyVisualizer<MeshT>::mesh->request_edge_colors();

    for (typename MeshT::EdgeIter e_it = OMPropertyVisualizer<MeshT>::mesh->edges_begin() ; e_it != OMPropertyVisualizer<MeshT>::mesh->edges_end() ; ++e_it){

        if (range == 0.0)
            OMPropertyVisualizer<MeshT>::mesh->set_color(e_it, colorMin);
        else {

            double v = OMPropertyVisualizer<MeshT>::mesh->property(prop, e_it);

            // absolut value?
            if ( doubleWidget->doubleAbsolute->isChecked())
            v = fabs(v);

            // clamping
            v = std::max(min,v);
            v = std::min(max,v);

            double t = (v-min)/range;

            typename MeshT::Color color;

            if( doubleWidget->doubleColorCoder->isChecked())
                color = cc.color_float4(t);
            else
                color = (colorMin)*(1.0-t) + (colorMax)*t;

            // set color
            OMPropertyVisualizer<MeshT>::mesh->set_color(e_it, color);
        }
    }


    if (_setDrawMode)
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::EDGES_COLORED);

}

template <typename MeshT>
void OMPropertyVisualizerDouble<MeshT>::visualizeHalfedgeProp(bool _setDrawMode)
{
    DoubleWidget* doubleWidget = static_cast<DoubleWidget*>(PropertyVisualizer::widget);
    typename MeshT::Color colorMin, colorMax;

    colorMin = OMPropertyVisualizer<MeshT>::convertColor(doubleWidget->doubleMin->color());
    colorMax = OMPropertyVisualizer<MeshT>::convertColor(doubleWidget->doubleMax->color());

    // color coder in [0,1]
    ACG::ColorCoder cc;


    //TODO check if this also works if the property is Vec3d
    OpenMesh::HPropHandleT< double > prop;

    if ( !OMPropertyVisualizer<MeshT>::mesh->get_property_handle(prop, OMPropertyVisualizer<MeshT>::propertyInfo.propName() ) )
        return;

    double min, max;

    if ( doubleWidget->doubleAbsolute->isChecked() ){
        min = FLT_MAX;
        max = 0.0;
    } else {
        min = FLT_MAX;
        max = FLT_MIN;
    }

    for (typename MeshT::HalfedgeIter he_it = OMPropertyVisualizer<MeshT>::mesh->halfedges_begin() ; he_it != OMPropertyVisualizer<MeshT>::mesh->halfedges_end() ; ++he_it){
        if ( doubleWidget->doubleAbsolute->isChecked() ){
            min = std::min( min, fabs(OMPropertyVisualizer<MeshT>::mesh->property(prop, he_it)));
            max = std::max( max, fabs(OMPropertyVisualizer<MeshT>::mesh->property(prop, he_it)));
        } else {
            min = std::min( min, OMPropertyVisualizer<MeshT>::mesh->property(prop, he_it));
            max = std::max( max, OMPropertyVisualizer<MeshT>::mesh->property(prop, he_it));
        }
    }

    // fixed range?
    if( doubleWidget->doubleFixedRange->isChecked())
    {
        min = doubleWidget->doubleFixedRangeMin->value();
        max = doubleWidget->doubleFixedRangeMax->value();
    }
    else
    {
        doubleWidget->doubleFixedRangeMin->setValue(min);
        doubleWidget->doubleFixedRangeMax->setValue(max);
    }

    double range = max - min;

    if ( ! OMPropertyVisualizer<MeshT>::mesh->has_halfedge_colors() )
        OMPropertyVisualizer<MeshT>::mesh->request_halfedge_colors();

    for (typename MeshT::HalfedgeIter he_it = OMPropertyVisualizer<MeshT>::mesh->halfedges_begin() ; he_it != OMPropertyVisualizer<MeshT>::mesh->halfedges_end() ; ++he_it){

        if (range == 0.0)
            OMPropertyVisualizer<MeshT>::mesh->set_color(he_it, colorMin);
        else {

            double v = OMPropertyVisualizer<MeshT>::mesh->property(prop, he_it);

            // absolut value?
            if ( doubleWidget->doubleAbsolute->isChecked())
            v = fabs(v);

            // clamping
            v = std::max(min,v);
            v = std::min(max,v);

            double t = (v-min)/range;

            typename MeshT::Color color;

            if( doubleWidget->doubleColorCoder->isChecked())
                color = cc.color_float4(t);
            else {
                color = (colorMin)*(1.0-t) + (colorMax)*t;
            }

            // set color
            OMPropertyVisualizer<MeshT>::mesh->set_color(he_it, color);
        }
    }
    if (_setDrawMode)
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::HALFEDGES_COLORED);
}

template <typename MeshT>
void OMPropertyVisualizerDouble<MeshT>::visualizeVertexProp(bool _setDrawMode)
{
    DoubleWidget* doubleWidget = static_cast<DoubleWidget*>(PropertyVisualizer::widget);
    typename MeshT::Color colorMin, colorMax;

    colorMin = OMPropertyVisualizer<MeshT>::convertColor(doubleWidget->doubleMin->color());
    colorMax = OMPropertyVisualizer<MeshT>::convertColor(doubleWidget->doubleMax->color());

    // color coder in [0,1]
    ACG::ColorCoder cc;

    //TODO check if this also works if the property is Vec3d
    OpenMesh::VPropHandleT< double > prop;

    if ( !OMPropertyVisualizer<MeshT>::mesh->get_property_handle(prop, OMPropertyVisualizer<MeshT>::propertyInfo.propName() ) )
        return;

    double min, max;

    if ( doubleWidget->doubleAbsolute->isChecked() ){
        min = FLT_MAX;
        max = 0.0;
    } else {
        min = FLT_MAX;
        max = FLT_MIN;
    }

    for (typename MeshT::VertexIter v_it = OMPropertyVisualizer<MeshT>::mesh->vertices_begin() ; v_it != OMPropertyVisualizer<MeshT>::mesh->vertices_end() ; ++v_it){
        if ( doubleWidget->doubleAbsolute->isChecked() ){
            min = std::min( min, fabs(OMPropertyVisualizer<MeshT>::mesh->property(prop, v_it)));
            max = std::max( max, fabs(OMPropertyVisualizer<MeshT>::mesh->property(prop, v_it)));
        } else {
            min = std::min( min, OMPropertyVisualizer<MeshT>::mesh->property(prop, v_it));
            max = std::max( max, OMPropertyVisualizer<MeshT>::mesh->property(prop, v_it));
        }
    }

    // fixed range?
    if( doubleWidget->doubleFixedRange->isChecked())
    {
        min = doubleWidget->doubleFixedRangeMin->value();
        max = doubleWidget->doubleFixedRangeMax->value();
    }
    else
    {
        doubleWidget->doubleFixedRangeMin->setValue(min);
        doubleWidget->doubleFixedRangeMax->setValue(max);
    }

    const double range = max - min;

    if ( ! OMPropertyVisualizer<MeshT>::mesh->has_vertex_colors() )
        OMPropertyVisualizer<MeshT>::mesh->request_vertex_colors();

    for (typename MeshT::VertexIter v_it = OMPropertyVisualizer<MeshT>::mesh->vertices_begin() ; v_it != OMPropertyVisualizer<MeshT>::mesh->vertices_end() ; ++v_it){

        if (range == 0.0)
            OMPropertyVisualizer<MeshT>::mesh->set_color(v_it, colorMin);
        else {

            double v = OMPropertyVisualizer<MeshT>::mesh->property(prop, v_it);

            // absolut value?
            if ( doubleWidget->doubleAbsolute->isChecked())
            v = fabs(v);

            // clamping
            v = std::max(min,v);
            v = std::min(max,v);

            double t = (v-min)/range;

            typename MeshT::Color color;

            if( doubleWidget->doubleColorCoder->isChecked())
                color = cc.color_float4(t);
            else {
                color = (colorMin)*(1.0-t) + (colorMax)*t;
            }

            // set color
            OMPropertyVisualizer<MeshT>::mesh->set_color(v_it, color);
        }
    }

    if (_setDrawMode)
        PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);
}

template <typename MeshT>
void OMPropertyVisualizerDouble<MeshT>::setFacePropertyFromText(unsigned int index, QString text)
{
    OpenMesh::FPropHandleT< double > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));


    typename MeshT::FaceHandle fh = mesh->face_handle(index);

    mesh->property(prop, fh) = this->strToDouble(text);
}

template <typename MeshT>
void OMPropertyVisualizerDouble<MeshT>::setEdgePropertyFromText(unsigned int index, QString text)
{
    OpenMesh::EPropHandleT< double > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));


    typename MeshT::EdgeHandle eh = mesh->edge_handle(index);

    mesh->property(prop, eh) = this->strToDouble(text);
}

template <typename MeshT>
void OMPropertyVisualizerDouble<MeshT>::setHalfedgePropertyFromText(unsigned int index, QString text)
{
    OpenMesh::HPropHandleT< double > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));


    typename MeshT::HalfedgeHandle heh = mesh->halfedge_handle(index);

    mesh->property(prop, heh) = this->strToDouble(text);
}

template <typename MeshT>
void OMPropertyVisualizerDouble<MeshT>::setVertexPropertyFromText(unsigned int index, QString text)
{
    OpenMesh::VPropHandleT< double > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));


    typename MeshT::VertexHandle vh = mesh->vertex_handle(index);

    mesh->property(prop, vh) = this->strToDouble(text);
}


template<typename MeshT>
void OMPropertyVisualizerDouble<MeshT>::removeProperty()
{
    OMPropertyVisualizer<MeshT>::template removeProperty_stage1<double>();
}

template<typename MeshT>
void OMPropertyVisualizerDouble<MeshT>::duplicateProperty()
{
    OMPropertyVisualizer<MeshT>::template duplicateProperty_stage1<double>();
}

