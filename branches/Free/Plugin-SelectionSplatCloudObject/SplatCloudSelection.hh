//================================================================
//
//  SplatCloudSelection
//
//================================================================


#ifndef SPLATCLOUDSELECTION_HH
#define SPLATCLOUDSELECTION_HH


/*! \file SplatCloudSelection.hh
    \brief Functions for selection on a SplatCloud

*/

//== INCLUDES =================================================================


#include <ObjectTypes/SplatCloud/SplatCloud.hh>

#include <vector>


//== NAMESPACES ===============================================================


namespace SplatCloudSelection {


//== DEFINITIONS ==============================================================


//===========================================================================
/** @name Vertex Selection
* @{ */
//===========================================================================

void             selectVertices       ( SplatCloud *_splatCloud, std::vector<int> &_vertices ); /// Select given vertices of a SplatCloud
void             unselectVertices     ( SplatCloud *_splatCloud, std::vector<int> &_vertices ); /// Unselect given vertices of a SplatCloud
void             selectAllVertices    ( SplatCloud *_splatCloud);                               /// Select all vertices of a SplatCloud
void             clearVertexSelection ( SplatCloud *_splatCloud);                               /// Set all vertices to unselected
void             invertVertexSelection( SplatCloud *_splatCloud);                               /// Invert vertex selection
std::vector<int> getVertexSelection   ( SplatCloud *_splatCloud);                               /// Get the current vertex selection
std::vector<int> getVertexSelection   ( SplatCloud *_splatCloud, bool &_invert);                /// Get the current vertex selection (@param _invert if true : vector has been inverted to save mem)

/** @} */


//=============================================================================


} // namespace SplatCloudSelection


//=============================================================================


#endif // SPLATCLOUDSELECTION_HH
