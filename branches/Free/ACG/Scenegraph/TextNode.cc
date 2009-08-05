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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS TextNode - IMPLEMENTATION
//
//=============================================================================

#ifdef USE_FTGL

//== INCLUDES =================================================================

#include "TextNode.hh"
#include "../GL/gl.hh"
#include <stdlib.h>
#include <FTGL/ftgl.h>				//<-- if these are moved to the .hh every plugin that includes the .hh has to link with FTGL
#include <FTGL/FTGLPixmapFont.h>
#include <FTGL/FTGLPolygonFont.h>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================

TextNode::
~TextNode()
{
	if ( textMode_ == SCREEN_ALIGNED ) {
		if(font_ != 0)
			delete reinterpret_cast<FTGLPixmapFont*>(font_);
	}else if ( textMode_ == OBJECT_ALIGNED ) {
		if(font_ != 0)
			delete reinterpret_cast<FTGLPolygonFont*>(font_);
	}
}

void
TextNode::
boundingBox(Vec3f& /*_bbMin*/, Vec3f& /*_bbMax*/)
{
}


//----------------------------------------------------------------------------


unsigned int
TextNode::
availableDrawModes() const
{
  return ( DrawModes::POINTS |
	        DrawModes::POINTS_SHADED |
	        DrawModes::POINTS_COLORED );
}


//----------------------------------------------------------------------------


void
TextNode::
draw(GLState& /* _state */ , unsigned int /*_drawMode*/)
{
  glRasterPos3f(0.0f, 0.0f, 0.0f);

  if ( textMode_ == SCREEN_ALIGNED ) {
	if(font_ == 0)
	  return;
	reinterpret_cast<FTGLPixmapFont*>(font_)->Render(text_.c_str());
  }

  if ( textMode_ == OBJECT_ALIGNED ) {
    glScalef(0.1,0.1,0.1);

    if(font_ == 0)
    	return;
    reinterpret_cast<FTGLPolygonFont*>(font_)->Render(text_.c_str());

    glScalef(10.0,10.0,10.0);
  }

}


//----------------------------------------------------------------------------


void
TextNode::
UpdateFont()
{
	if ( textMode_ == SCREEN_ALIGNED ) {
		if(font_ != 0)
			delete reinterpret_cast<FTGLPixmapFont*>(font_);
		font_ = reinterpret_cast<void*>(new FTGLPixmapFont(fontFile_.c_str()));
		reinterpret_cast<FTGLPixmapFont*>(font_)->FaceSize(size_);
		if(reinterpret_cast<FTGLPixmapFont*>(font_)->Error())
		{
			std::cerr << "Unable to find font: " << fontFile_.c_str() << std::endl;
			delete reinterpret_cast<FTGLPixmapFont*>(font_);
			font_ = 0;
		}
	}else if ( textMode_ == OBJECT_ALIGNED ) {
		if(font_ != 0)
			delete reinterpret_cast<FTGLPolygonFont*>(font_);
		font_ = reinterpret_cast<void*>(new FTGLPolygonFont(fontFile_.c_str()));
		reinterpret_cast<FTGLPolygonFont*>(font_)->FaceSize(size_);
		if(reinterpret_cast<FTGLPolygonFont*>(font_)->Error())
		{
			std::cerr << "Unable to find font: " << fontFile_.c_str() << std::endl;
			delete reinterpret_cast<FTGLPolygonFont*>(font_);
			font_ = 0;
		}
	}
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================

#endif // USE_FTGL
