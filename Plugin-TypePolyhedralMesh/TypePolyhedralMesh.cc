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
*   $Revision: 13576 $                                                       *
*   $LastChangedBy: kremer $                                                 *
*   $Date: 2012-01-30 11:22:27 +0100 (Mo, 30 Jan 2012) $                    *
*                                                                            *
\*===========================================================================*/

#include "TypePolyhedralMesh.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <ObjectTypes/Plane/Plane.hh>

TypePolyhedralMeshPlugin::TypePolyhedralMeshPlugin() :
render_switch_(NULL),
translucency_action_(NULL),
translucency_factor_action_(NULL) {
}

bool TypePolyhedralMeshPlugin::registerType() {

    addDataType("PolyhedralMesh", tr("Polyhedral Volume Mesh"));
    setTypeIcon("PolyhedralMesh", "PolyVolMeshType.png");
    return true;
}

//----------------------------------------------------------------------------

void TypePolyhedralMeshPlugin::pluginsInitialized() {

    if(OpenFlipper::Options::nogui()) return;

    emit registerKey(Qt::Key_F8, Qt::ShiftModifier, "Set scaling of cell shrinkage");

    QMenu* menu = new QMenu("Polyhedral Mesh Options");

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

    emit addContextMenuItem(menu->menuAction(), DATA_POLYHEDRAL_MESH, CONTEXTOBJECTMENU);
}

//----------------------------------------------------------------------------

int TypePolyhedralMeshPlugin::addEmpty() {

    // new object data struct
    PolyhedralMeshObject* object = new PolyhedralMeshObject(DATA_POLYHEDRAL_MESH);

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

QString TypePolyhedralMeshPlugin::get_unique_name(PolyhedralMeshObject* _object) {

    bool name_unique = false;

    int cur_idx = _object->id();

    while (!name_unique) {
        name_unique = true;

        QString cur_name = QString(tr("Polyhedral Mesh %1.ovm").arg(cur_idx));

        PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DATA_POLYHEDRAL_MESH);
        for (; o_it != PluginFunctions::objectsEnd(); ++o_it) {
            if (o_it->name() == cur_name) {
                name_unique = false;
                cur_idx += 10;
                break;
            }
        }
    }

    return QString(tr("Polyhedral Mesh %1.ovm").arg(cur_idx));
}

//----------------------------------------------------------------------------


void TypePolyhedralMeshPlugin::slotKeyEvent(QKeyEvent* _event) {

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

void TypePolyhedralMeshPlugin::slotObjectUpdated(int _identifier) {

    PlaneObject* pobj;
    if (PluginFunctions::getObject(_identifier, pobj))
        slot_update_planes_in_scenegraph_node();
}

//----------------------------------------------------------------------------

void TypePolyhedralMeshPlugin::objectDeleted(int _identifier) {

    PlaneObject* pobj;
    if (PluginFunctions::getObject(_identifier, pobj)) {
        slot_update_planes_in_scenegraph_node(_identifier);
    }
}

//----------------------------------------------------------------------------

void TypePolyhedralMeshPlugin::switchRendering() {

    QVariant contextObject = render_switch_->data();
    int objectId = contextObject.toInt();

    if(objectId == -1)
        return;

    BaseObjectData* bod = 0;
    if(!PluginFunctions::getObject(objectId, bod))
        return;

    PolyhedralMeshObject* polyMeshObject = dynamic_cast<PolyhedralMeshObject*>(bod);

    if(polyMeshObject) {
        polyMeshObject->meshNode()->set_boundary_only(render_switch_->isChecked());
        polyMeshObject->meshNode()->set_geometry_changed(true);
    }
}

//----------------------------------------------------------------------------

void TypePolyhedralMeshPlugin::switchTranslucency() {

    QVariant contextObject = translucency_action_->data();
    int objectId = contextObject.toInt();

    if(objectId == -1)
        return;

    BaseObjectData* bod = 0;
    if(!PluginFunctions::getObject(objectId, bod))
        return;

    PolyhedralMeshObject* polyMeshObject = dynamic_cast<PolyhedralMeshObject*>(bod);

    if(polyMeshObject) {
        polyMeshObject->meshNode()->set_translucent(translucency_action_->isChecked());
        polyMeshObject->meshNode()->set_geometry_changed(true);
    }
}

//----------------------------------------------------------------------------

void TypePolyhedralMeshPlugin::setTranslucencyFactor() {

    QVariant contextObject = translucency_factor_action_->data();
    int objectId = contextObject.toInt();

    if(objectId == -1)
        return;

    BaseObjectData* bod = 0;
    if(!PluginFunctions::getObject(objectId, bod))
        return;

    PolyhedralMeshObject* polyMeshObject = dynamic_cast<PolyhedralMeshObject*>(bod);

    if(polyMeshObject) {

        bool ok;
        float val = polyMeshObject->meshNode()->translucency_factor();
        double factor = QInputDialog::getDouble(0, tr("Set translucency factor"), tr("Factor [0, 1]:"), val,
                0.0, 1.0, 2, &ok);

        polyMeshObject->meshNode()->set_translucency_factor((float)factor);
        if(polyMeshObject->meshNode()->translucent()) {
            emit updatedObject(objectId, UPDATE_GEOMETRY);
        }
    }
}

//----------------------------------------------------------------------------

void TypePolyhedralMeshPlugin::slot_update_planes_in_scenegraph_node(int _deletedObject) {

    typedef ACG::SceneGraph::VolumeMeshNodeT<PolyhedralMesh>::Plane Plane;
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
    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DATA_POLYHEDRAL_MESH); o_it
            != PluginFunctions::objectsEnd(); ++o_it) {

        PluginFunctions::polyhedralMeshObject(*o_it)->meshNode()->clear_cut_planes();
        for (unsigned int i = 0; i < planes.size(); ++i) {
            PluginFunctions::polyhedralMeshObject(*o_it)->meshNode()->add_cut_plane(planes[i]);
        }
        PluginFunctions::polyhedralMeshObject(*o_it)->meshNode()->set_geometry_changed(true);
    }

    emit updateView();
}

//----------------------------------------------------------------------------


void TypePolyhedralMeshPlugin::slot_change_shrinkage() {

    for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS, DATA_POLYHEDRAL_MESH); o_it
            != PluginFunctions::objectsEnd(); ++o_it) {
        // Popup dialog
        bool ok;
        double val = PluginFunctions::polyhedralMeshObject(*o_it)->meshNode()->scaling();
        double scale = QInputDialog::getDouble(0, tr("Set singularity scaling for cell shrinkage"), tr("Size * :"), val,
                0.0, 1.0, 2, &ok);

        PluginFunctions::polyhedralMeshObject(*o_it)->meshNode()->set_scaling(scale);

        emit updatedObject((*o_it)->id(), UPDATE_GEOMETRY);
    }
}

Q_EXPORT_PLUGIN2( typepolyhedralmeshplugin , TypePolyhedralMeshPlugin );

