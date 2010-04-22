//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file LightObject.hh 
 * This File contains the Light Object
 */


#ifndef LIGHTOBJECT_HH
#define LIGHTOBJECT_HH

 

//== INCLUDES =================================================================

#include <OpenFlipper/common/BaseObjectData.hh>       
#include <OpenFlipper/common/GlobalDefines.hh>

#include "LightTypes.hh"

//== TYPEDEFS =================================================================

// //== CLASS DEFINITION =========================================================

class DLLEXPORT LightObject : public BaseObjectData {

  public: 
    /// constructor
    LightObject();
    
    /** \brief copy constructor
     *
     *  Create a copy of this object
     */
    LightObject(const LightObject& _object);

    /// destructor
    virtual ~LightObject();
    
    /// Reset current object, including all related nodes.
    virtual void cleanup();   

    /** return a full copy of this object ( All scenegraph nodes will be created )
     *  but the object will not be a part of the object tree.
     */
    BaseObject* copy();


  protected:
    /// Initialise current object, including all related nodes.
    virtual void init(LightNode* _light = 0, LightNode* _lightVis = 0);

  //===========================================================================
  /** @name Name and Path handling
   * @{ */
  //===========================================================================       
  public:
    
    /// Set the name of the Object
    void setName( QString _name );
    
  /** @} */   
  
  //===========================================================================
  /** @name Contents
   * @{ */
  //=========================================================================== 
  public:
    
    LightSource* lightSource();
    
    /** \brief Update the Light Object
    *
    *   Updates the rendering of the light object
    */
    virtual void update(UpdateType _type = UPDATE_ALL);
    
  private:
    LightSource lightSource_;
    
  /** @} */      
       
  //===========================================================================
  /** @name Visualization
   * @{ */
  //=========================================================================== 

  public:
    /// Get the scenegraph Node
    LightNode* lightNode();
    
    /// Get the scenegraph Node
    LightNode* lightNodeVis();
    
    virtual bool hasNode(BaseNode* _node);
    
  private:
    ///  Light status node
    LightNode* lightNode_;
    
    /// Light rendering node (only for rendering purposes)
    LightNode* lightNodeVis_;

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
    
    /// Show Light Node
    virtual void visible(bool _visible);
    
    /// Show Light Node
    virtual bool visible();
    
    /// Show Light Node
    virtual void show();
    
    /// Hide Light Node
    virtual void hide();
    
};

//=============================================================================
#endif // LIGHTOBJECT_HH defined
//=============================================================================
