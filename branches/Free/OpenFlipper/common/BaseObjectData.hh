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
 * \file BaseObjectData.hh
 * This File contains the Basic object class for all Objects which show content vi
 * the Scenegraph.
 */


#ifndef BASEOBJECTDATA_HH
#define BASEOBJECTDATA_HH


#ifndef DLLEXPORT
#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif
#endif 

//== INCLUDES =================================================================

#include "BaseObject.hh"
#include <vector>
#include <ACG/Scenegraph/MaterialNode.hh>
#include <ACG/Scenegraph/SeparatorNode.hh>
#include <ACG/Scenegraph/ShaderNode.hh>
#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/Scenegraph/ManipulatorNode.hh>

//== TYPEDEFS =================================================================

//== TYPEDEFS FOR SCENEGRAPH ===============================================
/// Materialnode
typedef ACG::SceneGraph::MaterialNode                     MaterialNode;
/// ManipulatorNode
typedef ACG::SceneGraph::ManipulatorNode                  ManipulatorNode;
/// Seperator Node
typedef ACG::SceneGraph::SeparatorNode                    SeparatorNode;
/// Base Node
typedef ACG::SceneGraph::BaseNode                         BaseNode;

//== CLASS DEFINITION =========================================================

/**
 * This is the basic Data class providing the functions common to all objects which show Objects in the SceneGraph
 */
class DLLEXPORT BaseObjectData : public BaseObject
{
  public:
    /// constructor
    BaseObjectData(SeparatorNode* _rootNode);

    ///destructor
    virtual ~BaseObjectData();

  protected:
    /** This function creates the scenegraph nodes */
    virtual void init();

  /** @} */

  //===========================================================================
  /** @name Data
   * @{ */
  //===========================================================================

  public:
    /** Clean all data structures of the object
      *
      * */
    virtual void cleanup();

  /** @} */

  //===========================================================================
  /** @name Name and Path handling
   * @{ */
  //===========================================================================

  public:

    /** Set the object name from a filename. The function will set the name of the
     * object to the filename. At the same time the path is set to the one given in
     * the parameter
     *
     * @param _filename path to the file.
     */
    void setFromFileName(QString _filename );

    /// set the name of the object. ( If you overwrite it, call BaseObjectData::setName(_name ) it in your funtion first)
    virtual void setName( QString _name );

    /// return the path to the object ( defaults to "." if unset )
    QString path();

    /// set the path to the object.
    void path(QString _path);

  private:

    /// path to the file from which the object is loaded ( defaults to "." )
    QString path_;

  /** @} */



  //===========================================================================
  /** @name Object visualization
   * @{ */
  //===========================================================================

  public :
    /// Sets the whole Scenegraph subtree of this node to visible
    void show();

    /// Sets the whole Scenegraph subtree of this node to invisible
    void hide();

    /** get the base node of this object (Use this node to add custom Nodes to the Object
     * which should not be transformed with the manipulator of the Object)
     */
    SeparatorNode* baseNode();

    /** get the ManipulatorNode node of this object (Use this node to add custom Nodes to the Object
     * which should be transformed with the manipulator of the Object)
     */
    ManipulatorNode* manipulatorNode();

    /** Return pointer to the shader node
     * If you want to support shaders, you have to add a shader node into your scenegraph structure
     * above your object to be rendered. If you do not have a shader, just ignore this function
     * and it will return a 0 pointer.
     */
    virtual ACG::SceneGraph::ShaderNode* shaderNode();

    /// get a pointer to the materialnode
    MaterialNode* materialNode();

    /// Set the color of the object
    void setBaseColor(ACG::Vec4f _color);

    /// Check if the manipulator has been placed
    bool manipPlaced();

    /// set the manipulator place status
    void manipPlaced( bool _placed );

    /// get the bounding box of the object
    void getBoundingBox(ACG::Vec3d& bbmin, ACG::Vec3d& bbmax);

  private :


    bool manipPlaced_;

    /// rootNode of global Scenegraph structure
    SeparatorNode*    rootNode_;

    /// Separator at top of Scenegraph structure used for this Object
    SeparatorNode*    separatorNode_;

    /// Manipulator used for this Object
    ManipulatorNode*  manipulatorNode_;

    /// Scenegraph Material Node for the object
    MaterialNode*    materialNode_;

  /** @} */


  //===========================================================================
  /** @name Picking
   * @{ */
  //===========================================================================

  public :
    /// detect if the node has been picked
    virtual bool picked( uint _node_idx );

    /** Enable or disable picking for this Node
     * The basic function defined here does nothing.
     * It has to be reimplemented in the derived class
     */
    virtual void enablePicking( bool _enable );

    /** Check if picking is enabled for this Node
     * This function will return true unless the derived class overwrites
     * this function.
     */
    virtual bool pickingEnabled();

  /** @} */

  //===========================================================================
  /** @name Content
   * @{ */
  //===========================================================================

  public:
    /// This function is called to update the object
    virtual void update();

  /** @} */

  //===========================================================================
  /** @name Additional nodes
   * @{ */
  //===========================================================================
  public:
    /** \brief add an additional node to the object
    *
    * This function can be used to store an additional Scenegraph node. If you add nodes there, you do not
    * need to keep track of deleted, added objects, as this will be done in the main application.
    * You have to create the node yourself as this function does not know the type. You should add the
    * new node below the manipulatorNode if you want that it moves with the rest of the data. Otherwise
    * add it below the seperatorNode of the object.
    *
    *  @param _node Node to add
    *  @param _pluginName Name of the current plugin
    *  @param _nodeName Name of the New Node
    *  @param _id extra index, if there will be multiple nodes with this name( defaults to 0)
    *  @return true if successfull
    */
    template< typename NodeT >
    bool addAdditionalNode(NodeT* _node , QString _pluginName, QString _nodeName , int _id = 0);

    /** \brief check if an object has the additional node
    *
    * If you store additional Scenegraph nodes with the objects you can check if they
    * exist with this function.
    *
    *  @param _pluginName Name of the current plugin
    *  @param _nodeName Name of the Node
    *  @param _id extra index, if there are multiple nodes with this name( defaults to 0)
    *  @return true if found
    */
    bool hasAdditionalNode(QString _pluginName, QString _nodeName , int _id = 0);

    /** \brief get an addition node from the object
    *
    * If you store additional Scenegraph node with the objects you can get these nodes with this function.
    *
    *  @param _node Returns the node
    *  @param _pluginName Name of the current plugin
    *  @param _nodeName Name of the Node
    *  @param _id extra index, if there are multiple nodes with this name ( defaults to 0)
    *  @return true if found
    */
    template< typename NodeT >
    bool getAdditionalNode(NodeT*& _node , QString _pluginName, QString _nodeName , int _id = 0 );

    /** \brief remove an additional node from the object
    *
    * If additional nodes are stored for this object, such a node can be removed using this function
    *
    *  @param _node Needed for type specification
    *  @param _pluginName Name of the current plugin
    *  @param _nodeName Name of the Node
    *  @param _id extra index, if there are multiple nodes with this name ( defaults to 0)
    *  @return true if found and removed
    */
    template< typename NodeT >
    bool removeAdditionalNode(NodeT*& _node, QString _pluginName, QString _nodeName , int _id = 0 );
  private:
    /** This pointer may be used to store additional Nodes belonging to this Object
     *   The String should be used by the plugin to identify its Nodes
     */
    std::vector< std::pair <BaseNode*,QString> > additionalNodes_;

  /** @} */

};


//=============================================================================

#if defined(INCLUDE_TEMPLATES) && !defined(BASEOBJECTDATA_C)
#define BASEOBJECT_TEMPLATES
#include "BaseObjectDataT.cc"
#endif

//=============================================================================
#endif // BASEOBJECTDATA_HH defined
//=============================================================================
