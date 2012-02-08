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


// == Default Draw Mode initialization ======================================

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
DrawMode SOLID_POINTS_COLORED_SHADED       = ModeFlagSet(1) << 14;
DrawMode SOLID_ENV_MAPPED                  = ModeFlagSet(1) << 15;
DrawMode SOLID_TEXTURED                    = ModeFlagSet(1) << 16;
DrawMode SOLID_TEXTURED_SHADED             = ModeFlagSet(1) << 17;
DrawMode SOLID_1DTEXTURED                  = ModeFlagSet(1) << 18;
DrawMode SOLID_1DTEXTURED_SHADED           = ModeFlagSet(1) << 19;
DrawMode SOLID_3DTEXTURED                  = ModeFlagSet(1) << 20;
DrawMode SOLID_3DTEXTURED_SHADED           = ModeFlagSet(1) << 21;
DrawMode SOLID_FACES_COLORED_FLAT_SHADED   = ModeFlagSet(1) << 22;
DrawMode SOLID_FACES_COLORED_SMOOTH_SHADED = ModeFlagSet(1) << 23;
DrawMode SOLID_2DTEXTURED_FACE             = ModeFlagSet(1) << 24;
DrawMode SOLID_2DTEXTURED_FACE_SHADED      = ModeFlagSet(1) << 25;
DrawMode SOLID_SHADER                      = ModeFlagSet(1) << 26;
DrawMode SOLID_SMOOTH_SHADED_FEATURES      = ModeFlagSet(1) << 27;
DrawMode CELLS                             = ModeFlagSet(1) << 28;
DrawMode CELLS_COLORED                     = ModeFlagSet(1) << 29;
DrawMode HALFEDGES                         = ModeFlagSet(1) << 30;
DrawMode HALFEDGES_COLORED                 = ModeFlagSet(1) << 31;
DrawMode UNUSED                            = ModeFlagSet(1) << 32;
  

//== IMPLEMENTATION ========================================================== 


/** \brief Definition of a draw mode
 *
 * This class is used to collect Information about a DrawMode in one central class.
 * It stores the name and the id of a DrawMode.
 *
 * If the DrawMode gets switched to a property based one, this class will also
 * store the properties.
 */
class DrawModeInternal {
  
  public:

    /** Initializing constructor. */
    DrawModeInternal(const std::string & _name, const DrawMode & _id, const bool _propertyBased = false) :
      name_(_name),
      id_(_id),
      propertyBased_(_propertyBased)
    {
    }

    /// Set the name of the DrawMode
    void name(const std::string& _name) {
      name_ = _name;
    }

    /// Get the name of the DrawMode
    const std::string& name() const {
      return name_;
    }
    
    const DrawMode& id() const {
      return id_;
    }

    bool propertyBased() const {
      return propertyBased_;
    }


    DrawModeProperties& properties() {
      return properties_;
    }

  private:
    std::string        name_;          ///< Human Readable Name
    DrawMode           id_;            ///< The id of the DrawMode
    bool               propertyBased_; ///< Flag if the DrawMode is property based
    DrawModeProperties properties_;    ///< The properties associated with this DrawMode
};


typedef std::vector< DrawModeInternal > VecDrawModes;

/** Vector of all currently defined DrawModes.
*/
static VecDrawModes registeredDrawModes_;

/** First free DrawMode ID for custom modes. */
static DrawMode firstFreeID_;



DrawModeProperties::DrawModeProperties() {

}


DrawMode::DrawMode(unsigned int _index)
{
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

bool DrawMode::propertyBased() const {
  if ( isAtomic() ) {
    return registeredDrawModes_[getIndex()].propertyBased();
  } else {
    // Combined drawmode, iterate over all contained modes and return
    VecDrawModes::const_iterator modeIter, modeEnd( registeredDrawModes_.end() );
    for( modeIter = registeredDrawModes_.begin();  modeIter != modeEnd;  ++modeIter )
      if( ((*this) & modeIter->id()) && modeIter->propertyBased() )
        return true;

    return false;
  }
}

DrawModeProperties* DrawMode::drawModeProperties() {
  if ( isAtomic() ) {
    return &(registeredDrawModes_[getIndex()].properties());
  } else {
    return 0;
  }
}

bool DrawMode::operator==(const DrawMode& _mode) const {
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
  
  VecDrawModes::const_iterator modeIter, modeEnd( registeredDrawModes_.end() );
  for( modeIter = registeredDrawModes_.begin();  modeIter != modeEnd;  ++modeIter )
  {
    if( (*this) & modeIter->id() )
    {
      if (!text.empty()) text += "+";
      text += modeIter->name();
    }
  }
  
  return text;
}

//----------------------------------------------------------------------------

std::vector< DrawMode >
DrawMode::getAtomicDrawModes() const
{
  std::vector< DrawMode > draw_mode_ids;
  
  VecDrawModes::const_iterator modeIter, modeEnd( registeredDrawModes_.end() );
  for( modeIter = registeredDrawModes_.begin();
  modeIter != modeEnd;
  ++modeIter )
  if( (*this) & modeIter->id() )
    draw_mode_ids.push_back( modeIter->id() );
  
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

    registeredDrawModes_.clear();

    registeredDrawModes_.push_back( DrawModeInternal( "<invalid>", NONE ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Default", DEFAULT ) );

    registeredDrawModes_.push_back( DrawModeInternal( "Points", POINTS ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Points (colored)", POINTS_COLORED ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Points (shaded)", POINTS_SHADED  ) );
    
    registeredDrawModes_.push_back( DrawModeInternal( "Edges", EDGES ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Edges Colored", EDGES_COLORED ) );
    
    registeredDrawModes_.push_back( DrawModeInternal( "Wireframe", WIREFRAME ) );
    
    registeredDrawModes_.push_back( DrawModeInternal( "Faces", FACES ) );
    
    registeredDrawModes_.push_back( DrawModeInternal( "Hiddenline", HIDDENLINE ) );
    
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (flat shaded)", SOLID_FLAT_SHADED ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (smooth shaded)", SOLID_SMOOTH_SHADED ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (Phong shaded)", SOLID_PHONG_SHADED ) );
    
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (colored per-face)", SOLID_FACES_COLORED ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (colored per-vertex)", SOLID_POINTS_COLORED ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (colored per-vertex, shaded)", SOLID_POINTS_COLORED_SHADED ) );
    
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (environment mapped)", SOLID_ENV_MAPPED ) );
    
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (textured)", SOLID_TEXTURED ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (textured, shaded)", SOLID_TEXTURED_SHADED ) );
    
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (scalar field)", SOLID_1DTEXTURED ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (scalar field, shaded)", SOLID_1DTEXTURED_SHADED ) );
    
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (3D textured)", SOLID_3DTEXTURED ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (3D textured, shaded)", SOLID_3DTEXTURED_SHADED ) );

    registeredDrawModes_.push_back( DrawModeInternal( "Solid (colored per-face, flat shaded)", SOLID_FACES_COLORED_FLAT_SHADED ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (colored per-face, smooth shaded)", SOLID_FACES_COLORED_SMOOTH_SHADED ) );

    registeredDrawModes_.push_back( DrawModeInternal( "Solid (face textured)", SOLID_2DTEXTURED_FACE ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Solid (face textured, shaded)", SOLID_2DTEXTURED_FACE_SHADED ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Shader controlled", SOLID_SHADER ) );

    registeredDrawModes_.push_back( DrawModeInternal( "Solid (smooth shaded, features)", SOLID_SMOOTH_SHADED_FEATURES ) );

    registeredDrawModes_.push_back( DrawModeInternal( "Cells", CELLS ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Cells Colored", CELLS_COLORED ) );
    
    registeredDrawModes_.push_back( DrawModeInternal( "Halfedges", HALFEDGES ) );
    registeredDrawModes_.push_back( DrawModeInternal( "Halfedges Colored", HALFEDGES_COLORED ) );

    firstFreeID_ = UNUSED;
    initialized_ = true;
}


//----------------------------------------------------------------------------


const DrawMode& addDrawMode( const std::string & _name , bool _propertyBased)
{
  // check if mode exists already
  VecDrawModes::iterator modeIter, modeEnd( registeredDrawModes_.end() );

  for( modeIter = registeredDrawModes_.begin(); modeIter != modeEnd; ++modeIter ) {
    if( _name == modeIter->name() ) {
      return modeIter->id();
    }
  }


  // add new mode
  registeredDrawModes_.push_back( DrawModeInternal( _name, firstFreeID_ , _propertyBased) );
  ++firstFreeID_;

  return registeredDrawModes_[ registeredDrawModes_.size() - 1 ].id();
}


//----------------------------------------------------------------------------


const DrawMode& getDrawMode( const std::string & _name )
{
  // check if mode exists
  VecDrawModes::const_iterator modeIter, modeEnd( registeredDrawModes_.end() );

  for( modeIter = registeredDrawModes_.begin(); modeIter != modeEnd;  ++modeIter )
  {
    if( _name == modeIter->name() )
    {
      return modeIter->id();
    }
  }

  // the DrawMode does not exists
  return DrawModes::NONE;
}

bool drawModeExists(const std::string & _name) {
  
  // check if mode exists
  VecDrawModes::const_iterator modeIter, modeEnd( registeredDrawModes_.end() );
  
  for( modeIter = registeredDrawModes_.begin();  modeIter != modeEnd;  ++modeIter )
  {
    if( _name == modeIter->name() )
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
