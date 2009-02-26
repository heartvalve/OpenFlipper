//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision: 1720 $
//   $Author: moebius $
//   $Date: 2008-05-09 14:15:53 +0200 (Fri, 09 May 2008) $
//
//=============================================================================




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
