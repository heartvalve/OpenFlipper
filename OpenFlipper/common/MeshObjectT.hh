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

 
#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

//== INCLUDES =================================================================

// -------------------- OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>

#include <ACG/Scenegraph/SeparatorNode.hh>
#include <ACG/Scenegraph/TextureNode.hh>
#include <ACG/Scenegraph/ShaderNode.hh>
#include <ACG/Scenegraph/StatusNodesT.hh>

/// Additional Node status Bits for Mesh 
enum StatusBits {
   /// Modifyable area
   AREA      = OpenMesh::Attributes::UNUSED << 1,
   /// Handle Area
   HANDLEAREA = OpenMesh::Attributes::UNUSED << 2
};

#include "StatusNodeMods.hh"
#include "TriangleMeshTypes.hh"
#include "PolyMeshTypes.hh"
             
// -------------------- BSP
#include <OpenFlipper/common/bsp/TriangleBSPT.hh>

             
#include "BaseObjectData.hh"           
             

             

//== TYPEDEFS =================================================================
   
/// Texture Node
typedef ACG::SceneGraph::TextureNode                      TextureNode;            

//== CLASS DEFINITION =========================================================

/** This class provides the functionality for all kind of meshes for the framework
 */
template < class MeshT, DataType objectDataType > 
class DLLEXPORT MeshObject : public BaseObjectData {
  public: 
    /// constructor
    MeshObject(SeparatorNode* _rootNode);
    
    /// destructor
    virtual ~MeshObject();
    
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
      
    /// Update Geometry of all data structures
    void updateGeometry();
      
    /// Update Topology of all data structures
    void updateTopology();
    
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
    
    /// Get the Scenegraph Mesh Node
    ACG::SceneGraph::MeshNodeT<MeshT>* meshNode();;
    
    /// Get the BoundingBox of this object
    void boundingBox( ACG::Vec3f& _bbMin , typename ACG::Vec3f& _bbMax );
        
  private :
    /// Status Node for a mesh, visualizing the selection state of a mesh
    ACG::SceneGraph::SelectionNodeT<MeshT>*  statusNode_;
      
    /// Area selection Vis
    ACG::SceneGraph::StatusNodeT<MeshT, AreaNodeMod<MeshT> > * areaNode_;
      
    /// Handle selection Vis
    ACG::SceneGraph::StatusNodeT<MeshT, HandleNodeMod<MeshT> > * handleNode_;
    
    /// Scenegraph Mesh Node
    ACG::SceneGraph::MeshNodeT<MeshT>*                     meshNode_;
      
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
    
    void addTexture(QString _property, QString _textureFile );
        
    /** This vector is used to store the available Textures, which will be handled
     *   by the texture control plugin. For meshes the first String identifies the mesh
     *   property used for the texture coordinates. The second String identifies the
     *  texture file. Use addTexture() to add additional textures. If the property does not exist,
     *  nothing happens if you select it.
     */
    std::vector< std::pair < QString , QString > > textures;
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
#define TYPES_TEMPLATES
#include "MeshObjectT.cc"
#endif
             
             
//=============================================================================
#endif // MESHOBJECT_HH defined
//=============================================================================
