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

#ifndef OVM_PROPERTY_VISUALIZER_HH
#define OVM_PROPERTY_VISUALIZER_HH

#include "../PropertyVisualizer.hh"

#include <ObjectTypes/VolumeMeshObject/VolumeMeshObject.hh>

#include <OpenFlipper/BasePlugin/PluginFunctionsViewControls.hh>

#include <ObjectTypes/VolumeMeshObject/VolumeMeshDrawModesContainer.hh>

#include <iostream>

template <typename MeshT>
class OVMPropertyVisualizer: public PropertyVisualizer{

public:
    OVMPropertyVisualizer(MeshT* _mesh, int objectID, PropertyInfo _propertyInfo)
        : PropertyVisualizer(_propertyInfo),
          mesh(_mesh),
          mObjectID(objectID),
          drawModes()
    {}

    virtual ~OVMPropertyVisualizer(){ clear(); }

    /// Visualizes a property.
    virtual void visualize(bool _setDrawMode = true);

    /**
     * @brief Should remove the property.
     *
     * Removing properties is not yet supported by OpenVolumeMesh. Therefor this method only informs the user of this fact via
     * a log message.
     */
    virtual void removeProperty(){ emit log("Removing properties not yet implemented for OpenVolumeMeshs."); }

    /// Duplicates a property.
    virtual void duplicateProperty(){ /*implemented by subclass*/}

    /// Clears a property.
    virtual void clear();

    virtual QString getPropertyText(unsigned int index)=0;

    /// Returns the ID of the closest primitive.
    unsigned int getClosestPrimitiveId(unsigned int _face, ACG::Vec3d &_hitPoint);

protected:
    MeshT* mesh;

    virtual void visualizeFaceProp(bool _setDrawMode = true);
    virtual void visualizeEdgeProp(bool _setDrawMode = true);
    virtual void visualizeHalfedgeProp(bool _setDrawMode = true);
    virtual void visualizeVertexProp(bool _setDrawMode = true);
    virtual void visualizeCellProp(bool _setDrawMode = true);
    virtual void visualizeHalffaceProp(bool _setDrawMode = true);

    template<typename PropType>
    void duplicateProperty_stage1();

    OpenMesh::Vec4f convertColor(QColor color);

    template <typename InnerType>
    QString getPropertyText_(unsigned int index);

    virtual void setCellPropertyFromText(unsigned int index, QString text);
    virtual void setFacePropertyFromText(unsigned int index, QString text);
    virtual void setHalffacePropertyFromText(unsigned int index, QString text);
    virtual void setEdgePropertyFromText(unsigned int index, QString text);
    virtual void setHalfedgePropertyFromText(unsigned int index, QString text);
    virtual void setVertexPropertyFromText(unsigned int index, QString text);

    virtual void setPropertyFromText(unsigned int index, QString text);

    virtual int getEntityCount();

    virtual QString getHeader();

    unsigned int getClosestCellId(unsigned int _face, ACG::Vec3d& _hitPoint);
    unsigned int getClosestFaceId(unsigned int _face, ACG::Vec3d& _hitPoint);
    unsigned int getClosestHalffaceId(unsigned int _face, ACG::Vec3d& _hitPoint);
    unsigned int getClosestEdgeId(unsigned int _face, ACG::Vec3d& _hitPoint);
    unsigned int getClosestHalfedgeId(unsigned int _face, ACG::Vec3d& _hitPoint);
    unsigned int getClosestVertexId(unsigned int _face, ACG::Vec3d& _hitPoint);

    int mObjectID;

    VolumeMeshDrawModesContainer drawModes;

private:

    template<typename Property>
    class CopyProperty
    {
        public:
            CopyProperty(Property& p1, const Property& p2, MeshT*& mesh) :
                p1(p1), p2(p2), mesh(mesh) {}

            template<typename PrimitiveHandleT>
            inline void operator() (const PrimitiveHandleT &pr) {
                p1[pr] = p2[pr];
            }

        private:
            Property &p1;
            const Property &p2;
            MeshT*& mesh;
    };
};

#define CALLS_TO_VISUALIZE_PROP(Classname, Template, PropType) \
template <Template> \
void Classname::visualizeCellProp(bool _setDrawMode) \
{\
    OpenVolumeMesh::CellPropertyT<PropType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_cell_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());\
    visualizeProp(prop, OVMPropertyVisualizer<MeshT>::mesh->cells_begin(), OVMPropertyVisualizer<MeshT>::mesh->cells_end());\
    if (_setDrawMode)\
    {\
        VolumeMeshObject<MeshT>* object;\
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);\
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.cellsColoredPerCell);\
    }\
}\
template <Template>\
void Classname::visualizeFaceProp(bool _setDrawMode)\
{\
    OpenVolumeMesh::FacePropertyT<PropType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_face_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());\
    visualizeProp(prop, OVMPropertyVisualizer<MeshT>::mesh->faces_begin(), OVMPropertyVisualizer<MeshT>::mesh->faces_end());\
    if (_setDrawMode)\
    {\
        VolumeMeshObject<MeshT>* object;\
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);\
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.facesColoredPerFace);\
    }\
}\
template <Template>\
void Classname::visualizeHalffaceProp(bool _setDrawMode)\
{\
    OpenVolumeMesh::HalfFacePropertyT<PropType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_halfface_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());\
    visualizeProp(prop, OVMPropertyVisualizer<MeshT>::mesh->halffaces_begin(), OVMPropertyVisualizer<MeshT>::mesh->halffaces_end());\
    if (_setDrawMode)\
    {\
        VolumeMeshObject<MeshT>* object;\
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);\
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.halffacesColoredPerHalfface);\
    }\
}\
template <Template>\
void Classname::visualizeEdgeProp(bool _setDrawMode)\
{\
    OpenVolumeMesh::EdgePropertyT<PropType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_edge_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());\
    visualizeProp(prop, OVMPropertyVisualizer<MeshT>::mesh->edges_begin(), OVMPropertyVisualizer<MeshT>::mesh->edges_end());\
    if (_setDrawMode)\
    {\
        VolumeMeshObject<MeshT>* object;\
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);\
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.edgesColoredPerEdge);\
    }\
}\
template <Template>\
void Classname::visualizeHalfedgeProp(bool _setDrawMode)\
{\
    OpenVolumeMesh::HalfEdgePropertyT<PropType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_halfedge_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());\
    visualizeProp(prop, OVMPropertyVisualizer<MeshT>::mesh->halfedges_begin(), OVMPropertyVisualizer<MeshT>::mesh->halfedges_end());\
    if (_setDrawMode)\
    {\
        VolumeMeshObject<MeshT>* object;\
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);\
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.halfedgesColoredPerHalfedge);\
    }\
}\
template <Template>\
void Classname::visualizeVertexProp(bool _setDrawMode)\
{\
    OpenVolumeMesh::VertexPropertyT<PropType> prop = OVMPropertyVisualizer<MeshT>::mesh->template request_vertex_property<PropType>(OVMPropertyVisualizer<MeshT>::propertyInfo.propName());\
    visualizeProp(prop, OVMPropertyVisualizer<MeshT>::mesh->vertices_begin(), OVMPropertyVisualizer<MeshT>::mesh->vertices_end());\
    if (_setDrawMode)\
    {\
        VolumeMeshObject<MeshT>* object;\
        PluginFunctions::getObject(OVMPropertyVisualizer<MeshT>::mObjectID, object);\
        object->setObjectDrawMode(OVMPropertyVisualizer<MeshT>::drawModes.verticesColored);\
    }\
}\


#if defined(INCLUDE_TEMPLATES) && !defined(OVM_PROPERTY_VISUALIZER_CC)
#include "OVMPropertyVisualizerT.cc"
#endif

#endif /* OVM_PROPERTY_VISUALIZER_HH */

#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */
