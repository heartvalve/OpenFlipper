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

#define OVM_PROPERTY_MODEL_CC

#include "OVMPropertyModel.hh"

#define INITIALIZE_PROPTYPES(primitive)                                                                                       \
    proptype_##primitive##_bool(TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<bool>), "bool")),                 \
    proptype_##primitive##_int(TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<int>), "int")),                     \
    proptype_##primitive##_uint(TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<unsigned int>), "unsigned int")),   \
    proptype_##primitive##_double(TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<double>), "double")),             \
    proptype_##primitive##_Vec3d(TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<ACG::Vec3d>), "Vec3d")),           \
    proptype_##primitive##_Vec3f(TypeInfoWrapper(typeid(OpenVolumeMesh::primitive##PropertyT<ACG::Vec3f>), "Vec3f"))


template <typename MeshT>
OVMPropertyModel<MeshT>::OVMPropertyModel(MeshT* mesh, int objectID, QObject *parent)
    : OVMPropertyModelSubclass(parent),
      mesh_(mesh),
      objectID_(objectID),
      mCombineProperty1(0),
      mCombineProperty2(0),
      INITIALIZE_PROPTYPES(Cell),
      INITIALIZE_PROPTYPES(Face),
      INITIALIZE_PROPTYPES(HalfFace),
      INITIALIZE_PROPTYPES(Edge),
      INITIALIZE_PROPTYPES(HalfEdge),
      INITIALIZE_PROPTYPES(Vertex)
{
    gatherProperties();
    bCombine.setText(tr("Combine"));
    bCombine.hide();
    connect(&bCombine, SIGNAL(clicked()),
            this,      SLOT(slotCombine()));
    widgets->layout()->addWidget(&bCombine);

    widgets->layout()->addWidget(&mLoadSaveWidget);

    connect(mLoadSaveWidget.save_property , SIGNAL(clicked()),
            this,                           SLOT(slotSaveProperty()));

    connect(mLoadSaveWidget.load_property , SIGNAL(clicked()),
            this,                           SLOT(slotLoadProperty()));

    widgets->layout()->addWidget(&mPickWidget);
    connect(mPickWidget.pickButton, SIGNAL(clicked()),
            this,                   SLOT(slotPickProperty()));
    mPickWidget.hide();
    QString iconPath = OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator();
    mPickWidget.pickButton->setIcon( QIcon(iconPath + "color-picker.png") );

    initializeSupportedPropertyTypes();
}

#undef INITIALIZE_PROPTYPES

template <typename MeshT>
void OVMPropertyModel<MeshT>::updateWidget(const QModelIndexList& selectedIndices)
{
    PropertyModel::updateWidget(selectedIndices);

    if (selectedIndices.size() == 2)
    {
        if (combinable(propertyVisualizers[selectedIndices[0].row()], propertyVisualizers[selectedIndices[1].row()]))
        {

            bCombine.show();
            mCombineProperty1 = &propertyVisualizers[selectedIndices[0].row()]->getPropertyInfo();
            mCombineProperty2 = &propertyVisualizers[selectedIndices[1].row()]->getPropertyInfo();
        }
        else
            bCombine.hide();
    }
    else
    {
        bCombine.hide();
    }

    if (selectedIndices.size() == 1)
        mPickWidget.show();
    else
    {
        mPickWidget.hide();
        resetPicking();
    }

}

template <typename MeshT>
void OVMPropertyModel<MeshT>::resetPicking()
{
    PluginFunctions::pickMode(lastPickMode);
    PluginFunctions::actionMode(lastActionMode);
}


/**
 * @brief Toggle picking on and off.
 *
 * This method will start and stop the picking.
 */
template <typename MeshT>
void OVMPropertyModel<MeshT>::pickProperty()
{
    if ( mPickWidget.pickButton->isChecked() ){

      lastPickMode   = PluginFunctions::pickMode();
      lastActionMode = PluginFunctions::actionMode();

      PluginFunctions::pickMode(PROP_VIS);
      PluginFunctions::actionMode(Viewer::PickingMode);

    } else {
        resetPicking();
    }
}

template <typename MeshT>
void OVMPropertyModel<MeshT>::pickModeChanged(const std::string& _mode)
{
    pickModeActive = (_mode == PROP_VIS);

    if (!pickModeActive)
    {
        lastPickMode = _mode;
    }

    mPickWidget.pickButton->setChecked(pickModeActive);
}

/**
 * @brief Handles mouse events for picking.
 *
 * If picking mode is active this method will find the picked entity and display
 * its handle and value in the PickWidget.
 */
template <typename MeshT>
void OVMPropertyModel<MeshT>::mouseEvent(QMouseEvent* _event)
{
    if (!pickModeActive) return;
    if (currentlySelectedIndices.size() < 1) return;

    if (_event->type() == QEvent::MouseButtonPress)
    {
        unsigned int   node_idx;
        ACG::Vec3d     hit_point;


        OVMPropertyVisualizer<MeshT>* viz = (OVMPropertyVisualizer<MeshT>*) propertyVisualizers[currentlySelectedIndices.first().row()];
        unsigned int entityId = 0;

        ACG::SceneGraph::PickTarget pickTarget;
        if (viz->getPropertyInfo().isCellProp())
            pickTarget = ACG::SceneGraph::PICK_CELL;
        if (viz->getPropertyInfo().isFaceProp())
            pickTarget = ACG::SceneGraph::PICK_FACE;
        if (viz->getPropertyInfo().isHalffaceProp())
            pickTarget = ACG::SceneGraph::PICK_FACE;
        if (viz->getPropertyInfo().isEdgeProp())
            pickTarget = ACG::SceneGraph::PICK_EDGE;
        if (viz->getPropertyInfo().isHalfedgeProp())
            pickTarget = ACG::SceneGraph::PICK_FACE;
        if (viz->getPropertyInfo().isVertexProp())
            pickTarget = ACG::SceneGraph::PICK_VERTEX;

        if (PluginFunctions::scenegraphPick(pickTarget, _event->pos(),node_idx, entityId, &hit_point)) {
            BaseObjectData* object;
            PluginFunctions::getPickedObject(node_idx, object);

            if (object->id() == objectID_)
            {
                if (viz->getPropertyInfo().isHalfedgeProp() || viz->getPropertyInfo().isHalffaceProp())   //cant be picked directly
                    entityId = viz->getClosestPrimitiveId(entityId, hit_point);

                mPickWidget.pickedHandle->setText(tr("%1").arg(entityId));
                mPickWidget.pickedValue->setText(viz->getPropertyText(entityId));
            }
        }

    }
}

template <typename MeshT>
bool OVMPropertyModel<MeshT>::parseHeader(QString header, PropertyVisualizer*& propVis, unsigned int& n)
{
    QStringList headerParts = header.split(tr(", "), QString::SkipEmptyParts );
    int headerVersion = headerParts[0].toUInt();
    if (headerVersion == 1)
    {
        n = headerParts[1].toUInt();
        unsigned int nExpected = 0;

        PropertyInfo::ENTITY_FILTER filter = (PropertyInfo::ENTITY_FILTER)headerParts[2].toInt();
        switch (filter)
        {
            case PropertyInfo::EF_CELL:
                nExpected = mesh_->n_cells();
                break;
            case PropertyInfo::EF_FACE:
                nExpected = mesh_->n_faces();
                break;
            case PropertyInfo::EF_HALFFACE:
                nExpected = mesh_->n_halffaces();
                break;
            case PropertyInfo::EF_EDGE:
                nExpected = mesh_->n_edges();
                break;
            case PropertyInfo::EF_HALFEDGE:
                nExpected = mesh_->n_halfedges();
                break;
            case PropertyInfo::EF_VERTEX:
                nExpected = mesh_->n_vertices();
                break;
            default:
                nExpected = -1;
                break;
        }

        if (n != nExpected)
        {
            std::cerr << "unexpected number of entities" << std::endl;
            return false;
        }

        QString friendlyName = headerParts[3];

        if (!isSupported(friendlyName))
        {
            std::cerr << "unsupported property type " << friendlyName.toStdString() << std::endl;
            return false;
        }


        TypeInfoWrapper typeInfo = getSupportedTypeInfoWrapper(friendlyName, filter);

        QString propName = QInputDialog::getText(0, "Property Name", "Please enter name.",QLineEdit::Normal,headerParts[4]);
        if (propName == "") return false;

        bool replace = false;
        if (!(isPropertyFree(propName, filter, typeInfo) || replace))
        {
            NewNameMessageBox* msgBox = new NewNameMessageBox(propName);
            msgBox->exec();

            if (msgBox->rename)
                propName = QInputDialog::getText(0, "New Property Name", "Please enter new name.");
            else if (msgBox->cancel)
                return false;
            else if (msgBox->replace)
                replace = true;

            delete msgBox;
        }

        if (!replace)
        {
            addProperty(propName, friendlyName, filter);
            gatherProperties();
        }

        propVis = getPropertyVisualizer(propName, filter, typeInfo);

        return true;

    }
    else
    {
        std::cerr << "unsupported header format" << std::endl;
        return false;
    }
}


template<typename MeshT>
QString OVMPropertyModel<MeshT>::getLoadFilenameFilter()
{
    QString filter;
    filter =       tr("Vertex Property (*.vprop)");
    filter += tr(";; HalfEdge Property (*.hprop)");
    filter +=     tr(";; Edge Property (*.eprop)");
    filter +=     tr(";; Halfface Property (*.hfprop)");
    filter +=     tr(";; Face Property (*.fprop)");
    filter +=     tr(";; Cell Property (*.cprop)");
    filter +=         tr(";; All Files (*)");
    return filter;
}

template<typename MeshT>
QString OVMPropertyModel<MeshT>::getSaveFilenameFilter(unsigned int propId)
{
    PropertyVisualizer* propViz = propertyVisualizers[propId];

    QString filter;

    if (propViz->getPropertyInfo().isVertexProp())
        filter = tr("Vertex Property (*.vprop)");
    else if (propViz->getPropertyInfo().isHalfedgeProp())
        filter = tr("HalfEdge Property (*.hprop)");
    else if (propViz->getPropertyInfo().isEdgeProp())
        filter = tr("Edge Property (*.eprop)");
    else if (propViz->getPropertyInfo().isFaceProp())
        filter = tr("Face Property (*.fprop)");

    filter += tr(";; All Files (*)");

    return filter;
}


/**
 * @brief Checks if a property name is still available for an entity type and a property type.
 *
 * @param propName The name of the property.
 * @param filter The entity type of the property.
 * @param typeInfo The type of the property.
 *
 * @return True if the name is still available, False if already in use.
 */
template<typename MeshT>
bool OVMPropertyModel<MeshT>::isPropertyFree(QString propName, PropertyInfo::ENTITY_FILTER filter, TypeInfoWrapper typeInfo)
{
    return getPropertyVisualizer(propName, filter, typeInfo) == 0;
}

/**
 * @brief Combines two properties.
 *
 * If the user selects two properties which are both of a vector type (Vec3f or Vec3d)
 * they can be combined. This method will create a new PropertyVisualizer (an
 * OVMPropertyVisualizerVectorFieldDifference<MeshT> to be exact) but not an additional
 * property.
 */
template<typename MeshT>
void OVMPropertyModel<MeshT>::combine()
{
    beginResetModel();
    propertyVisualizers.push_back(new OVMPropertyVisualizerVectorFieldDifference<MeshT>(mesh_, objectID_, *mCombineProperty1, *mCombineProperty2));
    endResetModel();
}

template<typename MeshT>
void OVMPropertyModel<MeshT>::saveProperty()
{
    for (QModelIndexList::const_iterator it = currentlySelectedIndices.begin(), it_end = currentlySelectedIndices.end();
                it != it_end; ++it) {
        PropertyModel::saveProperty(it->row());
    }
}

template<typename MeshT>
bool OVMPropertyModel<MeshT>::isVectorType(const TypeInfoWrapper& typeInfo) const
{
    return     (typeInfo == proptype_Cell_Vec3f)     || (typeInfo == proptype_Cell_Vec3d)
            || (typeInfo == proptype_Face_Vec3f)     || (typeInfo == proptype_Face_Vec3d)
            || (typeInfo == proptype_HalfFace_Vec3f) || (typeInfo == proptype_HalfFace_Vec3d)
            || (typeInfo == proptype_Edge_Vec3f)     || (typeInfo == proptype_Edge_Vec3d)
            || (typeInfo == proptype_HalfEdge_Vec3f) || (typeInfo == proptype_HalfEdge_Vec3d)
            || (typeInfo == proptype_Vertex_Vec3f)   || (typeInfo == proptype_Vertex_Vec3d);
}

/**
 * @brief Checks if two properties are combinable.
 *
 * @param propertyVisualizer1 First PropertyVisualizer for combination.
 * @param propertyVisualizer2 Second PropertyVisualizer for combination.
 *
 * @return True if the two properties are combinable, false if not.
 *
 * If the user selects two properties which are both of a vector type (Vec3f or Vec3d)
 * they can be combined. This method will check if the two provided PropertyVisualizers can
 * be comined. Currently PropertyVisualizers can only be combined if both visualize a
 * property with a vector type and the same entity type.
 */
template<typename MeshT>
bool OVMPropertyModel<MeshT>::combinable(PropertyVisualizer* propertyVisualizer1, PropertyVisualizer* propertyVisualizer2) const
{
    const PropertyInfo& propInfo1 = propertyVisualizer1->getPropertyInfo();
    const PropertyInfo& propInfo2 = propertyVisualizer2->getPropertyInfo();
    TypeInfoWrapper typeInfo1 = propInfo1.typeinfo();
    TypeInfoWrapper typeInfo2 = propInfo2.typeinfo();

    return (isVectorType(typeInfo1) && isVectorType(typeInfo2)) && (propInfo1.entityType() == propInfo2.entityType());
}

template<typename MeshT>
void OVMPropertyModel<MeshT>::gatherProperties( MeshT* mesh,
                       typename MeshT::Properties::const_iterator props_first,
                       typename MeshT::Properties::const_iterator props_last,
                       PropertyInfo::ENTITY_FILTER filter)
{
    for (typename MeshT::Properties::const_iterator pit = props_first; pit != props_last; ++pit) {
         OpenVolumeMesh::BaseProperty*const baseProp = *pit;
         if (baseProp && isSupported(baseProp) && isNew(baseProp, filter))
            addPropertyVisualizer(baseProp, mesh, filter);
    }
}


template<typename MeshT>
void OVMPropertyModel<MeshT>::gatherProperties()
{
    beginResetModel();
    if (mesh_) {
        gatherProperties(mesh_, mesh_->face_props_begin(), mesh_->face_props_end(), PropertyInfo::EF_FACE);
        gatherProperties(mesh_, mesh_->edge_props_begin(), mesh_->edge_props_end(), PropertyInfo::EF_EDGE);
        gatherProperties(mesh_, mesh_->halfedge_props_begin(), mesh_->halfedge_props_end(), PropertyInfo::EF_HALFEDGE);
        gatherProperties(mesh_, mesh_->vertex_props_begin(), mesh_->vertex_props_end(), PropertyInfo::EF_VERTEX);
        gatherProperties(mesh_, mesh_->halfface_props_begin(), mesh_->halfface_props_end(), PropertyInfo::EF_HALFFACE);
        gatherProperties(mesh_, mesh_->cell_props_begin(), mesh_->cell_props_end(), PropertyInfo::EF_CELL);
    }
    endResetModel();
}


/**
 * @brief Checks if visualizing this property is supported.
 *
 * @param baseProp A pointer to the property that we want to visualize.
 *
 * @return True if the property can be visualized, False if not.
 */
template<typename MeshT>
bool OVMPropertyModel<MeshT>::isSupported(OpenVolumeMesh::BaseProperty* const baseProp) const
{
    TypeInfoWrapper bp_type = typeid(*baseProp);
    TypeInfoWrapperSet::const_iterator propIt = supportedPropertyTypes.find(bp_type);
    return propIt != supportedPropertyTypes.end();
}

/**
 * @brief Checks if visualizing this property is supported.
 *
 * @param friendlyName The type we want to visualize in text form.
 *
 * @return True if the property type can be visualized, False if not.
 *
 * Currently supported are the types "bool", "int", "unsigned int", "double", "Vec3d"
 * and "Vec3f"
 */
template<typename MeshT>
bool OVMPropertyModel<MeshT>::isSupported(QString friendlyName) const
{
    for (TypeInfoWrapperSet::const_iterator it =  supportedPropertyTypes.begin();
                                            it != supportedPropertyTypes.end();
                                          ++it )
    {
        if (friendlyName.toStdString().compare(it->getName()) == 0)
            return true;
    }
    return false;
}

/**
 * @brief Checks if we already created a PropertyVisualizer for this property.
 *
 * @param baseProp A pointer to the property.
 * @param filter The entity type.
 *
 * @return True if we do not have PropertyVisualizer yet for that property, False if we do.
 */
template<typename MeshT>
bool OVMPropertyModel<MeshT>::isNew(OpenVolumeMesh::BaseProperty* const baseProp, PropertyInfo::ENTITY_FILTER filter) const
{
    for (unsigned int i = 0; i < propertyVisualizers.size(); ++i)
    {
        const PropertyInfo& propInfo =  propertyVisualizers[i]->getPropertyInfo();
        if (propInfo == PropertyInfo(baseProp->name(), getSupportedTypeInfoWrapper(baseProp) , filter))
            return false;
    }
    return true;
}


/**
 * @brief Returns the TypeInfoWrapper for the property if it is supported.
 *
 * @param baseProp A pointer to the property.
 *
 * @return A TypeInfoWrapper containing all the type information for the provided property.
 */
template<typename MeshT>
TypeInfoWrapper OVMPropertyModel<MeshT>::getSupportedTypeInfoWrapper(OpenVolumeMesh::BaseProperty* const baseProp) const
{
    TypeInfoWrapper bp_type = typeid(*baseProp);
    TypeInfoWrapperSet::const_iterator propIt = supportedPropertyTypes.find(bp_type);
    if (propIt != supportedPropertyTypes.end())
        return *propIt;
    else
    {
        std::cerr << "error" << std::endl;
        return *propIt;
    }
}

/**
 * @brief Returns the TypeInfoWrapper for the type of property and entity if it is supported.
 *
 * @param friendlyName The type we want to visualize in text form.
 * @param filter The type of the entity
 *
 * @return A TypeInfoWrapper containing all the type information for the property type.
 */
template<typename MeshT>
TypeInfoWrapper OVMPropertyModel<MeshT>::getSupportedTypeInfoWrapper(QString friendlyName, PropertyInfo::ENTITY_FILTER filter) const
{

    for (TypeInfoWrapperSet::const_iterator it =  supportedPropertyTypes.begin();
                                            it != supportedPropertyTypes.end();
                                          ++it )
    {
        if ((friendlyName.toStdString().compare(it->getName()) == 0) && isEntityType(*it, filter))
            return *it;
    }
    throw std::exception();
}

template<typename MeshT>
bool OVMPropertyModel<MeshT>::isBoolType(const PropertyInfo& propInfo) const
{
    return propInfo.typeinfo() == proptype_Cell_bool ||
           propInfo.typeinfo() == proptype_Face_bool ||
           propInfo.typeinfo() == proptype_HalfFace_bool ||
           propInfo.typeinfo() == proptype_Edge_bool ||
           propInfo.typeinfo() == proptype_HalfEdge_bool ||
           propInfo.typeinfo() == proptype_Vertex_bool;
}

template<typename MeshT>
bool OVMPropertyModel<MeshT>::isIntType(const PropertyInfo& propInfo) const
{
    return propInfo.typeinfo() == proptype_Cell_int ||
           propInfo.typeinfo() == proptype_Face_int ||
           propInfo.typeinfo() == proptype_HalfFace_int ||
           propInfo.typeinfo() == proptype_Edge_int ||
           propInfo.typeinfo() == proptype_HalfEdge_int ||
           propInfo.typeinfo() == proptype_Vertex_int;
}

template<typename MeshT>
bool OVMPropertyModel<MeshT>::isDoubleType(const PropertyInfo& propInfo) const
{
    return propInfo.typeinfo() == proptype_Cell_double ||
           propInfo.typeinfo() == proptype_Face_double ||
           propInfo.typeinfo() == proptype_HalfFace_double ||
           propInfo.typeinfo() == proptype_Edge_double ||
           propInfo.typeinfo() == proptype_HalfEdge_double ||
           propInfo.typeinfo() == proptype_Vertex_double;
}

template<typename MeshT>
bool OVMPropertyModel<MeshT>::isUnsignedIntType(const PropertyInfo& propInfo) const
{
    return propInfo.typeinfo() == proptype_Cell_uint ||
           propInfo.typeinfo() == proptype_Face_uint ||
           propInfo.typeinfo() == proptype_HalfFace_uint ||
           propInfo.typeinfo() == proptype_Edge_uint ||
           propInfo.typeinfo() == proptype_HalfEdge_uint ||
           propInfo.typeinfo() == proptype_Vertex_uint;
}

template<typename MeshT>
bool OVMPropertyModel<MeshT>::isVec3dType(const PropertyInfo& propInfo) const
{
    return propInfo.typeinfo() == proptype_Cell_Vec3d ||
           propInfo.typeinfo() == proptype_Face_Vec3d ||
           propInfo.typeinfo() == proptype_HalfFace_Vec3d ||
           propInfo.typeinfo() == proptype_Edge_Vec3d ||
           propInfo.typeinfo() == proptype_HalfEdge_Vec3d ||
           propInfo.typeinfo() == proptype_Vertex_Vec3d;
}

template<typename MeshT>
bool OVMPropertyModel<MeshT>::isVec3fType(const PropertyInfo& propInfo) const
{
    return propInfo.typeinfo() == proptype_Cell_Vec3f ||
           propInfo.typeinfo() == proptype_Face_Vec3f ||
           propInfo.typeinfo() == proptype_HalfFace_Vec3f ||
           propInfo.typeinfo() == proptype_Edge_Vec3f ||
           propInfo.typeinfo() == proptype_HalfEdge_Vec3f ||
           propInfo.typeinfo() == proptype_Vertex_Vec3f;
}

template<typename MeshT>
bool OVMPropertyModel<MeshT>::isVectorType(const PropertyInfo& propInfo) const
{
    return isVec3fType(propInfo) || isVec3dType(propInfo);
}

template<typename MeshT>
bool OVMPropertyModel<MeshT>::isEntityType(const TypeInfoWrapper& typeInfo, PropertyInfo::ENTITY_FILTER entity_type) const
{
    bool result = false;
    if (entity_type & PropertyInfo::EF_CELL)
    {
        result |=    (typeInfo == proptype_Cell_bool)
                  || (typeInfo == proptype_Cell_int)
                  || (typeInfo == proptype_Cell_double)
                  || (typeInfo == proptype_Cell_uint)
                  || (typeInfo == proptype_Cell_Vec3d)
                  || (typeInfo == proptype_Cell_Vec3d);
    }
    if (entity_type & PropertyInfo::EF_FACE)
    {
        result |=    (typeInfo == proptype_Face_bool)
                  || (typeInfo == proptype_Face_int)
                  || (typeInfo == proptype_Face_double)
                  || (typeInfo == proptype_Face_uint)
                  || (typeInfo == proptype_Face_Vec3d)
                  || (typeInfo == proptype_Face_Vec3d);
    }
    if (entity_type & PropertyInfo::EF_HALFFACE)
    {
        result |=    (typeInfo == proptype_HalfFace_bool)
                  || (typeInfo == proptype_HalfFace_int)
                  || (typeInfo == proptype_HalfFace_double)
                  || (typeInfo == proptype_HalfFace_uint)
                  || (typeInfo == proptype_HalfFace_Vec3d)
                  || (typeInfo == proptype_HalfFace_Vec3d);
    }
    if (entity_type & PropertyInfo::EF_EDGE)
    {
        result |=    (typeInfo == proptype_Edge_bool)
                  || (typeInfo == proptype_Edge_int)
                  || (typeInfo == proptype_Edge_double)
                  || (typeInfo == proptype_Edge_uint)
                  || (typeInfo == proptype_Edge_Vec3d)
                  || (typeInfo == proptype_Edge_Vec3d);
    }
    if (entity_type & PropertyInfo::EF_HALFEDGE)
    {
        result |=    (typeInfo == proptype_HalfEdge_bool)
                  || (typeInfo == proptype_HalfEdge_int)
                  || (typeInfo == proptype_HalfEdge_double)
                  || (typeInfo == proptype_HalfEdge_uint)
                  || (typeInfo == proptype_HalfEdge_Vec3d)
                  || (typeInfo == proptype_HalfEdge_Vec3d);
    }
    if (entity_type & PropertyInfo::EF_VERTEX)
    {
        result |=    (typeInfo == proptype_Vertex_bool)
                  || (typeInfo == proptype_Vertex_int)
                  || (typeInfo == proptype_Vertex_double)
                  || (typeInfo == proptype_Vertex_uint)
                  || (typeInfo == proptype_Vertex_Vec3d)
                  || (typeInfo == proptype_Vertex_Vec3d);
    }
    return result;
}


/**
 * @brief Adds a new PropertyVisualizer.
 *
 * @param baseProp A pointer to the property that should be visualized.
 * @param mesh A pointer to the mesh whose properties should be visualized.
 * @param filter The entity type that should be visualized.
 *
 * This method creates a new PropertyVisualizer depending on the provieded parameters and adds it to the list
 * of PropertyVisualizers.
 */
template<typename MeshT>
void OVMPropertyModel<MeshT>::addPropertyVisualizer(OpenVolumeMesh::BaseProperty* const baseProp, MeshT* mesh, PropertyInfo::ENTITY_FILTER filter)
{
    PropertyInfo propInfo = PropertyInfo(baseProp->name(), getSupportedTypeInfoWrapper(baseProp) , filter);
    if (isBoolType(propInfo))
        propertyVisualizers.push_back(new OVMPropertyVisualizerBoolean<MeshT>(mesh, objectID_, propInfo));
    else if (isIntType(propInfo))
        propertyVisualizers.push_back(new OVMPropertyVisualizerInteger<MeshT, int>(mesh, objectID_, propInfo, false));
    else if (isUnsignedIntType(propInfo))
        propertyVisualizers.push_back(new OVMPropertyVisualizerInteger<MeshT, unsigned int>(mesh, objectID_, propInfo, true));
    else if (isDoubleType(propInfo))
        propertyVisualizers.push_back(new OVMPropertyVisualizerDouble<MeshT>(mesh, objectID_, propInfo));
    else if (isVectorType(propInfo))
        propertyVisualizers.push_back(new OVMPropertyVisualizerVector<MeshT>(mesh, objectID_, propInfo));
    connectLogs(propertyVisualizers.back());
}

/**
 * @brief Adds a new property to the mesh.
 *
 * @param propName The name of the property that should be added.
 * @param friendlyTypeName The type of the property.
 * @param filter The entity type of the property.
 *
 * This method adds a new property to the mesh. The types "Vec3d", "Vec3d", "double", "unsigned int",
 * "int" and "bool" can be added for every entity type.
 */
template<typename MeshT>
void OVMPropertyModel<MeshT>::addProperty(QString propName, QString friendlyTypeName, PropertyInfo::ENTITY_FILTER filter)
{

    QString dtype = friendlyTypeName;
    std::string pname = propName.toStdString();

    MeshT* mesh = mesh_;

    if ( filter == PropertyInfo::EF_VERTEX )
    {
        if ( (dtype == tr("Vec3d")) || (dtype == tr("Vec3f")) )
        {
            OpenVolumeMesh::VertexPropertyT< ACG::Vec3d > prop = mesh->template request_vertex_property< ACG::Vec3d >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("double") )
        {
            OpenVolumeMesh::VertexPropertyT< double > prop = mesh->template request_vertex_property< double >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("unsigned int") )
        {
            OpenVolumeMesh::VertexPropertyT< unsigned int > prop = mesh->template request_vertex_property< unsigned int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("int") )
        {
            OpenVolumeMesh::VertexPropertyT< int > prop = mesh->template request_vertex_property< int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("bool") )
        {
            OpenVolumeMesh::VertexPropertyT< bool > prop = mesh->template request_vertex_property< bool >(pname);
            mesh->set_persistent(prop, true);
        }
    }
    else if ( filter == PropertyInfo::EF_EDGE )
    {
        if ( (dtype == tr("Vec3d")) || (dtype == tr("Vec3f")) )
        {
            OpenVolumeMesh::EdgePropertyT< ACG::Vec3d > prop = mesh->template request_edge_property< ACG::Vec3d >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("double") )
        {
            OpenVolumeMesh::EdgePropertyT< double > prop = mesh->template request_edge_property< double >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("unsgined int") )
        {
            OpenVolumeMesh::EdgePropertyT< unsigned int > prop = mesh->template request_edge_property< unsigned int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("int") )
        {
            OpenVolumeMesh::EdgePropertyT< int > prop = mesh->template request_edge_property< int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("bool") )
        {
            OpenVolumeMesh::EdgePropertyT< bool > prop = mesh->template request_edge_property< bool >(pname);
            mesh->set_persistent(prop, true);
        }
    }
    else if ( filter == PropertyInfo::EF_FACE )
    {
        if ( (dtype == tr("Vec3d")) || (dtype == tr("Vec3f")) )
        {
            OpenVolumeMesh::FacePropertyT< ACG::Vec3d > prop = mesh->template request_face_property< ACG::Vec3d >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("double") )
        {
            OpenVolumeMesh::FacePropertyT< double > prop = mesh->template request_face_property< double >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("unsigned int") )
        {
            OpenVolumeMesh::FacePropertyT< unsigned int > prop = mesh->template request_face_property< unsigned int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("int") )
        {
            OpenVolumeMesh::FacePropertyT< int > prop = mesh->template request_face_property< int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("bool") )
        {
            OpenVolumeMesh::FacePropertyT< bool > prop = mesh->template request_face_property< bool >(pname);
            mesh->set_persistent(prop, true);
        }
    }
    else if ( filter == PropertyInfo::EF_HALFEDGE )
    {
        if ( (dtype == tr("Vec3d")) || (dtype == tr("Vec3f")) )
        {
            OpenVolumeMesh::HalfEdgePropertyT< ACG::Vec3d > prop = mesh->template request_halfedge_property< ACG::Vec3d >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("double") )
        {
            OpenVolumeMesh::HalfEdgePropertyT< double > prop = mesh->template request_halfedge_property< double >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("unsigned int") )
        {
            OpenVolumeMesh::HalfEdgePropertyT< unsigned int > prop = mesh->template request_halfedge_property< unsigned int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("int") )
        {
            OpenVolumeMesh::HalfEdgePropertyT< int > prop = mesh->template request_halfedge_property< int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("bool") )
        {
            OpenVolumeMesh::HalfEdgePropertyT< bool > prop = mesh->template request_halfedge_property< bool >(pname);
            mesh->set_persistent(prop, true);
        }
    }
    else if ( filter == PropertyInfo::EF_HALFFACE )
    {
        if ( (dtype == tr("Vec3d")) || (dtype == tr("Vec3f")) )
        {
            OpenVolumeMesh::HalfFacePropertyT< ACG::Vec3d > prop = mesh->template request_halfface_property< ACG::Vec3d >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("double") )
        {
            OpenVolumeMesh::HalfFacePropertyT< double > prop = mesh->template request_halfface_property< double >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("unsigned int") )
        {
            OpenVolumeMesh::HalfFacePropertyT< unsigned int > prop = mesh->template request_halfface_property< unsigned int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("int") )
        {
            OpenVolumeMesh::HalfFacePropertyT< int > prop = mesh->template request_halfface_property< int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("bool") )
        {
            OpenVolumeMesh::HalfFacePropertyT< bool > prop = mesh->template request_halfface_property< bool >(pname);
            mesh->set_persistent(prop, true);
        }
    }
    else if ( filter == PropertyInfo::EF_CELL )
    {
        if ( (dtype == tr("Vec3d")) || (dtype == tr("Vec3f")) )
        {
            OpenVolumeMesh::CellPropertyT< ACG::Vec3d > prop = mesh->template request_cell_property< ACG::Vec3d >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("double") )
        {
            OpenVolumeMesh::CellPropertyT< double > prop = mesh->template request_cell_property< double >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("unsigned int") )
        {
            OpenVolumeMesh::CellPropertyT< unsigned int > prop = mesh->template request_cell_property< unsigned int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("int") )
        {
            OpenVolumeMesh::CellPropertyT< int > prop = mesh->template request_cell_property< int >(pname);
            mesh->set_persistent(prop, true);
        }
        else if ( dtype == tr("bool") )
        {
            OpenVolumeMesh::CellPropertyT< bool > prop = mesh->template request_cell_property< bool >(pname);
            mesh->set_persistent(prop, true);
        }
    }

}

template <typename MeshT>
void OVMPropertyModel<MeshT>::initializeSupportedPropertyTypes()
{
    
#define INSERT_PROPTYPES(primitive) \
supportedPropertyTypes.insert(proptype_##primitive##_bool);   \
supportedPropertyTypes.insert(proptype_##primitive##_int);    \
supportedPropertyTypes.insert(proptype_##primitive##_uint);   \
supportedPropertyTypes.insert(proptype_##primitive##_double); \
supportedPropertyTypes.insert(proptype_##primitive##_Vec3d);  \
supportedPropertyTypes.insert(proptype_##primitive##_Vec3f);  \

    INSERT_PROPTYPES(Cell)
    INSERT_PROPTYPES(Face)
    INSERT_PROPTYPES(HalfFace)
    INSERT_PROPTYPES(Edge)
    INSERT_PROPTYPES(HalfEdge)
    INSERT_PROPTYPES(Vertex)

#undef INITIALIZE_PROPTYPES

    
    
}

#endif /* ENABLE_OPENVOLUMEMESH_SUPPORT */
