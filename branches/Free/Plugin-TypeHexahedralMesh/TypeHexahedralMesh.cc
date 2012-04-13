/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
translucency_action_(0),
translucency_factor_action_(0),
show_irregs_action_(0),
show_outer_val_two_action_(0) {

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

    QMenu* menu = new QMenu("Hex Mesh Options");

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

    translucency_action_ = new QAction(tr("Translucent"), this);
    translucency_action_->setStatusTip(tr("Translucent"));
    translucency_action_->setCheckable(true);
    translucency_action_->setChecked(false);
    connect(translucency_action_, SIGNAL( triggered() ), this, SLOT( switchTranslucency() ));
    menu->addAction(translucency_action_);

    translucency_factor_action_ = new QAction(tr("Set Translucency Factor"), this);
    translucency_factor_action_->setStatusTip(tr("Set Translucency Factor"));
    translucency_factor_action_->setCheckable(false);
    connect(translucency_factor_action_, SIGNAL( triggered() ), this, SLOT( setTranslucencyFactor() ));
    menu->addAction(translucency_factor_action_);

    show_irregs_action_ = new QAction(tr("Show Irregularities"), this);
    show_irregs_action_->setStatusTip(tr("Show Irregularities"));
    show_irregs_action_->setCheckable(true);
    show_irregs_action_->setChecked(false);
    connect(show_irregs_action_, SIGNAL( triggered() ), this, SLOT( switchShowIrregs() ));
    menu->addAction(show_irregs_action_);

    show_outer_val_two_action_ = new QAction(tr("Show Outer Valence 2 Edges"), this);
    show_outer_val_two_action_->setStatusTip(tr("Show Outer Valence 2 Edges"));
    show_outer_val_two_action_->setCheckable(true);
    show_outer_val_two_action_->setChecked(false);
    connect(show_outer_val_two_action_, SIGNAL( triggered() ), this, SLOT( switchShowOuterValTwo() ));
    menu->addAction(show_outer_val_two_action_);

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

    QString name = get_unique_name(object);

    // call the local function to update names
    QFileInfo f(name);
    object->setName(f.fileName());

    // enable backface culling
    object->materialNode()->applyProperties(ACG::SceneGraph::MaterialNode::All);
    //object->materialNode()->enable_backface_culling();

    object->materialNode()->set_ambient_color(ACG::Vec4f(1.0, 1.0, 1.0, 1.0));

    // Set rendering props
    object->meshNode()->drawMode(ACG::SceneGraph::DrawModes::SOLID_FLAT_SHADED);
    object->meshNode()->set_scaling(0.8);

    object->update();

    object->show();

    emit log(LOGINFO, object->getObjectinfo());

    emit emptyObjectAdded(object->id());

    return object->id();
}

//----------------------------------------------------------------------------

QString TypeHexahedralMeshPlugin::get_unique_name(HexahedralMeshObject* _object) {

    bool name_unique = false;

    int cur_idx = _object->id();

    while (!name_unique) {
        name_unique = true;

        QString cur_name = QString(tr("Mesh HexahedralMesh %1.ovm").arg(cur_idx));

        PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DATA_HEXAHEDRAL_MESH);
        for (; o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->name() == cur_name) {
                name_unique = false;
                cur_idx += 10;
                break;
            }
        }
    }

    return QString(tr("Mesh HexahedralMesh %1.ovm").arg(cur_idx));
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

void TypeHexahedralMeshPlugin::slotObjectUpdated(int _identifier) {

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
        translucency_action_->setChecked(hmobj->meshNode()->translucent());
        show_irregs_action_->setChecked(hmobj->meshNode()->show_irregs());
        show_outer_val_two_action_->setChecked(hmobj->meshNode()->show_outer_valence_two());
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

void TypeHexahedralMeshPlugin::switchTranslucency() {

    QVariant contextObject = translucency_action_->data();
    int objectId = contextObject.toInt();

    if(objectId == -1)
        return;

    BaseObjectData* bod = 0;
    if(!PluginFunctions::getObject(objectId, bod))
        return;

    HexahedralMeshObject* hexMeshObject = dynamic_cast<HexahedralMeshObject*>(bod);

    if(hexMeshObject) {
        hexMeshObject->meshNode()->set_translucent(translucency_action_->isChecked());
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
        if(hexMeshObject->meshNode()->translucent()) {
            emit updatedObject(objectId, UPDATE_GEOMETRY);
        }
    }
}

//----------------------------------------------------------------------------

void TypeHexahedralMeshPlugin::switchShowIrregs() {

    QVariant contextObject = show_irregs_action_->data();
    int objectId = contextObject.toInt();

    if(objectId == -1)
        return;

    BaseObjectData* bod = 0;
    if(!PluginFunctions::getObject(objectId, bod))
        return;

    HexahedralMeshObject* hexMeshObject = dynamic_cast<HexahedralMeshObject*>(bod);

    if(hexMeshObject) {
        hexMeshObject->meshNode()->set_show_irregs(show_irregs_action_->isChecked());
        hexMeshObject->meshNode()->set_valence_changed(true);
    }
}

//----------------------------------------------------------------------------

void TypeHexahedralMeshPlugin::switchShowOuterValTwo() {

    QVariant contextObject = show_outer_val_two_action_->data();
    int objectId = contextObject.toInt();

    if(objectId == -1)
        return;

    BaseObjectData* bod = 0;
    if(!PluginFunctions::getObject(objectId, bod))
        return;

    HexahedralMeshObject* hexMeshObject = dynamic_cast<HexahedralMeshObject*>(bod);

    if(hexMeshObject) {
        hexMeshObject->meshNode()->set_show_outer_valence_two(show_outer_val_two_action_->isChecked());
        hexMeshObject->meshNode()->set_valence_changed(true);
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

Q_EXPORT_PLUGIN2( typehexahedralmeshplugin , TypeHexahedralMeshPlugin );

