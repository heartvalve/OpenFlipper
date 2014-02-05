/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
 \*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision: 11279 $                                                       *
*   $LastChangedBy: kremer $                                                 *
*   $Date: 2011-04-11 17:35:27 +0200 (Mon, 11 Apr 2011) $                    *
*                                                                            *
\*===========================================================================*/

#include "TypeHexahedralMesh.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <ObjectTypes/Plane/Plane.hh>

TypeHexahedralMeshPlugin::TypeHexahedralMeshPlugin() :
render_switch_(0),
translucency_factor_action_(0)
{
}

bool TypeHexahedralMeshPlugin::registerType() {

    addDataType("HexahedralMesh", tr("Hexahedral Volume Mesh"));
    setTypeIcon("HexahedralMesh", "PolyVolMeshType.png");
    return true;
}

//----------------------------------------------------------------------------

void TypeHexahedralMeshPlugin::pluginsInitialized() {

    if(OpenFlipper::Options::nogui()) return;

    emit registerKey(Qt::Key_F8, Qt::ShiftModifier, "Set scaling of Hex shrinkage");

    QMenu* menu = new QMenu("Hexahedral Mesh Options");

    // scaling action in context menu
    QAction* act_scale_cells = new QAction(tr("Scale cells (Shift-F8)"), this);
    act_scale_cells->setStatusTip(tr("Scale cells (Shift-F8)"));
    connect(act_scale_cells, SIGNAL( triggered() ), this, SLOT( slot_change_shrinkage() ));
    menu->addAction(act_scale_cells);

    // Change rendering
    render_switch_ = new QAction(tr("Render Boundary Only"), this);
    render_switch_->setStatusTip(tr("Render Boundary Only"));
    render_switch_->setCheckable(true);
    render_switch_->setChecked(false);
    connect(render_switch_, SIGNAL( triggered() ), this, SLOT( switchRendering() ));
    menu->addAction(render_switch_);

    translucency_factor_action_ = new QAction(tr("Set Translucency Factor"), this);
    translucency_factor_action_->setStatusTip(tr("Set Translucency Factor"));
    translucency_factor_action_->setCheckable(false);
    connect(translucency_factor_action_, SIGNAL( triggered() ), this, SLOT( setTranslucencyFactor() ));
    menu->addAction(translucency_factor_action_);

    emit addContextMenuItem(menu->menuAction(), DATA_HEXAHEDRAL_MESH, CONTEXTOBJECTMENU);
}

//----------------------------------------------------------------------------

int TypeHexahedralMeshPlugin::addEmpty() {

    // New object data struct
    HexahedralMeshObject* object = new HexahedralMeshObject(DATA_HEXAHEDRAL_MESH);

    if (PluginFunctions::objectCount() == 1)
        object->target(true);

    if (PluginFunctions::targetCount() == 0)
        object->target(true);

    QString name = QString(tr("New Hexahedral Mesh %1.ovm").arg( object->id() ));

    // call the local function to update names
    QFileInfo f(name);
    object->setName(f.fileName());

    // enable backface culling
    object->materialNode()->applyProperties(ACG::SceneGraph::MaterialNode::All);
    //object->materialNode()->enable_backface_culling();

    // set the default colors
    const QColor color = OpenFlipper::Options::defaultColor();
    const ACG::Vec4f default_color(color.redF(), color.greenF(), color.blueF(), color.alphaF());
    object->materialNode()->set_color(default_color);

    // Set rendering props
    if(OpenFlipper::Options::gui()) {
        object->meshNode()->set_scaling(0.8);

        object->update();

        object->show();
    }

    emit log(LOGINFO, object->getObjectinfo());

    emit emptyObjectAdded(object->id());

    return object->id();
}

//----------------------------------------------------------------------------

void TypeHexahedralMeshPlugin::slotKeyEvent(QKeyEvent* _event) {

    switch (_event->key()) {
    case Qt::Key_F8:
        if (_event->modifiers() & Qt::ShiftModifier)
            slot_change_shrinkage();
        break;
    default:
        break;
    }
}

//----------------------------------------------------------------------------

void TypeHexahedralMeshPlugin::slotObjectUpdated(int _identifier, const UpdateType& _type) {

    if( !_type.contains(UPDATE_ALL) && !_type.contains(UPDATE_GEOMETRY))
        return;

    PlaneObject* pobj;
    if (PluginFunctions::getObject(_identifier, pobj))
        slot_update_planes_in_scenegraph_node();
}

//----------------------------------------------------------------------------

void TypeHexahedralMeshPlugin::objectDeleted(int _identifier) {

    PlaneObject* pobj;
    if (PluginFunctions::getObject(_identifier, pobj)) {
        slot_update_planes_in_scenegraph_node(_identifier);
    }
}

//----------------------------------------------------------------------------

void TypeHexahedralMeshPlugin::slotUpdateContextMenu(int _objectId) {

    HexahedralMeshObject* hmobj;
    if (PluginFunctions::getObject(_objectId, hmobj)) {
        render_switch_->setChecked(hmobj->meshNode()->boundary_only());
    }
}

//----------------------------------------------------------------------------

void TypeHexahedralMeshPlugin::slot_update_planes_in_scenegraph_node(int _deletedObject) {

    typedef ACG::SceneGraph::VolumeMeshNodeT<HexahedralMesh>::Plane Plane;
    std::vector<Plane> planes;

    // collect planes
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DATA_PLANE); o_it
            != PluginFunctions::objectsEnd(); ++o_it) {

        if(o_it->id() == _deletedObject) continue;

        ACG::Vec3d p = (ACG::Vec3d) PluginFunctions::planeNode(*o_it)->position();
        ACG::Vec3d n = (ACG::Vec3d) PluginFunctions::planeNode(*o_it)->normal();
        ACG::Vec3d x = (ACG::Vec3d) PluginFunctions::planeNode(*o_it)->xDirection();
        ACG::Vec3d y = (ACG::Vec3d) PluginFunctions::planeNode(*o_it)->yDirection();
        x /= x.sqrnorm();
        y /= y.sqrnorm();

        planes.push_back(Plane(p, n, x, y));
    }

    // iterate over all target polyvolmeshes
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DATA_HEXAHEDRAL_MESH); o_it
            != PluginFunctions::objectsEnd(); ++o_it) {

        PluginFunctions::hexahedralMeshObject(*o_it)->meshNode()->clear_cut_planes();
        for (unsigned int i = 0; i < planes.size(); ++i) {
            PluginFunctions::hexahedralMeshObject(*o_it)->meshNode()->add_cut_plane(planes[i]);
        }
        PluginFunctions::hexahedralMeshObject(*o_it)->meshNode()->set_geometry_changed(true);
    }

    emit updateView();
}

//----------------------------------------------------------------------------

void TypeHexahedralMeshPlugin::switchRendering() {

    QVariant contextObject = render_switch_->data();
    int objectId = contextObject.toInt();

    if(objectId == -1)
        return;

    BaseObjectData* bod = 0;
    if(!PluginFunctions::getObject(objectId, bod))
        return;

    HexahedralMeshObject* hexMeshObject = dynamic_cast<HexahedralMeshObject*>(bod);

    if(hexMeshObject) {
        hexMeshObject->meshNode()->set_boundary_only(render_switch_->isChecked());
        hexMeshObject->meshNode()->set_geometry_changed(true);
    }
}

//----------------------------------------------------------------------------

void TypeHexahedralMeshPlugin::setTranslucencyFactor() {

    QVariant contextObject = translucency_factor_action_->data();
    int objectId = contextObject.toInt();

    if(objectId == -1)
        return;

    BaseObjectData* bod = 0;
    if(!PluginFunctions::getObject(objectId, bod))
        return;

    HexahedralMeshObject* hexMeshObject = dynamic_cast<HexahedralMeshObject*>(bod);

    if(hexMeshObject) {

        bool ok;
        float val = hexMeshObject->meshNode()->translucency_factor();
        double factor = QInputDialog::getDouble(0, tr("Set translucency factor"), tr("Factor [0, 1]:"), val,
                0.0, 1.0, 2, &ok);

        hexMeshObject->meshNode()->set_translucency_factor((float)factor);
    }
}

//----------------------------------------------------------------------------

void TypeHexahedralMeshPlugin::slot_change_shrinkage() {

    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DATA_HEXAHEDRAL_MESH); o_it
            != PluginFunctions::objectsEnd(); ++o_it) {
        // Popup dialog
        bool ok;
        double val = PluginFunctions::hexahedralMeshObject(*o_it)->meshNode()->scaling();
        double scale = QInputDialog::getDouble(0, tr("Set singularity scaling for hex shrinkage"), tr("Size * :"), val,
                0.0, 1.0, 2, &ok);

        PluginFunctions::hexahedralMeshObject(*o_it)->meshNode()->set_boundary_only(false);
        PluginFunctions::hexahedralMeshObject(*o_it)->meshNode()->set_scaling(scale);
    }
    emit updateView();
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( typehexahedralmeshplugin , TypeHexahedralMeshPlugin );
#endif

