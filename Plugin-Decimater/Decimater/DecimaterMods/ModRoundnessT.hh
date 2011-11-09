//=============================================================================
//
//  CLASS ModRoundnessT
//
//=============================================================================


#ifndef MB_MODROUNDNESST_HH
#define MB_MODROUNDNESST_HH


//== INCLUDES =================================================================

#include <OpenMesh/Tools/Decimater/ModBaseT.hh>
#include <OpenMesh/Core/Utils/Property.hh>


//== NAMESPACES ===============================================================

namespace OpenMesh  {
namespace Decimater {


//== CLASS DEFINITION =========================================================


template <class DecimaterT>
class ModRoundnessT : public ModBaseT<DecimaterT>
{
public:
   
  DECIMATING_MODULE( ModRoundnessT, DecimaterT, Roundness );


  // constructor
  ModRoundnessT(DecimaterT&  _dec, 
		float        _min_roundness = 5.0,
		bool         _is_binary = true)
    : Base(_dec, _is_binary),
      mesh_(Base::mesh()),
      min_roundness_(1.0/_min_roundness)
  {
    mesh_.add_property( roundness_ );
  }


  // destructor
  ~ModRoundnessT()
  {
    mesh_.remove_property( roundness_ );
  }

  

  /// get roundness
  float roundness() const { return 1.0/min_roundness_; }
  /// set roundness
  void set_roundness(float _f) { min_roundness_ = 1.0/_f; }
  
  // precompute face roundness
  void initialize();
  // blabla
  float collapse_priority(const CollapseInfo& _ci);
  // update roundness of one-ring
  void preprocess_collapse(const CollapseInfo& _ci);



private:

  Mesh&  mesh_;
  float  min_roundness_;
  FPropHandleT<float>  roundness_;
};


//=============================================================================
}
}
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(MB_MODROUNDNESST_C)
#define MB_MODROUNDNESST_TEMPLATES
#include "ModRoundnessT.cc"
#endif
//=============================================================================
#endif // MB_MODROUNDNESST_HH defined
//=============================================================================

