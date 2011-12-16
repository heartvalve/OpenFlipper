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
//    Points, normals, point sizes and colors are internally stored as arrays.
//
//================================================================


#ifndef SPLATCLOUD_HH
#define SPLATCLOUD_HH


//== INCLUDES ====================================================


#include <ACG/Math/VectorT.hh>

#include <vector>


//== CLASS DEFINITION ============================================


/** \class SplatCloud SplatCloud.hh <ObjectTypes/SplatCloud/SplatCloud/SplatCloud.hh>
 *
 * Storage of points, normals, point sizes and colors for a SplatCloud.
 */

class SplatCloud
{
public:

	//-- TYPEDEFS ----------------------------------------------------

	typedef ACG::Vec3f  Point;
	typedef ACG::Vec3f  Normal;
	typedef float       Pointsize;
	typedef ACG::Vec3uc Color;

	typedef std::vector<Point>     PointVector;
	typedef std::vector<Normal>    NormalVector;
	typedef std::vector<Pointsize> PointsizeVector;
	typedef std::vector<Color>     ColorVector;

	//----------------------------------------------------------------

	/// default constructor
	inline SplatCloud() : translation_( Point(0.0f,0.0f,0.0f) ), scaleFactor_( 1.0f ) { }

	/// copy constructor
	inline SplatCloud( const SplatCloud &_splatCloud ) : 
		points_     ( _splatCloud.points_      ), 
		normals_    ( _splatCloud.normals_     ), 
		pointsizes_ ( _splatCloud.pointsizes_  ), 
		colors_     ( _splatCloud.colors_      ), 
		translation_( _splatCloud.translation_ ), 
		scaleFactor_( _splatCloud.scaleFactor_ ) 
	{ }

	// ---- data vectors ----

	inline void clearPoints()     { points_.clear();     }
	inline void clearNormals()    { normals_.clear();    }
	inline void clearPointsizes() { pointsizes_.clear(); }
	inline void clearColors()     { colors_.clear();     }

	inline void clear() { clearPoints(); clearNormals(); clearPointsizes(); clearColors(); }

	inline void addPoint    ( const Point     &_point     ) { points_.push_back    ( _point     ); }
	inline void addNormal   ( const Normal    &_normal    ) { normals_.push_back   ( _normal    ); }
	inline void addPointsize( const Pointsize &_pointsize ) { pointsizes_.push_back( _pointsize ); }
	inline void addColor    ( const Color     &_color     ) { colors_.push_back    ( _color     ); }

	inline unsigned int numPoints()     const { return points_.size();     }
	inline unsigned int numNormals()    const { return normals_.size();    }
	inline unsigned int numPointsizes() const { return pointsizes_.size(); }
	inline unsigned int numColors()     const { return colors_.size();     }

	inline bool hasNormals()    const { return normals_.size()    == points_.size(); }
	inline bool hasPointsizes() const { return pointsizes_.size() == points_.size(); }
	inline bool hasColors()     const { return colors_.size()     == points_.size(); }

	inline       PointVector     &points()           { return points_;     }
	inline       NormalVector    &normals()          { return normals_;    }
	inline       PointsizeVector &pointsizes()       { return pointsizes_; }
	inline       ColorVector     &colors()           { return colors_;     }
	inline const PointVector     &points()     const { return points_;     }
	inline const NormalVector    &normals()    const { return normals_;    }
	inline const PointsizeVector &pointsizes() const { return pointsizes_; }
	inline const ColorVector     &colors()     const { return colors_;     }

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

	// ---- translation and scale ----
	Point translation_;
	float scaleFactor_;

	void translate( const Point &_t );
	void scale    ( float        _s );
};


//================================================================


#endif // SPLATCLOUD_HH
