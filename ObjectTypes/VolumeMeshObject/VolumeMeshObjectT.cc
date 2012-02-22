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
 *   $LastChangedBy$                                                 *
 *   $Date$                    *
 *                                                                            *
 \*===========================================================================*/

//=============================================================================
//
//  MyTypes
//
//=============================================================================

#define VOLUMEMESHOBJECTT_CC

//== INCLUDES =================================================================

#include <OpenFlipper/common/Types.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

template<class MeshT>
VolumeMeshObject<MeshT>::VolumeMeshObject(const VolumeMeshObject& _object) :
    BaseObjectData(_object), mesh_(_object.mesh_),
    statusAttrib_(*mesh_),
    colorAttrib_(*mesh_),
    normalAttrib_(*mesh_),
    meshNode_((OpenFlipper::Options::nogui() ?
               NULL :
               new ACG::SceneGraph::VolumeMeshNodeT<MeshT>(*mesh_, statusAttrib_, colorAttrib_,
                                                           normalAttrib_, NULL, "NEW VolumeMeshNode"))) {

    init(_object.mesh_);

    setName(name());
}

template<class MeshT>
VolumeMeshObject<MeshT>::VolumeMeshObject(DataType _typeId) :
    BaseObjectData(), mesh_(new MeshT()),
    statusAttrib_(*mesh_),
    colorAttrib_(*mesh_),
    normalAttrib_(*mesh_),
    meshNode_((OpenFlipper::Options::nogui() ?
               NULL :
               new ACG::SceneGraph::VolumeMeshNodeT<MeshT>(*mesh_, statusAttrib_, colorAttrib_,
                                                           normalAttrib_, NULL, "NEW VolumeMeshNode"))) {

    setDataType(_typeId);
    init();
}

template<class MeshT>
VolumeMeshObject<MeshT>::~VolumeMeshObject() {

    // Delete the data attached to this object (this will remove all perObject data)
    // Not the best way to do it but it will work.
    // This is only necessary if people use references to the mesh below and
    // they do something with the mesh in the destructor of their
    // perObjectData.
    deleteData();

    // Delete the mesh only, if this object contains a mesh
    if(mesh_ != NULL) {
        delete mesh_;
        mesh_ = NULL;
    } else {
        std::cerr << "Destructor error: Mesh already deleted." << std::endl;
    }

    // No need to delete the scenegraph nodes as this will be managed by baseplugin
    meshNode_ = 0;
}

/** Cleanup function for mesh objects. Deletes the contents of the whole object and
 * calls VolumeMeshObject::init afterwards.
 */
template<class MeshT>
void VolumeMeshObject<MeshT>::cleanup() {

    // Delete the mesh only, if this object contains a mesh
    if(mesh_ != NULL) {
        delete mesh_;
        mesh_ = NULL;
    } else {
        std::cerr << "Cleanup error: Triangle Mesh already deleted." << std::endl;
    }

    BaseObjectData::cleanup();

    meshNode_ = 0;

    init();
}

/** This function initalizes the mesh object. It creates the scenegraph nodes,
 *  the mesh and requests all required properties for the mesh.
 */
template<class MeshT>
void VolumeMeshObject<MeshT>::init(MeshT* _mesh) {

    // Only initialize scenegraph nodes when we initialized a gui!!
    if(OpenFlipper::Options::nogui())
        return;

    if(manipulatorNode() == NULL)
        std::cerr << "Error when creating volume mesh object! Manipulator node is NULL!" << std::endl;

    if(materialNode() == NULL)
        std::cerr << "Error when creating mesh object! Material node is NULL!" << std::endl;

    meshNode_->set_parent(materialNode());

    // Update all nodes
    update();
}

// ===============================================================================
// Name/Path Handling
// ===============================================================================

/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
 * BaseObjectData::setName.
 */
template<class MeshT>
void VolumeMeshObject<MeshT>::setName(QString _name) {
    BaseObjectData::setName(_name);

    // No update when gui is not active
    if(OpenFlipper::Options::nogui())
        return;

    std::string nodename = std::string("VolumeMeshNode for mesh " + _name.toUtf8());
    meshNode_->name(nodename);
}

// ===============================================================================
// Content
// ===============================================================================

/** Get a pointer to the objects mesh.
 * @return Pointer to the mesh
 */
template<class MeshT>
MeshT* VolumeMeshObject<MeshT>::mesh() {
    return mesh_;
}

/** Updates the visualization of the object. Calls VolumeMeshObject::updateGeometry,
 *  VolumeMeshObject::updateTopology, VolumeMeshObject::updateSelection and
 *  VolumeMeshObject::updateModelingRegions.
 */
template<class MeshT>
void VolumeMeshObject<MeshT>::update(UpdateType _type) {

    // No update necessary if no gui
    if(OpenFlipper::Options::nogui())
        return;

    PluginFunctions::setMainGLContext();

    if(_type.contains(UPDATE_ALL) || _type.contains(UPDATE_TOPOLOGY)) {
        updateGeometry();
        updateColor();
        updateTopology();
        updateSelection();
    } else {
        if(_type.contains(UPDATE_GEOMETRY)) {
            updateGeometry();
        }
        if(_type.contains(UPDATE_SELECTION)) {
            updateSelection();
        }
        if(_type.contains(UPDATE_COLOR)) {
            updateColor();
        }
    }
}

/** Updates the geometry information in the mesh scenegraph node */
template<class MeshT>
void VolumeMeshObject<MeshT>::updateSelection() {

    if(meshNode_) {
        meshNode_->set_selection_changed(true);
    }
}

/** Updates the geometry information in the mesh scenegraph node */
template<class MeshT>
void VolumeMeshObject<MeshT>::updateGeometry() {

    normalAttrib_.update_face_normals();

    if(meshNode_) {
        meshNode_->set_geometry_changed(true);
    }
}

/** Updates the color information in the mesh scenegraph node */
template<class MeshT>
void VolumeMeshObject<MeshT>::updateColor() {
    if(meshNode_)
        meshNode_->set_color_changed(true);
}

/** Updates the topology information in the mesh scenegraph node */
template<class MeshT>
void VolumeMeshObject<MeshT>::updateTopology() {
    if(meshNode_) {
        meshNode_->set_topology_changed(true);
    }
}

template<class MeshT>
BaseObject* VolumeMeshObject<MeshT>::copy() {
    VolumeMeshObject< MeshT >* object = new VolumeMeshObject<MeshT>(*this);
    return dynamic_cast<BaseObject*> (object);
}

// ===============================================================================
// Visualization
// ===============================================================================
/** Returns a pointer to the mesh node
 * @return Pointer to the mesh node
 */
template<class MeshT>
ACG::SceneGraph::VolumeMeshNodeT<MeshT>* VolumeMeshObject<MeshT>::meshNode() {
    return meshNode_;
}

/** Get the Bounding box size of this object
 */
template<class MeshT>
void VolumeMeshObject<MeshT>::boundingBox(ACG::Vec3d& _bbMin, ACG::Vec3d& _bbMax) {
    if(meshNode_) {
        _bbMin = ACG::Vec3d(FLT_MAX, FLT_MAX, FLT_MAX);
        _bbMax = ACG::Vec3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        meshNode_->boundingBox(_bbMin, _bbMax);
    } else {
        std::cerr << "Error: Bounding box computation via Scenegraph not available without gui." << std::endl;
    }
}

// ===============================================================================
// Object information
// ===============================================================================

/** Returns a string containing all information about the current object. This also
 * includes the information provided by BaseObjectData::getObjectinfo
 *
 * @return String containing the object information
 */
template<class MeshT>
QString VolumeMeshObject<MeshT>::getObjectinfo() {
    QString output;

    output += "========================================================================\n";
    output += BaseObjectData::getObjectinfo();

    if(dataType(typeId("PolyhedralMesh")))
        output += "Object Contains Polyhedral Mesh : ";

    if(dataType(typeId("HexahedralMesh")))
        output += "Object Contains Hexahedral Mesh : ";

    output += QString::number(mesh()->n_vertices()) + " vertices\n";
    output += QString::number(mesh()->n_edges()) += " edges\n";
    output += QString::number(mesh()->n_halfedges()) += " half-edges\n";
    output += QString::number(mesh()->n_faces()) += " faces\n";
    output += QString::number(mesh()->n_halffaces()) += " half-faces\n";
    output += QString::number(mesh()->n_cells()) += " cells\n";

    output += "========================================================================\n";
    return output;
}

// ===============================================================================
// Picking
// ===============================================================================

/** Given an node index from PluginFunctions::scenegraphPick this function can be used to
 * check if the meshNode of the object has been picked.
 *
 * @param _node_idx Index of the picked mesh node
 * @return bool if the meshNode of this object is the picking target.
 */
template<class MeshT>
bool VolumeMeshObject<MeshT>::picked(unsigned int _node_idx) {
    return (_node_idx == meshNode_->id());
}

template<class MeshT>
void VolumeMeshObject<MeshT>::enablePicking(bool _enable) {
    if(OpenFlipper::Options::nogui())
        return;

    meshNode_->enablePicking(_enable);
}

template<class MeshT>
bool VolumeMeshObject<MeshT>::pickingEnabled() {
    return meshNode_->pickingEnabled();
}

//=============================================================================

template<class MeshT>
BaseNode*
VolumeMeshObject<MeshT>::primaryNode() {
    return boundingBoxNode();
}

//=============================================================================
