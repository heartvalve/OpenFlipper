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
//    SplatCloudNode renders splats by passing points, normals, point sizes and colors (and picking colors) to the GL.
//    These elements are internally stored in an interleaved array using an OpenGL vertex-buffer-object
//    including vertices, normals, texcoords and colors.
//
//================================================================


#ifndef ACG_SPLATCLOUDNODE_HH
#define ACG_SPLATCLOUDNODE_HH


//== INCLUDES ====================================================


#include "SplatCloud/SplatCloud.hh"

#include <OpenFlipper/common/GlobalDefines.hh>

#include <ACG/Scenegraph/BaseNode.hh>

#include <ACG/Scenegraph/DrawModes.hh>

#include <ACG/GL/gl.hh>


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
private:

	//-- TYPEDEFS ----------------------------------------------------

	typedef SplatCloud::Point     Point;
	typedef SplatCloud::Normal    Normal;
	typedef SplatCloud::Pointsize Pointsize;
	typedef SplatCloud::Color     Color;

	//----------------------------------------------------------------

public:

	/// constructor
	SplatCloudNode( BaseNode *_parent = 0, std::string _name = "<SplatCloudNode>" );

	/// destructor
	~SplatCloudNode();

	ACG_CLASSNAME( SplatCloudNode );

	/// return available draw modes
	inline DrawModes::DrawMode availableDrawModes() const {
	  return splatsDrawMode_ | dotsDrawMode_ | pointsDrawMode_;
	}

	/// update bounding box
	void boundingBox( ACG::Vec3d &_bbMin, ACG::Vec3d &_bbMax );

	/// draw the SplatCloud
	void draw( GLState &_state, const DrawModes::DrawMode &_drawMode );

	/// picking
	void enterPick( GLState &_state, PickTarget _target, const DrawModes::DrawMode &_drawMode );
	void pick( GLState &_state, PickTarget _target );

	// ---- splat cloud ----

	void copySplatCloud( const SplatCloud &_splatCloud );

	inline       SplatCloud *splatCloud()       { return splatCloud_; }
	inline const SplatCloud *splatCloud() const { return splatCloud_; }

	// ---- default values ----

	// if an array is not present, changing the default value for this array will make the VBO invalid and trigger a rebuild()
	inline void setDefaultNormal   ( const Normal    &_normal    ) { defaultNormal_    = _normal;    if( splatCloud_ && !splatCloud_->hasNormals()    ) vboValid_ = false; }
	inline void setDefaultPointsize( const Pointsize &_pointsize ) { defaultPointsize_ = _pointsize; if( splatCloud_ && !splatCloud_->hasPointsizes() ) vboValid_ = false; }
	inline void setDefaultColor    ( const Color     &_color     ) { defaultColor_     = _color;     if( splatCloud_ && !splatCloud_->hasColors()     ) vboValid_ = false; }

	inline const Normal    &defaultNormal()    const { return defaultNormal_;    }
	inline const Pointsize &defaultPointsize() const { return defaultPointsize_; }
	inline const Color     &defaultColor()     const { return defaultColor_;     }

	// if the point, normal, pointsize or color with the given _index exists it is returned, otherwise the default value is returned (check for splatCloud_ != 0 first)
	inline Point     getPoint    ( unsigned int _index ) const { return                                splatCloud_->points()    [ _index ]                    ; }
	inline Normal    getNormal   ( unsigned int _index ) const { return splatCloud_->hasNormals()    ? splatCloud_->normals()   [ _index ] : defaultNormal_   ; }
	inline Pointsize getPointsize( unsigned int _index ) const { return splatCloud_->hasPointsizes() ? splatCloud_->pointsizes()[ _index ] : defaultPointsize_; }
	inline Color     getColor    ( unsigned int _index ) const { return splatCloud_->hasColors()     ? splatCloud_->colors()    [ _index ] : defaultColor_    ; }

	// ---- vertex buffer object ----

	/// make vertex-buffer-object invalid so it will be rebuilt the next time drawn (or picked)
	inline void invalidateVBO() { vboValid_ = false; }

	//----------------------------------------------------------------

private:

	// ---- splat cloud ----
	SplatCloud *splatCloud_;

	// ---- default values ----
	/// the default values will be used when the specific array is not present
	Normal    defaultNormal_;
	Pointsize defaultPointsize_;
	Color     defaultColor_;

	// ---- draw modes ----
	DrawModes::DrawMode splatsDrawMode_;
	DrawModes::DrawMode dotsDrawMode_;
	DrawModes::DrawMode pointsDrawMode_;

	// ---- picking base index ----
	unsigned int pickingBaseIndex_;

	// ---- vertex buffer object ----
	GLuint vboGlId_;
	bool   vboValid_;

	void createVBO();
	void destroyVBO();
	void rebuildVBO( GLState &_state );
};


//================================================================


} // namespace SceneGraph
} // namespace ACG


//================================================================


#endif // ACG_SPLATCLOUDNODE_HH
