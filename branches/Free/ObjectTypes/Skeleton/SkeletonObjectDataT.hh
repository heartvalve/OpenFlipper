#ifndef SKELETONOBJECTDATAT_HH
#define SKELETONOBJECTDATAT_HH

#include <OpenFlipper/common/Types.hh>
#include "OpenFlipper/common/perObjectData.hh"
#include "OpenFlipper/common/BaseObjectData.hh"

#define OBJECTDATA_SKELETON "Skeleton Object Data"

/**
 * @brief Data object attached to the skeleton
 *
 * It stores the skin attached to this skeleton, by both holding a pointer to the mesh as well as to the
 * object that stores the mesh.
 */
template<typename BaseSkinT>
class SkeletonObjectDataT : public PerObjectData
{
public:
  SkeletonObjectDataT()
  {
    pSkin_ = 0;
    pSkinObjectId_ = -1;
  }

  SkeletonObjectDataT(const SkeletonObjectDataT& _copy)
  {
    pSkin_         = _copy.pSkin_;
    pSkinObjectId_ = _copy.pSkinObjectId_;
  }

  ~SkeletonObjectDataT()
  {
    delete pSkin_;
  }

  // Copy function
  PerObjectData* copyPerObjectData() {
    
    SkeletonObjectDataT* copy = new SkeletonObjectDataT(*this);
    return copy;
  }

public:
  /// The skin object, an instance of SkinT or one of its derivatives
  BaseSkinT *pSkin_;

  /// The id of the mesh object used as skin
  int pSkinObjectId_;
};

#endif //SKELETONOBJECTDATAT_HH