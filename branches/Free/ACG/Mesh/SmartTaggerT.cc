//=============================================================================
//
//  CLASS SmartTaggerT - IMPLEMENTATION
//
//=============================================================================

#define ACG_SMARTTAGGERT_C

//== INCLUDES =================================================================

#include "SmartTaggerT.hh"

#include <iostream>
#include <limits>

//== NAMESPACES ===============================================================

namespace ACG {

//== IMPLEMENTATION ==========================================================

template <class Mesh, class EHandle, class EPHandle>
SmartTaggerT<Mesh, EHandle, EPHandle>::
SmartTaggerT(Mesh& _mesh, unsigned int _tag_range)
  : mesh_(_mesh),
    current_base_(0),
    tag_range_(_tag_range)
{
  // add new property
  mesh_.add_property(ep_tag_);
  
  // reset all tags once
  all_tags_to_zero();
}


//-----------------------------------------------------------------------------

 
template <class Mesh, class EHandle, class EPHandle>
SmartTaggerT<Mesh, EHandle, EPHandle>::
~SmartTaggerT()
{
  mesh_.remove_property(ep_tag_);
}


//-----------------------------------------------------------------------------


template <class Mesh, class EHandle, class EPHandle>
void
SmartTaggerT<Mesh, EHandle, EPHandle>::
untag_all()
{
  unsigned int max_uint = std::numeric_limits<unsigned int>::max();

  if( current_base_ < max_uint - 2*tag_range_)
    current_base_ += tag_range_;
  else
  {
    //overflow -> reset all tags
#ifdef STV_DEBUG_CHECKS
    std::cerr << "Tagging Overflow occured...\n";
#endif
    current_base_ = 0;
    all_tags_to_zero();
  }
}


//-----------------------------------------------------------------------------


template <class Mesh, class EHandle, class EPHandle>
void
SmartTaggerT<Mesh, EHandle, EPHandle>::
untag_all( const unsigned int _new_tag_range)
{
  set_tag_range(_new_tag_range);
}


//-----------------------------------------------------------------------------
  
template <class Mesh, class EHandle, class EPHandle>
void
SmartTaggerT<Mesh, EHandle, EPHandle>::
set_tag  ( const EHandle _eh, unsigned int _tag)
{
#ifdef STV_DEBUG_CHECKS
  if( _tag > tag_range_)
    std::cerr << "ERROR in set_tag tag range!!!\n";
#endif

  mesh_.property(ep_tag_, _eh) = current_base_ + _tag;
}


//-----------------------------------------------------------------------------


template <class Mesh, class EHandle, class EPHandle>
unsigned int
SmartTaggerT<Mesh, EHandle, EPHandle>::
get_tag  ( const EHandle _eh) const
{
  unsigned int t = mesh_.property(ep_tag_, _eh);

#ifdef STV_DEBUG_CHECKS
  if( t > current_base_ + tag_range_)
    std::cerr << "ERROR in get_tag tag range!!!\n";
#endif

  if( t<= current_base_) return 0;
  else                   return t-current_base_;
}


//-----------------------------------------------------------------------------


template <class Mesh, class EHandle, class EPHandle>
bool
SmartTaggerT<Mesh, EHandle, EPHandle>::
is_tagged( const EHandle _eh) const
{
  return bool(get_tag(_eh));
}


//-----------------------------------------------------------------------------


template <class Mesh, class EHandle, class EPHandle>
void
SmartTaggerT<Mesh, EHandle, EPHandle>::
set_tag_range( const unsigned int _tag_range)
{
  if( _tag_range <= tag_range_)
  {
    untag_all();
    tag_range_ = _tag_range;
  }
  else
  {
    tag_range_ = _tag_range;
    untag_all();
  }
}

  
//-----------------------------------------------------------------------------


template <class Mesh, class EHandle, class EPHandle>
void
SmartTaggerT<Mesh, EHandle, EPHandle>::
all_tags_to_zero()
{
  // iterate over property vector
  for(unsigned int i=0; i<mesh_.property(ep_tag_).n_elements(); ++i)
  {
    mesh_.property(ep_tag_)[i] = 0;
  }
}

//=============================================================================
} // namespace ACG
//=============================================================================
