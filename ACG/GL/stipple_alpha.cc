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
//  Stipple Polygon Transparency - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================


#include "stipple_alpha.hh"
#include "gl.hh"


//== NAMESPACES ===============================================================
namespace ACG {
//== IMPLEMENTATION ========================================================== 


const GLubyte stippleMask[17][128] =
{
  /* NOTE: 0% opaqueness is faster to set and probably faster to render with:
	glDisable(GL_POLYGON_STIPPLE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); */
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

  {0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

  {0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0x88, 0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00},

  {0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00},

  {0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00},

  {0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x00},

  {0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x44, 0x44, 0x44, 0x44, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11},

  {0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11},

  {0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55},

  {0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55},

  {0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xee, 0xee, 0xee, 0xee, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55},

  {0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xbb, 0xbb, 0xbb, 0xbb, 0x55, 0x55, 0x55, 0x55},

  {0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55},

  {0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x55},

  {0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xdd, 0xdd, 0xdd, 0xdd, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77},

  {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0x77, 0x77, 0x77, 0x77},

  /* NOTE: 100% opaqueness is faster to set and probably faster to render with:
        glDisable(GL_POLYGON_STIPPLE); */
  {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
};


//-----------------------------------------------------------------------------


void stipple_alpha(float _alpha)
{
  // transparent
  if (_alpha < 1.0)
  {
    if (_alpha < 0.0) _alpha = 0.0;

    glEnable(GL_POLYGON_STIPPLE);
    glPolygonStipple(stippleMask[ (unsigned int)(_alpha * 16.0 + 0.5)]);
  }

  // opaque
  else
  {
    glDisable(GL_POLYGON_STIPPLE);
  }
}


//=============================================================================
} // namespace ACG
//=============================================================================
