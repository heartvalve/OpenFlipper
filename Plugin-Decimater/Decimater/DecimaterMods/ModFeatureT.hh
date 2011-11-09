//=============================================================================
//
//  CLASS ModFeatureT
//
//=============================================================================


#ifndef MB_MODFEATURET_HH
#define MB_MODFEATURET_HH


//== INCLUDES =================================================================

#include <mb_base/mb_base.hh>
#include <OpenMesh/Tools/Decimater/ModBaseT.hh>
#include <OpenMesh/Core/Utils/Property.hh>


//== NAMESPACES ===============================================================

namespace OpenMesh  {
namespace Decimater {


//== CLASS DEFINITION =========================================================



template <class DecimaterT>
class ModFeatureT : public ModBaseT< DecimaterT >
{
public:
   
  DECIMATING_MODULE( ModFeatureT, DecimaterT, Feature );



public:

  /// Constructor
  ModFeatureT(DecimaterT& _dec) : Base(_dec, true)
  {
    Base::mesh().add_property(feature_IDs_);
  }
 

  /// Destructor
  ~ModFeatureT() 
  {
    Base::mesh().remove_property(feature_IDs_);
  }


  int& feature_ID(typename Mesh::VertexHandle _vh)
  {
    return Base::mesh().property(feature_IDs_, _vh);
  }


  float collapse_priority(const CollapseInfo& _ci) 
  {
    int f0 = feature_ID(_ci.v0);
    if (f0)
    {
      int f1 = feature_ID(_ci.v1);
      return (f0 == f1 ? 0.0 : -1.0);
    }
    return true;
  }



private:

  OpenMesh::VPropHandleT<int>  feature_IDs_;
};


//=============================================================================
}
}
//=============================================================================
#endif // MB_MODFEATURET_HH defined
//=============================================================================

