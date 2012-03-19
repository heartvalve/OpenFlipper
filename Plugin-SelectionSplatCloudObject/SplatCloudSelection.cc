//================================================================
//
//  SplatCloudSelection - IMPLEMENTATION
//
//================================================================


//== INCLUDES =================================================================


#include "SplatCloudSelection.hh"

#include <stack>


//== NAMESPACES ===============================================================


namespace SplatCloudSelection {


//== IMPLEMENTATION ==========================================================


//=========================================================
//== Vertex Selection =====================================
//=========================================================

void selectVertices( SplatCloud *_splatCloud, std::vector<int> &_vertices )
{
	int n_vertices = (int) _splatCloud->numPoints();

	unsigned int i;
	for( i=0; i<_vertices.size(); ++i )
	{
		const int &v = _vertices[i];
		if( (v >= 0) && (v < n_vertices) )
			_splatCloud->selections()[ v ] = true;
	}
}


//----------------------------------------------------------------


void unselectVertices( SplatCloud *_splatCloud, std::vector<int> &_vertices )
{
	int n_vertices = (int) _splatCloud->numPoints();

	unsigned int i;
	for( i=0; i<_vertices.size(); ++i )
	{
		const int &v = _vertices[i];
		if( (v >= 0) && (v < n_vertices) )
			_splatCloud->selections()[ v ] = false;
	}
}


//----------------------------------------------------------------


void selectAllVertices( SplatCloud *_splatCloud )
{
	_splatCloud->setSelections( true );
}


//----------------------------------------------------------------


void clearVertexSelection( SplatCloud *_splatCloud )
{
	_splatCloud->setSelections( false );
}


//----------------------------------------------------------------


void invertVertexSelection( SplatCloud *_splatCloud )
{
	_splatCloud->invertSelections();
}


//----------------------------------------------------------------


std::vector<int> getVertexSelection( SplatCloud *_splatCloud )
{
	std::vector<int> selection;

	unsigned int i, num = _splatCloud->numPoints();
	for( i=0; i<num; ++i )
	{
		if( _splatCloud->selections()[ i ] )
			selection.push_back( i );
	}

	return selection;
}


//----------------------------------------------------------------


std::vector<int> getVertexSelection( SplatCloud *_splatCloud, bool &_invert)
{
	unsigned int numSelected = 0;

	unsigned int i, num = _splatCloud->numPoints();
	for( i=0; i<num; ++i )
	{
		if( _splatCloud->selections()[ i ] )
			++numSelected;
	}

	_invert = (numSelected > _splatCloud->numPoints() / 2);

	std::vector<int> selection;

	for( i=0; i<num; ++i )
	{
		if( _splatCloud->selections()[ i ] ^ _invert )
			selection.push_back( i );
	}

	return selection;
}


//=============================================================================


} // namespace SplatCloudSelection


//=============================================================================
