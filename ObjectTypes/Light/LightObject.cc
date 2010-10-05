//=============================================================================
//
//  MyTypes
//
//=============================================================================

#define LIGHTOBJECT_C

//== INCLUDES =================================================================

#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctionsCore.hh>
#include <ACG/QtWidgets/QtSceneGraphWidget.hh>
#include "LightWidget.hh"
#include "Light.hh"

//== DEFINES ==================================================================

//== TYPEDEFS =================================================================

//== CLASS DEFINITION =========================================================

class LightWidgetGenerator : public ACG::QtWidgets::SceneGraphWidgetGenerator {
  public:
  // constructor
  LightWidgetGenerator() {
  }


  virtual QWidget* getWidget(ACG::SceneGraph::BaseNode* _node ) {
    
    // Check if we can handle this type
    if ( ! canHandle( _node->className() ) )
      return 0;
   
    // Check if we already have a widget, otherwise generate a new one.
    if ( widgetMap_.contains(_node) ) {
      return widgetMap_[_node];
    } else {
      QWidget* widget = new LightWidget(_node);
      widgetMap_[_node] = widget;
      return widget;
    }
  
  }

  virtual bool canHandle(std::string _className) {
    return ( _className == std::string("LightNode") );
  }

  virtual std::string handles() {
    return std::string("LightNode");
  }

  virtual QString contextMenuName() {
    return QString("Edit Light");
  }
};

static LightWidgetGenerator lightWidgetGenerator_;

/** Constructor for Light Objects. 
*  You dont need to create an object of this type manually. 
*/
LightObject::LightObject( ) :
  BaseObjectData(),
  lightNode_(0),
  lightNodeVis_(0)
{
  setDataType(DATA_LIGHT);
  setTypeIcon(DATA_LIGHT,"LightType.png");
  init();

  // Make sure, the generator is added to the widget
  PluginFunctions::addSceneGraphGenerator( &lightWidgetGenerator_  );
}

//=============================================================================


/**
 * Copy Constructor - generates a copy of the given object
 */
LightObject::LightObject(const LightObject & _object) :
  BaseObjectData(_object),
  defaultLightSource_(false)
{

    init(_object.lightNode_);

    setName( name() );
}

/** Destructor for Light Objects. The destructor deletes the light and all
*  Scenegraph nodes associated with the Light or the object.
*/
LightObject::~LightObject()
{
  // Delete the data attached to this object ( this will remove all perObject data)
  // Not the best way to do it but it will work.
  // This is only necessary if people use references to the light below and
  // they do something with the light in the destructor of their
  // perObjectData.
  deleteData();

  // Move children to parent
  BaseNode* parent = lightNode_->parent();
  
  // First, collect all children as the iterator will get invalid if we delete while iterating!
  std::vector< BaseNode*> children;
  for (BaseNode::ChildIter cIt=lightNode_->childrenBegin(); cIt!=lightNode_->childrenEnd(); ++cIt) 
    children.push_back( (*cIt) );
  
  // Move the children
  for (unsigned int i = 0 ; i < children.size(); ++i ) 
    children[i]->set_parent(parent);   
  
  // Delete the scenegraph node
  delete lightNode_;
}

/** Cleanup Function for Light Objects. Deletes the contents of the whole object and
* calls LightObject::init afterwards.
*/
void LightObject::cleanup() {

  BaseObjectData::cleanup();

  lightNode_   = NULL;

  setDataType(DATA_LIGHT);
  setTypeIcon(DATA_LIGHT,"LightType.png");

  init();

}

/**
 * Generate a copy
 */
BaseObject* LightObject::copy() {
    LightObject* object = new LightObject(*this);
    return dynamic_cast< BaseObject* >(object);
}

/** This function initalizes the light object. It creates the scenegraph nodes.
*/
void LightObject::init(LightNode* _light, LightNode* _lightVis) {
///\TODO Add Light Node here

// Light nodes have to be on top of all other nodes.

  lightNode_ = new LightNode( 0 , "LightNode");
  lightNode_->visualize(false);
  PluginFunctions::addGlobalStatusNode(lightNode_);
  
  lightNodeVis_ = new LightNode( materialNode() , "LightNode Visualization");
  lightNodeVis_->visualize(true);
  lightNodeVis_->show();

}

// ===============================================================================
// Name/Path Handling
// ===============================================================================

/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
* BaseObjectData::setName.
*/
void LightObject::setName( QString _name ) {
  BaseObjectData::setName(_name);

  std::string nodename = std::string("LightNode for Light "     + _name.toUtf8() );
  lightNode_->name( nodename );
  
  std::string nodenameVis = std::string("LightNode Visualization for Light "     + _name.toUtf8() );
  lightNodeVis_->name( nodenameVis );
}

// ===============================================================================
// Visualization
// ===============================================================================

LightNode* LightObject::lightNode() {
  return lightNode_;
}

LightNode* LightObject::lightNodeVis() {
    return lightNodeVis_;
}

bool LightObject::hasNode(BaseNode* _node) {
 if ( BaseObjectData::hasNode(_node) )
   return true;
 
 if ( _node == lightNode_ )
   return true;
 
 return false;
}

// ===============================================================================
// Object information
// ===============================================================================

/** Returns a string containing all information about the current object. This also
* includes the information provided by BaseObjectData::getObjectinfo
*
* @return String containing the object information
*/
QString LightObject::getObjectinfo() {
  QString output;

  output += "========================================================================\n";
  output += BaseObjectData::getObjectinfo();

  if ( dataType( DATA_LIGHT ) )
    output += "Object Contains Light : ";

  ///\TODO Output info
  ACG::Vec3d pos = lightSource_.position();
//   ACG::Vec3f nor = lightNode_->normal();

  output += " Position ( " + QString::number(pos[0]) + ", " + QString::number(pos[1]) + ", " + QString::number(pos[2]) + ")";
//   output += " Normal ( " + QString::number(nor[0]) + ", " + QString::number(nor[1]) + ", " + QString::number(nor[2]) + ")";

  output += "========================================================================\n";
  return output;
}

// ===============================================================================
// Picking
// ===============================================================================

/** Given an node index from PluginFunctions::scenegraphPick this function can be used to
* check if the lightNode of the object has been picked.
*
* @param _node_idx Index of the picked light node
* @return bool if the lightNode of this object is the picking target.
*/
bool LightObject::picked( uint _node_idx ) {
  return ( _node_idx == lightNode_->id() || _node_idx == lightNodeVis_->id() );
}

void LightObject::enablePicking( bool _enable ) {
  lightNode_->enablePicking( _enable );
}

bool LightObject::pickingEnabled() {
  return lightNode_->pickingEnabled();
}

void LightObject::show() {
  if ( !visible_ ) {
    BaseObjectData::show();
    visible_ = true;
    
    lightNodeVis_->visualize(true);
    
    emit visibilityChanged( id() );
  }
}

void LightObject::hide() {
  if ( visible_ ) {
    BaseObjectData::hide();
    visible_ = false;
    
    emit visibilityChanged( id() );
  }
}

void LightObject::visible(bool _visible) {
  if ( _visible )
    show();
  else 
    hide();
}

bool LightObject::visible() {
  return visible_;
}

LightSource* LightObject::lightSource() {
  return &lightSource_;
}

void LightObject::update(UpdateType _type) {
  
  if(lightSource_.enabled()) {
    lightNode_->setMultipassStatus(ACG::SceneGraph::BaseNode::ALLPASSES);
    lightNode_->set_status( ACG::SceneGraph::BaseNode::Active  );
  } else {
    lightNode_->setMultipassStatus(ACG::SceneGraph::BaseNode::NOPASS);
    lightNode_->set_status( ACG::SceneGraph::BaseNode::HideNode );
  }
  
  lightNode_->setLightSource(lightSource_);
  lightNodeVis_->setLightSource(lightSource_);
}


//=============================================================================

