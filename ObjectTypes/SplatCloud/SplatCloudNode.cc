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


//== DEFINES =====================================================


//#define REPORT_VBO_UPDATES


//== NAMESPACES ==================================================


namespace ACG {
namespace SceneGraph {

//== STATIC MEMBER VARIABLES =====================================


const SplatCloudNode::Position  SplatCloudNode::DEFAULT_POSITION (0,0,0);
const SplatCloudNode::Index     SplatCloudNode::DEFAULT_INDEX    (   -1);
const SplatCloudNode::Viewlist  SplatCloudNode::DEFAULT_VIEWLIST (  0.0);
const SplatCloudNode::Selection SplatCloudNode::DEFAULT_SELECTION(false);


//== IMPLEMENTATION ==============================================


SplatCloudNode::SplatCloudNode( const SplatCloud &_splatCloud, BaseNode *_parent, std::string _name ) : 
  BaseNode            ( _parent, _name ), 
  splatCloud_         ( _splatCloud ), 
  positionsModified_  ( false ), 
  colorsModified_     ( false ), 
  normalsModified_    ( false ), 
  pointsizesModified_ ( false ), 
  selectionsModified_ ( false ), 
  pickColorsModified_ ( false ), 
  defaultColor_       ( Color    (0,0,0) ), 
  defaultNormal_      ( Normal   (0,0,0) ), 
  defaultPointsize_   ( Pointsize(  0.0) ), 
  splatsDrawMode_     ( DrawModes::NONE ), 
  dotsDrawMode_       ( DrawModes::NONE ), 
  pointsDrawMode_     ( DrawModes::NONE ), 
  pickingBaseIndex_   ( 0 ), 
  pickDrawMode_       ( DrawModes::NONE ), // TODO: hack, see enterPick()
  vboGlId_            ( 0 ), 
  vboNumSplats_       ( 0 ), 
  vboData_            ( 0 ), 
  vboPositionsOffset_ ( -1 ), 
  vboColorsOffset_    ( -1 ), 
  vboNormalsOffset_   ( -1 ), 
  vboPointsizesOffset_( -1 ), 
  vboSelectionsOffset_( -1 ), 
  vboPickColorsOffset_( -1 ) 
{
  // add (possibly) new drawmodes
  pointsDrawMode_ = DrawModes::addDrawMode( "Points" );
  dotsDrawMode_   = DrawModes::addDrawMode( "Dots"   );
  splatsDrawMode_ = DrawModes::addDrawMode( "Splats" );

  // create a new VBO (will be invalid and rebuilt the next time drawn (or picked))
  createVBO();
}


//----------------------------------------------------------------


SplatCloudNode::~SplatCloudNode()
{
  destroyVBO();
}


//----------------------------------------------------------------


void SplatCloudNode::boundingBox( ACG::Vec3d &_bbMin, ACG::Vec3d &_bbMax )
{
  ACG::Vec3f bbMin( FLT_MAX, FLT_MAX, FLT_MAX );
  ACG::Vec3f bbMax(-FLT_MAX,-FLT_MAX,-FLT_MAX );

  if( splatCloud_.hasPositions() )
  {
    unsigned int i, num = splatCloud_.numSplats();
    for( i=0; i<num; ++i )
    {
      const Position &p = splatCloud_.positions( i );

      ACG::Vec3f acgp( p[0], p[1], p[2] );

      bbMin.minimize( acgp );
      bbMax.maximize( acgp );
    }
  }

  _bbMin.minimize( ACG::Vec3d( bbMin ) );
  _bbMax.maximize( ACG::Vec3d( bbMax ) );
}


//----------------------------------------------------------------


void SplatCloudNode::draw( GLState &_state, const DrawModes::DrawMode &_drawMode )
{
  static const int RENDERMODE_POINTS = 0;
  static const int RENDERMODE_DOTS   = 1;
  static const int RENDERMODE_SPLATS = 2;

  // check if drawmode is valid
  int rendermode;
  if( _drawMode.containsAtomicDrawMode( splatsDrawMode_ ) )
    rendermode = RENDERMODE_SPLATS;
  else if( _drawMode.containsAtomicDrawMode( dotsDrawMode_ ) )
    rendermode = RENDERMODE_DOTS;
  else if( _drawMode.containsAtomicDrawMode( pointsDrawMode_ ) )
    rendermode = RENDERMODE_POINTS;
  else
    return;

  // set desired depth function
  ACG::GLState::depthFunc( _state.depthFunc() );

  // if VBO is invalid or was (partially) modified, then rebuild
  if( (vboData_ == 0) || vboModified() )
    rebuildVBO( _state );

  // if VBO is valid...
  if( vboData_ != 0 )
  {
    // activate VBO
    ACG::GLState::bindBufferARB( GL_ARRAY_BUFFER_ARB, vboGlId_ );

    // enable arrays:
    // --------------

    // positions
    if( vboPositionsOffset_ != -1 )
    {
      ACG::GLState::enableClientState( GL_VERTEX_ARRAY );
      ACG::GLState::vertexPointer( 3, GL_FLOAT, 0, (unsigned char *) 0 + vboPositionsOffset_ );
    }
    else
    {
      ACG::GLState::disableClientState( GL_VERTEX_ARRAY );
    }

    // colors
    if( vboColorsOffset_ != -1 )
    {
      ACG::GLState::enableClientState( GL_SECONDARY_COLOR_ARRAY );
      glSecondaryColorPointer( 3, GL_UNSIGNED_BYTE, 0, (unsigned char *) 0 + vboColorsOffset_ ); // TODO: use ACG::GLState::secondaryColorPointer() when implemented
    }
    else
    {
      ACG::GLState::disableClientState( GL_SECONDARY_COLOR_ARRAY );
      glSecondaryColor3ub( defaultColor_[0], defaultColor_[1], defaultColor_[2] );
    }

    // normals
    if( vboNormalsOffset_ != -1 )
    {
      ACG::GLState::enableClientState( GL_NORMAL_ARRAY );
      ACG::GLState::normalPointer( GL_FLOAT, 0, (unsigned char *) 0 + vboNormalsOffset_ );
    }
    else
    {
      ACG::GLState::disableClientState( GL_NORMAL_ARRAY );
      glNormal3f( defaultNormal_[0], defaultNormal_[1], defaultNormal_[2] );
    }

    // pointsizes
    if( vboPointsizesOffset_ != -1 )
    {
      glClientActiveTexture( GL_TEXTURE0 ); // TODO: use ACG::GLState::clientActiveTexture() when implemented
      ACG::GLState::enableClientState( GL_TEXTURE_COORD_ARRAY );
      ACG::GLState::texcoordPointer( 1, GL_FLOAT, 0, (unsigned char *) 0 + vboPointsizesOffset_ );
    }
    else
    {
      glClientActiveTexture( GL_TEXTURE0 ); // TODO: use ACG::GLState::clientActiveTexture() when implemented
      ACG::GLState::disableClientState( GL_TEXTURE_COORD_ARRAY );
      glMultiTexCoord1f( GL_TEXTURE0, defaultPointsize_ );
    }

    // selections
    if( vboSelectionsOffset_ != -1 )
    {
      glClientActiveTexture( GL_TEXTURE1 ); // TODO: use ACG::GLState::clientActiveTexture() when implemented
      ACG::GLState::enableClientState( GL_TEXTURE_COORD_ARRAY );
      ACG::GLState::texcoordPointer( 1, GL_FLOAT, 0, (unsigned char *) 0 + vboSelectionsOffset_ );
    }
    else
    {
      glClientActiveTexture( GL_TEXTURE1 ); // TODO: use ACG::GLState::clientActiveTexture() when implemented
      ACG::GLState::disableClientState( GL_TEXTURE_COORD_ARRAY );
      glMultiTexCoord1f( GL_TEXTURE1, 0.0f );
    }

    // pick colors
    if( vboPickColorsOffset_ != -1 )
    {
      ACG::GLState::enableClientState( GL_COLOR_ARRAY );
      ACG::GLState::colorPointer( 4, GL_UNSIGNED_BYTE, 0, (unsigned char *) 0 + vboPickColorsOffset_ );
    }
    else
    {
      ACG::GLState::disableClientState( GL_COLOR_ARRAY );
      glColor4ub( 255, 255, 255, 255 );
    }

    // render:
    // -------

    // enable "pointsize by program" depending on current rendermode
    if( rendermode == RENDERMODE_SPLATS || rendermode == RENDERMODE_DOTS )
      ACG::GLState::enable( GL_VERTEX_PROGRAM_POINT_SIZE );
    else
      ACG::GLState::disable( GL_VERTEX_PROGRAM_POINT_SIZE );

    // draw as GLpoints
    glDrawArrays( GL_POINTS, 0, vboNumSplats_ );

    // disable arrays:
    // ---------------

    // positions
    ACG::GLState::disableClientState( GL_VERTEX_ARRAY );

    // colors
    ACG::GLState::disableClientState( GL_SECONDARY_COLOR_ARRAY );

    // normals
    ACG::GLState::disableClientState( GL_NORMAL_ARRAY );

    // pointsizes
    glClientActiveTexture( GL_TEXTURE0 ); // TODO: use ACG::GLState::clientActiveTexture() when implemented
    ACG::GLState::disableClientState( GL_TEXTURE_COORD_ARRAY );

    // selections
    glClientActiveTexture( GL_TEXTURE1 ); // TODO: use ACG::GLState::clientActiveTexture() when implemented
    ACG::GLState::disableClientState( GL_TEXTURE_COORD_ARRAY );

    // pick colors
    ACG::GLState::disableClientState( GL_COLOR_ARRAY );

    // reset states:
    // -------------

    // disable "pointsize by program"
    ACG::GLState::disable( GL_VERTEX_PROGRAM_POINT_SIZE );

    // make defaults current again
    glClientActiveTexture( GL_TEXTURE0 ); // TODO: use ACG::GLState::clientActiveTexture() when implemented
    glColor4f         ( 1.0f, 1.0f, 1.0f, 1.0f );
    glSecondaryColor3f( 1.0f, 1.0f, 1.0f );

    // deactivate VBO
    ACG::GLState::bindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
  }
}


//----------------------------------------------------------------


void SplatCloudNode::pick( GLState &_state, PickTarget _target )
{
  // if pick target is valid...
  if( _target == PICK_ANYTHING || _target == PICK_VERTEX )
  {
    // set number of pick colors used (each splats gets a unique pick color)
    if( !_state.pick_set_maximum( splatCloud_.numSplats() ) ) // number of splats could have changed, so use new number of splats (*not* the one in VBO memory!)
    {
      std::cerr << "SplatCloudNode::pick() : Color range too small, picking failed." << std::endl;
      return;
    }

    // if in color picking mode...
    if( _state.color_picking() )
    {
      // if picking base index has changed, rebuild pick colors block in VBO so new pick colors will be used
      if( pickingBaseIndex_ != _state.pick_current_index() )
      {
        pickColorsModified_ = true;
      }

      // TODO: hack, see enterPick()
      draw( _state, pickDrawMode_ );
    }
  }
}


//----------------------------------------------------------------


void SplatCloudNode::createVBO()
{
  // create new VBO (if *not* already existing)
  if( vboGlId_ == 0 )
  {
    glGenBuffersARB( 1, &vboGlId_ );
    vboNumSplats_ = 0;
    vboData_      = 0;

    modifiedAll(); // (re-)build all data block in VBO memory the first time
  }
}


//----------------------------------------------------------------


void SplatCloudNode::destroyVBO()
{
  // delete VBO (if existing)
  if( vboGlId_ != 0 )
  {
    glDeleteBuffersARB( 1, &vboGlId_ );
    vboGlId_      = 0;
    vboNumSplats_ = 0;
    vboData_      = 0;
  }
}


//----------------------------------------------------------------


void SplatCloudNode::rebuildVBO( GLState &_state )
{
  // if something went wrong in the initialization, make VBO invalid and abort
  if( vboGlId_ == 0 )
  {
    vboData_ = 0;
    return;
  }

  // activate VBO
  ACG::GLState::bindBufferARB( GL_ARRAY_BUFFER_ARB, vboGlId_ );

  // calculate size of data and offsets
  unsigned int numSplats = splatCloud_.numSplats();
  unsigned int size      = 0;

  int positionsOffset  = -1;
  int colorsOffset     = -1;
  int normalsOffset    = -1;
  int pointsizesOffset = -1;
  int selectionsOffset = -1;
  int pickColorsOffset = -1;

  if( splatCloud_.hasPositions()  ) { positionsOffset  = size; size += numSplats * 12; }
  if( splatCloud_.hasColors()     ) { colorsOffset     = size; size += numSplats * 3;  }
  if( splatCloud_.hasNormals()    ) { normalsOffset    = size; size += numSplats * 12; }
  if( splatCloud_.hasPointsizes() ) { pointsizesOffset = size; size += numSplats * 4;  }
  if( splatCloud_.hasSelections() ) { selectionsOffset = size; size += numSplats * 4;  }
  /* has pick colors is true     */ { pickColorsOffset = size; size += numSplats * 4;  }

  // tell GL that we are seldomly updating the VBO but are often drawing it
  glBufferDataARB( GL_ARRAY_BUFFER_ARB, size, 0, GL_STATIC_DRAW_ARB );

  // get pointer to VBO memory
  unsigned char *data = (unsigned char *) glMapBufferARB( GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB );

  // check if something went wrong during VBO mapping
  if( data == 0 )
  {
    std::cout << "SplatCloudNode::rebuildVBO() : glMapBufferARB() failed." << std::endl;
    vboData_ = 0;
  }
  else
  {
    // if VBO memory block was moved or the internal block structure has to be changed, rebuild entire VBO
    if( (vboData_ != data) || vboStructureModified() )
    {
      vboNumSplats_ = numSplats;
      vboData_      = data;

      vboPositionsOffset_  = positionsOffset;
      vboColorsOffset_     = colorsOffset;
      vboNormalsOffset_    = normalsOffset;
      vboPointsizesOffset_ = pointsizesOffset;
      vboSelectionsOffset_ = selectionsOffset;
      vboPickColorsOffset_ = pickColorsOffset;

      // mark all data blocks to rebuild them lateron
      modifiedAll();
    }

    // if in color picking mode...
    if( _state.color_picking() )
    {
      // store picking base index
      pickingBaseIndex_ = _state.pick_current_index();
    }

    // rebuild data blocks if needed
    if( positionsModified_  ) rebuildVBOPositions();
    if( colorsModified_     ) rebuildVBOColors();
    if( normalsModified_    ) rebuildVBONormals();
    if( pointsizesModified_ ) rebuildVBOPointsizes();
    if( selectionsModified_ ) rebuildVBOSelections();
    if( pickColorsModified_ ) rebuildVBOPickColors( _state );

#   ifdef REPORT_VBO_UPDATES
    std::cout << std::endl;
#   endif

    // every block in VBO memory has been updated
    positionsModified_  = false;
    colorsModified_     = false;
    normalsModified_    = false;
    pointsizesModified_ = false;
    selectionsModified_ = false;
    pickColorsModified_ = false;

    // release pointer to VBO memory. if something went wrong, make VBO invalid and abort
    if( !glUnmapBufferARB( GL_ARRAY_BUFFER_ARB ) )
    {
      std::cout << "SplatCloudNode::rebuildVBO() : glUnmapBufferARB() failed." << std::endl;
      vboData_ = 0;
    }
  }

  // deactivate VBO
  ACG::GLState::bindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
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


static void addIntToBuffer( int _value, unsigned char *&_buffer )
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


static void addUCharToBuffer( unsigned char _value, unsigned char *&_buffer )
{
  // get pointer
  unsigned char *v = (unsigned char *) &_value;

  // copy over 1 byte
  *_buffer++ = *v;
}


//----------------------------------------------------------------


void SplatCloudNode::rebuildVBOPositions()
{
  if( (vboPositionsOffset_ == -1) || !splatCloud_.hasPositions() )
    return;

# ifdef REPORT_VBO_UPDATES
  std::cout << "SplatCloudNode::rebuildVBOPositions()" << std::endl;
# endif

  // get pointer to buffer
  unsigned char *buffer = vboData_ + vboPositionsOffset_;

  // for all positions...
  unsigned int i, num = splatCloud_.numSplats();
  for( i=0; i<num; ++i )
  {
    // add position
    const Position &p = getPosition( i );
    addFloatToBuffer( p[0], buffer );
    addFloatToBuffer( p[1], buffer );
    addFloatToBuffer( p[2], buffer );
  }
}


//----------------------------------------------------------------


void SplatCloudNode::rebuildVBOColors()
{
  if( (vboColorsOffset_ == -1) || !splatCloud_.hasColors()  )
    return;

# ifdef REPORT_VBO_UPDATES
  std::cout << "SplatCloudNode::rebuildVBOColors()" << std::endl;
# endif

  // get pointer to buffer
  unsigned char *buffer = vboData_ + vboColorsOffset_;

  // for all colors...
  unsigned int i, num = splatCloud_.numSplats();
  for( i=0; i<num; ++i )
  {
    // add color
    const Color &c = getColor( i );
    addUCharToBuffer( c[0], buffer );
    addUCharToBuffer( c[1], buffer );
    addUCharToBuffer( c[2], buffer );
  }
}


//----------------------------------------------------------------


void SplatCloudNode::rebuildVBONormals()
{
  if( (vboNormalsOffset_ == -1) || !splatCloud_.hasNormals()  )
    return;

# ifdef REPORT_VBO_UPDATES
  std::cout << "SplatCloudNode::rebuildVBONormals()" << std::endl;
# endif

  // get pointer to buffer
  unsigned char *buffer = vboData_ + vboNormalsOffset_;

  // for all normals...
  unsigned int i, num = splatCloud_.numSplats();
  for( i=0; i<num; ++i )
  {
    // add normal
    const Normal &n = getNormal( i );
    addFloatToBuffer( n[0], buffer );
    addFloatToBuffer( n[1], buffer );
    addFloatToBuffer( n[2], buffer );
  }
}


//----------------------------------------------------------------


void SplatCloudNode::rebuildVBOPointsizes()
{
  if( (vboPointsizesOffset_ == -1) || !splatCloud_.hasPointsizes()  )
    return;

# ifdef REPORT_VBO_UPDATES
  std::cout << "SplatCloudNode::rebuildVBOPointsizes()" << std::endl;
# endif

  // get pointer to buffer
  unsigned char *buffer = vboData_ + vboPointsizesOffset_;

  // for all pointsizes...
  unsigned int i, num = splatCloud_.numSplats();
  for( i=0; i<num; ++i )
  {
    // add pointsize
    const Pointsize &ps = getPointsize( i );
    addFloatToBuffer( ps, buffer );
  }
}


//----------------------------------------------------------------


void SplatCloudNode::rebuildVBOSelections()
{
  if( (vboSelectionsOffset_ == -1) || !splatCloud_.hasSelections()  )
    return;

# ifdef REPORT_VBO_UPDATES
  std::cout << "SplatCloudNode::rebuildVBOSelections()" << std::endl;
# endif

  // get pointer to buffer
  unsigned char *buffer = vboData_ + vboSelectionsOffset_;

  // for all selections...
  unsigned int i, num = splatCloud_.numSplats();
  for( i=0; i<num; ++i )
  {
    const bool &s = getSelection( i );
    addFloatToBuffer( (s ? 1.0f : 0.0f), buffer );
  }
}


//----------------------------------------------------------------


void SplatCloudNode::rebuildVBOPickColors( GLState &_state )
{
  if( (vboPickColorsOffset_ == -1) || !splatCloud_.hasPositions() )
    return;

# ifdef REPORT_VBO_UPDATES
  std::cout << "SplatCloudNode::rebuildVBOPickColors()" << std::endl;
# endif

  // get pointer to buffer
  unsigned char *buffer = vboData_ + vboPickColorsOffset_;

  // for all pick colors...
  unsigned int i, num = splatCloud_.numSplats();
  for( i=0; i<num; ++i )
  {
    // add pick color
    const Vec4uc &pc = _state.pick_get_name_color( i );
    addUCharToBuffer( pc[0], buffer );
    addUCharToBuffer( pc[1], buffer );
    addUCharToBuffer( pc[2], buffer );
    addUCharToBuffer( pc[3], buffer );
  }
}


//================================================================


} // namespace SceneGraph
} // namespace ACG
