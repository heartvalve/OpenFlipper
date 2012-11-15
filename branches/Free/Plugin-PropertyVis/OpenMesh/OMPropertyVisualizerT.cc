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


#define OM_PROPERTY_VISUALIZER_CC

#include "OMPropertyVisualizer.hh"



namespace {

template <typename EntityType, typename Handle, typename MeshT>
QString getPropertyText__(Handle handle, MeshT mesh, PropertyInfo propInfo)
{
    EntityType prop;

    if ( !mesh->get_property_handle(prop, propInfo.propName() ) )
        return QObject::tr("Error: No property with name ").append(propInfo.propName().c_str());

    return PropertyVisualizer::toStr(mesh->property(prop, handle));
}

}


template <typename MeshT>
template <typename InnerType>
QString OMPropertyVisualizer<MeshT>::getPropertyText_(unsigned int index)
{
    MeshT* mesh = OMPropertyVisualizer<MeshT>::mesh;
    if (PropertyVisualizer::propertyInfo.isFaceProp())
        return getPropertyText__<OpenMesh::FPropHandleT< InnerType > >(mesh->face_handle(index),OMPropertyVisualizer<MeshT>::mesh, PropertyVisualizer::propertyInfo);
    else if (PropertyVisualizer::propertyInfo.isEdgeProp())
        return getPropertyText__<OpenMesh::EPropHandleT< InnerType > >(mesh->edge_handle(index),OMPropertyVisualizer<MeshT>::mesh, PropertyVisualizer::propertyInfo);
    else if (PropertyVisualizer::propertyInfo.isHalfedgeProp())
        return getPropertyText__<OpenMesh::HPropHandleT< InnerType > >(mesh->halfedge_handle(index),OMPropertyVisualizer<MeshT>::mesh, PropertyVisualizer::propertyInfo);
    else //if (propertyInfo.isVertexProp())
        return getPropertyText__<OpenMesh::VPropHandleT< InnerType > >(mesh->vertex_handle(index),OMPropertyVisualizer<MeshT>::mesh, PropertyVisualizer::propertyInfo);
}


template<typename MeshT>
template<typename PropHandle>
void OMPropertyVisualizer<MeshT>::removeProperty_stage2() {
    PropHandle propHandle;
    if (!mesh->get_property_handle(propHandle, propertyInfo.propName())) return;
    mesh->remove_property(propHandle);
}

template<typename MeshT>
template<typename PropType>
inline void OMPropertyVisualizer<MeshT>::removeProperty_stage1() {
    if (propertyInfo.isEdgeProp())
        removeProperty_stage2<OpenMesh::EPropHandleT<PropType> >();
    else if (propertyInfo.isVertexProp())
        removeProperty_stage2<OpenMesh::VPropHandleT<PropType> >();
    else if (propertyInfo.isFaceProp())
        removeProperty_stage2<OpenMesh::FPropHandleT<PropType> >();
    else if (propertyInfo.isHalfedgeProp())
        removeProperty_stage2<OpenMesh::HPropHandleT<PropType> >();
}


template<typename MeshT>
template<typename PropHandle, typename Iterator>
inline void OMPropertyVisualizer<MeshT>::duplicateProperty_stage2(Iterator first, Iterator last) {
    PropHandle propHandle;
    if (!mesh->get_property_handle(propHandle, propertyInfo.propName())) return;

    std::string newPropertyName;
    for (int i = 1;; ++i) {
        std::ostringstream oss;
        oss << propertyInfo.propName() << " Copy " << i;
        newPropertyName = oss.str();

        PropHandle tmp;
        if (!mesh->get_property_handle(tmp, newPropertyName)) break;
    }

    PropHandle newProperty;
    mesh->add_property(newProperty, newPropertyName);
    std::for_each(first, last, CopyProperty<PropHandle>(newProperty, propHandle, mesh));
}

template<typename MeshT>
template<typename PropType>
inline void OMPropertyVisualizer<MeshT>::duplicateProperty_stage1() {
    if (propertyInfo.isEdgeProp())
        duplicateProperty_stage2<OpenMesh::EPropHandleT<PropType> >(mesh->edges_begin(), mesh->edges_end());
    else if (propertyInfo.isVertexProp())
        duplicateProperty_stage2<OpenMesh::VPropHandleT<PropType> >( mesh->vertices_begin(), mesh->vertices_end());
    else if (propertyInfo.isFaceProp())
        duplicateProperty_stage2<OpenMesh::FPropHandleT<PropType> >(mesh->faces_begin(), mesh->faces_end());
    else if (propertyInfo.isHalfedgeProp())
        duplicateProperty_stage2<OpenMesh::HPropHandleT<PropType> >(mesh->halfedges_begin(), mesh->halfedges_end());
}



template <typename MeshT>
QString OMPropertyVisualizer<MeshT>::getPropertyText(unsigned int index)
{
    return "";
}


template <typename MeshT>
void OMPropertyVisualizer<MeshT>::setPropertyFromText(unsigned int index, QString text)
{
    if (propertyInfo.isFaceProp())
        setFacePropertyFromText(index, text);
    else if (propertyInfo.isEdgeProp())
        setEdgePropertyFromText(index, text);
    else if (propertyInfo.isHalfedgeProp())
        setHalfedgePropertyFromText(index, text);
    else //if (propertyInfo.isVertexProp())
        setVertexPropertyFromText(index, text);
}

template <typename MeshT>
int OMPropertyVisualizer<MeshT>::getEntityCount()
{
    if (propertyInfo.isFaceProp())
        return mesh->n_faces();
    else if (propertyInfo.isEdgeProp())
        return mesh->n_edges();
    else if (propertyInfo.isHalfedgeProp())
        return mesh->n_halfedges();
    else //if (propertyInfo.isVertexProp())
        return mesh->n_vertices();
}

template <typename MeshT>
QString OMPropertyVisualizer<MeshT>::getHeader()
{
    //Header: headerVersion, numberOfEntities, typeOfEntities, typeOfProperty, propertyName

    QString header = QObject::tr("1"); //version
    header.append(QObject::tr(", %1").arg(getEntityCount())); //number of entities
    header.append(QObject::tr(", %1").arg(propertyInfo.entityType())); //type of entities
    header.append(", ").append(propertyInfo.friendlyTypeName()); //type of property
    header.append(", ").append(propertyInfo.propName().c_str()); // name of property
    return header;
}


/**
 * @brief Returns the ID of the closest primitive.
 *
 * @param _face The face that was hit.
 * @param _hitPoint The position where the face was hit.
 * @return The ID of the primitive that is closest to the hitpoint.
 *
 * This method is used for picking. For some entities it might be convenient to not pick them
 * directly but pick faces instead. Then, this method iterates over the entities adjacent to the face
 * and return the one that is closest to the actual hit point.
 */
template <typename MeshT>
unsigned int OMPropertyVisualizer<MeshT>::getClosestPrimitiveId(unsigned int _face, ACG::Vec3d& _hitPoint)
{
    if (propertyInfo.isFaceProp())
        return getClosestFaceId(_face, _hitPoint);
    else if (propertyInfo.isEdgeProp())
        return getClosestEdgeId(_face, _hitPoint);
    else if (propertyInfo.isHalfedgeProp())
        return getClosestHalfedgeId(_face, _hitPoint);
    else //if (propertyInfo.isVertexProp())
        return getClosestVertexId(_face, _hitPoint);
}

template <typename MeshT>
unsigned int OMPropertyVisualizer<MeshT>::getClosestFaceId(unsigned int _face, ACG::Vec3d& _hitPoint)
{
    return _face;
}

template <typename MeshT>
unsigned int OMPropertyVisualizer<MeshT>::getClosestEdgeId(unsigned int _face, ACG::Vec3d& _hitPoint)
{
    unsigned int closest_halfedge_id = getClosestHalfedgeId(_face, _hitPoint);
    typename MeshT::HalfedgeHandle heh;
    heh = mesh->halfedge_handle(closest_halfedge_id);

    typename MeshT::EdgeHandle eh;
    eh = mesh->edge_handle(heh);

    return eh.idx();
}

template <typename MeshT>
unsigned int OMPropertyVisualizer<MeshT>::getClosestHalfedgeId(unsigned int _face, ACG::Vec3d& _hitPoint)
{
    typename MeshT::FaceHalfedgeIter fh_it;

    int closest_h_idx = 0;
    double dist = DBL_MAX;
    double temp_dist = 0.0;

    ACG::Vec3d vTemp = ACG::Vec3d(0.0, 0.0, 0.0);
    typename MeshT::Point p;

    for (fh_it = mesh->fh_iter(mesh->face_handle(_face)); fh_it; ++fh_it){

        typename MeshT::HalfedgeHandle heh;
        heh = fh_it.handle();

        typename MeshT::VertexHandle v1;
        v1 = mesh->to_vertex_handle(heh);
        typename MeshT::VertexHandle v2;
        v2 = mesh->from_vertex_handle(heh);

        p = 0.5* (mesh->point( v1 ) + mesh->point( v2 ));

        // Find closest vertex to selection
        vTemp = ACG::Vec3d(p[0], p[1], p[2]);
        temp_dist = (vTemp - _hitPoint).length();

        if (temp_dist < dist) {
            dist = temp_dist;
            closest_h_idx = fh_it.handle().idx();
        }

    }
    return closest_h_idx;
}

template <typename MeshT>
unsigned int OMPropertyVisualizer<MeshT>::getClosestVertexId(unsigned int _face, ACG::Vec3d& _hitPoint)
{
    typename MeshT::FaceVertexIter fv_it;

    int closest_v_idx = 0;
    double dist = DBL_MAX;
    double temp_dist = 0.0;

    ACG::Vec3d vTemp = ACG::Vec3d(0.0, 0.0, 0.0);
    typename MeshT::Point p;

    for (fv_it = mesh->fv_iter(mesh->face_handle(_face)); fv_it; ++fv_it){

      p = mesh->point( fv_it.handle() );

      // Find closest vertex to selection
      vTemp = ACG::Vec3d(p[0], p[1], p[2]);
      temp_dist = (vTemp - _hitPoint).length();

      if (temp_dist < dist) {
          dist = temp_dist;
          closest_v_idx = fv_it.handle().idx();
      }

    }
    return closest_v_idx;
}

/**
 * \brief Visualizes the property.
 *
 * To visualize the property this method calls the corresponding visualization method
 * visualizeFaceProp, -EdgeProp, -HalfedgeProp or -VertexProp depending on the entity
 * type of the property.
 */
template <typename MeshT>
void OMPropertyVisualizer<MeshT>::visualize()
{
    if (propertyInfo.isFaceProp())
        visualizeFaceProp();
    else if (propertyInfo.isEdgeProp())
        visualizeEdgeProp();
    else if (propertyInfo.isHalfedgeProp())
        visualizeHalfedgeProp();
    else if (propertyInfo.isVertexProp())
        visualizeVertexProp();
}

/**
 * @brief Clears the property.
 *
 * This method clears the property by releasing the colors of the mesh.
 */
template <typename MeshT>
void OMPropertyVisualizer<MeshT>::clear()
{
    if (propertyInfo.isFaceProp())
        clearFaceProp();
    else if (propertyInfo.isEdgeProp())
        clearEdgeProp();
    else if (propertyInfo.isHalfedgeProp())
        clearHalfedgeProp();
    else if (propertyInfo.isVertexProp())
        clearVertexProp();

    PluginFunctions::setDrawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED);
}


template <typename MeshT>
OpenMesh::Vec4f OMPropertyVisualizer<MeshT>::convertColor(const QColor _color){

  OpenMesh::Vec4f color;

  color[0] = _color.redF();
  color[1] = _color.greenF();
  color[2] = _color.blueF();
  color[3] = _color.alphaF();

  return color;
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::visualizeFaceProp()
{
    emit log(LOGERR, "Visualizing FaceProp not implemented");
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::visualizeEdgeProp()
{
    emit log(LOGERR, "Visualizing EdgeProp not implemented");
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::visualizeHalfedgeProp()
{
    emit log(LOGERR, "Visualizing HalfedgeProp not implemented");
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::visualizeVertexProp()
{
    emit log(LOGERR, "Visualizing VertexProp not implemented");
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::clearFaceProp()
{
    if ( mesh->has_face_colors() )
        OMPropertyVisualizer<MeshT>::mesh->release_face_colors();
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::clearEdgeProp()
{
    if ( mesh->has_edge_colors() )
        OMPropertyVisualizer<MeshT>::mesh->release_edge_colors();
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::clearHalfedgeProp()
{
    if ( mesh->has_halfedge_colors() )
        OMPropertyVisualizer<MeshT>::mesh->release_halfedge_colors();
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::clearVertexProp()
{
    if ( mesh->has_vertex_colors() )
        OMPropertyVisualizer<MeshT>::mesh->release_vertex_colors();
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::setFacePropertyFromText(unsigned int index, QString text)
{
    emit log(LOGERR, "Setting face property not implemented");
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::setEdgePropertyFromText(unsigned int index, QString text)
{
    emit log(LOGERR, "Settingedge property not implemented");
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::setHalfedgePropertyFromText(unsigned int index, QString text)
{
    emit log(LOGERR, "Setting halfedge property not implemented");
}

template <typename MeshT>
void OMPropertyVisualizer<MeshT>::setVertexPropertyFromText(unsigned int index, QString text)
{
    emit log(LOGERR, "Setting vertex property not implemented");
}
