#include "DecimaterInfo.hh"

DecimaterInfo::DecimaterInfo(DecimaterToolbarWidget* _tool , TriMesh* _mesh) :
  tool_(_tool),
  decimater_(0),
  normalDeviation_(false),
  distance_(false),
  roundness_(false)
{
  decimater_ = new DecimaterType( *_mesh );
    
  decimater_->add( hModQuadric_priority );
  decimater_->module( hModQuadric_priority ).unset_max_err();
    
  update();    
}


DecimaterInfo::~DecimaterInfo() {
  if ( decimater_ )
    delete decimater_;     
}

//-----------------------------------------------------------------------------------

void DecimaterInfo::update() {
    
  decimater_->module( hModQuadric_priority ).unset_max_err();
    
    //remove old module
    
  if ( normalDeviation_ && !tool_->cbNormalDev->isChecked() ) {
    decimater_->remove( hModNormalFlipping ); 
    normalDeviation_ = false;
  }
  
  if ( distance_ && !tool_->cbDistance->isChecked() ) {
    decimater_->remove( hModQuadric );
    distance_ = false;
  }
    
  if ( roundness_ && !tool_->cbRoundness->isChecked() ) {
    decimater_->remove( hModRoundness );
    roundness_ = false;
  }
    
  //add new module
      
  if ( tool_->cbNormalDev->isChecked() ) {
    
    if ( ! normalDeviation_)
      if ( decimater_->add( hModNormalFlipping ) ) {
        decimater_->module( hModNormalFlipping ).set_normal_deviation( tool_->sbNormalDev->value() );
        normalDeviation_ = true;
      }
  }
    
  if ( tool_->cbDistance->isChecked() ) {
    if ( ! distance_)
      if (  decimater_->add( hModQuadric ) ) {
        decimater_->module( hModQuadric ).set_max_err( tool_->sbDistance->value() );
        distance_ = true;
      }
  }
    
  if ( tool_->cbRoundness->isChecked() ) {
    if ( ! roundness_)
      if ( ! decimater_->add( hModRoundness ) ) {
        decimater_->module( hModRoundness ).set_min_roundness( tool_->roundness->value() , true );
        roundness_ = true;
      }
  }
  
  decimater_->initialize();
    
}

