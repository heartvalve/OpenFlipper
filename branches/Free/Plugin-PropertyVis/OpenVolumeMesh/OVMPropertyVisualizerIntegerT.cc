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

#ifdef ENABLE_OPENVOLUMEMESH_SUPPORT

#define OVM_PROPERTY_VISUALIZER_ITEGER_CC

#include "OVMPropertyVisualizerInteger.hh"

template <typename MeshT, typename T>
OVMPropertyVisualizerInteger<MeshT,T>::OVMPropertyVisualizerInteger(MeshT* _mesh, int objectID,  PropertyInfo _propertyInfo, bool isUnsigned)
    : OVMPropertyVisualizer<MeshT>(_mesh, objectID, _propertyInfo)
{
    if (PropertyVisualizer::widget) delete PropertyVisualizer::widget;
    IntegerWidget* w = new IntegerWidget();
    w->paramInt->setTitle(QString("Integer Parameters of ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
    PropertyVisualizer::widget = w;

    if (isUnsigned)
    {
        w->intAbsolute->setChecked(false); //because we already have unsigned integers wo don't have to calculate their absolute value
        w->intAbsolute->setCheckable(false);
    }


    mNumericLimitMax = std::numeric_limits<T>::max();
    mNumericLimitMin = std::numeric_limits<T>::min();

}

template <typename MeshT, typename T>
template <typename PropType, typename EntityIterator>
void OVMPropertyVisualizerInteger<MeshT, T>::visualizeProp(PropType prop, EntityIterator e_begin, EntityIterator e_end)
{
    if (!prop) return;

    IntegerWidget* integerWidget = static_cast<IntegerWidget*>(PropertyVisualizer::widget);
    ACG::Vec4f colorMin, colorMax;

    colorMin = OVMPropertyVisualizer<MeshT>::convertColor(integerWidget->intMin->color());
    colorMax = OVMPropertyVisualizer<MeshT>::convertColor(integerWidget->intMax->color());

    std::map< int, ACG::Vec4f> randomColor;

    if ( integerWidget->intRandom->isChecked() && integerWidget->intMapBlack->isChecked() )
        randomColor[ integerWidget->intMapBlackValue->value() ] = ACG::Vec4f(0.0, 0.0, 0.0,  1.0);

    T min = mNumericLimitMin;
    T max = mNumericLimitMax;

    for (EntityIterator e_it = e_begin; e_it != e_end; ++e_it)
    {
        T value = prop[*e_it];
        min = std::min( min, value);
        max = std::max( max, value);
    }

    unsigned int range = max - min;
    VolumeMeshObject<MeshT>* object;
    PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);

    for (EntityIterator e_it = e_begin; e_it != e_end; ++e_it)
    {
        if (range == 0)
            object->colors()[*e_it] = colorMin;
        else {
            T value = prop[*e_it];
            double pos = (value - min) / (double) range;
            ACG::Vec4f color;
            if ( !integerWidget->intRandom->isChecked() )
            {
                color[0] = colorMin[0] * (1-pos) + pos * colorMax[0];
                color[1] = colorMin[1] * (1-pos) + pos * colorMax[1];
                color[2] = colorMin[2] * (1-pos) + pos * colorMax[2];
                color[3] = 1.0;
            }
            else
            {
                if ( randomColor.find( value ) == randomColor.end() )
                {
                    color = mColorGenerator.generateNextColor();
                    color[3] = 1.0;
                    randomColor[ value ] = color;
                }
                color = randomColor[ value ];
            }
            object->colors()[*e_it] = color;
        }
    }

}
#define KOMMA ,
CALLS_TO_VISUALIZE_PROP(OVMPropertyVisualizerInteger<MeshT KOMMA T>, typename MeshT KOMMA typename T)
#undef KOMMA

template <typename MeshT, typename T>
void OVMPropertyVisualizerInteger<MeshT, T>::duplicateProperty()
{
    OVMPropertyVisualizer<MeshT>::template duplicateProperty_stage1<T>();
}

template <typename MeshT, typename T>
QString OVMPropertyVisualizerInteger<MeshT, T>::getPropertyText(unsigned int index)
{
    return OVMPropertyVisualizer<MeshT>::template getPropertyText_<T>(index);
}

#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */
