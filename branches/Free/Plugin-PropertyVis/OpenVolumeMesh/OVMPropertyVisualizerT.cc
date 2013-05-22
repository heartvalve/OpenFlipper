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

#define OVM_PROPERTY_VISUALIZER_CC

#ifdef ENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT
#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>
#endif
#ifdef ENABLE_OPENVOLUMEMESH_HEXAHEDRAL_SUPPORT
#include <ObjectTypes/HexahedralMesh/HexahedralMesh.hh>
#endif

#include "OVMPropertyVisualizer.hh"

template <typename MeshT>
template <typename InnerType>
QString OVMPropertyVisualizer<MeshT>::getPropertyText_(unsigned int index)
{
    if (PropertyVisualizer::propertyInfo.isCellProp())
    {
        OpenVolumeMesh::CellPropertyT<InnerType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_cell_property<InnerType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        return PropertyVisualizer::toStr(prop[OpenVolumeMesh::CellHandle(index)]);
    }
    else if (PropertyVisualizer::propertyInfo.isFaceProp())
    {
        OpenVolumeMesh::FacePropertyT<InnerType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_face_property<InnerType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        return PropertyVisualizer::toStr(prop[OpenVolumeMesh::FaceHandle(index)]);
    }
    else if (PropertyVisualizer::propertyInfo.isHalffaceProp())
    {
        OpenVolumeMesh::HalfFacePropertyT<InnerType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_halfface_property<InnerType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        return PropertyVisualizer::toStr(prop[OpenVolumeMesh::HalfFaceHandle(index)]);
    }
    else if (PropertyVisualizer::propertyInfo.isEdgeProp())
    {
        OpenVolumeMesh::EdgePropertyT<InnerType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_edge_property<InnerType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        return PropertyVisualizer::toStr(prop[OpenVolumeMesh::EdgeHandle(index)]);
    }
    else if (PropertyVisualizer::propertyInfo.isHalfedgeProp())
    {
        OpenVolumeMesh::HalfEdgePropertyT<InnerType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_halfedge_property<InnerType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        return PropertyVisualizer::toStr(prop[OpenVolumeMesh::HalfEdgeHandle(index)]);
    }
    else //if (propertyInfo.isVertexProp())
    {
        OpenVolumeMesh::VertexPropertyT<InnerType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_vertex_property<InnerType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        return PropertyVisualizer::toStr(prop[OpenVolumeMesh::VertexHandle(index)]);
    }
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::setPropertyFromText(unsigned int index, QString text)
{
    if (propertyInfo.isCellProp())
        setCellPropertyFromText(index, text);
    else if (propertyInfo.isFaceProp())
        setFacePropertyFromText(index, text);
    else if (propertyInfo.isHalffaceProp())
        setHalffacePropertyFromText(index, text);
    else if (propertyInfo.isEdgeProp())
        setEdgePropertyFromText(index, text);
    else if (propertyInfo.isHalfedgeProp())
        setHalfedgePropertyFromText(index, text);
    else //if (propertyInfo.isVertexProp())
        setVertexPropertyFromText(index, text);
}

template <typename MeshT>
int OVMPropertyVisualizer<MeshT>::getEntityCount()
{
    if (propertyInfo.isCellProp())
        return mesh->n_cells();
    if (propertyInfo.isFaceProp())
        return mesh->n_faces();
    if (propertyInfo.isHalffaceProp())
        return mesh->n_halffaces();
    else if (propertyInfo.isEdgeProp())
        return mesh->n_edges();
    else if (propertyInfo.isHalfedgeProp())
        return mesh->n_halfedges();
    else //if (propertyInfo.isVertexProp())
        return mesh->n_vertices();
}

template <typename MeshT>
QString OVMPropertyVisualizer<MeshT>::getHeader()
{
    //Header: headerVersion, numberOfEntities, typeOfEntites, typeOfProperty, propertyName

    QString header = QObject::tr("1"); //version
    header.append(QObject::tr(", %1").arg(getEntityCount())); //number of entities
    header.append(QObject::tr(", %1").arg(propertyInfo.entityType())); //type of entities
    header.append(", ").append(propertyInfo.friendlyTypeName()); //type of property
    header.append(", ").append(propertyInfo.propName().c_str()); // name of property
    return header;
}

template <typename MeshT>
unsigned int OVMPropertyVisualizer<MeshT>::getClosestPrimitiveId(unsigned int _face, ACG::Vec3d& _hitPoint)
{
    if (propertyInfo.isHalffaceProp())
        return getClosestHalffaceId(_face, _hitPoint);
    else// if (propertyInfo.isHalfedgeProp())
        return getClosestHalfedgeId(_face, _hitPoint);
}

template <typename MeshT>
unsigned int OVMPropertyVisualizer<MeshT>::getClosestHalffaceId(unsigned int _face, ACG::Vec3d& _hitPoint)
{
    ACG::Vec3d direction = PluginFunctions::viewingDirection();

    OpenVolumeMesh::HalfFaceHandle hfh = mesh->halfface_handle(OpenVolumeMesh::FaceHandle(_face), 0);
    OpenVolumeMesh::HalfFaceVertexIter hfv_it = mesh->hfv_iter(hfh);
    ACG::Vec3d p1 = mesh->vertex(*(hfv_it+0));
    ACG::Vec3d p2 = mesh->vertex(*(hfv_it+1));
    ACG::Vec3d p3 = mesh->vertex(*(hfv_it+2));

    ACG::Vec3d normal = (p2-p1)%(p3-p1);

    if ((direction | normal) < 0)
        return hfh.idx();
    else
        return mesh->halfface_handle(OpenVolumeMesh::FaceHandle(_face), 1).idx();
}

template <typename MeshT>
unsigned int OVMPropertyVisualizer<MeshT>::getClosestHalfedgeId(unsigned int _face, ACG::Vec3d& _hitPoint)
{
    unsigned int halfface = getClosestHalffaceId(_face, _hitPoint);

    OpenVolumeMesh::OpenVolumeMeshFace face = mesh->halfface(halfface);

    const std::vector<OpenVolumeMesh::HalfEdgeHandle> & halfedges = face.halfedges();

    double min_distance = DBL_MAX;
    OpenVolumeMesh::HalfEdgeHandle closestHalfEdgeHandle;

    for (std::vector<OpenVolumeMesh::HalfEdgeHandle>::const_iterator he_it = halfedges.begin(); he_it != halfedges.end(); ++he_it)
    {
        OpenVolumeMesh::OpenVolumeMeshEdge edge = OVMPropertyVisualizer<MeshT>::mesh->halfedge(*he_it);
        ACG::Vec3d v1 = mesh->vertex(edge.from_vertex());
        ACG::Vec3d v2 = mesh->vertex(edge.to_vertex());
        ACG::Vec3d p = 0.5 * (v1+v2);
        double distance = (p-_hitPoint).length();
        if (distance < min_distance)
        {
            min_distance = distance;
            closestHalfEdgeHandle = *he_it;
        }

    }

    return closestHalfEdgeHandle.idx();
}


template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::visualize(bool _setDrawMode)
{
    if (propertyInfo.isCellProp())
        visualizeCellProp(_setDrawMode);
    else if (propertyInfo.isFaceProp())
        visualizeFaceProp(_setDrawMode);
    else if (propertyInfo.isHalffaceProp())
        visualizeHalffaceProp(_setDrawMode);
    else if (propertyInfo.isEdgeProp())
        visualizeEdgeProp(_setDrawMode);
    else if (propertyInfo.isHalfedgeProp())
        visualizeHalfedgeProp(_setDrawMode);
    else if (propertyInfo.isVertexProp())
        visualizeVertexProp(_setDrawMode);
}

template <typename MeshT>
OpenMesh::Vec4f OVMPropertyVisualizer<MeshT>::convertColor(const QColor _color){

  OpenMesh::Vec4f color;

  color[0] = _color.redF();
  color[1] = _color.greenF();
  color[2] = _color.blueF();
  color[3] = _color.alphaF();

  return color;
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::visualizeFaceProp(bool /*_setDrawMode*/)
{
    emit log(LOGERR, "Visualizing FaceProp not implemented");
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::visualizeEdgeProp(bool /*_setDrawMode*/)
{
    emit log(LOGERR, "Visualizing EdgeProp not implemented");
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::visualizeHalfedgeProp(bool /*_setDrawMode*/)
{
    emit log(LOGERR, "Visualizing HalfedgeProp not implemented");
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::visualizeVertexProp(bool /*_setDrawMode*/)
{
    emit log(LOGERR, "Visualizing VertexProp not implemented");
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::visualizeCellProp(bool /*_setDrawMode*/)
{
    emit log(LOGERR, "Visualizing CellProp not implemented");
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::visualizeHalffaceProp(bool /*_setDrawMode*/)
{
    emit log(LOGERR, "Visualizing HalffaceProp not implemented");
}

template<typename MeshT>
template<typename PropType>
inline void OVMPropertyVisualizer<MeshT>::duplicateProperty_stage1() {
    std::string newPropertyName;
    for (int i = 1;; ++i) {
        std::ostringstream oss;
        oss << propertyInfo.propName() << " Copy " << i;
        newPropertyName = oss.str();

        if (propertyInfo.isCellProp())
        {
            if(!mesh->template cell_property_exists<PropType>(newPropertyName)) break;
        }
        else if (propertyInfo.isFaceProp())
        {
            if(!mesh->template face_property_exists<PropType>(newPropertyName)) break;
        }
        else if (propertyInfo.isHalffaceProp())
        {
            if(!mesh->template halfface_property_exists<PropType>(newPropertyName)) break;
        }
        else if (propertyInfo.isEdgeProp())
        {
            if(!mesh->template edge_property_exists<PropType>(newPropertyName)) break;
        }
        else if (propertyInfo.isHalfedgeProp())
        {
            if(!mesh->template halfedge_property_exists<PropType>(newPropertyName)) break;
        }
        else if (propertyInfo.isVertexProp())
        {
            if(!mesh->template vertex_property_exists<PropType>(newPropertyName)) break;
        }
    }

    if (propertyInfo.isCellProp())
    {
        OpenVolumeMesh::CellPropertyT<PropType> prop = mesh->template request_cell_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        OpenVolumeMesh::CellPropertyT<PropType> newProp = mesh->template request_cell_property< PropType >(newPropertyName);
        mesh->set_persistent(newProp, true);
        std::for_each(mesh->cells_begin(), mesh->cells_end(), CopyProperty<OpenVolumeMesh::CellPropertyT<PropType> >(newProp, prop, mesh));
    }
    else if (propertyInfo.isFaceProp())
    {
        OpenVolumeMesh::FacePropertyT<PropType> prop = mesh->template request_face_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        OpenVolumeMesh::FacePropertyT<PropType> newProp = mesh->template request_face_property< PropType >(newPropertyName);
        mesh->set_persistent(newProp, true);
        std::for_each(mesh->faces_begin(), mesh->faces_end(), CopyProperty<OpenVolumeMesh::FacePropertyT<PropType> >(newProp, prop, mesh));
    }
    else if (propertyInfo.isHalffaceProp())
    {
        OpenVolumeMesh::HalfFacePropertyT<PropType> prop = mesh->template request_halfface_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        OpenVolumeMesh::HalfFacePropertyT<PropType> newProp = mesh->template request_halfface_property< PropType >(newPropertyName);
        mesh->set_persistent(newProp, true);
        std::for_each(mesh->halffaces_begin(), mesh->halffaces_end(), CopyProperty<OpenVolumeMesh::HalfFacePropertyT<PropType> >(newProp, prop, mesh));
    }
    else if (propertyInfo.isEdgeProp())
    {
        OpenVolumeMesh::EdgePropertyT<PropType> prop = mesh->template request_edge_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        OpenVolumeMesh::EdgePropertyT<PropType> newProp = mesh->template request_edge_property< PropType >(newPropertyName);
        mesh->set_persistent(newProp, true);
        std::for_each(mesh->edges_begin(), mesh->edges_end(), CopyProperty<OpenVolumeMesh::EdgePropertyT<PropType> >(newProp, prop, mesh));
    }
    else if (propertyInfo.isHalfedgeProp())
    {
        OpenVolumeMesh::HalfEdgePropertyT<PropType> prop = mesh->template request_halfedge_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        OpenVolumeMesh::HalfEdgePropertyT<PropType> newProp = mesh->template request_halfedge_property< PropType >(newPropertyName);
        mesh->set_persistent(newProp, true);
        std::for_each(mesh->halfedges_begin(), mesh->halfedges_end(), CopyProperty<OpenVolumeMesh::HalfEdgePropertyT<PropType> >(newProp, prop, mesh));
    }
    else if (propertyInfo.isVertexProp())
    {
        OpenVolumeMesh::VertexPropertyT<PropType> prop = mesh->template request_vertex_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());
        OpenVolumeMesh::VertexPropertyT<PropType> newProp = mesh->template request_vertex_property< PropType >(newPropertyName);
        mesh->set_persistent(newProp, true);
        std::for_each(mesh->vertices_begin(), mesh->vertices_end(), CopyProperty<OpenVolumeMesh::VertexPropertyT<PropType> >(newProp, prop, mesh));
    }
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::clear()
{
    VolumeMeshObject<MeshT>* object;
    PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);

    MeshT* mesh = OVMPropertyVisualizer<MeshT>::mesh;

    if (propertyInfo.isCellProp())
        object->colors().clear_cell_colors();
    else if (propertyInfo.isFaceProp())
        object->colors().clear_face_colors();
    else if (propertyInfo.isHalffaceProp())
        object->colors().clear_halfface_colors();
    else if (propertyInfo.isEdgeProp())
        object->colors().clear_edge_colors();
    else if (propertyInfo.isHalfedgeProp())
        object->colors().clear_halfedge_colors();
    else if (propertyInfo.isVertexProp())
        object->colors().clear_vertex_colors();

    object->setObjectDrawMode(drawModes.cellsFlatShaded);
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::setCellPropertyFromText(unsigned int /*index*/, QString /*text*/)
{
    emit log(LOGERR, "Setting CellProp not implemented for this property type");
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::setFacePropertyFromText(unsigned int /*index*/, QString /*text*/)
{
    emit log(LOGERR, "Setting FaceProp not implemented for this property type");
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::setHalffacePropertyFromText(unsigned int /*index*/, QString /*text*/)
{
    emit log(LOGERR, "Setting HalffaceProp not implemented for this property type");
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::setEdgePropertyFromText(unsigned int /*index*/, QString /*text*/)
{
    emit log(LOGERR, "Setting EdgeProp not implemented for this property type");
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::setHalfedgePropertyFromText(unsigned int /*index*/, QString /*text*/)
{
    emit log(LOGERR, "Setting HalfedgeProp not implemented for this property type");
}

template <typename MeshT>
void OVMPropertyVisualizer<MeshT>::setVertexPropertyFromText(unsigned int /*index*/, QString /*text*/)
{
    emit log(LOGERR, "Setting VertexProp not implemented for this property type");
}

#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */
