/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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
//  CLASS SplatCloud
//
//    SplatCloud stores the data for a SplatCloud.
//    Points, normals, point sizes, colors, indices and selections are internally stored as arrays.
//
//================================================================


#ifndef SPLATCLOUD_HH
#define SPLATCLOUD_HH


//== INCLUDES ====================================================


#include <OpenFlipper/common/GlobalDefines.hh>

#include <ACG/Math/VectorT.hh>

#include <vector>


//== CLASS DEFINITION ============================================


/** \class SplatCloud SplatCloud.hh <ObjectTypes/SplatCloud/SplatCloud/SplatCloud.hh>
 *
 * Storage of points, normals, point sizes, colors, indices and selections for a SplatCloud.
 */

class DLLEXPORT SplatCloud
{
public:

	//-- TYPEDEFS ----------------------------------------------------

	typedef ACG::Vec3f  Point;
	typedef ACG::Vec3f  Normal;
	typedef float       Pointsize;
	typedef ACG::Vec3uc Color;
	typedef int         Index;
	typedef bool        Selection;

	typedef std::vector<Point>     PointVector;
	typedef std::vector<Normal>    NormalVector;
	typedef std::vector<Pointsize> PointsizeVector;
	typedef std::vector<Color>     ColorVector;
	typedef std::vector<Index>     IndexVector;
	typedef std::vector<Selection> SelectionVector;

	//----------------------------------------------------------------

	/// default constructor
	inline SplatCloud() : translation_( Point(0.0f,0.0f,0.0f) ), scaleFactor_( 1.0f ) { }

	/// copy constructor
	inline SplatCloud( const SplatCloud &_splatCloud ) : 
		points_     ( _splatCloud.points_      ), 
		normals_    ( _splatCloud.normals_     ), 
		pointsizes_ ( _splatCloud.pointsizes_  ), 
		colors_     ( _splatCloud.colors_      ), 
		indices_    ( _splatCloud.indices_     ), 
		selections_ ( _splatCloud.selections_  ), 
		translation_( _splatCloud.translation_ ), 
		scaleFactor_( _splatCloud.scaleFactor_ ) 
	{ }

	// ---- data vectors ----

	inline void clearPoints()     { PointVector     temp; points_.swap    ( temp ); }
	inline void clearNormals()    { NormalVector    temp; normals_.swap   ( temp ); }
	inline void clearPointsizes() { PointsizeVector temp; pointsizes_.swap( temp ); }
	inline void clearColors()     { ColorVector     temp; colors_.swap    ( temp ); }
	inline void clearIndices()    { IndexVector     temp; indices_.swap   ( temp ); }
	inline void clearSelections() { SelectionVector temp; selections_.swap( temp ); }

	inline void clear() { clearPoints(); clearNormals(); clearPointsizes(); clearColors(); clearIndices(); clearSelections(); }

	inline void addPoint    ( const Point     &_point     ) { points_.push_back    ( _point     ); }
	inline void addNormal   ( const Normal    &_normal    ) { normals_.push_back   ( _normal    ); }
	inline void addPointsize( const Pointsize &_pointsize ) { pointsizes_.push_back( _pointsize ); }
	inline void addColor    ( const Color     &_color     ) { colors_.push_back    ( _color     ); }
	inline void addIndex    ( const Index     &_index     ) { indices_.push_back   ( _index     ); }
	inline void addSelection( const Selection &_selection ) { selections_.push_back( _selection ); }

	inline unsigned int numPoints()     const { return points_.size();     }
	inline unsigned int numNormals()    const { return normals_.size();    }
	inline unsigned int numPointsizes() const { return pointsizes_.size(); }
	inline unsigned int numColors()     const { return colors_.size();     }
	inline unsigned int numIndices()    const { return indices_.size();    }
	inline unsigned int numSelections() const { return selections_.size(); } /// *not* numSelected() ! use countSelected() for this

	inline bool hasPoints()     const { return numPoints() > 0; }
	inline bool hasNormals()    const { return hasPoints() && (numNormals()    == numPoints()); }
	inline bool hasPointsizes() const { return hasPoints() && (numPointsizes() == numPoints()); }
	inline bool hasColors()     const { return hasPoints() && (numColors()     == numPoints()); }
	inline bool hasIndices()    const { return hasPoints() && (numIndices()    == numPoints()); }
	inline bool hasSelections() const { return hasPoints() && (numSelections() == numPoints()); }

	inline void initNormals()    { if( !hasNormals()    ) normals_    = NormalVector   ( numPoints(), Normal(0.0f,0.0f,0.0f) ); }
	inline void initPointsizes() { if( !hasPointsizes() ) pointsizes_ = PointsizeVector( numPoints(), Pointsize(0.0f)        ); }
	inline void initColors()     { if( !hasColors()     ) colors_     = ColorVector    ( numPoints(), Color(0,0,0)           ); }
	inline void initIndices()    { if( !hasIndices()    ) indices_    = IndexVector    ( numPoints(), Index(-1)              ); }
	inline void initSelections() { if( !hasSelections() ) selections_ = SelectionVector( numPoints(), Selection(false)       ); }

	inline       PointVector     &points()           { return points_;     }
	inline       NormalVector    &normals()          { return normals_;    }
	inline       PointsizeVector &pointsizes()       { return pointsizes_; }
	inline       ColorVector     &colors()           { return colors_;     }
	inline       IndexVector     &indices()          { return indices_;    }
	inline       SelectionVector &selections()       { return selections_; }
	inline const PointVector     &points()     const { return points_;     }
	inline const NormalVector    &normals()    const { return normals_;    }
	inline const PointsizeVector &pointsizes() const { return pointsizes_; }
	inline const ColorVector     &colors()     const { return colors_;     }
	inline const IndexVector     &indices()    const { return indices_;    }
	inline const SelectionVector &selections() const { return selections_; }

	unsigned int countSelected() const;

	void setSelections( const Selection &_selection );
	void setSphereSelections( const Point &_center, float _sqRadius, const Selection &_selection );
	void invertSelections();

	bool deleteSelected();
	bool colorizeSelected( const Color &_color );

	// ---- translation and scale ----

	/// move center-of-gravety to origin and normalize overall size of model (points and pointsizes will be modified)
	void normalizeSize();

	inline const Point &translation() const { return translation_; }
	inline float        scaleFactor() const { return scaleFactor_; }

	//----------------------------------------------------------------

private:

	// ---- data vectors ----
	PointVector     points_;
	NormalVector    normals_;
	PointsizeVector pointsizes_;
	ColorVector     colors_;
	IndexVector     indices_;
	SelectionVector selections_;

	// ---- translation and scale ----
	Point translation_;
	float scaleFactor_;

	void translate( const Point &_t );
	void scale    ( float        _s );
};


//================================================================


#endif // SPLATCLOUD_HH
