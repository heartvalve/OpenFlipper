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

#ifdef ENABLE_OPENVOLUMEMESH

#define OVM_PROPERTY_VISUALIZER_DOUBLE_CC

#include "OVMPropertyVisualizerDouble.hh"

template <typename MeshT>
OVMPropertyVisualizerDouble<MeshT>::OVMPropertyVisualizerDouble(MeshT* _mesh, int objectID, PropertyInfo _propertyInfo)
    : OVMPropertyVisualizer<MeshT>(_mesh, objectID, _propertyInfo)
{
    if (PropertyVisualizer::widget) delete PropertyVisualizer::widget;
    DoubleWidget* w = new DoubleWidget();
    w->paramDouble->setTitle(QString("Double Parameters of ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
    PropertyVisualizer::widget = w;
}

template <typename MeshT>
template <typename PropType, typename EntityIterator>
void OVMPropertyVisualizerDouble<MeshT>::visualizeProp(PropType prop, EntityIterator e_begin, EntityIterator e_end)
{
    if (!prop) return;

    DoubleWidget* doubleWidget = static_cast<DoubleWidget*>(PropertyVisualizer::widget);
    ACG::Vec4f colorMin, colorMax;

    colorMin = OVMPropertyVisualizer<MeshT>::convertColor(doubleWidget->doubleMin->color());
    colorMax = OVMPropertyVisualizer<MeshT>::convertColor(doubleWidget->doubleMax->color());

    // color codern in [0,1]
    ACG::ColorCoder cc;

    double min, max;

    if ( doubleWidget->doubleAbsolute->isChecked() ){
        min = FLT_MAX;
        max = 0.0;
    } else {
        min = FLT_MAX;
        max = FLT_MIN;
    }

    for (EntityIterator e_it = e_begin; e_it != e_end; ++e_it){
        double value = prop[e_it];
        if ( doubleWidget->doubleAbsolute->isChecked() ){
            min = std::min( min, fabs(value));
            max = std::max( max, fabs(value));
        } else {
            min = std::min( min, value);
            max = std::max( max, value);
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

    VolumeMeshObject<MeshT>* object;
    PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);
    for (EntityIterator e_it = e_begin; e_it != e_end; ++e_it){

        if (range == 0.0)
            object->colors()[*e_it] = colorMin;
        else {

            double value = prop[e_it];

            // absolut value?
            if ( doubleWidget->doubleAbsolute->isChecked())
                value = fabs(value);

            // clamping
            value = std::max(min,value);
            value = std::min(max,value);

            double t = (value-min)/range;

            ACG::Vec4f color;

            if( doubleWidget->doubleColorCoder->isChecked())
                color = cc.color_float4(t);
            else {
                color = (colorMin)*(1.0-t) + (colorMax)*t;
            }

            // set color
            object->colors()[*e_it] = color;
        }
    }
}
CALLS_TO_VISUALIZE_PROP(OVMPropertyVisualizerDouble<MeshT>, typename MeshT)

template <typename MeshT>
void OVMPropertyVisualizerDouble<MeshT>::duplicateProperty()
{
    OVMPropertyVisualizer<MeshT>::template duplicateProperty_stage1<double>();
}

template <typename MeshT>
QString OVMPropertyVisualizerDouble<MeshT>::getPropertyText(unsigned int index)
{
    return OVMPropertyVisualizer<MeshT>::template getPropertyText_<double>(index);
}

#endif /* ENABLE_OPENVOLUMEMESH */
