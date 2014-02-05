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

//================================================================
//
//  CLASS SplatCloud - IMPLEMENTATION
//
//================================================================


//== INCLUDES ====================================================


#include "SplatCloud.hh"

#include <iostream>


//== STATIC MEMBERS ==============================================


const SplatCloud::PositionsHandle  SplatCloud::POSITIONS_HANDLE ( "<Positions>"  );
const SplatCloud::ColorsHandle     SplatCloud::COLORS_HANDLE    ( "<Colors>"     );
const SplatCloud::NormalsHandle    SplatCloud::NORMALS_HANDLE   ( "<Normals>"    );
const SplatCloud::PointsizesHandle SplatCloud::POINTSIZES_HANDLE( "<Pointsizes>" );
const SplatCloud::IndicesHandle    SplatCloud::INDICES_HANDLE   ( "<Indices>"    );
const SplatCloud::ViewlistsHandle  SplatCloud::VIEWLISTS_HANDLE ( "<Viewlists>"  );
const SplatCloud::SelectionsHandle SplatCloud::SELECTIONS_HANDLE( "<Selections>" );


//== IMPLEMENTATION ==============================================


void SplatCloud::copySplatProperties( const SplatCloud &_splatCloud )
{
  // deep copy all splat-properties
  SplatPropertyMap::const_iterator splatPropertyIter;
  for( splatPropertyIter = _splatCloud.splatProperties_.begin(); splatPropertyIter != _splatCloud.splatProperties_.end(); ++splatPropertyIter )
  {
    // create new deep copy of current splat-property
    SplatPropertyInterface *prop = splatPropertyIter->second.property_->clone();

    // check if out of memory
    if( prop == 0 )
    {
      std::cerr << "Out of memory for a copy of SplatCloud's splat-property \"" << splatPropertyIter->first << "\"." << std::endl;
      continue;
    }

    // insert new property map entry into splat-property map with same name as before
    splatProperties_[ splatPropertyIter->first ] = SplatPropertyMapEntry( prop, splatPropertyIter->second.numRequests_ );
  }

  // Get pointers to predefined splat-properties.
  // These can *not* be copied because they have to point to the newly
  // created deep copies of the properties and not to the old properties.
  getPredefinedSplatPropertyPointers();
}


//----------------------------------------------------------------


void SplatCloud::copyCloudProperties( const SplatCloud &_splatCloud )
{
  // deep copy all cloud-properties
  CloudPropertyMap::const_iterator cloudPropertyIter;
  for( cloudPropertyIter = _splatCloud.cloudProperties_.begin(); cloudPropertyIter != _splatCloud.cloudProperties_.end(); ++cloudPropertyIter )
  {
    // create new deep copy of current cloud-property
    CloudPropertyInterface *prop = cloudPropertyIter->second.property_->clone();

    // check if out of memory
    if( prop == 0 )
    {
      std::cerr << "Out of memory for a copy of SplatCloud's cloud-property \"" << cloudPropertyIter->first << "\"." << std::endl;
      continue;
    }

    // insert new property map entry into cloud-property map with same name as before
    cloudProperties_[ cloudPropertyIter->first ] = CloudPropertyMapEntry( prop, cloudPropertyIter->second.numRequests_ );
  }

  // Get pointers to predefined cloud-properties.
  // These can *not* be copied because they have to point to the newly
  // created deep copies of the properties and not to the old properties.
  getPredefinedCloudPropertyPointers();
}


//----------------------------------------------------------------


SplatCloud::SplatCloud( const SplatCloud &_splatCloud )
{
  // copy number of splats
  numSplats_ = _splatCloud.numSplats_;

  // deep copy all properties
  copySplatProperties( _splatCloud );
  copyCloudProperties( _splatCloud );
}


//----------------------------------------------------------------


void SplatCloud::clearSplatProperties()
{
  // free memory of all splat-properties
  SplatPropertyMap::const_iterator splatPropertyIter;
  for( splatPropertyIter = splatProperties_.begin(); splatPropertyIter != splatProperties_.end(); ++splatPropertyIter )
    delete splatPropertyIter->second.property_;

  // clear splat-property map
  SplatPropertyMap().swap( splatProperties_ );

  // reset pointers to predefined splat-properties
  resetPredefinedSplatPropertyPointers();
}


//----------------------------------------------------------------


void SplatCloud::clearCloudProperties()
{
  // free memory of all cloud-properties
  CloudPropertyMap::const_iterator cloudPropertyIter;
  for( cloudPropertyIter = cloudProperties_.begin(); cloudPropertyIter != cloudProperties_.end(); ++cloudPropertyIter )
    delete cloudPropertyIter->second.property_;

  // clear cloud-property map
  CloudPropertyMap().swap( cloudProperties_ );

  // reset pointers to predefined cloud-properties
  resetPredefinedCloudPropertyPointers();
}


//----------------------------------------------------------------


void SplatCloud::clear()
{
  // reset number of splats
  numSplats_ = 0;

  // clear all properties
  clearSplatProperties();
  clearCloudProperties();
}


//----------------------------------------------------------------


void SplatCloud::swap( SplatCloud &_splatCloud )
{
  // swap number of splats
  std::swap( numSplats_,       _splatCloud.numSplats_       );

  // swap all properties
  std::swap( splatProperties_, _splatCloud.splatProperties_ );
  std::swap( cloudProperties_, _splatCloud.cloudProperties_ );

  // swap pointers to predefined properties
  swapPredefinedSplatPropertyPointers( _splatCloud );
  swapPredefinedCloudPropertyPointers( _splatCloud );
}


//----------------------------------------------------------------


void SplatCloud::clearSplats()
{
  // clear data vector of all splat-properties
  SplatPropertyMap::const_iterator splatPropertyIter;
  for( splatPropertyIter = splatProperties_.begin(); splatPropertyIter != splatProperties_.end(); ++splatPropertyIter )
    splatPropertyIter->second.property_->clear();

  // reset number of splats
  numSplats_ = 0;
}


//----------------------------------------------------------------


void SplatCloud::pushbackSplat()
{
  // add one element at end of data vector of all splat-properties
  SplatPropertyMap::const_iterator splatPropertyIter;
  for( splatPropertyIter = splatProperties_.begin(); splatPropertyIter != splatProperties_.end(); ++splatPropertyIter )
    splatPropertyIter->second.property_->pushback();

  // increase number of splats
  ++numSplats_;
}


//----------------------------------------------------------------


void SplatCloud::resizeSplats( unsigned int _num )
{
  // resize data vector of all splat-properties
  SplatPropertyMap::const_iterator splatPropertyIter;
  for( splatPropertyIter = splatProperties_.begin(); splatPropertyIter != splatProperties_.end(); ++splatPropertyIter )
    splatPropertyIter->second.property_->resize( _num );

  // update number of splats
  numSplats_ = _num;
}
