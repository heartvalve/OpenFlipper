//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file PlaneObject.hh 
 * This File contains the PolyLine Object
 */


#ifndef PLANEOBJECT_HH
#define PLANEOBJECT_HH

 

//== INCLUDES =================================================================

#include <OpenFlipper/common/BaseObjectData.hh>       

#include <OpenFlipper/common/GlobalDefines.hh>

#include "PlaneTypes.hh"

//== TYPEDEFS =================================================================

// //== CLASS DEFINITION =========================================================

class DLLEXPORT PlaneObject : public BaseObjectData {

  public: 
    /// constructor
    PlaneObject(SeparatorNode* _rootNode);
    
    /** \brief copy constructor
     *
     *  Create a copy of this object
     */
    PlaneObject(const PlaneObject& _object);

    /// destructor
    virtual ~PlaneObject();
    
    /// Reset current object, including all related nodes.
    virtual void cleanup();   

    /** return a full copy of this object ( All scenegraph nodes will be created )
     *  but the object will not be a part of the object tree.
     */
    BaseObject* copy();


  protected:
    /// Initialise current object, including all related nodes.
    virtual void init(PlaneNode* _plane = 0);

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
    PlaneNode* planeNode();
    
  private:
    PlaneNode* planeNode_;

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
#endif // PLANEOBJECT_HH defined
//=============================================================================
