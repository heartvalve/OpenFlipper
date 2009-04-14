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
#include <ACG/Scenegraph/TextureNode.hh>
#include <ACG/Scenegraph/ShaderNode.hh>
#include <ACG/Scenegraph/StatusNodesT.hh>

#include <OpenFlipper/common/GlobalDefines.hh>

/// Additional Node status Bits for Mesh
enum StatusBits {
   /// Modifyable area
   AREA      = OpenMesh::Attributes::UNUSED << 1,
   /// Handle Area
   HANDLEAREA = OpenMesh::Attributes::UNUSED << 2
};

#include <ObjectTypes/MeshObject/StatusNodeMods.hh>
#include <OpenFlipper/common/BaseObjectData.hh>

// -------------------- BSP
#include <OpenFlipper/common/bsp/TriangleBSPT.hh>


#include <OpenFlipper/common/BaseObjectData.hh>
#include <ACG/Scenegraph/TriStripNodeT.hh>




//== TYPEDEFS =================================================================

/// Texture Node
typedef ACG::SceneGraph::TextureNode                      TextureNode;

//== CLASS DEFINITION =========================================================

/** This class provides the functionality for all kind of meshes for the framework
 */
template < class MeshT, DataType objectDataType >
class DLLEXPORTONLY MeshObject : public BaseObjectData {
  public:

    /** \brief copy constructor
     *
     *  Create a copy of this object
     */
    MeshObject(const MeshObject& _object);

    /// constructor
    MeshObject(SeparatorNode* _rootNode);

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
    virtual void update();

    /// Call this function to update the modeling regions
    void updateSelection();

    /// Call this function to update the modeling regions
    void updateModelingRegions();

    /// Update Feature Visualization Node
    void updateFeatures();

    /// Update Geometry of all data structures
    void updateGeometry();

    /// Update Topology of all data structures
    void updateTopology();

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
    TextureNode* textureNode();

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
    void hideAreas( bool _hide );

    /// return if the areas are currently visible
    bool areasVisible();

    /// Get the Scenegraph Mesh Node
    ACG::SceneGraph::TriStripNodeT<MeshT>* meshNode();;

    /// Get the BoundingBox of this object
    void boundingBox( ACG::Vec3f& _bbMin , typename ACG::Vec3f& _bbMax );

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
    ACG::SceneGraph::TriStripNodeT<MeshT>*                     meshNode_;

    /// Scenegraph TextureNode
    TextureNode*     textureNode_;

    /// Scenegraph ShaderNode
    ACG::SceneGraph::ShaderNode*     shaderNode_;

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
  /** @name Texture Handling
   * @{ */
  //===========================================================================
  public :

    /** Add a texture to the object.
    * @param _image Image to use as texture
    * @param _name  Name of the texture. Required to access the texture
    * @param _id    Id of the texture to be used in the texture_index property of the mesh
    *              ( for multiple textures per object only)
    */
    void addTexture( QString _name , QImage& _image , int& _id );

    /** \brief Change an already loaded texture by replacing it with the given image.
     *
     * If the texture does not exist, a new one will be created.
     *
     */
    void setTexture( QString _name , QImage& _image );

    /** Checks if a texture already exists
     *
     */
    bool textureExists( QString _name );

    /** \brief Enable the given texture
     *
     * Use this function to enable a specific texture.
     */
    bool enableTexture( QString _name );

  private:
    /** This vector is used to store the available Textures. Each call to addTexture will load
     *  the texture in the textureNode. The MeshNode will use only the first texture unless
     *  the mesh has requested a face_index_texture with request_face_texture_index. Additionally
     *  request_halfedge_texcoords2D has to be used to set per face texture coordinates.
     */
    std::map< int, GLuint > textures_;
    std::map< int, std::string> textureNames_;
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
