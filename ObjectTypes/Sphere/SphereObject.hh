//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file SphereObject.hh 
 * This File contains the Sphere Object
 */


#ifndef SPHEREOBJECT_HH
#define SPHEREOBJECT_HH

 

//== INCLUDES =================================================================

#include <OpenFlipper/common/BaseObjectData.hh>       

#include <OpenFlipper/common/GlobalDefines.hh>

#include "SphereTypes.hh"

//== TYPEDEFS =================================================================

// //== CLASS DEFINITION =========================================================

class DLLEXPORT SphereObject : public BaseObjectData {

  public: 
    /// constructor
    SphereObject();
    
    /** \brief copy constructor
     *
     *  Create a copy of this object
     */
    SphereObject(const SphereObject& _object);

    /// destructor
    virtual ~SphereObject();
    
    /// Reset current object, including all related nodes.
    virtual void cleanup();   

    /** return a full copy of this object ( All scenegraph nodes will be created )
     *  but the object will not be a part of the object tree.
     */
    BaseObject* copy();


  protected:
    /// Initialise current object, including all related nodes.
    virtual void init(SphereNode* _sphere = 0);

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
    SphereNode* sphereNode();
    
  private:
    SphereNode* sphereNode_;

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
#endif // SPHEREOBJECT_HH defined
//=============================================================================
