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
