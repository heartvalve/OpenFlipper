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
 * \file BaseObjectData.hh
 * This File contains the Basic object class for all Objects which show content vi
 * the Scenegraph.
 */


#ifndef BASEOBJECTDATA_HH
#define BASEOBJECTDATA_HH


//== INCLUDES =================================================================

#include <OpenFlipper/common/GlobalDefines.hh>
#include <OpenFlipper/common/BaseObject.hh>
#include <QObject>
#include <vector>
#include <ACG/Scenegraph/MaterialNode.hh>
#include <ACG/Scenegraph/SeparatorNode.hh>
#include <ACG/Scenegraph/ShaderNode.hh>
#include <ACG/Scenegraph/BaseNode.hh>
#include <ACG/Scenegraph/BoundingBoxNode.hh>
#include <ACG/Scenegraph/StencilRefNode.hh>
#include <ACG/QtScenegraph/QtTranslationManipulatorNode.hh>

//== TYPEDEFS =================================================================

//== TYPEDEFS FOR SCENEGRAPH ===============================================
/// Materialnode
typedef ACG::SceneGraph::MaterialNode                     MaterialNode;
/// ManipulatorNode
typedef ACG::SceneGraph::QtTranslationManipulatorNode     QtTranslationManipulatorNode;
/// Seperator Node
typedef ACG::SceneGraph::SeparatorNode                    SeparatorNode;
/// Base Node
typedef ACG::SceneGraph::BaseNode                         BaseNode;
/// Bounding box Node
typedef ACG::SceneGraph::BoundingBoxNode                  BoundingBoxNode;
/// Stencil reference Node
typedef ACG::SceneGraph::StencilRefNode                   StencilRefNode;

//== CLASS DEFINITION =========================================================

/**
 * This is the basic Data class providing the functions common to all objects which show Objects in the SceneGraph
 */
class DLLEXPORT BaseObjectData : public BaseObject
{
  Q_OBJECT
  
  public:

    /** \brief copy constructor
     *
     *  Create new basic scenegraph nodes for this object
     */
    BaseObjectData(const BaseObjectData& _object);

    /// constructor
    BaseObjectData();

    ///destructor
    virtual ~BaseObjectData();

  private:
    /** This function creates the basic scenegraph nodes */
    void initializeScenegraphNodes();

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

    /// set the name of the object. ( If you overwrite it, call BaseObjectData::setName(_name ) it in your function first)
    virtual void setName( QString _name );

    /// return the path to the object ( defaults to "." if unset )
    QString path();

    /// set the path to the object.
    void setPath(QString _path);

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
    virtual void show();

    /// Sets the whole Scenegraph subtree of this node to invisible
    virtual void hide();

    /// return visiblity
    virtual bool visible();

    /// Sets visiblity of the whole Scenegraph subtree of this node
    virtual void visible(bool _visible);

    /** get the base node of this object (Use this node to add custom Nodes to the Object
     * which should not be transformed with the manipulator of the Object)
     */
    SeparatorNode* baseNode();

    /** \brief Check if the given node is owned by this object
    *
    * You can overload this function and return true, if your object generated the given node. 
    * Don't forget to call this baseclass function on overload!
    */
    virtual bool hasNode(BaseNode* _node);
    
    /** get the primary node of this object (Use this node to change drawModes)
     */
    virtual BaseNode* primaryNode();
    
    /** get the ManipulatorNode node of this object (Use this node to add custom Nodes to the Object
     * which should be transformed with the manipulator of the Object)
     */
    QtTranslationManipulatorNode* manipulatorNode();

    /** Return pointer to the shader node
     * If you want to support shaders, you have to add a shader node into your scenegraph structure
     * above your object to be rendered. If you do not have a shader, just ignore this function
     * and it will return a 0 pointer.
     */
    virtual ACG::SceneGraph::ShaderNode* shaderNode();

    /// get a pointer to the materialnode
    MaterialNode* materialNode();

    /// get a pointer to the bounding box node
    BoundingBoxNode* boundingBoxNode();

    /// get a pointer to the stencil reference node
    StencilRefNode* stencilRefNode();

    /// Check if the manipulator has been placed
    bool manipPlaced();

    /// set the manipulator place status
    void manipPlaced( bool _placed );

    /// get the bounding box of the object
    void getBoundingBox(ACG::Vec3d& bbmin, ACG::Vec3d& bbmax);
    
    /** \brief Set the draw mode for the object
     * @param _mode  The draw mode that should be active for this object
     * @param _force If true, the mode is set ignoring if its supported by the node and its subnodes
     */
    void setObjectDrawMode(const ACG::SceneGraph::DrawModes::DrawMode _mode, bool _force = false);

  private :


    bool manipPlaced_;

    /// rootNode of global Scenegraph structure
    SeparatorNode*    rootNode_;

    /// Separator at top of Scenegraph structure used for this Object
    SeparatorNode*    separatorNode_;

    /// Manipulator used for this Object
    QtTranslationManipulatorNode*  manipulatorNode_;

    /// Scenegraph Material Node for the object
    MaterialNode*    materialNode_;

    /// Bounding box node for the object
    BoundingBoxNode * boundingBoxNode_;

    /// Stencil reference node for the object
    StencilRefNode* stencilRefNode_;

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
    /** \brief  This function is called to update the object
    *
    * If the object changes, the core will call this function. Normally this will update
    * the corresponding scenegraph nodes or trigger other data handling which has to be done
    * when the object changes.
    *
    * \note Do not call this function yourself to avoid unnecessary overhead(the core will call it when it is required)
    */
    virtual void update(UpdateType _type = UPDATE_ALL );

  /** @} */

  //===========================================================================
  /** @name Additional nodes
   *
   *  \anchor BaseObjectData_AdditionalNodes_header Functions to attach additional Nodes to objects
   *
   *  Manage additional scenegraph nodes that belong to an object.
   *
   * @{ */
  //===========================================================================
  public:
    /** \brief add an additional node to the object
    *
    * This function can be used to store an additional Scenegraph node. If you add nodes there, you do not
    * need to keep track of deleted, added objects, as this will be done in the main application.
    * You have to create the node yourself as this function does not know the type. You should add the
    * new node below the manipulatorNode ( BaseObjectData::manipulatorNode() ) if you want that it moves with the rest of the data. Otherwise
    * add it below the baseNode ( BaseObjectData::baseNode() of the object.
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
    * If additional nodes are stored for this object, such a node can be removed using this function.
    * If this node has children, they will be removed from the scenegraph as well (and their
    * memory is freed).
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
