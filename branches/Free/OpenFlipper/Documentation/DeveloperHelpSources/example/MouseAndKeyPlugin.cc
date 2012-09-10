//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================

#include "MouseAndKeyPlugin.hh"


#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

/*
 * Constructor
 */
MouseAndKeyPlugin::MouseAndKeyPlugin() :
        contextMenuEntry_(0),
        tool_(0),
        pickButton_(0),
        activeObject_(-1),
        axis_x_(ACG::Vec3d(1.0, 0.0, 0.0)),
        axis_y_(ACG::Vec3d(0.0, 1.0, 0.0))
{

}

/*
 * Initialize plugin
 */
void MouseAndKeyPlugin::initializePlugin() {

	// Register keys
	emit registerKey(Qt::Key_J,	Qt::NoModifier, "Rotate object down");
	emit registerKey(Qt::Key_K,	Qt::NoModifier, "Rotate object up");
	emit registerKey(Qt::Key_H,	Qt::NoModifier, "Rotate object left");
	emit registerKey(Qt::Key_L,	Qt::NoModifier, "Rotate object right");

  tool_ = new QWidget();
  QSize size(300, 300);
  tool_->resize(size);

  // Create button that can be toggled
  // to (de)activate plugin's picking mode
  pickButton_ = new QPushButton(tr("Select object"));
  pickButton_->setCheckable(true);

  // Create label
  QLabel* label = new QLabel();
  label->setText("(De)activate pick mode");

  // Set label to be above the button
  QGridLayout* grid = new QGridLayout;
  grid->addWidget(label, 0, 0);
  grid->addWidget(pickButton_, 1, 0);

  tool_->setLayout(grid);

  // Connect button to slotButtonClicked()
  connect(pickButton_, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));

  // Add the Toolbox
  emit addToolbox(tr("Mouse and Key"), tool_);

} // End initializePlugin

/*
 * Is called after all plugins have been initialized
 */
void MouseAndKeyPlugin::pluginsInitialized() {

	// Add pickmode
	emit addPickMode("MouseAndKeyPlugin");

	// Add context menu entry

	// Create submenu
	contextMenuEntry_ = new QMenu("Mouse and key plugin");

	QAction* lastAction;

	// Add action to recently created menu
	lastAction = contextMenuEntry_->addAction( "Hide object" );
	lastAction->setToolTip("Hide selected object");
	lastAction->setStatusTip( lastAction->toolTip() );

	// Finally insert created context submenu to OpenFlipper's context menu
	// We want our action to be visible for triangle and polygon meshes
	emit addContextMenuItem(contextMenuEntry_->menuAction() , DATA_TRIANGLE_MESH , CONTEXTOBJECTMENU );
	emit addContextMenuItem(contextMenuEntry_->menuAction() , DATA_POLY_MESH , CONTEXTOBJECTMENU );

	// Connect the created context menu entry to local function contextMenuItemSelected(QAction*)
	connect(contextMenuEntry_, SIGNAL(triggered(QAction*)), this, SLOT(contextMenuItemSelected(QAction*)));

} // End pluginsInitialized


/*
 * Is called when button in toolbox has been clicked
 */
void MouseAndKeyPlugin::slotButtonClicked() {

	if(pickButton_->isChecked()) {
		// Picking mode of our plugin shall be activated
		// set OpenFlipper's action mode to picking
		PluginFunctions::actionMode( Viewer::PickingMode );

		// Now activate our picking mode
		PluginFunctions::pickMode( "MouseAndKeyPlugin" );
	} else {
		// Picking mode shall be deactivated
		PluginFunctions::actionMode( Viewer::ExamineMode );
	}

} // End slotButtonClicked

/*
 * Is called when pick mode is changed in OpenFlipper
 */
void MouseAndKeyPlugin::slotPickModeChanged(const std::string& _mode) {

	// Set button checked if pick mode is our
	// plugin's pick mode
	pickButton_->setChecked(_mode == "MouseAndKeyPlugin");

} // End slotPickModeChanged

/*
 * Is called each time the mouse has moved or been clicked
 */
void MouseAndKeyPlugin::slotMouseEvent(QMouseEvent* _event) {

	if ( PluginFunctions::pickMode() == "MouseAndKeyPlugin" &&
		PluginFunctions::actionMode() == Viewer::PickingMode ) {

		// If double click has been performed
		if (_event->type() == QEvent::MouseButtonDblClick) {

			unsigned int node_idx, target_idx;
			OpenMesh::Vec3d hitPoint;

			// Get picked object's identifier
			if ( PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,_event->pos(), node_idx, target_idx, &hitPoint) ) {

				BaseObjectData* object;

				// Get picked object
				if ( PluginFunctions::getPickedObject(node_idx, object) ) {

					// Show small dialog window
					QDialog* dlg = new QDialog;

					QGridLayout* grid = new QGridLayout;
					QLabel* label = new QLabel;
					QString str = QString("You selected object ");
					str += QString::number(node_idx);
					label->setText(str);
					grid->addWidget(label, 0,0);

					dlg->setLayout(grid);

					dlg->show();

					// Set last selected object
					activeObject_ = node_idx;
				}
				else {
					emit log(LOGINFO, "Picking failed");
				}
			}

			return;
		}

		// Continue traversing scene graph
		ACG::SceneGraph::MouseEventAction action(_event, PluginFunctions::viewerProperties().glState());
		PluginFunctions::traverse(action);
	}

} // End slotMouseEvent

/*
 * Is called when a key on the keyboard is pressed
 */
void MouseAndKeyPlugin::slotKeyEvent( QKeyEvent* _event ) {

	BaseObjectData* object;

	// Get last clicked object (selected in pick mode)
	if ( PluginFunctions::getPickedObject(activeObject_, object) ) {

		// Switch pressed keys
		switch (_event->key())
		{
			case Qt::Key_J:

				object->manipulatorNode()->loadIdentity();
				object->manipulatorNode()->rotate(10.0, axis_x_);

				break;

			case Qt::Key_K :

				object->manipulatorNode()->loadIdentity();
				object->manipulatorNode()->rotate(-10.0, axis_x_);

				break;

			case Qt::Key_H :

				object->manipulatorNode()->loadIdentity();
				object->manipulatorNode()->rotate(10.0, axis_y_);

				break;

			case Qt::Key_L :

				object->manipulatorNode()->loadIdentity();
				object->manipulatorNode()->rotate(-10.0, axis_y_);

				break;

			default:
			break;
		}

		// Perform rotation
		if ( object->dataType( DATA_TRIANGLE_MESH ) )
			transformMesh(object->manipulatorNode()->matrix(), (*PluginFunctions::triMesh(object)));
		if ( object->dataType( DATA_POLY_MESH ) )
			transformMesh(object->manipulatorNode()->matrix(), (*PluginFunctions::polyMesh(object)));

		// Tell core that object has been modified
		emit updatedObject(object->id(), UPDATE_GEOMETRY);

		// Update view
		emit updateView();
	} else {
		emit log(LOGINFO, "No object has been selected to rotate! Select object first.");
	}

} // End slotKeyEvent

/*
 * Transform a mesh with the given transformation matrix
 *
 * _mat : transformation matrix
 * _mesh : the mesh
 */
template<typename MeshT>
void MouseAndKeyPlugin::transformMesh(ACG::Matrix4x4d _mat, MeshT& _mesh) {

	typename MeshT::VertexIter v_it = _mesh.vertices_begin();
	typename MeshT::VertexIter v_end = _mesh.vertices_end();

	// Iterator over all vertices and transform them by _mat
	// Update normals
	for (; v_it != v_end; ++v_it) {
		_mesh.set_point(v_it, (typename MeshT::Point) _mat.transform_point(
				(OpenMesh::Vec3d)(_mesh.point(v_it))));
		_mesh.set_normal(v_it, (typename MeshT::Point) _mat.transform_vector(
				(OpenMesh::Vec3d)(_mesh.normal(v_it))));
	}

	typename MeshT::FaceIter f_it = _mesh.faces_begin();
	typename MeshT::FaceIter f_end = _mesh.faces_end();

	// Iterate over all faces and update face normals
	for (; f_it != f_end; ++f_it)
		_mesh.set_normal(f_it, (typename MeshT::Point) _mat.transform_vector(
				(OpenMesh::Vec3d)(_mesh.normal(f_it))));

} // End transformMesh

/*
 * Is called when context menu entry has been clicked
 */
void MouseAndKeyPlugin::contextMenuItemSelected(QAction* _action) {

	// Get object id from QAction object
	QVariant contextObject = _action->data();
	int objectId = contextObject.toInt();

	if (objectId == -1) {

		log(LOGINFO, "Could not get associated object id.");
		return;
	}

	// Get node associated to object id
	ACG::SceneGraph::BaseNode* node = ACG::SceneGraph::find_node(
			PluginFunctions::getSceneGraphRootNode(), objectId);

	// Return if node id was not valid
	if (!node) {

		log(LOGINFO, "Could not find associated object.");
		return;
	}

	BaseObjectData* obj;
	if (!PluginFunctions::getObject(objectId, obj))
		return;

	// Hide object
	obj->hide();

} // End contextMenuItemSelected

Q_EXPORT_PLUGIN2( mouseandkeyplugin , MouseAndKeyPlugin );

