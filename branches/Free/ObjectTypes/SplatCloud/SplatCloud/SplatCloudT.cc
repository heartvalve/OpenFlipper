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
bool SplatCloud::requestSplatProperty( const SplatCloud::PropertyHandleT<T> &_handle, SplatCloud::SplatPropertyT<T> *(&_property) )
{
  // try to find property map entry
  SplatPropertyMap::iterator iter = splatProperties_.find( _handle );

  // check if a property with the same name is already present
  if( iter != splatProperties_.end() )
  {
    // try to cast
    _property = dynamic_cast<SplatPropertyT<T> *>( iter->second.property_ );

    // check if cast failed and property in map has different type. if so, return failure (_property is 0)
    if( _property == 0 )
      return false;

    // increase number of requests and return success (_property is a valid pointer)
    ++iter->second.numRequests_;
    return true;
  }

  // create new property
  _property = new SplatPropertyT<T>( _handle, numSplats_ );

  // if out of memory, abort and return failure (_property is 0)
  if( _property == 0 )
    return false;

  // if creation went wrong, free memory and return failure (_property will be set to 0)
  if( _property->data_.size() != numSplats_ )
  {
    delete _property;
    _property = 0;
    return false;
  }

  // insert new property map entry and return success (_property is a valid pointer)
  splatProperties_[ _handle ] = SplatPropertyMapEntry( _property, 1 );
  return true;
}


//----------------------------------------------------------------


template <typename T>
bool SplatCloud::requestCloudProperty( const SplatCloud::PropertyHandleT<T> &_handle, SplatCloud::CloudPropertyT<T> *(&_property) )
{
  // try to find property map entry
  CloudPropertyMap::iterator iter = cloudProperties_.find( _handle );

  // check if a property with the same name is already present
  if( iter != cloudProperties_.end() )
  {
    // try to cast
    _property = dynamic_cast<CloudPropertyT<T> *>( iter->second.property_ );

    // check if cast failed and property in map has different type. if so, return failure (_property is 0)
    if( _property == 0 )
      return false;

    // increase number of requests and return success (_property is a valid pointer)
    ++iter->second.numRequests_;
    return true;
  }

  // create new property
  _property = new CloudPropertyT<T>( _handle );

  // if out of memory, abort and return failure (_property is 0)
  if( _property == 0 )
    return false;

  // insert new property map entry and return success (_property is a valid pointer)
  cloudProperties_[ _handle ] = CloudPropertyMapEntry( _property, 1 );
  return true;
}


//----------------------------------------------------------------


template <typename T>
bool SplatCloud::releaseSplatProperty( const SplatCloud::PropertyHandleT<T> &_handle, SplatCloud::SplatPropertyT<T> *(&_property) )
{
  // try to find property map entry
  SplatPropertyMap::iterator iter = splatProperties_.find( _handle );

  // set _property to 0 if *not* found or cast fails, otherwise to a valid pointer
  _property = (iter == splatProperties_.end()) ? 0 : dynamic_cast<SplatPropertyT<T> *>( iter->second.property_ );

  // if a property with the same name but a different type is present, do *not* remove any property

  // if *not* a valid pointer, abort and return failure (_property is 0)
  if( _property == 0 )
    return false;

  // decrease number of request
  --iter->second.numRequests_;

  // check if property should *not* be removed yet. if so, return success (_property is a valid pointer)
  if( iter->second.numRequests_ != 0 )
    return true;

  // free memory of property and reset pointer
  delete _property;
  _property = 0;

  // remove property map entry and return false because _property is 0
  splatProperties_.erase( iter );
  return false;
}


//----------------------------------------------------------------


template <typename T>
bool SplatCloud::releaseCloudProperty( const SplatCloud::PropertyHandleT<T> &_handle, SplatCloud::CloudPropertyT<T> *(&_property) )
{
  // try to find property map entry
  CloudPropertyMap::iterator iter = cloudProperties_.find( _handle );

  // set _property to 0 if *not* found or cast fails, otherwise to a valid pointer
  _property = (iter == cloudProperties_.end()) ? 0 : dynamic_cast<CloudPropertyT<T> *>( iter->second.property_ );

  // if a property with the same name but a different type is present, do *not* remove any property

  // if *not* a valid pointer, abort and return failure (_property is 0)
  if( _property == 0 )
    return false;

  // decrease number of request
  --iter->second.numRequests_;

  // check if property should *not* be removed yet. if so, return success (_property is a valid pointer)
  if( iter->second.numRequests_ != 0 )
    return true;

  // free memory of property and reset pointer
  delete _property;
  _property = 0;

  // remove property map entry and return false because _property is 0
  cloudProperties_.erase( iter );
  return false;
}


//----------------------------------------------------------------


template <typename T>
void SplatCloud::getSplatProperty( const SplatCloud::PropertyHandleT<T> &_handle, SplatCloud::SplatPropertyT<T> *(&_property) )
{
  // try to find property map entry
  SplatPropertyMap::const_iterator iter = splatProperties_.find( _handle );

  // set _property to 0 if *not* found or cast fails, otherwise to a valid pointer
  _property = (iter == splatProperties_.end()) ? 0 : dynamic_cast<SplatPropertyT<T> *>( iter->second.property_ );
}


//----------------------------------------------------------------


template <typename T>
void SplatCloud::getSplatProperty( const SplatCloud::PropertyHandleT<T> &_handle, const SplatCloud::SplatPropertyT<T> *(&_property) ) const
{
  // try to find property map entry
  SplatPropertyMap::const_iterator iter = splatProperties_.find( _handle );

  // set _property to 0 if *not* found or cast fails, otherwise to a valid pointer
  _property = (iter == splatProperties_.end()) ? 0 : dynamic_cast<const SplatPropertyT<T> *>( iter->second.property_ );
}


//----------------------------------------------------------------


template <typename T>
void SplatCloud::getCloudProperty( const SplatCloud::PropertyHandleT<T> &_handle, SplatCloud::CloudPropertyT<T> *(&_property) )
{
  // try to find property map entry
  CloudPropertyMap::const_iterator iter = cloudProperties_.find( _handle );

  // set _property to 0 if *not* found or cast fails, otherwise to a valid pointer
  _property = (iter == cloudProperties_.end()) ? 0 : dynamic_cast<CloudPropertyT<T> *>( iter->second.property_ );
}


//----------------------------------------------------------------


template <typename T>
void SplatCloud::getCloudProperty( const SplatCloud::PropertyHandleT<T> &_handle, const SplatCloud::CloudPropertyT<T> *(&_property) ) const
{
  // try to find property map entry
  CloudPropertyMap::const_iterator iter = cloudProperties_.find( _handle );

  // set _property to 0 if *not* found or cast fails, otherwise to a valid pointer
  _property = (iter == cloudProperties_.end()) ? 0 : dynamic_cast<const CloudPropertyT<T> *>( iter->second.property_ );
}
