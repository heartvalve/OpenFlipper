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
 * \file SkeletonObject.hh
 * This File contains the Skeleton Object
 */


#ifndef SKELETONOBJECT_HH
#define SKELETONOBJECT_HH


//== INCLUDES =================================================================

#include "SkeletonTypes.hh"
#include <OpenFlipper/common/BaseObjectData.hh>
#include <OpenFlipper/common/GlobalDefines.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include "SkeletonNodeT.hh"
#include "SkeletonTypes.hh"

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** This class provides the functionality for all kind of meshes for the framework
 */
class DLLEXPORT SkeletonObject : public BaseObjectData {
  public:
    /// Constructor
    SkeletonObject();
    /// Copy Constructor
    SkeletonObject(const SkeletonObject &_other);
    /// Destructor
    virtual ~SkeletonObject();
    /// Reset current object, including all related nodes.
    virtual void cleanup();

    virtual BaseObject* copy();

  protected:
    /// Initialize current object, including all related nodes.
    virtual void init();
    void init(Skeleton* _pSkeleton);

  /**
   * @name Name and Path handling
   */
  //@{
  public:
    /// Set the name of the Object
    void setName( QString _name );
  //@}

  /**
   * @name Content
   */
  //@{
  public:
    /// Returns a pointer to the skeleton
    Skeleton* skeleton();

  private:
    /// A pointer to the skeleton data
    Skeleton* skeleton_;
  //@}

  /**
   * @name Visualization
   */
  //@{
  public:
    /// Returns the skeleton scenegraph node
    ACG::SceneGraph::SkeletonNodeT< Skeleton >* skeletonNode();

  private:
    /// A pointer to the scenegraph node
    ACG::SceneGraph::SkeletonNodeT< Skeleton >* skeletonNode_;
  //@}

  /**
   * @name Object Information
   */
  //@{
  public:
    /// Returns a string holding information on this object
    QString getObjectinfo();
  //@}

  /**
   * @name Picking
   */
  //@{
  public:
    /// Returns true if the picked node given by _node_idx is this objects scenegraph node
    bool picked( uint _node_idx );
    /// Enable or disable picking for this object
    void enablePicking( bool _enable );
    /// Check if picking is enabled for this object
    bool pickingEnabled();
  //@}

  AnimationHandle activePose();
  void setActivePose(const AnimationHandle &_hAni);
  
  void showIndices(bool _bVisible = true);
  bool indicesVisible();

  void updateIndices();
  void updateIndices(const AnimationHandle &_hAni);
  
  void showMotionPath(bool _visible = true);
  bool motionPathVisible();

  void updateMotionPath();
  void updateMotionPath(const AnimationHandle &_hAni);
};

//=============================================================================
#endif // SKELETONOBJECT_HH defined
//=============================================================================
