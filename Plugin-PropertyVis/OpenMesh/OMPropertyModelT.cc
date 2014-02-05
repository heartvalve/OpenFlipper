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

#define OM_PROPERTY_MODEL_CC

#include "OMPropertyModel.hh"

#include "OMPropertyVisualizerBoolean.hh"
#include "OMPropertyVisualizerDouble.hh"
#include "OMPropertyVisualizerInteger.hh"
#include "OMPropertyVisualizerVector.hh"
#include "OMPropertyVisualizerVectorFieldDifference.hh"

#ifdef ENABLE_SKELETON_SUPPORT
#include "OMPropertyVisualizerSkinWeights.hh"
#endif

#include "../Utils.hh"

#include <vector>

#define PROP_VIS "PropertyVisualization"

template<typename MeshT>
OMPropertyModel<MeshT>::OMPropertyModel(MeshT* mesh, int objectID, QObject *parent)
    : OMPropertyModelSubclass(parent),
      mesh_(mesh),
      objectID_(objectID),
      mCombineProperty1(0),
      mCombineProperty2(0),
      pickModeActive(false),
      proptype_bool(TypeInfoWrapper(typeid(OpenMesh::PropertyT<bool>), "bool")),
      proptype_int(TypeInfoWrapper(typeid(OpenMesh::PropertyT<int>), "int")),
      proptype_uint(TypeInfoWrapper(typeid(OpenMesh::PropertyT<unsigned int>), "unsigned int")),
      proptype_double(TypeInfoWrapper(typeid(OpenMesh::PropertyT<double>), "double")),
      proptype_Vec3d(TypeInfoWrapper(typeid(OpenMesh::PropertyT<ACG::Vec3d>), "Vec3d")),
      proptype_Vec3f(TypeInfoWrapper(typeid(OpenMesh::PropertyT<ACG::Vec3f>), "Vec3f"))
#ifdef ENABLE_SKELETON_SUPPORT
      ,proptype_SkinWeights(TypeInfoWrapper(typeid(OpenMesh::PropertyT<BaseSkin::SkinWeights>), "SkinWeights"))
#endif
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


    lastPickMode   = PluginFunctions::pickMode();
    lastActionMode = PluginFunctions::actionMode();
    
    initializeSupportedPropertyTypes();

}

template<typename MeshT>
void OMPropertyModel<MeshT>::updateWidget(const QModelIndexList& selectedIndices)
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
        bCombine.hide();

    if (selectedIndices.size() == 1)
        mPickWidget.show();
    else
    {
        mPickWidget.hide();
        resetPicking();
    }
}

template<typename MeshT>
QString OMPropertyModel<MeshT>::getLoadFilenameFilter()
{
    QString filter;
    filter =       tr("Vertex Property (*.vprop)");
    filter += tr(";; HalfEdge Property (*.hprop)");
    filter +=     tr(";; Edge Property (*.eprop)");
    filter +=     tr(";; Face Property (*.fprop)");
    filter +=         tr(";; All Files (*)");
    return filter;
}

template<typename MeshT>
QString OMPropertyModel<MeshT>::getSaveFilenameFilter(unsigned int propId)
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
 * @brief Combines two properties.
 *
 * If the user selects two properties which are both of a vector type (Vec3f or Vec3d)
 * they can be combined. This method will create a new PropertyVisualizer (an
 * OMPropertyVisualizerVectorFieldDifference<MeshT> to be exact) but not an additional
 * property.
 */
template<typename MeshT>
void OMPropertyModel<MeshT>::combine()
{
    beginResetModel();
    if (isVectorType(mCombineProperty1->typeinfo()))
        propertyVisualizers.push_back(new OMPropertyVisualizerVectorFieldDifference<MeshT>(mesh_, *mCombineProperty1, *mCombineProperty2));
    endResetModel();
}

/**
 * @brief Checks if two properties are combinable.
 *
 * @param propertyVisualizer1 First PropertyVisualizer for combination.
 * @param propertyVisualizer2 Second PropertyVisualizer for combination.
 *
 * @return True if the two properties are combinable, False if not.
 *
 * If the user selects two properties which are both of a vector type (Vec3f or Vec3d)
 * they can be combined. This method will check if the two provided PropertyVisualizers can
 * be comined. Currently PropertyVisualizers can only be combined if both visualize a
 * property with a vector type and the same entity type.
 */
template<typename MeshT>
bool OMPropertyModel<MeshT>::combinable(PropertyVisualizer* propertyVisualizer1, PropertyVisualizer* propertyVisualizer2)
{
    const PropertyInfo& propInfo1 = propertyVisualizer1->getPropertyInfo();
    const PropertyInfo& propInfo2 = propertyVisualizer2->getPropertyInfo();
    TypeInfoWrapper typeInfo1 = propInfo1.typeinfo();
    TypeInfoWrapper typeInfo2 = propInfo2.typeinfo();

    return (isVectorType(typeInfo1) && isVectorType(typeInfo2)) && (propInfo1.entityType() == propInfo2.entityType());
}

template<typename MeshT>
void OMPropertyModel<MeshT>::saveProperty()
{
    for (QModelIndexList::const_iterator it = currentlySelectedIndices.begin(), it_end = currentlySelectedIndices.end();
                it != it_end; ++it) {
        PropertyModel::saveProperty(it->row());
    }
}

template<typename MeshT>
bool OMPropertyModel<MeshT>::parseHeader(QString header, PropertyVisualizer*& propVis, unsigned int &n)
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
            case PropertyInfo::EF_FACE:
                nExpected = mesh_->n_faces();
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
            emit log(LOGERR, "Could not load property: unexpected number of entities");
            return false;
        }

        QString friendlyName = headerParts[3];

        if (!isSupported(friendlyName))
        {
            emit log(LOGERR, tr("Could not load property: unsupported property type %1").arg(friendlyName));
            return false;
        }

        TypeInfoWrapper typeInfo = getSupportedTypeInfoWrapper(friendlyName);

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
        emit log(LOGERR, "Could not load property: unsupported header format");
        return false;
    }
}


template<typename MeshT>
void OMPropertyModel<MeshT>::setPropertyFromFile(QTextStream*& file_stream_, unsigned int n, PropertyVisualizer* propVis)
{
#ifdef ENABLE_SKELETON_SUPPORT
    if (propVis->getPropertyInfo().typeinfo() == proptype_SkinWeights)
    {
        for (unsigned int i = 0; i < n; ++i)
        {
            QString propertyText = "";
            QString tmp;
            while ((tmp = readLine(file_stream_)) != "")
                propertyText = propertyText + tmp;
            propVis->setPropertyFromText(i, propertyText);
        }
    }
    else
#endif
    {
        PropertyModel::setPropertyFromFile(file_stream_, n, propVis);
    }
}

template<typename MeshT>
void OMPropertyModel<MeshT>:: resetPicking()
{
    PluginFunctions::pickMode(lastPickMode);
    PluginFunctions::actionMode(lastActionMode);
}


/**
 * @brief Toggle picking on and off.
 *
 * This method will start and stop the picking.
 */
template<typename MeshT>
void OMPropertyModel<MeshT>::pickProperty()
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


template<typename MeshT>
void OMPropertyModel<MeshT>::pickModeChanged(const std::string& _mode)
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
template<typename MeshT>
void OMPropertyModel<MeshT>::mouseEvent(QMouseEvent* _event)
{
    if (!pickModeActive) return;

    if (_event->type() == QEvent::MouseButtonPress)
    {
        unsigned int   node_idx, face_idx;
        ACG::Vec3d     hit_point;

        if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE, _event->pos(),node_idx, face_idx, &hit_point)) {
            BaseObjectData* object;
            PluginFunctions::getPickedObject(node_idx, object);

            if (object->id() == objectID_)
            {
                OMPropertyVisualizer<MeshT>* viz = (OMPropertyVisualizer<MeshT>*) propertyVisualizers[currentlySelectedIndices.first().row()];
                unsigned int primitiveId = viz->getClosestPrimitiveId(face_idx, hit_point);
                mPickWidget.pickedHandle->setText(tr("%1").arg(primitiveId));
                mPickWidget.pickedValue->setText(viz->getPropertyText(primitiveId));
            }
        }
    }
}

template<typename MeshT>
bool OMPropertyModel<MeshT>::isVectorType(const TypeInfoWrapper& typeInfo) const
{
    return (typeInfo == proptype_Vec3f) || (typeInfo == proptype_Vec3d);
}

template<typename MeshT>
void OMPropertyModel<MeshT>::gatherProperties( MeshT* mesh,
                       typename MeshT::prop_iterator props_first,
                       typename MeshT::prop_iterator props_last,
                       PropertyInfo::ENTITY_FILTER filter)
{
    for (typename MeshT::prop_iterator pit = props_first; pit != props_last; ++pit) {
         OpenMesh::BaseProperty*const baseProp = *pit;
        if (baseProp && isSupported(baseProp) && isNew(baseProp, filter))
            addPropertyVisualizer(baseProp, mesh, filter);
    }
}

template<typename MeshT>
void OMPropertyModel<MeshT>::gatherProperties()
{
    beginResetModel();
    if (mesh_) {
        gatherProperties(mesh_, mesh_->fprops_begin(), mesh_->fprops_end(), PropertyInfo::EF_FACE);
        gatherProperties(mesh_, mesh_->eprops_begin(), mesh_->eprops_end(), PropertyInfo::EF_EDGE);
        gatherProperties(mesh_, mesh_->hprops_begin(), mesh_->hprops_end(), PropertyInfo::EF_HALFEDGE);
        gatherProperties(mesh_, mesh_->vprops_begin(), mesh_->vprops_end(), PropertyInfo::EF_VERTEX);
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
bool OMPropertyModel<MeshT>::isSupported(OpenMesh::BaseProperty* const baseProp) const
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
 * Currently supported are the types "bool", "int", "unsigned int", "double", "Vec3d",
 * "Vec3f" and "SkinWeights"
 */
template<typename MeshT>
bool OMPropertyModel<MeshT>::isSupported(QString friendlyName) const
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
 * @return True if we do not have PropertyVisualizer yet for that property, False if we do
 */
template<typename MeshT>
bool OMPropertyModel<MeshT>::isNew(OpenMesh::BaseProperty* const baseProp, PropertyInfo::ENTITY_FILTER filter)
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
TypeInfoWrapper OMPropertyModel<MeshT>::getSupportedTypeInfoWrapper(OpenMesh::BaseProperty* const baseProp)
{
    TypeInfoWrapper bp_type = typeid(*baseProp);
    TypeInfoWrapperSet::const_iterator propIt = supportedPropertyTypes.find(bp_type);
    return *propIt;
}

/**
 * @brief Returns the TypeInfoWrapper for the type of property if it is supported.
 *
 * @param friendlyName The type we want to visualize in text form.
 *
 * @return A TypeInfoWrapper containing all the type information for the property type.
 */
template<typename MeshT>
TypeInfoWrapper OMPropertyModel<MeshT>::getSupportedTypeInfoWrapper(QString friendlyName)
{
    for (TypeInfoWrapperSet::const_iterator it =  supportedPropertyTypes.begin();
                                            it != supportedPropertyTypes.end();
                                          ++it )
    {
        if (friendlyName.toStdString().compare(it->getName()) == 0)
            return *it;
    }
    throw std::exception();
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
void OMPropertyModel<MeshT>::addPropertyVisualizer(OpenMesh::BaseProperty* const baseProp, MeshT* mesh, PropertyInfo::ENTITY_FILTER filter)
{
    PropertyInfo propInfo = PropertyInfo(baseProp->name(), getSupportedTypeInfoWrapper(baseProp) , filter);
    if (propInfo.typeinfo() == proptype_bool)
        propertyVisualizers.push_back(new OMPropertyVisualizerBoolean<MeshT>(mesh, propInfo));
    else if (propInfo.typeinfo() == proptype_int)
        propertyVisualizers.push_back(new OMPropertyVisualizerInteger<MeshT, int>(mesh, propInfo, false));
    else if (propInfo.typeinfo() == proptype_uint)
        propertyVisualizers.push_back(new OMPropertyVisualizerInteger<MeshT, unsigned int>(mesh, propInfo, true));
    else if (propInfo.typeinfo() == proptype_double)
        propertyVisualizers.push_back(new OMPropertyVisualizerDouble<MeshT>(mesh, propInfo));
    else if ((propInfo.typeinfo() == proptype_Vec3d) || (propInfo.typeinfo() == proptype_Vec3f))
        propertyVisualizers.push_back(new OMPropertyVisualizerVector<MeshT>(mesh, propInfo));
#ifdef ENABLE_SKELETON_SUPPORT
    else if (propInfo.typeinfo() == proptype_SkinWeights)
        propertyVisualizers.push_back(new OMPropertyVisualizerSkinWeights<MeshT>(mesh, propInfo));
#endif
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
 * "int" and "bool" can be added for every entity type. "SkinWeights" can only be added for vertices.
 */
template<typename MeshT>
void OMPropertyModel<MeshT>:: addProperty(QString propName, QString friendlyTypeName, PropertyInfo::ENTITY_FILTER filter)
{

    QString dtype = friendlyTypeName;
    QString pname = propName;

    MeshT* mesh = mesh_;

    if ( filter == PropertyInfo::EF_VERTEX )
    {
        if ( (dtype == tr("Vec3d")) || (dtype == tr("Vec3f")) )
        {
            OpenMesh::VPropHandleT< TriMesh::Point > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("double") )
        {
            OpenMesh::VPropHandleT< double > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("unsigned int") )
        {
            OpenMesh::VPropHandleT< unsigned int > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("int") )
        {
            OpenMesh::VPropHandleT< int > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("bool") )
        {
            OpenMesh::VPropHandleT< bool > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        //please update the doc if you write "skin weights"-property support

#ifdef ENABLE_SKELETON_SUPPORT
        else if ( dtype == tr("SkinWeights") )
        {
            OpenMesh::VPropHandleT< BaseSkin::SkinWeights > prop;
            mesh->add_property(prop, pname.toStdString());
        }
#endif
    }
    else if ( filter == PropertyInfo::EF_EDGE )
    {
        if ( (dtype == tr("Vec3d")) || (dtype == tr("Vec3f")) )
        {
            OpenMesh::EPropHandleT< TriMesh::Point > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("double") )
        {
            OpenMesh::EPropHandleT< double > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("unsgined int") )
        {
            OpenMesh::EPropHandleT< unsigned int > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("int") )
        {
            OpenMesh::EPropHandleT< int > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("bool") )
        {
            OpenMesh::EPropHandleT< bool > prop;
            mesh->add_property(prop, pname.toStdString());
        }
    }
    else if ( filter == PropertyInfo::EF_FACE )
    {
        if ( (dtype == tr("Vec3d")) || (dtype == tr("Vec3f")) )
        {
            OpenMesh::FPropHandleT< TriMesh::Point > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("double") )
        {
            OpenMesh::FPropHandleT< double > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("unsigned int") )
        {
            OpenMesh::FPropHandleT< unsigned int > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("int") )
        {
            OpenMesh::FPropHandleT< int > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("bool") )
        {
            OpenMesh::FPropHandleT< bool > prop;
            mesh->add_property(prop, pname.toStdString());
        }
    }
    else if ( filter == PropertyInfo::EF_HALFEDGE )
    {
        if ( (dtype == tr("Vec3d")) || (dtype == tr("Vec3f")) )
        {
            OpenMesh::HPropHandleT< TriMesh::Point > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("double") )
        {
            OpenMesh::HPropHandleT< double > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("unsigned int") )
        {
            OpenMesh::HPropHandleT< unsigned int > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("int") )
        {
            OpenMesh::HPropHandleT< int > prop;
            mesh->add_property(prop, pname.toStdString());
        }
        else if ( dtype == tr("bool") )
        {
            OpenMesh::HPropHandleT< bool > prop;
            mesh->add_property(prop, pname.toStdString());
        }
    }

}


template<typename MeshT>
void OMPropertyModel<MeshT>::initializeSupportedPropertyTypes()
{
    
    
    supportedPropertyTypes.insert(proptype_bool);
    supportedPropertyTypes.insert(proptype_int);
    supportedPropertyTypes.insert(proptype_uint);
    supportedPropertyTypes.insert(proptype_double);
    supportedPropertyTypes.insert(proptype_Vec3d);
    supportedPropertyTypes.insert(proptype_Vec3f);
    
#ifdef ENABLE_SKELETON_SUPPORT
    supportedPropertyTypes.insert(proptype_SkinWeights);
#endif
    
//#ifdef ENABLE_SKELETON_SUPPORT
//    //template <typename MeshT>
//    //const typename OMPropertyModel<MeshT>::TypeInfoWrapperSet OMPropertyModel<MeshT>::supportedPropertyTypes =
//    //        (OpenFlipper::Options::nogui() ? typename OMPropertyModel<MeshT>::TypeInfoWrapperSet() : typename OMPropertyModel<MeshT>::TypeInfoWrapperSet(propertyTypes, propertyTypes + 7));
//    supportedPropertyTypes = typename OMPropertyModel<MeshT>::TypeInfoWrapperSet();
//#else
//    //const typename OMPropertyModel<MeshT>::TypeInfoWrapperSet OMPropertyModel<MeshT>::supportedPropertyTypes =
//    //        (OpenFlipper::Options::nogui() ? typename OMPropertyModel<MeshT>::TypeInfoWrapperSet() : typename OMPropertyModel<MeshT>::TypeInfoWrapperSet(propertyTypes, propertyTypes + 6));
//    supportedPropertyTypes = typename OMPropertyModel<MeshT>::TypeInfoWrapperSet();
//#endif
    
}


