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

//================================================================
//
//  Types
//
//================================================================


/**
 * \file SplatCloudObject.hh
 * This File contains the SplatCloud Object
 */


#ifndef SPLATCLOUDOBJECT_HH
#define SPLATCLOUDOBJECT_HH


//== INCLUDES ====================================================


#include "SplatCloudTypes.hh"

#include "SplatCloud/SplatCloud.hh"

#include <OpenFlipper/common/BaseObjectData.hh>

#include <OpenFlipper/common/GlobalDefines.hh>


//== CLASS DEFINITION ============================================


class DLLEXPORT SplatCloudObject : public BaseObjectData
{
public:
  /// Constructor
  SplatCloudObject();

  /** \brief Copy constructor
   *
   *  Create a copy of this Object
   */
  SplatCloudObject( const SplatCloudObject &_object );

  /// Destructor
  virtual ~SplatCloudObject();

  /// Reset current Object, including all related Nodes
  virtual void cleanup();

  /** Return a full copy of this Object ( all scenegraph Nodes will be created )
   *  but the object will not be a part of the object tree
   */
  BaseObject *copy();

  /** \brief Reload standard and picking Shaders from file
   *
   *  Shader uniforms has to be updated after this!
   */
  void reloadShaders();

  /** \brief Enable or disable backface culling for all Shaders
   */
  void enableBackfaceCulling( bool _enable );

  /** \brief Set the scaling factor for pointsizes for all Shaders
   */
  void setPointsizeScale( float _scale );

  inline bool isBackfaceCullingEnabled() const { return backfaceCullingEnabled_; }
  inline float pointsizeScale() const { return pointsizeScale_; }

private:
  bool  backfaceCullingEnabled_;
  float pointsizeScale_;

protected:
  /// Initialise current Object, including all related Nodes
  virtual void init( const SplatCloud *_splatCloud = 0 );

public:
  /// Called by the core if the object has to be updated
  void update( UpdateType _type = UPDATE_ALL );

//--------------------------------
/** @name Name and Path handling
 * @{ */
//--------------------------------
public:
  /// Set the name of the Object
  void setName( QString _name );

/** @} */

//--------------------------------
/** @name Content
 * @{ */
//--------------------------------
public:
  /// Get SplatCloud
  inline       SplatCloud *splatCloud()       { return splatCloud_; }
  inline const SplatCloud *splatCloud() const { return splatCloud_; }

private:
  SplatCloud *splatCloud_;

/** @} */

//--------------------------------
/** @name Visualization
 * @{ */
//--------------------------------
public:
  /// Get Shader's scenegraph Node
  inline       ShaderNode *shaderNode()       { return shaderNode_; }
  inline const ShaderNode *shaderNode() const { return shaderNode_; }

  /// Get SplatCloud's scenegraph Node
  inline       SplatCloudNode *splatCloudNode()       { return splatCloudNode_; }
  inline const SplatCloudNode *splatCloudNode() const { return splatCloudNode_; }

private:
  ShaderNode     *shaderNode_;
  SplatCloudNode *splatCloudNode_;

/** @} */

//--------------------------------
/** @name Object Information
 * @{ */
//--------------------------------
public:
  /// Get all Info for the Object as a string
  QString getObjectinfo();

/** @} */

//--------------------------------
/** @name Picking
 * @{ */
//--------------------------------
public:
  /// Detect if the node has been picked
  bool picked( uint _node_idx );

  /// Enable or disable picking for this Object
  void enablePicking( bool _enable );

  /// Check if picking is enabled for this Object
  bool pickingEnabled();

/** @} */

};


//================================================================


#endif // SPLATCLOUDOBJECT_HH
