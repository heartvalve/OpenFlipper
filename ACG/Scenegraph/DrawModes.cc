/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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


//== IMPLEMENTATION ========================================================== 


/** Definition of a draw mode */
class DrawMode {
	
public:
    /** Default constructor. */
    DrawMode() {}
    
    /** Initializing constructor. */
    DrawMode( const std::string & _name, const int & _id )
	: name_( _name ), id_( _id ) {}
    
    std::string name_;
    unsigned int id_;
};

typedef std::vector< DrawMode > VecDrawModes;

/** Vector of all currently defined DrawModes.
*/
static VecDrawModes currentDrawModes_;

/** First free DrawMode ID for custom modes. */
static unsigned int firstFreeID_;



//----------------------------------------------------------------------------


std::string description(unsigned int _drawMode)
{
  /* HACK TODO!! Required when using plugins under windows. ACG as static lib
	links a static drawmodes for each plugins->only one initialized.
	Solution : Build ACG as dll under windows */
 // if ( currentDrawModes_.empty() )
	//initializeDefaultDrawModes();
  

  std::string text("");
  
  VecDrawModes::iterator modeIter, modeEnd( currentDrawModes_.end() );
  for( modeIter = currentDrawModes_.begin();
       modeIter != modeEnd;
       ++modeIter )
  {
    if( _drawMode & modeIter->id_ )
    {
      if (!text.empty()) text += "+";
      text += modeIter->name_;
    }
  }

  return text;
}
 

//----------------------------------------------------------------------------

 
std::vector< unsigned int >
getDrawModeIDs( unsigned int _drawModes )
{
  /* HACK TODO!! Required when using plugins under windows. ACG as static lib
	links a static drawmodes for each plugins->only one initialized.
	Solution : Build ACG as dll under windows */
 // if ( currentDrawModes_.empty() )
//	initializeDefaultDrawModes();

  std::vector< unsigned int > draw_mode_ids;

  VecDrawModes::iterator modeIter, modeEnd( currentDrawModes_.end() );
  for( modeIter = currentDrawModes_.begin();
       modeIter != modeEnd;
       ++modeIter )
    if( _drawModes & modeIter->id_ )
      draw_mode_ids.push_back( modeIter->id_ );

  return draw_mode_ids;
}


//----------------------------------------------------------------------------

 
bool
containsId( unsigned int _drawMode, unsigned int _id )
{
  /* HACK TODO!! Required when using plugins under windows. ACG as static lib
	links a static drawmodes for each plugins->only one initialized.
	Solution : Build ACG as dll under windows */
 // if ( currentDrawModes_.empty() )
//	initializeDefaultDrawModes();

  return _drawMode & _id;
}


//----------------------------------------------------------------------------


void initializeDefaultDrawModes( void )
{
    static bool initialized_ = false;

    if( initialized_ )
	return;

    currentDrawModes_.clear();

    currentDrawModes_.push_back( DrawMode( "<invalid>", NONE ) );
    currentDrawModes_.push_back( DrawMode( "Default", DEFAULT ) );

    currentDrawModes_.push_back( DrawMode( "Points", POINTS ) );
    currentDrawModes_.push_back( DrawMode( "Points (colored)", POINTS_COLORED ) );
    currentDrawModes_.push_back( DrawMode( "Points (shaded)", POINTS_SHADED  ) );
    
    currentDrawModes_.push_back( DrawMode( "Wireframe", WIREFRAME ) );
    currentDrawModes_.push_back( DrawMode( "Hiddenline", HIDDENLINE ) );
    
    currentDrawModes_.push_back( DrawMode( "Solid (flat shaded)", SOLID_FLAT_SHADED ) );
    currentDrawModes_.push_back( DrawMode( "Solid (smooth shaded)", SOLID_SMOOTH_SHADED ) );
    currentDrawModes_.push_back( DrawMode( "Solid (Phong shaded)", SOLID_PHONG_SHADED ) );
    
    currentDrawModes_.push_back( DrawMode( "Solid (colored per-face)", SOLID_FACES_COLORED ) );
    currentDrawModes_.push_back( DrawMode( "Solid (colored per-vertex)", SOLID_POINTS_COLORED ) );
    
    currentDrawModes_.push_back( DrawMode( "Solid (environment mapped)", SOLID_ENV_MAPPED ) );
    
    currentDrawModes_.push_back( DrawMode( "Solid (textured)", SOLID_TEXTURED ) );
    currentDrawModes_.push_back( DrawMode( "Solid (textured, shaded)", SOLID_TEXTURED_SHADED ) );
    
    currentDrawModes_.push_back( DrawMode( "Solid (scalar field)", SOLID_1DTEXTURED ) );
    currentDrawModes_.push_back( DrawMode( "Solid (scalar field, shaded)", SOLID_1DTEXTURED_SHADED ) );
    
    currentDrawModes_.push_back( DrawMode( "Solid (3D textured)", SOLID_3DTEXTURED ) );
    currentDrawModes_.push_back( DrawMode( "Solid (3D textured, shaded)", SOLID_3DTEXTURED_SHADED ) );

    currentDrawModes_.push_back( DrawMode( "Solid (colored per-face, flat shaded)", SOLID_FACES_COLORED_FLAT_SHADED ) );

    currentDrawModes_.push_back( DrawMode( "Solid (face textured)", SOLID_2DTEXTURED_FACE ) );
    currentDrawModes_.push_back( DrawMode( "Solid (face textured, shaded)", SOLID_2DTEXTURED_FACE_SHADED ) );
    currentDrawModes_.push_back( DrawMode( "Shader controlled", SOLID_SHADER ) );
    
    firstFreeID_ = UNUSED;
    initialized_ = true;
}


//----------------------------------------------------------------------------


bool addDrawMode( const std::string & _name, unsigned int & _newId )
{
    // check if mode exists already
    VecDrawModes::iterator modeIter, modeEnd( currentDrawModes_.end() );

    for( modeIter = currentDrawModes_.begin();
	 modeIter != modeEnd;
	 ++modeIter )
    {
	if( _name == modeIter->name_ )
	    return modeIter->id_;
    }

    // check if there is another ID available
    if( firstFreeID_ == (unsigned int) (1<<31) )
	return false;

    // add new mode
    _newId = firstFreeID_;
    currentDrawModes_.push_back( DrawMode( _name, _newId ) );
    firstFreeID_ <<= 1;

    return true;
}


//----------------------------------------------------------------------------


bool getDrawModeId( const std::string & _name, unsigned int & _Id  )
{
    // check if mode exists
    VecDrawModes::iterator modeIter, modeEnd( currentDrawModes_.end() );

    for( modeIter = currentDrawModes_.begin();
	 modeIter != modeEnd;
	 ++modeIter )
    {
       if( _name == modeIter->name_ )
       {
	   _Id = modeIter->id_;
           return true;
       }
    }

    // the DrawMode does not exists
    return false;
}


//=============================================================================
} // namespace DrawModes
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
