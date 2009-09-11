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
 *                                                                           *
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




  //=============================================================================
  //
  //  MyTypes
  //
  //=============================================================================

  #define MESHOBJECT_C


  //== INCLUDES =================================================================

  #include <OpenFlipper/common/Types.hh>
  #include <ACG/Scenegraph/DrawModes.hh>
  #include <OpenFlipper/common/GlobalOptions.hh>
  #include <OpenFlipper/BasePlugin/PluginFunctions.hh>


  //== TYPEDEFS =================================================================

  //== CLASS DEFINITION =========================================================

  template < class MeshT , DataType objectDataType >
  MeshObject< MeshT , objectDataType >::MeshObject(const MeshObject& _object) :
      BaseObjectData(_object),
      statusNode_(0),
      areaNode_(0),
      handleNode_(0),
      featureNode_(0),
      meshNode_(0),
      textureNode_(0),
      shaderNode_(0),
      triangle_bsp_(0)
  {
    init(_object.mesh_);

    setName( name() );

//     textures_     = _object.textures_;
//     textureNames_ = _object.textureNames_;
  }

  /** Constructor for Mesh Objects. This object class gets a Separator Node giving
  *  the root node to which it should be connected. The mesh is generated internally
  *  and all nodes for visualization will be added below the scenegraph node.\n
  *  You dont need to create an object of this type manually. Use
  *  PluginFunctions::addTriMesh or PluginFunctions::addPolyMesh instead. The
  *  objectDataType has to match the one of MeshT ( see Types.hh::DataType )
  */
  template < class MeshT , DataType objectDataType >
  MeshObject< MeshT , objectDataType >::MeshObject( SeparatorNode* _rootNode ) :
    BaseObjectData(_rootNode ),
    mesh_(0),
    statusNode_(0),
    areaNode_(0),
    handleNode_(0),
    featureNode_(0),
    meshNode_(0),
    textureNode_(0),
    shaderNode_(0),
    triangle_bsp_(0)
  {
    setDataType(objectDataType);
    init();
  }

  /** Destructor for Mesh Objects. The destructor deletes the mesh and all
  *  Scenegraph nodes associated with the mesh or the object.
  */
  template < class MeshT , DataType objectDataType >
  MeshObject< MeshT , objectDataType >::~MeshObject()
  {
    // Delete the data attached to this object ( this will remove all perObject data)
    // Not the best way to do it but it will work.
    // This is only necessary if people use references to the mesh below and
    // they do something with the mesh in the destructor of their
    // perObjectData.
    deleteData();

    // Delete the Mesh only, if this object contains a mesh
    if ( mesh_ != NULL)  {
      delete mesh_;
      mesh_ = NULL;
    } else {
      std::cerr << "Destructor error : Mesh already deleted" << std::endl;
    }

    if ( triangle_bsp_ != 0 )
      delete triangle_bsp_;
    triangle_bsp_ = 0;

    // No need to delete the scenegraph Nodes as this will be managed by baseplugin
    areaNode_    = 0;
    handleNode_  = 0;
    featureNode_ = 0;
    meshNode_    = 0;
    textureNode_ = 0;
    shaderNode_  = 0;
  }

  /** Cleanup Function for Mesh Objects. Deletes the contents of the whole object and
  * calls MeshObject::init afterwards.
  */
  template < class MeshT , DataType objectDataType >
  void MeshObject< MeshT , objectDataType >::cleanup() {
    // Delete the Mesh only, if this object contains a mesh
    if ( mesh_ != NULL)  {
      delete mesh_;
      mesh_ = NULL;
    } else {
      std::cerr << "Cleanup error : Triangle Mesh already deleted" << std::endl;
    }

    if ( triangle_bsp_ != 0 )
      delete triangle_bsp_;
    triangle_bsp_ = 0;

    BaseObjectData::cleanup();

    statusNode_  = 0;
    areaNode_    = 0;
    handleNode_  = 0;
    featureNode_ = 0;
    textureNode_ = 0;
    shaderNode_  = 0;
    meshNode_    = 0;

    setDataType(objectDataType);

    init();

  }

  /** This function initalizes the mesh object. It creates the scenegraph nodes,
  *  the mesh and requests all required properties for the mesh.
  */
  template < class MeshT , DataType objectDataType >
  void MeshObject< MeshT , objectDataType >::init(MeshT* _mesh) {

    if ( _mesh == 0 )
      mesh_ = new MeshT();
    else
      mesh_ = new MeshT(*_mesh);

    mesh_->request_vertex_normals();
    mesh_->request_face_normals();
    mesh_->request_vertex_status();
    mesh_->request_halfedge_status();
    mesh_->request_face_status();
    mesh_->request_edge_status();
    mesh_->request_vertex_colors();
    mesh_->request_face_colors();
    mesh_->request_vertex_texcoords2D();
    mesh_->request_halfedge_texcoords2D();
    mesh_->request_face_texture_index();


    // Only initialize scenegraph nodes when we initialized a gui!!
    if ( OpenFlipper::Options::nogui() )
      return;


 	 if ( manipulatorNode() == NULL)
		std::cerr << "Error when creating Mesh Object! manipulatorNode is NULL!" << std::endl;

    // Node showing selection
    statusNode_ = new ACG::SceneGraph::SelectionNodeT<MeshT>(*mesh_,manipulatorNode(),"NEW StatusNode for mesh " );
    statusNode_->set_point_size(4.0);
    statusNode_->set_color(ACG::Vec4f(1.0,0.0,0.0,1.0));

    // Node showing modeling region
    areaNode_ = new ACG::SceneGraph::StatusNodeT<MeshT, AreaNodeMod<MeshT> >(*mesh_, manipulatorNode(), "NEW AreaNode for mesh ");
    areaNode_->set_round_points(true);
    areaNode_->enable_alpha_test(0.5);
    areaNode_->set_point_size(7.0);
    areaNode_->set_color(ACG::Vec4f(0.4, 0.4, 1.0, 1.0));

    // Node showing handle region
    handleNode_ = new ACG::SceneGraph::StatusNodeT<MeshT, HandleNodeMod<MeshT> >(*mesh_, manipulatorNode(), "NEW HandleNode for mesh ");
    handleNode_->set_round_points(true);
    handleNode_->enable_alpha_test(0.5);
    handleNode_->set_line_width(2.0);
    handleNode_->set_point_size(7.0);
    handleNode_->set_color(ACG::Vec4f(0.2, 1.0, 0.2, 1.0));

    // Node showing feature selection
    featureNode_ = new ACG::SceneGraph::StatusNodeT<MeshT, FeatureNodeMod<MeshT> >(*mesh_, manipulatorNode(), "NEW FeatureNode for mesh ");
    featureNode_->set_round_points(true);
    featureNode_->enable_alpha_test(0.5);
    featureNode_->set_line_width(2.0);
    featureNode_->set_point_size(7.0);
    featureNode_->set_color(ACG::Vec4f(1.0, 0.2, 1.0, 1.0));


    if ( materialNode() == NULL)
		std::cerr << "Error when creating Mesh Object! materialNode is NULL!" << std::endl;

    textureNode_ = new ACG::SceneGraph::EnvMapNode(materialNode(),"NEW TextureNode for ", true, GL_LINEAR_MIPMAP_LINEAR );

    shaderNode_  = new ACG::SceneGraph::ShaderNode(textureNode() , "NEW ShaderNode for ");

    meshNode_    = new ACG::SceneGraph::TriStripNodeT<MeshT>(*mesh_, shaderNode_, "NEW MeshNode");

    QString shaderDir = OpenFlipper::Options::shaderDirStr() +
                        OpenFlipper::Options::dirSeparator();

    std::string shaderDirectory = std::string( shaderDir.toUtf8() );
    shaderNode_->setShaderDir( shaderDirectory );



    if ( QFile( shaderDir + "Phong/Vertex.glsl").exists() && QFile( shaderDir + "Phong/Fragment.glsl" ).exists() ) {
      shaderNode_->setShader(ACG::SceneGraph::DrawModes::SOLID_PHONG_SHADED,
                            "Phong/Vertex.glsl" ,
                            "Phong/Fragment.glsl" );
    } else
      std::cerr << "Shader Files for Phong not found!" << std::endl;

    if ( QFile( shaderDir + "Ward/Vertex.glsl").exists() && QFile( shaderDir + "Ward/Fragment.glsl" ).exists() ) {

      shaderNode_->setShader(ACG::SceneGraph::DrawModes::SOLID_SHADER,
                              "Ward/Vertex.glsl" ,
                              "Ward/Fragment.glsl" );

      // Ward shader uses 3 parameters so activate shader, set params and deactivate it again
      GLSL::PtrProgram shader = shaderNode_->getShader( ACG::SceneGraph::DrawModes::SOLID_SHADER );
      if ( shader == 0 ) {
        std::cerr << "Unable to get shader for shader mode" << std::endl;
      } else {
        shader->use();

        shader->setUniform("ward_specular" , 0.5f);
        shader->setUniform("ward_diffuse"  , 3.0f);
        shader->setUniform("ward_alpha"    , 0.2f);

        shader->disable();
      }

    } else
        std::cerr << "Shader Files for Ward not found!!" << std::endl;

    // Update all nodes
    update();
  }

  // ===============================================================================
  // Name/Path Handling
  // ===============================================================================

  /** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
  * BaseObjectData::setName.
  */
  template < class MeshT , DataType objectDataType >
  void MeshObject< MeshT , objectDataType >::setName( QString _name ) {
    BaseObjectData::setName(_name);

    // No update when gui is not active
    if ( OpenFlipper::Options::nogui() )
      return;

    std::string nodename = std::string("StatusNode for mesh " + _name.toUtf8() );
    statusNode_->name( nodename );

    nodename = std::string("AreaNode for mesh   " + _name.toUtf8() );
    areaNode_->name( nodename );

    nodename = std::string("HandleNode for mesh " + _name.toUtf8() );
    handleNode_->name( nodename );

    nodename = std::string("FeatureNode for mesh " + _name.toUtf8() );
    featureNode_->name( nodename );

    nodename = std::string("TextureNode for mesh "+ _name.toUtf8() );
    textureNode_->name( nodename );

    nodename = std::string("ShaderNode for mesh "+ _name.toUtf8() );
    shaderNode_->name( nodename );

    nodename = std::string("MeshNode for mesh "     + _name.toUtf8() );
    meshNode_->name( nodename );
  }

  // ===============================================================================
  // Content
  // ===============================================================================

  /** Get a pointer to the objects mesh.
  * @return Pointer to the mesh
  */
  template < class MeshT , DataType objectDataType >
  MeshT* MeshObject< MeshT , objectDataType >::mesh() {
    return mesh_;
  }

  /** Updates the visualization of the object. Calls MeshObject::updateGeometry,
  *  MeshObject::updateTopology, MeshObject::updateSelection and
  *  MeshObject::updateModelingRegions.
  */
  template < class MeshT , DataType objectDataType >
  void MeshObject< MeshT , objectDataType >::update() {

    // No update necessary if no gui
    if ( OpenFlipper::Options::nogui() )
      return;

    PluginFunctions::setMainGLContext();

    updateGeometry();
    updateTopology();
    updateSelection();
    updateFeatures();
    updateModelingRegions();
  }

  /** Updates the selection scenegraph nodes */
  template < class MeshT , DataType objectDataType >
  void MeshObject< MeshT , objectDataType >::updateSelection() {
    if ( statusNode_ )
      statusNode_->update_cache();
  }

  /** Updates the geometry information in the mesh scenegraph node */
  template < class MeshT , DataType objectDataType >
  void MeshObject< MeshT , objectDataType >::updateGeometry() {
    if ( meshNode_ )
      meshNode_->update_geometry();
    invalidateTriangleBsp();
  }

  /** Updates the topology information in the mesh scenegraph node */
  template < class MeshT , DataType objectDataType >
  void MeshObject< MeshT , objectDataType >::updateTopology() {
    if ( meshNode_ ) {
      meshNode_->update_topology();
      meshNode_->update_strips();
    }
    invalidateTriangleBsp();
  }

  /** Updates the modeling regions scenegraph nodes */
  template < class MeshT , DataType objectDataType >
  void MeshObject< MeshT , objectDataType >::updateModelingRegions() {
    if ( areaNode_ && handleNode_ ) {
      areaNode_->update_cache();
      handleNode_->update_cache();
    }
  }

  /** Updates the modeling regions scenegraph nodes */
  template < class MeshT , DataType objectDataType >
  void MeshObject< MeshT , objectDataType >::updateFeatures() {
    if ( featureNode_ )
      featureNode_->update_cache();
  }


  template < class MeshT , DataType objectDataType >
  BaseObject* MeshObject< MeshT , objectDataType >::copy() {
    MeshObject< MeshT , objectDataType >* object = new MeshObject< MeshT , objectDataType >(*this);
    return dynamic_cast< BaseObject* >(object);
  }

  // ===============================================================================
  // Visualization
  // ===============================================================================
  /** Returns a pointer to the texture node
  * @return Pointer to the texture node
  */
  template < class MeshT , DataType objectDataType >
  ACG::SceneGraph::EnvMapNode* MeshObject< MeshT , objectDataType >::textureNode() {
    return textureNode_;
  }

  /** Returns a pointer to the shader node
  * @return Pointer to the shader node
  */
  template < class MeshT , DataType objectDataType >
  ACG::SceneGraph::ShaderNode* MeshObject< MeshT , objectDataType >::shaderNode() {
    return shaderNode_;
  }

  /** Shows or hides the selections on the object
   */
  template < class MeshT , DataType objectDataType >
  void  MeshObject< MeshT , objectDataType >::hideSelection( bool _hide ) {

    if ( _hide ) {
      statusNode_->set_status( ACG::SceneGraph::BaseNode::HideNode );
    } else {
      statusNode_->set_status( ACG::SceneGraph::BaseNode::Active );
    }

  }

  template < class MeshT , DataType objectDataType >
  void  MeshObject< MeshT , objectDataType >::hideFeatures( bool _hide ) {
    if ( _hide ) {
      featureNode_->set_status( ACG::SceneGraph::BaseNode::HideNode );
    } else {
      featureNode_->set_status( ACG::SceneGraph::BaseNode::Active );
    }
  }

  template < class MeshT , DataType objectDataType >
  bool  MeshObject< MeshT , objectDataType >::featuresVisible( ) {
    return ( featureNode_->status() == ACG::SceneGraph::BaseNode::Active );
  }

  /** Shows or hides the areas on the object
   */
  template < class MeshT , DataType objectDataType >
  void  MeshObject< MeshT , objectDataType >::hideAreas( bool _hide ) {
    if ( _hide ) {
      areaNode_->set_status( ACG::SceneGraph::BaseNode::HideNode );
      handleNode_->set_status( ACG::SceneGraph::BaseNode::HideNode );
    } else {
      areaNode_->set_status( ACG::SceneGraph::BaseNode::Active );
      handleNode_->set_status( ACG::SceneGraph::BaseNode::Active );
    }
  }

  template < class MeshT , DataType objectDataType >
  bool MeshObject< MeshT , objectDataType >::selectionVisible() {
    return ( statusNode_->status() == ACG::SceneGraph::BaseNode::Active );
  }

  template < class MeshT , DataType objectDataType >
  bool MeshObject< MeshT , objectDataType >::areasVisible() {
    return ( areaNode_->status() == ACG::SceneGraph::BaseNode::Active );
  }

  /** Returns a pointer to the mesh node
  * @return Pointer to the mesh node
  */
  template < class MeshT , DataType objectDataType >
  ACG::SceneGraph::TriStripNodeT<MeshT>* MeshObject< MeshT , objectDataType >::meshNode() {
    return meshNode_;
  }

  /** Get the Bounding box size of this object
   */
  template < class MeshT , DataType objectDataType >
  void MeshObject< MeshT , objectDataType >::boundingBox( ACG::Vec3f& _bbMin , ACG::Vec3f& _bbMax ) {
    if ( meshNode_ ) {
      _bbMin = ACG::Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
      _bbMax = ACG::Vec3f(FLT_MIN, FLT_MIN, FLT_MIN);
      meshNode_->boundingBox(_bbMin,_bbMax);
    } else {
      std::cerr << "Error: Bounding box computation via Scenegraph not available without gui" << std::endl;
    }
  }

  // ===============================================================================
  // Load/Save
  // ===============================================================================

  /** Load the mesh data from a file. The loader uses the OpenMesh object loaders and
  * therefore supports all mesh types supported by OpenMesh.
  *
  * @param _filename Full path of the file to load.
  */
  template < class MeshT , DataType objectDataType >
  bool MeshObject< MeshT , objectDataType >::loadMesh(QString _filename) {

    setFromFileName(_filename);

    // call the local function to update names
    setName( name() );

    std::string filename = std::string( _filename.toUtf8() );

    // load file
    bool ok = OpenMesh::IO::read_mesh( (*mesh()) , filename );
    if (!ok)
    {
      if ( dataType() == DATA_TRIANGLE_MESH )
        std::cerr << "Main Application : Read error for Triangle Mesh at "<< filename << std::endl;
      if ( dataType() == DATA_POLY_MESH )
        std::cerr << "Main Application : Read error for Poly Mesh\n";
      return false;
    }

    mesh()->update_normals();

    update();

    show();

    return true;
  }

  // ===============================================================================
  // Object information
  // ===============================================================================

  /** Returns a string containing all information about the current object. This also
  * includes the information provided by BaseObjectData::getObjectinfo
  *
  * @return String containing the object information
  */
  template < class MeshT , DataType objectDataType >
  QString MeshObject< MeshT , objectDataType >::getObjectinfo() {
    QString output;

    output += "========================================================================\n";
    output += BaseObjectData::getObjectinfo();

    if ( dataType( DATA_TRIANGLE_MESH ) )
      output += "Object Contains Triangle Mesh : ";

    if ( dataType( DATA_POLY_MESH ) )
      output += "Object Contains Poly Mesh : ";

    output += QString::number( mesh()->n_vertices() ) + " vertices, ";
    output += QString::number( mesh()->n_edges() ) += " edges ";
    output += QString::number( mesh()->n_faces() ) += " faces.\n";

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
  template < class MeshT , DataType objectDataType >
  bool MeshObject< MeshT , objectDataType >::picked( uint _node_idx ) {
    return ( _node_idx == meshNode_->id() );
  }


  template < class MeshT , DataType objectDataType >
  void MeshObject< MeshT , objectDataType >::enablePicking( bool _enable ) {
    if ( OpenFlipper::Options::nogui())
      return;

    meshNode_->enablePicking( _enable );
    areaNode_->enablePicking( _enable );
    handleNode_->enablePicking( _enable );
    featureNode_->enablePicking( _enable );
    textureNode_->enablePicking( _enable );
    shaderNode_->enablePicking( _enable );
  }

  template < class MeshT , DataType objectDataType >
  bool MeshObject< MeshT , objectDataType >::pickingEnabled() {
    return meshNode_->pickingEnabled();
  }

  // ===============================================================================
  // Octree
  // ===============================================================================

  template < class MeshT , DataType objectDataType >
  typename MeshObject< MeshT , objectDataType >::OMTriangleBSP*
  MeshObject< MeshT , objectDataType >::requestTriangleBsp() {

    if ( ! dataType( DATA_TRIANGLE_MESH) ) {
      std::cerr << "Bsps are only supported for triangle meshes." << std::endl;
      return 0;
    }

    // Create the tree if needed.
    if ( triangle_bsp_ == 0 )
    {
      // create Triangle BSP
      triangle_bsp_ = new OMTriangleBSP( *mesh() );

      // build Triangle BSP
      triangle_bsp_->reserve(mesh()->n_faces());

      typename MeshT::FIter f_it  = mesh()->faces_begin();
      typename MeshT::FIter f_end = mesh()->faces_end();

      for (; f_it!=f_end; ++f_it)
	     triangle_bsp_->push_back(f_it.handle());

      triangle_bsp_->build(10, 100);
    }

    // return pointer to triangle bsp
    return triangle_bsp_;
  }

  template < class MeshT , DataType objectDataType >
  typename MeshObject< MeshT , objectDataType >::OMTriangleBSP*
  MeshObject< MeshT , objectDataType >::resetTriangleBsp() {
     if ( triangle_bsp_ != 0 )
     {
       delete triangle_bsp_;
       triangle_bsp_ = 0;
     }

     return requestTriangleBsp();
  }

  template < class MeshT , DataType objectDataType >
  void 
  MeshObject< MeshT , objectDataType >::invalidateTriangleBsp() {
     if ( triangle_bsp_ != 0 )
     {
       delete triangle_bsp_;
       triangle_bsp_ = 0;
     }
  }


  //=============================================================================

