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

//#ifdef ENABLE_OPENVOLUMEMESH

#ifndef OVM_PROPERTY_MODEL_H
#define OVM_PROPERTY_MODEL_H

#include "../PropertyModel.hh"
#include "OVMPropertyVisualizer.hh"
#include "OVMPropertyVisualizerBoolean.hh"
#include "OVMPropertyVisualizerDouble.hh"
#include "OVMPropertyVisualizerInteger.hh"
#include "OVMPropertyVisualizerVector.hh"
#include "OVMPropertyVisualizerVectorFieldDifference.hh"

#include "../Utils.hh"
#include <OpenVolumeMesh/Core/BaseProperty.hh>
#include <ObjectTypes/PolyhedralMesh/PolyhedralMesh.hh>

#include "Widgets/LoadSaveWidget.hh"
#include "Widgets/PickWidget.hh"

#include <vector>

#include "../Utils.hh"

#define PROP_VIS "PropertyVisualization"

class PropertyVisPlugin;

/* This class adds signal/slot support for OVMPropertyModelSubclass. This needs
 * to be done because "Sorry, Qt does not support templates that contain signals,
 * slots or Q_OBJECT"
 * http://doc.trolltech.com/qq/qq15-academic.html
 */
class OVMPropertyModelSubclass: public PropertyModel
{
Q_OBJECT
public:
    OVMPropertyModelSubclass(QObject *parent = 0)
        : PropertyModel(parent)
    {
    }

public slots:
    virtual void slotCombine(){ combine(); }
    virtual void slotSaveProperty(){ saveProperty(); }
    virtual void slotLoadProperty(){ loadProperty(); }
    virtual void slotPickProperty(){ pickProperty(); }

private:
    virtual void combine(){}
    virtual void saveProperty(){}
    virtual void pickProperty(){}

};

template<typename MeshT>
class OVMPropertyModel: public OVMPropertyModelSubclass {

public:
    OVMPropertyModel(MeshT* mesh, int objectID, QObject *parent = 0);
    virtual ~OVMPropertyModel(){}

    virtual void updateWidget(const QModelIndexList& selectedIndices);

    /// Disables picking.
    void resetPicking();

    /// Toggle picking on and off.
    virtual void pickProperty();

    /// Handles changing of pick mode.
    virtual void pickModeChanged(const std::string& _mode);

    /// Handles mouse events for picking.
    virtual void mouseEvent(QMouseEvent* _event);

    virtual bool parseHeader(QString header, PropertyVisualizer*& propVis, unsigned int& n);

protected:

    virtual QString getLoadFilenameFilter();
    virtual QString getSaveFilenameFilter(unsigned int propId);

private:

    /// Combines two properties.
    virtual void combine();

    /// Checks if two properties are combinable.
    bool combinable(PropertyVisualizer* propertyVisualizer1, PropertyVisualizer* propertyVisualizer2) const;

    /// Saves the currently slected properties.
    virtual void saveProperty();

    /// Searches for properties with a specific entity type and creates visualizers.
    void gatherProperties( MeshT* mesh,
                           typename MeshT::Properties::const_iterator props_first,
                           typename MeshT::Properties::const_iterator props_last,
                           PropertyInfo::ENTITY_FILTER filter);
    /// Searches for all properties and creates the visualizers.
    void gatherProperties();

    /// Checks if visualizing this property is supported.
    bool isSupported(OpenVolumeMesh::BaseProperty* const baseProp) const;

    /// Checks if visualizing this type of property is supported.
    bool isSupported(QString friendlyName) const;

    /// Checks if we already created a PropertyVisualizer for this property.
    bool isNew(OpenVolumeMesh::BaseProperty* const baseProp, PropertyInfo::ENTITY_FILTER filter) const;

    /// Checks if a property name is still available for an entity type and a property type.
    bool isPropertyFree(QString propName, PropertyInfo::ENTITY_FILTER filter, TypeInfoWrapper typeInfo);

    /// Returns the TypeInfoWrapper for the property if it is supported.
    TypeInfoWrapper getSupportedTypeInfoWrapper(OpenVolumeMesh::BaseProperty* const baseProp) const;

    /// Returns the TypeInfoWrapper for the type of property if it is supported.
    TypeInfoWrapper getSupportedTypeInfoWrapper(QString friendlyName) const;

    bool isBoolType(const PropertyInfo& propInfo) const;
    bool isIntType(const PropertyInfo& propInfo) const;
    bool isDoubleType(const PropertyInfo& propInfo) const;
    bool isUnsignedIntType(const PropertyInfo& propInfo) const;
    bool isVec3dType(const PropertyInfo& propInfo) const;
    bool isVec3fType(const PropertyInfo& propInfo) const;
    bool isVectorType(const PropertyInfo& propInfo) const;
    bool isVectorType(const TypeInfoWrapper& typeInfo) const;

    /// Adds a new PropertyVisualizer.
    void addPropertyVisualizer(OpenVolumeMesh::BaseProperty* const baseProp, MeshT* mesh, PropertyInfo::ENTITY_FILTER filter);

    /// Adds a new property to the mesh.
    void addProperty(QString propName, QString friendlyTypeName, PropertyInfo::ENTITY_FILTER filter);

    MeshT* mesh_;

    int objectID_;

#define DECLARE_PROPTYPES(primitive)                                              \
    static const TypeInfoWrapper proptype_##primitive##_bool;                     \
    static const TypeInfoWrapper proptype_##primitive##_int;                      \
    static const TypeInfoWrapper proptype_##primitive##_uint;                     \
    static const TypeInfoWrapper proptype_##primitive##_double;                   \
    static const TypeInfoWrapper proptype_##primitive##_Vec3d;                    \
    static const TypeInfoWrapper proptype_##primitive##_Vec3f;

    DECLARE_PROPTYPES(Cell)
    DECLARE_PROPTYPES(Face)
    DECLARE_PROPTYPES(HalfFace)
    DECLARE_PROPTYPES(Edge)
    DECLARE_PROPTYPES(HalfEdge)
    DECLARE_PROPTYPES(Vertex)

#undef DECLARE_PROPTYPES


    typedef std::set<TypeInfoWrapper> TypeInfoWrapperSet;
    static const TypeInfoWrapper propertyTypes[];
    static const TypeInfoWrapperSet supportedPropertyTypes;

    QPushButton bCombine;
    const PropertyInfo* mCombineProperty1;
    const PropertyInfo* mCombineProperty2;

    LoadSaveWidget mLoadSaveWidget;

    PickWidget mPickWidget;

    bool pickModeActive;
    std::string lastPickMode;
    Viewer::ActionMode lastActionMode;

};


#define INITIALIZE_PROPTYPES(primitive)                                                                 \
template <typename MeshT>                                                                               \
const TypeInfoWrapper OVMPropertyModel<MeshT>::proptype_##primitive##_bool =                            \
        TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<bool>), "bool");                    \
template <typename MeshT>                                                                               \
const TypeInfoWrapper OVMPropertyModel<MeshT>::proptype_##primitive##_int =                             \
        TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<int>), "int");                      \
template <typename MeshT>                                                                               \
const TypeInfoWrapper OVMPropertyModel<MeshT>::proptype_##primitive##_uint =                            \
        TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<unsigned int>), "unsigned int");    \
template <typename MeshT>                                                                               \
const TypeInfoWrapper OVMPropertyModel<MeshT>::proptype_##primitive##_double =                          \
        TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<double>), "double");                \
template <typename MeshT>                                                                               \
const TypeInfoWrapper OVMPropertyModel<MeshT>::proptype_##primitive##_Vec3d =                           \
        TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<ACG::Vec3d>), "Vec3d");             \
template <typename MeshT>                                                                               \
const TypeInfoWrapper OVMPropertyModel<MeshT>::proptype_##primitive##_Vec3f =                           \
        TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<ACG::Vec3f>), "Vec3f");

INITIALIZE_PROPTYPES(Cell)
INITIALIZE_PROPTYPES(Face)
INITIALIZE_PROPTYPES(HalfFace)
INITIALIZE_PROPTYPES(Edge)
INITIALIZE_PROPTYPES(HalfEdge)
INITIALIZE_PROPTYPES(Vertex)

#undef INITIALIZE_PROPTYPES


#define LIST_PROPTYPES(primitive)                       \
OVMPropertyModel<MeshT>::proptype_##primitive##_bool,   \
OVMPropertyModel<MeshT>::proptype_##primitive##_int,    \
OVMPropertyModel<MeshT>::proptype_##primitive##_uint,   \
OVMPropertyModel<MeshT>::proptype_##primitive##_double, \
OVMPropertyModel<MeshT>::proptype_##primitive##_Vec3d,  \
OVMPropertyModel<MeshT>::proptype_##primitive##_Vec3f


template <typename MeshT>
const TypeInfoWrapper OVMPropertyModel<MeshT>::propertyTypes[] = { LIST_PROPTYPES(Cell),
                                                                   LIST_PROPTYPES(Face),
                                                                   LIST_PROPTYPES(HalfFace),
                                                                   LIST_PROPTYPES(Edge),
                                                                   LIST_PROPTYPES(HalfEdge),
                                                                   LIST_PROPTYPES(Vertex)
                                                                };

#undef LIST_PROPTYPES


template <typename MeshT>
const typename OVMPropertyModel<MeshT>::TypeInfoWrapperSet OVMPropertyModel<MeshT>::supportedPropertyTypes(propertyTypes, propertyTypes + (6/*num of primitves*/ * 6/*num of types per primitive*/));


#if defined(INCLUDE_TEMPLATES) && !defined(OVM_PROPERTY_MODEL_CC)
#include "OVMPropertyModelT.cc"
#endif

#endif /* OVM_PROPERTY_MODEL_H */

//#endif /* ENABLE_OPENVOLUMEMESH */
