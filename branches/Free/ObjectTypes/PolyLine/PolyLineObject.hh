//=============================================================================
//
//  Types
//
//=============================================================================

/**
 * \file PolyLineObject.hh 
 * This File contains the PolyLine Object
 */


#ifndef POLYLINEOBJECT_HH
#define POLYLINEOBJECT_HH

 

//== INCLUDES =================================================================

#include "PolyLineTypes.hh"
#include <OpenFlipper/common/BaseObjectData.hh>       

#include <ObjectTypes/PolyLine/PolyLineNodeT.hh>

#include <OpenFlipper/common/GlobalDefines.hh>

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

/** This class provides the functionality for all kind of meshes for the framework
 */
class DLLEXPORT PolyLineObject : public BaseObjectData {
  public: 

    typedef PolyLine PolyLineType;

    /// constructor
    PolyLineObject();
    
    /** \brief copy constructor
     *
     *  Create a copy of this object
     */
    PolyLineObject(const PolyLineObject& _object);

    /// destructor
    virtual ~PolyLineObject();
    
	/// Reset current object, including all related nodes.
    virtual void cleanup();   

    /** return a full copy of this object ( All scenegraph nodes will be created )
     *  but the object will not be a part of the object tree.
     */
    BaseObject* copy();

  protected:
    
    /// Initialise current object, including all related nodes.
    virtual void init(PolyLine* _line = 0);
    
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
    /// return a pointer to the line
    PolyLine* line(); 
    
  private:
    PolyLine*           line_;
    
  /** @} */ 
    
  //===========================================================================
  /** @name Visualization
   * @{ */
  //=========================================================================== 
    
  public :    
    
    /// Get the scenegraph Node
    ACG::SceneGraph::PolyLineNodeT< PolyLine >* lineNode();
    
  private :
    
    /// Scenegraph Mesh Node
    ACG::SceneGraph::PolyLineNodeT< PolyLine >*                     lineNode_;
      
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
#endif // POLYLINEOBJECT_HH defined
//=============================================================================
