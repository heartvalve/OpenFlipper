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
// BSplineSurface object type
// Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//=============================================================================

/**
 * \file BSplineSurfaceObject.hh
 * This file contains the BSpline Surface Object
 */


#ifndef BSPLINESURFACEOBJECT_HH
#define BSPLINESURFACEOBJECT_HH

//== INCLUDES =================================================================

#include <ACG/Scenegraph/ShaderNode.hh>
#include <ACG/Scenegraph/EnvMapNode.hh>

#include "BSplineSurfaceTypes.hh"
#include <OpenFlipper/common/BaseObjectData.hh>
#include <OpenFlipper/common/GlobalDefines.hh>


#include "BSplineSurfaceNodeT.hh"


//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** This class provides the functionality for bspline surfaces
 */
class DLLEXPORT BSplineSurfaceObject : public BaseObjectData {

  public:

    /// Constructor
    BSplineSurfaceObject();

    /// copy constructor
    BSplineSurfaceObject(const BSplineSurfaceObject& _object);

    /// Destructor
    virtual ~BSplineSurfaceObject();

    /// Reset current object, including all related nodes.
    virtual void cleanup();

    /** return a full copy of this object ( All scenegraph nodes will be created )
     *  but the object will not be a part of the object tree.
     */
    BaseObject* copy();


  protected:

    /// Initialise current object, including all related nodes.
    virtual void init(BSplineSurface* _surface = 0);


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
    /// return a pointer to the spline curve
    BSplineSurface* splineSurface();

    /// Update the whole Object (Selection,Topology,...)
    virtual void update(UpdateType _type = UPDATE_ALL);
    
  private:
    BSplineSurface* splineSurface_;

  /** @} */

  //===========================================================================
  /** @name Visualization
   * @{ */
  //===========================================================================

  public :

    /// Return pointer to the bspline surface node
    ACG::SceneGraph::BSplineSurfaceNodeT< BSplineSurface > * splineSurfaceNode();

    /// Return pointer to the shader node
    ACG::SceneGraph::ShaderNode * shaderNode();

    /// Get the TextureNode (actually its an environment map node) of the bspline surface
    ACG::SceneGraph::EnvMapNode * textureNode();

  private :

    /// Scenegraph BSplineSurface Node
    ACG::SceneGraph::BSplineSurfaceNodeT< BSplineSurface >* splineSurfaceNode_;

    /// Scenegraph ShaderNode
    ACG::SceneGraph::ShaderNode * shaderNode_;

    /// Scenegraph TextureNode (whoch is actually an environment node)
    ACG::SceneGraph::EnvMapNode * textureNode_;

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

    /// Enable or disable picking for this Object
    void enablePicking( bool _enable );

    /// Check if picking is enabled for this Object
    bool pickingEnabled();

  /** @} */

};

//=============================================================================
#endif // BSPLINESURFACEOBJECT_HH defined
//=============================================================================
