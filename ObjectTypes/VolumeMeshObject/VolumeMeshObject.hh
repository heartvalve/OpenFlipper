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

#ifndef VOLUME_MESH_OBJECT_HH
#define VOLUME_MESH_OBJECT_HH

// Includes
#include <ACG/Utils/SmartPointer.hh>

#include <ACG/Math/VectorT.hh>
#include <ACG/Scenegraph/SeparatorNode.hh>
#include <ACG/Scenegraph/StatusNodesT.hh>
#include <ACG/Scenegraph/ShaderNode.hh>
#include <OpenFlipper/common/GlobalDefines.hh>
#include <OpenFlipper/common/BaseObjectData.hh>
#include <ObjectTypes/VolumeMeshObject/VolumeMeshNode.hh>

#include <OpenVolumeMesh/Attribs/StatusAttrib.hh>
#include <OpenVolumeMesh/Attribs/ColorAttrib.hh>
#include <OpenVolumeMesh/Attribs/NormalAttrib.hh>

template<class MeshT>
class DLLEXPORTONLY VolumeMeshObject : public BaseObjectData {
public:

    /** \brief copy constructor
     *
     *  Create a copy of this object
     */
    VolumeMeshObject(const VolumeMeshObject& _object);

    /** \brief Constructor
    *
    * This is the standard constructor for MeshObjects. As triangle and Poly Meshes are handled by this class, the
    * typeId is passed to the MeshObject to specify it.
    *
    * @param _typeId   This is the type Id the Object will use. Should be typeId("TriangleMesh") or typeId("PolyMesh")
    */
    VolumeMeshObject(DataType _typeId);

    /// destructor
    virtual ~VolumeMeshObject();

    /// Reset current object, including all related nodes.
    virtual void cleanup();

  protected:

    /// Initialise current object, including all related nodes.
    virtual void init();

  //===========================================================================
  /** @name Name and Path handling
   * @{ */
  //===========================================================================
  public:

    /// Set the name of the Object
    void setName( QString _name );

  /** @} */

  //===========================================================================
  /** @name Content
   * @{ */
  //===========================================================================

  public:
    /// return a pointer to the mesh
    MeshT* mesh();

    /// Update the whole Object (Selection,Topology,...)
    virtual void update(UpdateType _type = UPDATE_ALL);

    /// Call this function to update the selection
    void updateSelection();

    /// Update Geometry of all data structures
    void updateGeometry();

    /// Update Colors of all data structures
    void updateColor();

    /// Update Topology of all data structures
    void updateTopology();

    /** return a full copy of this object ( All scenegraph nodes will be created )
     *  but the object will not be a part of the object tree.
     */
    BaseObject* copy();

    typedef OpenVolumeMesh::ColorAttrib<ACG::Vec4f> ColorAttrib;
    typedef OpenVolumeMesh::NormalAttrib<MeshT> NormalAttrib;
    typedef OpenVolumeMesh::StatusAttrib StatusAttrib;

    const StatusAttrib& status() const { return statusAttrib_; }
    StatusAttrib& status() { return statusAttrib_; }

    const ColorAttrib& colors() const { return colorAttrib_; }
    ColorAttrib& colors() { return colorAttrib_; }

    const NormalAttrib& normals() const { return normalAttrib_; }
    NormalAttrib& normals() { return normalAttrib_; }

  private:
    ptr::shared_ptr<MeshT> mesh_;

    StatusAttrib statusAttrib_;

    ColorAttrib colorAttrib_;

    NormalAttrib normalAttrib_;

  /** @} */

  //===========================================================================
  /** @name Visualization
   * @{ */
  //===========================================================================

  public:

    /// Return pointer to the shader node
    ACG::SceneGraph::ShaderNode* shaderNode();

    /// Get the Scenegraph Mesh Node
    ACG::SceneGraph::VolumeMeshNodeT<MeshT>* meshNode();

    /// Get the BoundingBox of this object
    void boundingBox(ACG::Vec3d& _bbMin, typename ACG::Vec3d& _bbMax);

    BaseNode* primaryNode();

  private:

    /// Scenegraph Mesh Node
    ACG::SceneGraph::VolumeMeshNodeT<MeshT>* meshNode_;

    /// Scenegraph Shader Node
    ACG::SceneGraph::ShaderNode* shaderNode_;

  /** @} */

  //===========================================================================
  /** @name Object Information
   * @{ */
  //===========================================================================
  public:
    /// Get all Info for the Object as a string
    QString getObjectinfo();

  /** @} */

  //===========================================================================
  /** @name Picking
   * @{ */
  //===========================================================================
  public:

    /// detect if the node has been picked
    bool picked(unsigned int _node_idx);

    /// Enable or disable picking for this Node
    void enablePicking(bool _enable);

    /// Check if picking is enabled for this Node
    bool pickingEnabled();

  /** @} */
};

//=============================================================================

#if defined(INCLUDE_TEMPLATES) && !defined(VOLUMEMESHOBJECTT_CC)
#define VOLUMEMESHOBJECT_TEMPLATES
#include "VolumeMeshObjectT.cc"
#endif
#endif
