/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

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