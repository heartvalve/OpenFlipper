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
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
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
        randomColor[ integerWidget->intMapBlackValue->value() ] = ACG::Vec4f(0.0, 0.0, 0.0, 0.0);

    T min = mNumericLimitMax;
    T max = mNumericLimitMin;

    for (EntityIterator e_it = e_begin; e_it != e_end; ++e_it)
    {
        T value = prop[*e_it];
        min = std::min( min, value);
        max = std::max( max, value);
    }

    if( integerWidget->intFixedRange->isChecked())
    {
        min = integerWidget->intFixedRangeMin->value();
        max = integerWidget->intFixedRangeMax->value();
    }
    else
    {
        integerWidget->intFixedRangeMin->setValue(min);
        integerWidget->intFixedRangeMax->setValue(max);
    }

    ACG::ColorCoder cc;

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
            if (integerWidget->intColorCoder->isChecked())
            {
                color = cc.color_float4(pos);
            }
            else
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
CALLS_TO_VISUALIZE_PROP(OVMPropertyVisualizerInteger<MeshT KOMMA T>, typename MeshT KOMMA typename T, T)
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

template <typename MeshT, typename T>
void OVMPropertyVisualizerInteger<MeshT, T>::setCellPropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::CellPropertyT<int> prop = mesh->template request_cell_property<int>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::CellHandle ch(index);

    prop[ch] = this->strToInt(text);
}

template <typename MeshT, typename T>
void OVMPropertyVisualizerInteger<MeshT, T>::setFacePropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::FacePropertyT<int> prop = mesh->template request_face_property<int>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::FaceHandle fh(index);

    prop[fh] = this->strToInt(text);
}

template <typename MeshT, typename T>
void OVMPropertyVisualizerInteger<MeshT, T>::setHalffacePropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::HalfFacePropertyT<int> prop = mesh->template request_halfface_property<int>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::HalfFaceHandle hfh(index);

    prop[hfh] = this->strToInt(text);
}

template <typename MeshT, typename T>
void OVMPropertyVisualizerInteger<MeshT, T>::setEdgePropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::EdgePropertyT<int> prop = mesh->template request_edge_property<int>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::EdgeHandle eh(index);

    prop[eh] = this->strToInt(text);
}

template <typename MeshT, typename T>
void OVMPropertyVisualizerInteger<MeshT, T>::setHalfedgePropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::HalfEdgePropertyT<int> prop = mesh->template request_halfedge_property<int>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::HalfEdgeHandle heh(index);

    prop[heh] = this->strToInt(text);
}

template <typename MeshT, typename T>
void OVMPropertyVisualizerInteger<MeshT, T>::setVertexPropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::VertexPropertyT<int> prop = mesh->template request_vertex_property<int>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
        emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::VertexHandle vh(index);

    prop[vh] = this->strToInt(text);
}

#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */
