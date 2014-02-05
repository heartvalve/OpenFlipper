/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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

#ifndef SKELETONOBJECTDATA_HH
#define SKELETONOBJECTDATA_HH

#include "OpenFlipper/common/perObjectData.hh"

#define OBJECTDATA_SKELETON "Skeleton Object Data"

/**
 * @brief Data object attached to the skeleton
 *
 * It stores the object ids of skins attached to this skeleton
 *
 */
class SkeletonObjectData : public PerObjectData
{
public:
  SkeletonObjectData(){
  }

  SkeletonObjectData(const SkeletonObjectData& _copy) :
    PerObjectData(),
    skins_(_copy.skins_)
  {
  }

  ~SkeletonObjectData(){
  }

  // Copy function
  PerObjectData* copyPerObjectData() {
    SkeletonObjectData* copy = new SkeletonObjectData(*this);
    return copy;
  }

public:
  /**
   *  \brief Get the skin with given index (0 <= _index < skinCount())
   */ 
  int skin( unsigned int _index ){
    if ( _index >= skins_.size() )
      return -1;
    return skins_[_index];
  }

  /**
   *  \brief Get the number of associated skins
   */ 
  unsigned int skinCount(){
    return skins_.size();
  }

  /**
   *  \brief Add a skin to the skeleton
   */ 
  void addSkin(int _objectId){
    //check if already available
    for(unsigned int i=0; i < skins_.size(); i++)
      if ( skins_[i] == _objectId )
        return;
    //add the skin
    skins_.push_back(_objectId);
  }

  /**
   *  \brief Remove a skin from the skeleton
   */ 
  void removeSkin(int _objectId){
    //check if already available
    for(unsigned int i=0; i < skins_.size(); i++)
      if ( skins_[i] == _objectId ){
        skins_.erase( skins_.begin() + i );
        return;
      }
    //not found
    std::cerr << "Cannot remove skin with object id:" << _objectId << ". Not found!" << std::endl;
  }

  /**
   *  \brief Remove all skins from the skeleton
   */ 
  void clearSkins(){
    skins_.clear();
  }

private:
  //vector of object ids from all skins attached to this skeleton
  std::vector< int > skins_;
};

#endif //SKELETONOBJECTDATA_HH
