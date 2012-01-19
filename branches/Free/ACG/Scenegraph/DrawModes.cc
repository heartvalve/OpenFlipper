/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS DrawModes - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "DrawModes.hh"
#include "BaseNode.hh"

#include <algorithm>


//== NAMESPACES ============================================================== 

namespace ACG {
namespace SceneGraph {
namespace DrawModes {


// == Default Draw Mode intitialization ======================================  

DrawMode NONE                              = ModeFlagSet(0);
DrawMode DEFAULT                           = ModeFlagSet(1);
DrawMode POINTS                            = ModeFlagSet(1) << 1;
DrawMode POINTS_COLORED                    = ModeFlagSet(1) << 2;
DrawMode POINTS_SHADED                     = ModeFlagSet(1) << 3;
DrawMode EDGES                             = ModeFlagSet(1) << 4;
DrawMode EDGES_COLORED                     = ModeFlagSet(1) << 5;
DrawMode WIREFRAME                         = ModeFlagSet(1) << 6;
DrawMode FACES                             = ModeFlagSet(1) << 7;
DrawMode HIDDENLINE                        = ModeFlagSet(1) << 8;
DrawMode SOLID_FLAT_SHADED                 = ModeFlagSet(1) << 9;
DrawMode SOLID_SMOOTH_SHADED               = ModeFlagSet(1) << 10;
DrawMode SOLID_PHONG_SHADED                = ModeFlagSet(1) << 11;
DrawMode SOLID_FACES_COLORED               = ModeFlagSet(1) << 12;
DrawMode SOLID_POINTS_COLORED              = ModeFlagSet(1) << 13;
DrawMode SOLID_ENV_MAPPED                  = ModeFlagSet(1) << 14;
DrawMode SOLID_TEXTURED                    = ModeFlagSet(1) << 15;
DrawMode SOLID_TEXTURED_SHADED             = ModeFlagSet(1) << 16;
DrawMode SOLID_1DTEXTURED                  = ModeFlagSet(1) << 17;
DrawMode SOLID_1DTEXTURED_SHADED           = ModeFlagSet(1) << 18;
DrawMode SOLID_3DTEXTURED                  = ModeFlagSet(1) << 19;
DrawMode SOLID_3DTEXTURED_SHADED           = ModeFlagSet(1) << 20;
DrawMode SOLID_FACES_COLORED_FLAT_SHADED   = ModeFlagSet(1) << 21;
DrawMode SOLID_FACES_COLORED_SMOOTH_SHADED = ModeFlagSet(1) << 22;
DrawMode SOLID_2DTEXTURED_FACE             = ModeFlagSet(1) << 23;
DrawMode SOLID_2DTEXTURED_FACE_SHADED      = ModeFlagSet(1) << 24;
DrawMode SOLID_SHADER                      = ModeFlagSet(1) << 25;
DrawMode SOLID_SMOOTH_SHADED_FEATURES      = ModeFlagSet(1) << 26;
DrawMode CELLS                             = ModeFlagSet(1) << 27;
DrawMode CELLS_COLORED                     = ModeFlagSet(1) << 28;
DrawMode HALFEDGES                         = ModeFlagSet(1) << 29;
DrawMode HALFEDGES_COLORED                 = ModeFlagSet(1) << 30;
DrawMode UNUSED                            = ModeFlagSet(1) << 31;
  

//== IMPLEMENTATION ========================================================== 


/** Definition of a draw mode */
class DrawModeInternal {
  
  public:
    /** Default constructor. */
    DrawModeInternal() {}
    
    /** Initializing constructor. */
    DrawModeInternal( const std::string & _name, const DrawMode & _id )
    : name_( _name ), id_( _id ) {}
    
    std::string name_;
    DrawMode id_;
};


typedef std::vector< DrawModeInternal > VecDrawModes;

/** Vector of all currently defined DrawModes.
*/
static VecDrawModes currentDrawModes_;

/** First free DrawMode ID for custom modes. */
static DrawMode firstFreeID_;


DrawMode::DrawMode(unsigned int _index) {
  modeFlags_.reset();
  if ( _index >= modeFlags_.size() ) {
    std::cerr << "Illegal drawMode specification from unsigned int. This should not be a bitset!!!" << std::endl;
  } else {
    modeFlags_.set(_index);
  }
}

DrawMode::DrawMode() {
      
}
    
DrawMode::DrawMode( ModeFlagSet _flags ) {
  modeFlags_ = _flags;
}

DrawMode::operator bool() const {
  return( modeFlags_ != NONE.modeFlags_ );
}

bool DrawMode::operator==(const DrawMode& _mode) const {
  // If we are asked for Point, also return the Modes for Points shaded and colors
//   if ( (_mode.modeFlags_ & POINTS.modeFlags_).any() ) 
//     return ( (modeFlags_ & ( POINTS_COLORED | POINTS_SHADED ).modeFlags_).any() );
  
  ///\todo Implement more combined returns for faces and Shading
  
  return ((modeFlags_ & _mode.modeFlags_).any());
}

bool DrawMode::operator!=( const DrawMode& _mode2  ) const {
  return (modeFlags_ != _mode2.modeFlags_);
}

DrawMode& DrawMode::operator++() {
  if ( modeFlags_.count() != 1 ) {
    std::cerr << "Operator ++ for drawMode which is not atomic!!" << std::endl;
  }

  modeFlags_ <<= 1;
  
  return (*this);
}

DrawMode DrawMode::operator&(const DrawMode& _mode) const {
  
  // If we are asked for Point, also return the Modes for Points shaded and colors
//   if ( (_mode.modeFlags_ & POINTS.modeFlags_).any() ) 
//     return ( modeFlags_ & ( POINTS_COLORED | POINTS_SHADED ).modeFlags_ );
  
  ///\todo Implement more combined returns for faces and Shading
  return (modeFlags_ & _mode.modeFlags_);
}

DrawMode& DrawMode::operator|=( const DrawMode& _mode2  ) {
  modeFlags_ |= _mode2.modeFlags_;
  
  return (*this);
}

DrawMode& DrawMode::operator&=( const DrawMode& _mode2  ) {
  modeFlags_ &= _mode2.modeFlags_;
  
  return (*this);
}

DrawMode DrawMode::operator|( const DrawMode& _mode2  ) const {
  return( modeFlags_ | _mode2.modeFlags_ );
}

DrawMode DrawMode::operator^( const DrawMode& _mode2  ) const {
  return( modeFlags_ ^ _mode2.modeFlags_ );
}

DrawMode DrawMode::operator~( ) const {
  return( ~modeFlags_ );
}    



unsigned int DrawMode::getIndex() const {
  if ( modeFlags_.count() == 1 ) {
    for ( size_t i = 0 ; i < modeFlags_.size() ; ++i ) 
     if ( modeFlags_[i] ) 
      return i; 
  } 
    
  return 0;
}

std::string DrawMode::description() const
{
  std::string text("");
  
  VecDrawModes::const_iterator modeIter, modeEnd( currentDrawModes_.end() );
  for( modeIter = currentDrawModes_.begin();  modeIter != modeEnd;  ++modeIter )
  {
    if( (*this) & modeIter->id_ )
    {
      if (!text.empty()) text += "+";
      text += modeIter->name_;
    }
  }
  
  return text;
}

//----------------------------------------------------------------------------

std::vector< DrawMode >
DrawMode::getAtomicDrawModes() const
{
  std::vector< DrawMode > draw_mode_ids;
  
  VecDrawModes::const_iterator modeIter, modeEnd( currentDrawModes_.end() );
  for( modeIter = currentDrawModes_.begin();
  modeIter != modeEnd;
  ++modeIter )
  if( (*this) & modeIter->id_ )
    draw_mode_ids.push_back( modeIter->id_ );
  
  return draw_mode_ids;
}


//----------------------------------------------------------------------------

bool DrawMode::isAtomic() const {
  return(modeFlags_.count() == 1 );
}
 

//----------------------------------------------------------------------------
 
bool
DrawMode::containsAtomicDrawMode( DrawMode _atomicDrawMode) const
{
  return (*this) & _atomicDrawMode;
}
 
//---------------------------------------------------------------------------- 
 
unsigned int DrawMode::maxModes() const {
  return (modeFlags_.size() );
}
 
 
 





//----------------------------------------------------------------------------


void initializeDefaultDrawModes( void )
{
    static bool initialized_ = false;

    if( initialized_ )
	return;

    currentDrawModes_.clear();

    currentDrawModes_.push_back( DrawModeInternal( "<invalid>", NONE ) );
    currentDrawModes_.push_back( DrawModeInternal( "Default", DEFAULT ) );

    currentDrawModes_.push_back( DrawModeInternal( "Points", POINTS ) );
    currentDrawModes_.push_back( DrawModeInternal( "Points (colored)", POINTS_COLORED ) );
    currentDrawModes_.push_back( DrawModeInternal( "Points (shaded)", POINTS_SHADED  ) );
    
    currentDrawModes_.push_back( DrawModeInternal( "Edges", EDGES ) );
    currentDrawModes_.push_back( DrawModeInternal( "Edges Colored", EDGES_COLORED ) );
    
    currentDrawModes_.push_back( DrawModeInternal( "Wireframe", WIREFRAME ) );
    
    currentDrawModes_.push_back( DrawModeInternal( "Faces", FACES ) );
    
    currentDrawModes_.push_back( DrawModeInternal( "Hiddenline", HIDDENLINE ) );
    
    currentDrawModes_.push_back( DrawModeInternal( "Solid (flat shaded)", SOLID_FLAT_SHADED ) );
    currentDrawModes_.push_back( DrawModeInternal( "Solid (smooth shaded)", SOLID_SMOOTH_SHADED ) );
    currentDrawModes_.push_back( DrawModeInternal( "Solid (Phong shaded)", SOLID_PHONG_SHADED ) );
    
    currentDrawModes_.push_back( DrawModeInternal( "Solid (colored per-face)", SOLID_FACES_COLORED ) );
    currentDrawModes_.push_back( DrawModeInternal( "Solid (colored per-vertex)", SOLID_POINTS_COLORED ) );
    
    currentDrawModes_.push_back( DrawModeInternal( "Solid (environment mapped)", SOLID_ENV_MAPPED ) );
    
    currentDrawModes_.push_back( DrawModeInternal( "Solid (textured)", SOLID_TEXTURED ) );
    currentDrawModes_.push_back( DrawModeInternal( "Solid (textured, shaded)", SOLID_TEXTURED_SHADED ) );
    
    currentDrawModes_.push_back( DrawModeInternal( "Solid (scalar field)", SOLID_1DTEXTURED ) );
    currentDrawModes_.push_back( DrawModeInternal( "Solid (scalar field, shaded)", SOLID_1DTEXTURED_SHADED ) );
    
    currentDrawModes_.push_back( DrawModeInternal( "Solid (3D textured)", SOLID_3DTEXTURED ) );
    currentDrawModes_.push_back( DrawModeInternal( "Solid (3D textured, shaded)", SOLID_3DTEXTURED_SHADED ) );

    currentDrawModes_.push_back( DrawModeInternal( "Solid (colored per-face, flat shaded)", SOLID_FACES_COLORED_FLAT_SHADED ) );
    currentDrawModes_.push_back( DrawModeInternal( "Solid (colored per-face, smooth shaded)", SOLID_FACES_COLORED_SMOOTH_SHADED ) );

    currentDrawModes_.push_back( DrawModeInternal( "Solid (face textured)", SOLID_2DTEXTURED_FACE ) );
    currentDrawModes_.push_back( DrawModeInternal( "Solid (face textured, shaded)", SOLID_2DTEXTURED_FACE_SHADED ) );
    currentDrawModes_.push_back( DrawModeInternal( "Shader controlled", SOLID_SHADER ) );

    currentDrawModes_.push_back( DrawModeInternal( "Solid (smooth shaded, features)", SOLID_SMOOTH_SHADED_FEATURES ) );

    currentDrawModes_.push_back( DrawModeInternal( "Cells", CELLS ) );
    currentDrawModes_.push_back( DrawModeInternal( "Cells Colored", CELLS_COLORED ) );
    
    currentDrawModes_.push_back( DrawModeInternal( "Halfedges", HALFEDGES ) );
    currentDrawModes_.push_back( DrawModeInternal( "Halfedges Colored", HALFEDGES_COLORED ) );

    firstFreeID_ = UNUSED;
    initialized_ = true;
}


//----------------------------------------------------------------------------


const DrawMode& addDrawMode( const std::string & _name )
{
  // check if mode exists already
  VecDrawModes::iterator modeIter, modeEnd( currentDrawModes_.end() );

  for( modeIter = currentDrawModes_.begin(); modeIter != modeEnd; ++modeIter ) {
    if( _name == modeIter->name_ ) {
      return modeIter->id_;
    }
  }


  // add new mode
  currentDrawModes_.push_back( DrawModeInternal( _name, firstFreeID_ ) );
  ++firstFreeID_;

  return currentDrawModes_[ currentDrawModes_.size() - 1 ].id_;
}


//----------------------------------------------------------------------------


const DrawMode& getDrawMode( const std::string & _name )
{
  // check if mode exists
  VecDrawModes::const_iterator modeIter, modeEnd( currentDrawModes_.end() );

  for( modeIter = currentDrawModes_.begin(); modeIter != modeEnd;  ++modeIter )
  {
    if( _name == modeIter->name_ )
    {
      return modeIter->id_;
    }
  }

  // the DrawMode does not exists
  return DrawModes::NONE;
}

bool drawModeExists(const std::string & _name) {
  
  // check if mode exists
  VecDrawModes::const_iterator modeIter, modeEnd( currentDrawModes_.end() );
  
  for( modeIter = currentDrawModes_.begin();  modeIter != modeEnd;  ++modeIter )
  {
    if( _name == modeIter->name_ )
      return true;
  }
  
  // the DrawMode does not exists
  return false;  
}


DrawMode getDrawModeFromIndex( unsigned int _index ) {
  return DrawMode(_index);
}

//=============================================================================
} // namespace DrawModes
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
