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




//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file Types.hh
 * This File contains the basic available datatypes in the Framework.
 */


#ifndef MESHOBJECT_HH
#define MESHOBJECT_HH

//== INCLUDES =================================================================

// -------------------- OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>

#include <ACG/Scenegraph/SeparatorNode.hh>
#include <ACG/Scenegraph/EnvMapNode.hh>
#include <ACG/Scenegraph/ShaderNode.hh>
#include <ACG/Scenegraph/StatusNodesT.hh>

#include <OpenFlipper/common/GlobalDefines.hh>

#include <ObjectTypes/MeshObject/StatusNodeMods.hh>
#include <ObjectTypes/MeshObject/StatusViewNodeT.hh>
#include <OpenFlipper/common/BaseObjectData.hh>

// -------------------- BSP
#include <ACG/Geometry/bsp/TriangleBSPT.hh>


#include <OpenFlipper/common/BaseObjectData.hh>
#include <ACG/Scenegraph/MeshNode2T.hh>



//== TYPEDEFS =================================================================

/// Texture Node
typedef ACG::SceneGraph::TextureNode                      TextureNode;

//== CLASS DEFINITION =========================================================

/** This class provides the functionality for all kind of meshes for the framework
 */
template < class MeshT >
class DLLEXPORTONLY MeshObject : public BaseObjectData {
  public:

    /** \brief copy constructor
     *
     *  Create a copy of this object
     */
    MeshObject(const MeshObject& _object);

    /** \brief Constructor
    *
    * This is the standard constructor for MeshObjects. As triangle and Poly Meshes are handled by this class, the 
    * typeId is passed to the MeshObject to specify it. 
    *
    * @param _typeId   This is the type Id the Object will use. Should be typeId("TriangleMesh") or typeId("PolyMesh")
    */
    MeshObject(DataType _typeId);

    /// destructor
    virtual ~MeshObject();

	/// Reset current object, including all related nodes.
    virtual void cleanup();

  protected:

    /// Initialise current object, including all related nodes.
    virtual void init( MeshT* _mesh = 0);

  //===========================================================================
  /** @name Name and Path handling
   * @{ */
  //===========================================================================
  public:

    /// Set the name of the Object
    void setName( QString _name );


  //===========================================================================
  /** @name Content
   * @{ */
  //===========================================================================

  public:
    /// return a pointer to the mesh
    MeshT* mesh();

    /// Update the whole Object (Selection,Topology,...)
    virtual void update(UpdateType _type = UPDATE_ALL);

    /// Call this function to update the modeling regions
    void updateSelection();

    /// Call this function to update the modeling regions
    void updateModelingRegions();

    /// Update Feature Visualization Node
    void updateFeatures();

    /// Update Geometry of all data structures
    void updateGeometry();

    /// Update Colors of all data structures
    void updateColor();
    
    /// Update Topology of all data structures
    void updateTopology();

    /// Update Texture of all data structures
    void updateTexture();

    /** return a full copy of this object ( All scenegraph nodes will be created )
     *  but the object will not be a part of the object tree.
     */
    BaseObject* copy();

  private:
    MeshT*           mesh_;

  /** @} */

  //===========================================================================
  /** @name Visualization
   * @{ */
  //===========================================================================

  public:

    /// Get the TextureNode of the current mesh
    ACG::SceneGraph::EnvMapNode* textureNode();

    /// Return pointer to the shader node
    ACG::SceneGraph::ShaderNode* shaderNode();

    /// Hide or show the selection Node of the object
    void hideSelection( bool _hide );

    /// return if the selections are currently visible
    bool selectionVisible();

    /// Hide or show the feature Node of the object
    void hideFeatures( bool _hide );

    /// return if the feature Node of the object is currently visible
    bool featuresVisible( );

    /// Hide or show the area Nodes of the object
    void hideArea(  StatusBits _bit, bool _hide );

    /** \brief Return if the selected areas are currently visible
     *
     * \note If the bits contain multiple area bits, than only if all are visible,
     *       this function will return true
     *
     *  @param _bit Status bit to check
     *  @return All visible?
     */
    bool areaVisible( StatusBits _bit );

    /// Get the Scenegraph Mesh Node
    ACG::SceneGraph::MeshNodeT<MeshT>* meshNode();

    /// Get the BoundingBox of this object
    void boundingBox( ACG::Vec3d& _bbMin , typename ACG::Vec3d& _bbMax );
    
    BaseNode* primaryNode();

  private :
    /// Status Node for a mesh, visualizing the selection state of a mesh
    ACG::SceneGraph::SelectionNodeT<MeshT>*  statusNode_;

    /// Area selection Vis
    ACG::SceneGraph::StatusNodeT<MeshT, AreaNodeMod<MeshT> > * areaNode_;

    /// Handle selection Vis
    ACG::SceneGraph::StatusNodeT<MeshT, HandleNodeMod<MeshT> > * handleNode_;

    /// Feature selection Vis
    ACG::SceneGraph::StatusNodeT<MeshT, FeatureNodeMod<MeshT> > *featureNode_;

    /// Scenegraph Mesh Node
    ACG::SceneGraph::MeshNodeT<MeshT>*                     meshNode_;

    /// Scenegraph TextureNode
    ACG::SceneGraph::EnvMapNode*     textureNode_;

    /// Scenegraph ShaderNode
    ACG::SceneGraph::ShaderNode*     shaderNode_;

    /// Scenegraph StatusNodeView
    ACG::SceneGraph::StatusViewNodeT<MeshT>* statusView_;

  /** @} */

  //===========================================================================
  /** @name Load/Save
   * @{ */
  //===========================================================================

  public:

    /// Load a mesh from the given file
    bool loadMesh(QString _filename);

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
    bool picked( uint _node_idx );

    /// Enable or disable picking for this Node
    void enablePicking( bool _enable );

    /// Check if picking is enabled for this Node
    bool pickingEnabled();

  /** @} */

  //===========================================================================
  /** @name BSP Extension ( Implements a BSP for the mesh used for fast searches )
   * @{ */
  //===========================================================================
  public:

    typedef OpenMeshTriangleBSPT< MeshT > OMTriangleBSP;

    /** Get a bsp for this object. Only supported for Triangle Meshes at the moment.
     *
     *
     * @return Pointer to bsp or Null if unsupported for this type.
     */
    OMTriangleBSP* requestTriangleBsp();

    /**  If something in the mesh changes, call this function to clear the octree.
     *  You have to do a new request as the old one will be deleted.
     * @todo : Update the tree when the update function of this object is called.
     * @todo : dont recreate but update the old one.
     * @return The new pointer to the bsp or Null if unsupported
     */
     void invalidateTriangleBsp();
  
    
    /** Update the bsp for this object. Only supported for Triangle Meshes at the moment.
     *
     *
     * @return Pointer to bsp or Null if unsupported for this type.
     */
     OMTriangleBSP* resetTriangleBsp();


  private :
    /// If requested a bsp is created for this object
    OMTriangleBSP* triangle_bsp_;

  /** @} */
};

//=============================================================================

#if defined(INCLUDE_TEMPLATES) && !defined(MESHOBJECT_C)
#define MESHOBJECT_TEMPLATES
#include "MeshObjectT.cc"
#endif


//=============================================================================
#endif // MESHOBJECT_HH defined
//=============================================================================
