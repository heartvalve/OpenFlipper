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
//  CLASS SplatCloudNode
//
//    SplatCloudNode renders splats by passing points, normals, point sizes and colors to the GL.
//    These elements are internally stored in arrays and rendered using OpenGL vertex, normal,
//    texcoord and color arrays.
//
//================================================================


#ifndef ACG_SPLATCLOUDNODE_HH
#define ACG_SPLATCLOUDNODE_HH


//== INCLUDES ====================================================


#include <OpenFlipper/common/GlobalDefines.hh>

#include <ACG/Scenegraph/BaseNode.hh>

#include <ACG/Scenegraph/DrawModes.hh>

#include <ACG/GL/gl.hh>

#include <vector>


//== NAMESPACES ==================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION ============================================


/** \class SplatCloudNode SplatCloudNode.hh <ObjectTypes/SplatCloud/SplatCloudNode.hh>
 *
 * This class is able to render a SplatCloud as splats, dots or points
 */

class DLLEXPORT SplatCloudNode : public BaseNode
{
public:

	//-- TYPEDEFS ----------------------------------------------------

	typedef ACG::Vec3f	Point;
	typedef ACG::Vec3f	Normal;
	typedef float		Pointsize;
	typedef ACG::Vec3uc	Color;

	typedef std::vector<Point>		PointVector;
	typedef std::vector<Normal>		NormalVector;
	typedef std::vector<Pointsize>	PointsizeVector;
	typedef std::vector<Color>		ColorVector;

	//----------------------------------------------------------------

	/// Constructor
	SplatCloudNode( BaseNode *_parent = 0, std::string _name = "<SplatCloudNode>" ) : 
		BaseNode         ( _parent, _name ), 
		defaultNormal_   ( Normal(0.0f,0.0f,1.0f) ), 
		defaultPointsize_( 0.01f ), 
		defaultColor_    ( Color(255,255,255) ), 
		vboGlId_         ( 0 ), 
		vboValid_        ( false )
	{
		// add (possibly) new drawmodes
		pointsDrawMode_ = DrawModes::addDrawMode( "Points" );
		dotsDrawMode_   = DrawModes::addDrawMode( "Dots"   );
		splatsDrawMode_ = DrawModes::addDrawMode( "Splats" );

		// create a new vertex-buffer-object (will be invalid and rebuilt the next time drawn (or picked))
		createVBO();
	}

	/// Destructor
	~SplatCloudNode() { destroyVBO(); }

	ACG_CLASSNAME( SplatCloudNode );

	/// return available draw modes
	inline DrawModes::DrawMode availableDrawModes() { return splatsDrawMode_ | dotsDrawMode_ | pointsDrawMode_; }

	/// update bounding box
	void boundingBox( ACG::Vec3d &_bbMin, ACG::Vec3d &_bbMax );

	/// draw the SplatCloud
	void draw( GLState &_state, const DrawModes::DrawMode &_drawMode );

	/// picking
	void enterPick( GLState &_state, PickTarget _target, const DrawModes::DrawMode &_drawMode );
	void pick( GLState &_state, PickTarget _target );

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

	// ---- default values ----

	// if an array is not present, changing the default value for this array will make the VBO invalid and trigger a rebuild()
	inline void setDefaultNormal   ( const Normal    &_normal    ) { defaultNormal_    = _normal;    if( !hasNormals()    ) vboValid_ = false; }
	inline void setDefaultPointsize( const Pointsize &_pointsize ) { defaultPointsize_ = _pointsize; if( !hasPointsizes() ) vboValid_ = false; }
	inline void setDefaultColor    ( const Color     &_color     ) { defaultColor_     = _color;     if( !hasColors()     ) vboValid_ = false; }

	inline const Normal    &defaultNormal()    const { return defaultNormal_;    }
	inline const Pointsize &defaultPointsize() const { return defaultPointsize_; }
	inline const Color     &defaultColor()     const { return defaultColor_;     }

	// ---- vertex buffer object ----

	/// make vertex-buffer-object invalid so it will be rebuilt the next time drawn (or picked)
	inline void invalidateVBO() { vboValid_ = false; }

	//----------------------------------------------------------------

private:

	// ---- draw modes ----
	DrawModes::DrawMode	splatsDrawMode_;
	DrawModes::DrawMode	dotsDrawMode_;
	DrawModes::DrawMode	pointsDrawMode_;

	// ---- data vectors ----
	PointVector  	points_;
	NormalVector 	normals_;
	PointsizeVector	pointsizes_;
	ColorVector  	colors_;

	// ---- default values ----
	/// the default values will be used when the specific array is not present
	Normal		defaultNormal_;
	Pointsize	defaultPointsize_;
	Color		defaultColor_;

	// ---- vertex buffer object ----
	GLuint	vboGlId_;
	bool	vboValid_;

	void createVBO();
	void destroyVBO();
	void rebuildVBO();
};


//================================================================


} // namespace SceneGraph
} // namespace ACG


//================================================================


#endif // ACG_SPLATCLOUDNODE_HH
