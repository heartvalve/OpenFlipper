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

#define OVM_PROPERTY_VISUALIZER_BOOLEAN_CC

#include "OVMPropertyVisualizerBoolean.hh"

template <typename MeshT>
OVMPropertyVisualizerBoolean<MeshT>::OVMPropertyVisualizerBoolean(MeshT* _mesh, int objectID, PropertyInfo _propertyInfo)
    : OVMPropertyVisualizer<MeshT>(_mesh, objectID, _propertyInfo)
{
    if (PropertyVisualizer::widget) delete PropertyVisualizer::widget;
    BooleanWidget* w = new BooleanWidget();
    w->paramBool->setTitle(QString("Boolean Parameters of ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
    PropertyVisualizer::widget = w;
}

template <typename MeshT>
template <typename PropType, typename EntityIterator>
void OVMPropertyVisualizerBoolean<MeshT>::visualizeProp(PropType prop, EntityIterator e_begin, EntityIterator e_end)
{
    if (!prop)
        return;

    BooleanWidget* booleanWidget = static_cast<BooleanWidget*>(PropertyVisualizer::widget);
    ACG::Vec4f colorTrue, colorFalse;

    colorTrue  = OVMPropertyVisualizer<MeshT>::convertColor(booleanWidget->colorTrue->color());
    colorFalse = OVMPropertyVisualizer<MeshT>::convertColor(booleanWidget->colorFalse->color());

    VolumeMeshObject<MeshT>* object;
    PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);

    for (EntityIterator e_it = e_begin ; e_it != e_end; ++e_it)
        if ( prop[*e_it] )
            object->colors()[*e_it] = colorTrue;
        else
            object->colors()[*e_it] = colorFalse;
}
CALLS_TO_VISUALIZE_PROP(OVMPropertyVisualizerBoolean<MeshT>, typename MeshT)

template <typename MeshT>
void OVMPropertyVisualizerBoolean<MeshT>::duplicateProperty()
{
    OVMPropertyVisualizer<MeshT>::template duplicateProperty_stage1<bool>();
}

template <typename MeshT>
QString OVMPropertyVisualizerBoolean<MeshT>::getCellPropertyText(unsigned int index)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::CellPropertyT<bool> prop = mesh->template request_cell_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
        return QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str());;

    OpenVolumeMesh::CellHandle ch(index);

    return this->toStr(prop[ch]);
}

template <typename MeshT>
QString OVMPropertyVisualizerBoolean<MeshT>::getFacePropertyText(unsigned int index)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::FacePropertyT<bool> prop = mesh->template request_face_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
        return QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str());;

    OpenVolumeMesh::FaceHandle fh(index);

    return this->toStr(prop[fh]);
}

template <typename MeshT>
QString OVMPropertyVisualizerBoolean<MeshT>::getHalffacePropertyText(unsigned int index)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::HalfFacePropertyT<bool> prop = mesh->template request_halfface_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
        return QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str());;

    OpenVolumeMesh::HalfFaceHandle hfh(index);

    return this->toStr(prop[hfh]);
}

template <typename MeshT>
QString OVMPropertyVisualizerBoolean<MeshT>::getEdgePropertyText(unsigned int index)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::EdgePropertyT<bool> prop = mesh->template request_edge_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
        return QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str());;

    OpenVolumeMesh::EdgeHandle eh(index);

    return this->toStr(prop[eh]);
}

template <typename MeshT>
QString OVMPropertyVisualizerBoolean<MeshT>::getHalfedgePropertyText(unsigned int index)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::HalfEdgePropertyT<bool> prop = mesh->template request_halfedge_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
        return QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str());;

    OpenVolumeMesh::HalfEdgeHandle heh(index);

    return this->toStr(prop[heh]);
}

template <typename MeshT>
QString OVMPropertyVisualizerBoolean<MeshT>::getVertexPropertyText(unsigned int index)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::VertexPropertyT<bool> prop = mesh->template request_vertex_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
        return QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str());;

    OpenVolumeMesh::VertexHandle vh(index);

    return this->toStr(prop[vh]);
}

template <typename MeshT>
void OVMPropertyVisualizerBoolean<MeshT>::setCellPropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::CellPropertyT<bool> prop = mesh->template request_cell_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
        //emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::CellHandle ch(index);

    prop[ch] = this->strToBool(text);
}

template <typename MeshT>
void OVMPropertyVisualizerBoolean<MeshT>::setFacePropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::FacePropertyT<bool> prop = mesh->template request_face_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
       // emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::FaceHandle fh(index);

    prop[fh] = this->strToBool(text);
}

template <typename MeshT>
void OVMPropertyVisualizerBoolean<MeshT>::setHalffacePropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::HalfFacePropertyT<bool> prop = mesh->template request_halfface_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
       // emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::HalfFaceHandle hfh(index);

    prop[hfh] = this->strToBool(text);
}

template <typename MeshT>
void OVMPropertyVisualizerBoolean<MeshT>::setEdgePropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::EdgePropertyT<bool> prop = mesh->template request_edge_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
       // emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::EdgeHandle eh(index);

    prop[eh] = this->strToBool(text);
}

template <typename MeshT>
void OVMPropertyVisualizerBoolean<MeshT>::setHalfedgePropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::HalfEdgePropertyT<bool> prop = mesh->template request_halfedge_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
        //emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::HalfEdgeHandle heh(index);

    prop[heh] = this->strToBool(text);
}

template <typename MeshT>
void OVMPropertyVisualizerBoolean<MeshT>::setVertexPropertyFromText(unsigned int index, QString text)
{
    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    OpenVolumeMesh::VertexPropertyT<bool> prop = mesh->template request_vertex_property<bool>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
    if ( !prop )
    {
       // emit this->log(LOGERR, QObject::tr("Error: No property with name ").append(PropertyVisualizer::propertyInfo.propName().c_str()));
        return;
    }

    OpenVolumeMesh::VertexHandle vh(index);

    prop[vh] = this->strToBool(text);
}


template <typename MeshT>
QString OVMPropertyVisualizerBoolean<MeshT>::getPropertyText(unsigned int index)
{
    return OVMPropertyVisualizer<MeshT>::template getPropertyText_<bool>(index);
}

#endif /* ENABLE_OPENVOLUMEMESH */
