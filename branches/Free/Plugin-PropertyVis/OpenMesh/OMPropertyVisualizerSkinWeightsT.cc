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


#ifdef ENABLE_SKELETON_SUPPORT

#define OM_PROPERTY_VISUALIZER_SKIN_WEIGHTS_CC

#include "OMPropertyVisualizerSkinWeights.hh"

template <typename MeshT>
OMPropertyVisualizerSkinWeights<MeshT>::OMPropertyVisualizerSkinWeights(MeshT* _mesh, PropertyInfo _propertyInfo)
    : OMPropertyVisualizer<MeshT>(_mesh, _propertyInfo)
{
    if (PropertyVisualizer::widget) delete PropertyVisualizer::widget;
    SkinWeightsWidget* w = new SkinWeightsWidget();
    w->paramSkinWeights->setTitle(QString("SkinWeights Parameters of ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
    PropertyVisualizer::widget = w;
}

template <typename MeshT>
QString OMPropertyVisualizerSkinWeights<MeshT>::getPropertyText(unsigned int index)
{
    return OMPropertyVisualizer<MeshT>::template getPropertyText_<BaseSkin::SkinWeights>(index);
}

template <typename MeshT>
void OMPropertyVisualizerSkinWeights<MeshT>::visualizeFaceProp()
{
    OMPropertyVisualizer<MeshT>::log("Visualizing SkinWeights for faces not supported.");
}

template <typename MeshT>
void OMPropertyVisualizerSkinWeights<MeshT>::visualizeEdgeProp()
{
    OMPropertyVisualizer<MeshT>::log("Visualizing SkinWeights for edges not supported.");
}

template <typename MeshT>
void OMPropertyVisualizerSkinWeights<MeshT>::visualizeHalfedgeProp()
{
    OMPropertyVisualizer<MeshT>::log("Visualizing SkinWeights for halfedges not supported.");
}

template <typename MeshT>
void OMPropertyVisualizerSkinWeights<MeshT>::visualizeVertexProp()
{
    SkinWeightsWidget* skinWeightsWidget = static_cast<SkinWeightsWidget*>(PropertyVisualizer::widget);

    OpenMesh::VPropHandleT< BaseSkin::SkinWeights  > prop;

    if ( !OMPropertyVisualizer<MeshT>::mesh->get_property_handle(prop, OMPropertyVisualizer<MeshT>::propertyInfo.propName() ) )
        return;

    // Request vertex colors if required
    if ( !OMPropertyVisualizer<MeshT>::mesh->has_vertex_colors() )
        OMPropertyVisualizer<MeshT>::mesh->request_vertex_colors();

    for (typename MeshT::VertexIter v_it = OMPropertyVisualizer<MeshT>::mesh->vertices_begin() ; v_it != OMPropertyVisualizer<MeshT>::mesh->vertices_end() ; ++v_it){

        BaseSkin::SkinWeights weight = OMPropertyVisualizer<MeshT>::mesh->property(prop, v_it);

        double value = 0.0;
        if ( weight.find( skinWeightsWidget->boneId->value() ) != weight.end() ) {
            value = weight[skinWeightsWidget->boneId->value()];
        }


        typename MeshT::Color color(value,1.0-value,0.0,1.0);

        // set color
        OMPropertyVisualizer<MeshT>::mesh->set_color(v_it, color);
    }

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_POINTS_COLORED);
}

template<typename MeshT>
void OMPropertyVisualizerSkinWeights<MeshT>::removeProperty()
{
    OMPropertyVisualizer<MeshT>::template removeProperty_stage1<BaseSkin::SkinWeights>();
}


template<typename MeshT>
void OMPropertyVisualizerSkinWeights<MeshT>::duplicateProperty()
{
    OMPropertyVisualizer<MeshT>::template duplicateProperty_stage1<BaseSkin::SkinWeights>();
}

template<typename MeshT>
void OMPropertyVisualizerSkinWeights<MeshT>::setFacePropertyFromText(unsigned int index, QString text)
{
    OMPropertyVisualizer<MeshT>::log("Setting SkinWeights for faces not supported.");
}

template<typename MeshT>
void OMPropertyVisualizerSkinWeights<MeshT>::setEdgePropertyFromText(unsigned int index, QString text)
{
    OMPropertyVisualizer<MeshT>::log("Setting SkinWeights for edges not supported.");
}

template<typename MeshT>
void OMPropertyVisualizerSkinWeights<MeshT>::setHalfedgePropertyFromText(unsigned int index, QString text)
{
    OMPropertyVisualizer<MeshT>::log("Setting SkinWeights for halfedges not supported.");
}

template<typename MeshT>
void OMPropertyVisualizerSkinWeights<MeshT>::setVertexPropertyFromText(unsigned int index, QString text)
{
    OpenMesh::VPropHandleT< BaseSkin::SkinWeights > prop;
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;

    if ( !mesh->get_property_handle(prop, PropertyVisualizer::propertyInfo.propName() ) )
        std::cerr << "Error: No property with name " <<  PropertyVisualizer::propertyInfo.propName().c_str() << std::endl;

    typename MeshT::VertexHandle vh = mesh->vertex_handle(index);

    BaseSkin::SkinWeights sw;

    text.remove(0,1);
    text.remove(text.length()-1, 1);

    QStringList slAll = text.split(")(");

    for (QStringList::iterator it = slAll.begin(); it != slAll.end(); ++it)
    {
        QStringList slCurrent = it->split(",");
        sw.insert(std::pair<int, double>(slCurrent.at(0).toInt(), slCurrent.at(1).toDouble()));
    }

    mesh->property(prop, vh) = sw;
}

#endif /* ENABLE_SKELETON_SUPPORT */
