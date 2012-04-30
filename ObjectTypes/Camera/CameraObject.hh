//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file CameraObject.hh
 * This File contains the PolyLine Object
 */


#ifndef CAMERAOBJECT_HH
#define CAMERAOBJECT_HH



//== INCLUDES =================================================================

#include <OpenFlipper/common/BaseObjectData.hh>

#include <OpenFlipper/common/GlobalDefines.hh>

#include "CameraTypes.hh"

//== TYPEDEFS =================================================================

// //== CLASS DEFINITION =========================================================

class DLLEXPORT CameraObject : public BaseObjectData {

  public:
    /// constructor
    CameraObject( );

    /** \brief copy constructor
     *
     *  Create a copy of this object
     */
    CameraObject(const CameraObject& _object);

    /// destructor
    virtual ~CameraObject();

    /// Reset current object, including all related nodes.
    virtual void cleanup();

    /** return a full copy of this object ( All scenegraph nodes will be created )
     *  but the object will not be a part of the object tree.
     */
    BaseObject* copy();


  protected:
    /// Initialize current object, including all related nodes.
    virtual void init(CameraNode* _plane = 0);

  //===========================================================================
  /** @name Name and Path handling
   * @{ */
  //===========================================================================
  public:

    /// Set the name of the Object
    void setName( QString _name );

  //===========================================================================
  /** @name Visualization
   * @{ */
  //===========================================================================

  public:
    /// Get the scenegraph Node
    CameraNode* cameraNode();

  private:
    CameraNode* cameraNode_;

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

  public:
      void viewerId(int _viewerId) { viewerId_ = _viewerId; }

      int viewerId() { return viewerId_; }

      void examinerId(int _examinerId) { examinerId_ = _examinerId; }

      int examinerId() { return examinerId_; }


  private:

    /// Attached viewer id
    int viewerId_;

    /// Attached examiner id
    int examinerId_;
};

//=============================================================================
#endif // CAMERAOBJECT_HH defined
//=============================================================================
