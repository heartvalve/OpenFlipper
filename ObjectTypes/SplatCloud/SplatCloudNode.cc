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
//  CLASS SplatCloudNode - IMPLEMENTATION
//
//================================================================


//== INCLUDES ====================================================


#include "SplatCloudNode.hh"


//== NAMESPACES ==================================================


namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==============================================


void SplatCloudNode::boundingBox( ACG::Vec3d &_bbMin, ACG::Vec3d &_bbMax )
{
    ACG::Vec3f bbMin( FLT_MAX, FLT_MAX, FLT_MAX );
    ACG::Vec3f bbMax(-FLT_MAX,-FLT_MAX,-FLT_MAX );

    PointVector::const_iterator pointIter;
    for ( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
    {
        const Point &p = *pointIter;

        ACG::Vec3f acgp( p[0], p[1], p[2] );

        bbMin.minimize( acgp );
        bbMax.maximize( acgp );
    }

    ACG::Vec3d bbMind = ACG::Vec3d( bbMin );
    ACG::Vec3d bbMaxd = ACG::Vec3d( bbMax );

    _bbMin.minimize( bbMind );
    _bbMax.maximize( bbMaxd );
}


//----------------------------------------------------------------


void SplatCloudNode::draw( GLState &_state, const DrawModes::DrawMode &_drawMode )
{
    static const int RENDERMODE_POINTS = 0;
    static const int RENDERMODE_DOTS   = 1;
    static const int RENDERMODE_SPLATS = 2;

    // check if drawmode is valid
    int rendermode;
    if ( _drawMode.containsAtomicDrawMode( splatsDrawMode_ ) )
        rendermode = RENDERMODE_SPLATS;
    else if ( _drawMode.containsAtomicDrawMode( dotsDrawMode_ ) )
        rendermode = RENDERMODE_DOTS;
    else if ( _drawMode.containsAtomicDrawMode( pointsDrawMode_ ) )
        rendermode = RENDERMODE_POINTS;
    else
        return;

    // set desired depth function
    ACG::GLState::depthFunc( _state.depthFunc() );

    // is vertex-buffer-object is invalid then rebuild
    if ( !vboValid_ )
        rebuildVBO();

    // if vertex-buffer-object is valid now...
    // (if not it will be rebuilt the next time, but this should not happen)
    if ( vboValid_ )
    {
        // activate vertex-buffer-object
        ACG::GLState::bindBufferARB( GL_ARRAY_BUFFER_ARB, vboGlId_ );

        // tell GL where our data is (NULL = beginning of vertex-buffer-object)
        glInterleavedArrays( GL_C4F_N3F_V3F, 0, 0 );

        // arrays are automatically enabled by glInterleavedArrays()
        //glEnableClientState( GL_VERTEX_ARRAY );
        //glEnableClientState( GL_NORMAL_ARRAY );
        //glEnableClientState( GL_COLOR_ARRAY  );

        // Normals are always needed for backface culling, even in drawmode 'Points'!
        // Colors are always needed for pointsizes, even in color picking mode!

        // enable "pointsize by program" depending on current rendermode
        if ( rendermode != RENDERMODE_POINTS )
        {
            ACG::GLState::enable( GL_VERTEX_PROGRAM_POINT_SIZE );
        }

        // draw as GLpoints
        glDrawArrays( GL_POINTS, 0, numPoints() );

        // disable "pointsize by program" if it was enabled
        if ( rendermode != RENDERMODE_POINTS )
        {
            ACG::GLState::disable( GL_VERTEX_PROGRAM_POINT_SIZE );
        }

        // disable arrays
        ACG::GLState::disableClientState( GL_VERTEX_ARRAY );
        ACG::GLState::disableClientState( GL_NORMAL_ARRAY );
        ACG::GLState::disableClientState( GL_COLOR_ARRAY  );

        // deactivate vertex-buffer-object
        ACG::GLState::bindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
    }
}


//----------------------------------------------------------------


// TODO: hack, because pick() doesn't get a drawmode
static DrawModes::DrawMode g_pickDrawMode;
void SplatCloudNode::enterPick( GLState &_state, PickTarget _target, const DrawModes::DrawMode &_drawMode )
{
    g_pickDrawMode = _drawMode;
}

// ----

void SplatCloudNode::pick( GLState &_state, PickTarget _target )
{
//     if ( !_state.pick_set_maximum( points_.size() ) )
//      return;

    if ( _target == PICK_ANYTHING || _target == PICK_VERTEX )
    {
        // change GL's primary color according to pick name
//         _state.pick_set_name( 0 );

        // Copy GL's primary color to texture coordinate, which will be outputted by the picking shader as color.
        // This enables us to use RGBA instead of just RGB as it is the case when using GL's secondary color.
//         GLfloat color[4];
//         glGetFloatv( GL_CURRENT_COLOR, color );
//         glTexCoord4fv( color );

        // TODO: see above ( enterPick() )
//         draw( _state, g_pickDrawMode );

        draw_pick( _state );
    }
}


//----------------------------------------------------------------

void
SplatCloudNode::
draw_pick( GLState &_state )
{
    _state.pick_set_maximum(points_.size());

    for ( unsigned int i = 0; i < points_.size(); ++i )
    {
        _state.pick_set_name(i);
        glBegin(GL_POINTS);
        glVertex3f( float(points_[i][0] ),float(points_[i][1] ),float(points_[i][2] ) );
        glEnd();
    }

/*
    if (!_state.pick_set_maximum (points_.size()))
    {
        std::cerr << "Splat Node::draw_pick: color range too small, " << "picking failed\n";
        return;
    }

    if ( updateVertexPicking_ || _state.pick_current_index () != vertexPickingBaseIndex_) {
        pick_colors_.clear();

        pick_colors_.reserve( points_.size() );
        for ( unsigned int i = 0; i < points_.size(); ++i )
            pick_colors_[i] = _state.pick_get_name_color( i );

        vertexPickingBaseIndex_ = _state.pick_current_index ();
        updateVertexPicking_    = false;
    }

    // For this version we load the colors directly not from vbo
    ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
    ACG::GLState::colorPointer( &(pick_colors_[0]) );
    ACG::GLState::enableClientState(GL_COLOR_ARRAY);

    // vertex positions
    ACG::GLState::vertexPointer( &(points_[0]) );
    ACG::GLState::enableClientState(GL_VERTEX_ARRAY);

    // Draw color picking
    glDrawArrays(GL_POINTS, 0, points_.size());

    // Disable color array
    ACG::GLState::disableClientState(GL_COLOR_ARRAY);

    // disable vertex array
    ACG::GLState::disableClientState(GL_VERTEX_ARRAY);*/
}

//----------------------------------------------------------------

void SplatCloudNode::createVBO()
{
    // create a new vertex-buffer-object if not already existing
    if ( vboGlId_ == 0 )
    {
        glGenBuffersARB( 1, &vboGlId_ );
    }
}


//----------------------------------------------------------------


void SplatCloudNode::destroyVBO()
{
    // delete existing vertex-buffer-object
    if ( vboGlId_ != 0 )
    {
        glDeleteBuffersARB( 1, &vboGlId_ );
        vboGlId_ = 0;
    }

    // make vertex-buffer-object invalid so it will not be used
    vboValid_ = false;
}


//----------------------------------------------------------------


static void addFloatToBuffer( float _value, unsigned char *&_buffer )
{
    // get pointer
    unsigned char *v = (unsigned char *) &_value;

    // copy over 4 bytes
    *_buffer++ = *v++;
    *_buffer++ = *v++;
    *_buffer++ = *v++;
    *_buffer++ = *v;
}


//----------------------------------------------------------------


void SplatCloudNode::rebuildVBO()
{
    // check if vertex-buffer-object has already been created (and not destroyed so far)
    if ( vboGlId_ == 0 )
        return;

    // check if there could be a valid vertex-buffer-object
    if ( numPoints() == 0 )
    {
        vboValid_ = false;
        return;
    }

    // we use GL_C4F_N3F_V3F as interleaved array type, so we have 4+3+3 = 10 floats per splat
    unsigned int size = numPoints() * 10 * sizeof(float);

    // activate vertex-buffer-object
    ACG::GLState::bindBufferARB( GL_ARRAY_BUFFER_ARB, vboGlId_ );

    // tell GL that we are seldom updating the vertex-buffer-object but are often drawing it
    glBufferDataARB( GL_ARRAY_BUFFER_ARB, size, 0, GL_STATIC_DRAW_ARB );

    // get pointer to vertex-buffer-object's memory
    unsigned char *buffer = (unsigned char *) glMapBufferARB( GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB );

    // if pointer is valid...
    if ( buffer )
    {
        bool hasNrm = hasNormals();
        bool hasPS  = hasPointsizes();
        bool hasCol = hasColors();

        // for all points...
        int i, num = numPoints();
        for ( i=0; i<num; ++i )
        {
            static const float RCP255 = 1.0f / 255.0f;

            // add color
            const Color &c = hasCol ? colors_[i] : defaultColor_;
            addFloatToBuffer( RCP255 * c[0], buffer );
            addFloatToBuffer( RCP255 * c[1], buffer );
            addFloatToBuffer( RCP255 * c[2], buffer );

            // add pointsize (as alpha-component of color)
            const Pointsize &ps = hasPS ? pointsizes_[i] : defaultPointsize_;
            addFloatToBuffer( ps, buffer );

            // add normal
            const Normal &n = hasNrm ? normals_[i] : defaultNormal_;
            addFloatToBuffer( n[0], buffer );
            addFloatToBuffer( n[1], buffer );
            addFloatToBuffer( n[2], buffer );

            // add point
            const Point &p = points_[i];
            addFloatToBuffer( p[0], buffer );
            addFloatToBuffer( p[1], buffer );
            addFloatToBuffer( p[2], buffer );

            // add pick color

        }
    }

    // release pointer to vertex-buffer-object's memory
    bool success = glUnmapBufferARB( GL_ARRAY_BUFFER_ARB );

    // deactivate vertex-buffer-object
    ACG::GLState::bindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

    // mark vertex-buffer-object as valid only if it was successfully updated and unmaped
    vboValid_ = (buffer != 0) && success;
}


//----------------------------------------------------------------


void SplatCloudNode::normalizeSize()
{
        // check if there is nothing to do
	if( points_.size() == 0 )
		return;

	// calculate center-of-gravety
	float cogX = 0.0f;
	float cogY = 0.0f;
	float cogZ = 0.0f;
	SplatCloudNode::PointVector::iterator pointIter;
	for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
	{
		const SplatCloudNode::Point &p = *pointIter;

		cogX += p[0];
		cogY += p[1];
		cogZ += p[2];
	}

	float rcp_count = 1.0f / (float) points_.size();
	cogX *= rcp_count;
	cogY *= rcp_count;
	cogZ *= rcp_count;
	
	cur_translation_ = -Point ( cogX, cogY, cogZ );
	translate( cur_translation_ );
	std::cout << "SplatCloudNode::normalizeSize(): translate points by: " << cur_translation_ << std::endl;
	
	// calculate squared length
	float sqLength = 0.0f;
	for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
	{
		SplatCloudNode::Point &p = *pointIter;
		sqLength += p[0]*p[0] + p[1]*p[1] + p[2]*p[2];
	}

	float s = (float) sqrt( sqLength * rcp_count );

	if( s == 0.0f )
	  return;
	
	cur_scale_factor_ = 1.0f / s;

	scale( cur_scale_factor_ );
	std::cout << "SplatCloudNode::normalizeSize(): scaling points with factor: " << cur_scale_factor_ << std::endl;
}

//----------------------------------------------------------------


void SplatCloudNode::translate( const Point& _t )
{
        SplatCloudNode::PointVector::iterator pointIter;
	for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
	{
		SplatCloudNode::Point &p = *pointIter;
		
		p[0] += _t[0];
		p[1] += _t[1];
		p[2] += _t[2];
	}
}

//----------------------------------------------------------------


void SplatCloudNode::scale( const float _scale )
{
        // scale points (and pointsizes as well) by calculated scale
        SplatCloudNode::PointVector::iterator pointIter;
	if( pointsizes_.size() == points_.size() )
	{
		SplatCloudNode::PointsizeVector::iterator pointsizeIter = pointsizes_.begin();
		for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter, ++pointsizeIter )
		{
			SplatCloudNode::Point     &p  = *pointIter;
			SplatCloudNode::Pointsize &ps = *pointsizeIter;

			p[0] *= _scale;
			p[1] *= _scale;
			p[2] *= _scale;
			ps   *= _scale; // scale pointsize as well
		}
	}
	else
	{
		for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
		{
			SplatCloudNode::Point &p = *pointIter;

			p[0] *= _scale;
			p[1] *= _scale;
			p[2] *= _scale;
		}
	}
	
}

//================================================================


} // namespace SceneGraph
} // namespace ACG
