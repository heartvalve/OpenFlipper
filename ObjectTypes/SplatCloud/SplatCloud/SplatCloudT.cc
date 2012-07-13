//================================================================
//
//  CLASS SplatCloud - TEMPLATE IMPLEMENTATION
//
//================================================================


#define SPLATCLOUDT_CC


//== INCLUDES ====================================================


#include "SplatCloud.hh"


//== IMPLEMENTATION ==============================================


template <typename T>
unsigned int SplatCloud::eraseSplatsByIndex( const T &_indices )
{
  int maxIdx = numSplats_ - 1;

  // create vector of flags indicating which elements to delete (initialized to false)
  std::vector<bool> flags( numSplats_, false );

  // set flags given by indices
  typename T::const_iterator idxIter;
  for( idxIter = _indices.begin(); idxIter != _indices.end(); ++idxIter )
  {
    // convert index to int
    int idx = static_cast<int>( *idxIter );

    // if index is valid, set flag
    if( (idx >= 0) && (idx <= maxIdx) )
      flags[ idx ] = true;
  }

  // delete elements and return number of deleted elements
  return eraseSplatsByFlag( flags );
}


//----------------------------------------------------------------


template <typename T>
unsigned int SplatCloud::eraseSplatsByFlag( const std::vector<T> &_flags )
{
  // check for right size of flags vector
  if( _flags.size() != numSplats_ )
    return 0; // error; no elements have been deleted so return 0

  // create index vector indicating which elements to keep
  std::vector<int> indices;

  // reserve enough memory
  indices.reserve( numSplats_ );

  // fill index vector
  unsigned int i;
  for( i=0; i<numSplats_; ++i )
  {
    if( !_flags[i] )
      indices.push_back( i );
  }

  // calculate number of elements to delete
  unsigned int numDelete = numSplats_ - indices.size();

  // check if something has to be deleted
  if( numDelete != 0 )
  {
    // keep only elements with given indices in data vector of all splat-properties
    SplatPropertyMap::const_iterator splatPropertyIter;
    for( splatPropertyIter = splatProperties_.begin(); splatPropertyIter != splatProperties_.end(); ++splatPropertyIter )
      splatPropertyIter->second.property_->crop( indices );

    // update number of splats
    numSplats_ = indices.size();
  }

  // return number of deleted elements
  return numDelete;
}


//----------------------------------------------------------------


template <typename T>
/*virtual*/ void SplatCloud::SplatPropertyT<T>::crop( const std::vector<int> &_indices )
{
  // create new data vector
  std::vector<T> newData;
  newData.reserve( _indices.size() );

  // fill new data vector by inserting elements of old data vector with given indices
  std::vector<int>::const_iterator idxIter;
  for( idxIter = _indices.begin(); idxIter != _indices.end(); ++idxIter )
    newData.push_back( data_[ *idxIter ] );

  // set old data vector to new one (swap and discard old vector)
  data_.swap( newData );
}


//----------------------------------------------------------------


template <typename T>
SplatCloud::SplatPropertyT<T> *SplatCloud::requestSplatProperty( const SplatCloud::PropertyHandleT<T> &_handle )
{
  // try to find property map entry
  SplatPropertyMap::iterator iter = splatProperties_.find( _handle );

  // check if a property with the same name is already present
  if( iter != splatProperties_.end() )
  {
    // try to cast
    SplatPropertyT<T> *oldProp = dynamic_cast<SplatPropertyT<T> *>( iter->second.property_ );

    // if found property is of the right type, increase number of requests
    if( oldProp != 0 )
      ++iter->second.numRequests_;

    // return pointer to old property
    return oldProp;
  }

  // create new property
  SplatPropertyT<T> *newProp = new SplatPropertyT<T>( _handle, numSplats_ );

  // if creation went wrong, free memory and reset pointer
  if( newProp->data_.size() != numSplats_ )
  {
    delete newProp;
    newProp = 0;
  }

  // if pointer is valid, insert new property map entry
  if( newProp != 0 )
    splatProperties_[ _handle ] = SplatPropertyMapEntry( newProp, 1 );

  // return pointer to new property
  return newProp;
}


//----------------------------------------------------------------


template <typename T>
SplatCloud::CloudPropertyT<T> *SplatCloud::requestCloudProperty( const SplatCloud::PropertyHandleT<T> &_handle )
{
  // try to find property map entry
  CloudPropertyMap::iterator iter = cloudProperties_.find( _handle );

  // check if a property with the same name is already present
  if( iter != cloudProperties_.end() )
  {
    // try to cast
    CloudPropertyT<T> *oldProp = dynamic_cast<CloudPropertyT<T> *>( iter->second.property_ );

    // if found property is of the right type, increase number of requests
    if( oldProp != 0 )
      ++iter->second.numRequests_;

    // return pointer to old property
    return oldProp;
  }

  // create new property
  CloudPropertyT<T> *newProp = new CloudPropertyT<T>( _handle );

  // if pointer is valid, insert new property map entry
  if( newProp != 0 )
    cloudProperties_[ _handle ] = CloudPropertyMapEntry( newProp, 1 );

  // return pointer to new property
  return newProp;
}


//----------------------------------------------------------------


template <typename T>
SplatCloud::SplatPropertyT<T> *SplatCloud::releaseSplatProperty( const SplatCloud::PropertyHandleT<T> &_handle )
{
  // try to find property map entry
  SplatPropertyMap::iterator iter = splatProperties_.find( _handle );

  // if *not* found, abort
  if( iter == splatProperties_.end() )
    return 0;

  // try to cast
  SplatPropertyT<T> *prop = dynamic_cast<SplatPropertyT<T> *>( iter->second.property_ );

  // check if found property is of the right type
  if( prop != 0 )
  {
    // decrease number of request
    --iter->second.numRequests_;

    // check if property should be removed now
    if( iter->second.numRequests_ == 0 )
    {
      // free memory of property and reset pointer
      delete prop;
      prop = 0;

      // remove property map entry
      splatProperties_.erase( iter );
    }
  }

  // return pointer to property
  return prop;
}


//----------------------------------------------------------------


template <typename T>
SplatCloud::CloudPropertyT<T> *SplatCloud::releaseCloudProperty( const SplatCloud::PropertyHandleT<T> &_handle )
{
  // try to find property map entry
  CloudPropertyMap::iterator iter = cloudProperties_.find( _handle );

  // if *not* found, abort
  if( iter == cloudProperties_.end() )
    return 0;

  // try to cast
  CloudPropertyT<T> *prop = dynamic_cast<CloudPropertyT<T> *>( iter->second.property_ );

  // check if found property is of the right type
  if( prop != 0 )
  {
    // decrease number of request
    --iter->second.numRequests_;

    // check if property should be removed now
    if( iter->second.numRequests_ == 0 )
    {
      // free memory of property and reset pointer
      delete prop;
      prop = 0;

      // remove property map entry
      cloudProperties_.erase( iter );
    }
  }

  // return pointer to property
  return prop;
}


//----------------------------------------------------------------


template <typename T>
SplatCloud::SplatPropertyT<T> *SplatCloud::getSplatProperty( const SplatCloud::PropertyHandleT<T> &_handle )
{
  // try to find property map entry
  SplatPropertyMap::const_iterator iter = splatProperties_.find( _handle );

  // if *not* found or cast fails return 0, otherwise return a valid pointer
  return (iter == splatProperties_.end()) ? 0 : dynamic_cast<SplatPropertyT<T> *>( iter->second.property_ );
}


//----------------------------------------------------------------


template <typename T>
const SplatCloud::SplatPropertyT<T> *SplatCloud::getSplatProperty( const SplatCloud::PropertyHandleT<T> &_handle ) const
{
  // try to find property map entry
  SplatPropertyMap::const_iterator iter = splatProperties_.find( _handle );

  // if *not* found or cast fails return 0, otherwise return a valid pointer
  return (iter == splatProperties_.end()) ? 0 : dynamic_cast<const SplatPropertyT<T> *>( iter->second.property_ );
}


//----------------------------------------------------------------


template <typename T>
SplatCloud::CloudPropertyT<T> *SplatCloud::getCloudProperty( const SplatCloud::PropertyHandleT<T> &_handle )
{
  // try to find property map entry
  CloudPropertyMap::const_iterator iter = cloudProperties_.find( _handle );

  // if *not* found or cast fails return 0, otherwise return a valid pointer
  return (iter == cloudProperties_.end()) ? 0 : dynamic_cast<CloudPropertyT<T> *>( iter->second.property_ );
}


//----------------------------------------------------------------


template <typename T>
const SplatCloud::CloudPropertyT<T> *SplatCloud::getCloudProperty( const SplatCloud::PropertyHandleT<T> &_handle ) const
{
  // try to find property map entry
  CloudPropertyMap::const_iterator iter = cloudProperties_.find( _handle );

  // if *not* found or cast fails return 0, otherwise return a valid pointer
  return (iter == cloudProperties_.end()) ? 0 : dynamic_cast<const CloudPropertyT<T> *>( iter->second.property_ );
}
