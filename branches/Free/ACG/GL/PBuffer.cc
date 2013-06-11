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
//  CLASS PBuffer - IMPLEMENTATION
//
//============================================================================
#ifdef ARCH_LINUX
//=============================================================================


//== INCLUDES =================================================================

#include "PBuffer.hh"
#include <iostream>
#include <cstdlib>


//== IMPLEMENTATION ==========================================================

PBuffer::PBuffer(int _bits)
{
  int n;

  sbAttrib_.clear();
  sbAttrib_.push_back(GLX_DOUBLEBUFFER);  sbAttrib_.push_back(true);
  sbAttrib_.push_back(GLX_RED_SIZE);      sbAttrib_.push_back(_bits);
  sbAttrib_.push_back(GLX_GREEN_SIZE);    sbAttrib_.push_back(_bits);
  sbAttrib_.push_back(GLX_BLUE_SIZE);     sbAttrib_.push_back(_bits);
  sbAttrib_.push_back(GLX_ALPHA_SIZE);    sbAttrib_.push_back(_bits);
  sbAttrib_.push_back(GLX_DEPTH_SIZE);    sbAttrib_.push_back(24);
  sbAttrib_.push_back(GLX_RENDER_TYPE);   sbAttrib_.push_back(GLX_RGBA_BIT);
  sbAttrib_.push_back(GLX_DRAWABLE_TYPE); sbAttrib_.push_back(GLX_PBUFFER_BIT);
  sbAttrib_.push_back(None);


  pbAttrib_.clear();
  pbAttrib_.push_back(GLX_PBUFFER_WIDTH);       pbAttrib_.push_back(100);
  pbAttrib_.push_back(GLX_PBUFFER_HEIGHT);      pbAttrib_.push_back(100);
  pbAttrib_.push_back(GLX_PRESERVED_CONTENTS);  pbAttrib_.push_back(true);
  pbAttrib_.push_back(None);


  // Create the pbuffer
  dpy_      = glXGetCurrentDisplay();
  currctx_  = glXGetCurrentContext();
  currdraw_ = glXGetCurrentDrawable();

  fbc_ = glXChooseFBConfig(dpy_, DefaultScreen(dpy_), &sbAttrib_[0], &n);
  if (!fbc_) 
  {
    std::cerr << "glXChooseFBConfig failed.\n";
    return;
  }

  pbuf_ = glXCreatePbuffer(dpy_, fbc_[0], &pbAttrib_[0]);
  if (!pbuf_) 
  {
    std::cerr << "glXCreatePbuffer failed.\n";
    return;
  }

  pbufctx_ = glXCreateNewContext(dpy_, fbc_[0], GLX_RGBA_TYPE, currctx_, true);
  if (!pbufctx_) 
  {
    std::cerr << "glXCreateNewContextWithConfigSGIX failed.\n";
    return;
  }
}


//-----------------------------------------------------------------------------


PBuffer::PBuffer(int _w, int _h, int _bits)
{
  int n;

  sbAttrib_.clear();
  sbAttrib_.push_back(GLX_DOUBLEBUFFER);  sbAttrib_.push_back(true);
  sbAttrib_.push_back(GLX_RED_SIZE);      sbAttrib_.push_back(_bits);
  sbAttrib_.push_back(GLX_GREEN_SIZE);    sbAttrib_.push_back(_bits);
  sbAttrib_.push_back(GLX_BLUE_SIZE);     sbAttrib_.push_back(_bits);
  sbAttrib_.push_back(GLX_ALPHA_SIZE);    sbAttrib_.push_back(_bits);
  sbAttrib_.push_back(GLX_DEPTH_SIZE);    sbAttrib_.push_back(24);
  sbAttrib_.push_back(GLX_RENDER_TYPE);   sbAttrib_.push_back(GLX_RGBA_BIT);
  sbAttrib_.push_back(GLX_DRAWABLE_TYPE); sbAttrib_.push_back(GLX_PBUFFER_BIT);
  sbAttrib_.push_back(None);

  pbAttrib_.clear();
  pbAttrib_.push_back(GLX_PBUFFER_WIDTH);       pbAttrib_.push_back(_w);
  pbAttrib_.push_back(GLX_PBUFFER_HEIGHT);      pbAttrib_.push_back(_h);
  pbAttrib_.push_back(GLX_PRESERVED_CONTENTS);  pbAttrib_.push_back(true);
  pbAttrib_.push_back(None);


  // Create the pbuffer
  dpy_      = glXGetCurrentDisplay();
  currctx_  = glXGetCurrentContext();
  currdraw_ = glXGetCurrentDrawable();

  fbc_ = glXChooseFBConfig(dpy_, DefaultScreen(dpy_), &sbAttrib_[0], &n);
  if (!fbc_) 
  {
    std::cerr << "glXChooseFBConfig failed.\n";
    return;
  }

  pbuf_ = glXCreatePbuffer(dpy_, fbc_[0], &pbAttrib_[0]);
  if (!pbuf_) 
  {
    std::cerr << "glXCreatePbuffer failed.\n";
    return;
  }

  pbufctx_ = glXCreateNewContext(dpy_, fbc_[0], GLX_RGBA_TYPE, currctx_, true);
  if (!pbufctx_) 
  {
    std::cerr << "glXCreateNewContextWithConfigSGIX failed.\n";
    return;
  }
}


//-----------------------------------------------------------------------------


PBuffer::~PBuffer()
{
  glXDestroyContext(dpy_, currctx_);
  glXDestroyPbuffer(dpy_, pbuf_);
}


//-----------------------------------------------------------------------------


void PBuffer::resize(int _w, int _h)
{
  pbAttrib_[1] = _w;
  pbAttrib_[3] = _h;
  glXDestroyPbuffer(dpy_, pbuf_);
  pbuf_ = glXCreatePbuffer(dpy_, fbc_[0], &pbAttrib_[0]);
  if (!pbuf_) std::cerr << "Resizing pbuffer failed.\n";
}


//-----------------------------------------------------------------------------


int PBuffer::bits()
{
  return sbAttrib_[3];
}


//-----------------------------------------------------------------------------


void PBuffer::activate()
{
  if (!glXMakeCurrent(dpy_, pbuf_, pbufctx_))
    std::cerr << "PBuffer:activate() failed.\n";
}


//-----------------------------------------------------------------------------


void PBuffer::deactivate()
{
  if (!glXMakeCurrent(dpy_, currdraw_, currctx_))
    std::cerr << "PBuffer:deactivate() failed.\n";
}


//-----------------------------------------------------------------------------


const int PBuffer::width() const
{
  return pbAttrib_[1];
}


//-----------------------------------------------------------------------------


const int PBuffer::height() const
{
  return pbAttrib_[3];
}


//=============================================================================
#endif // Linx only
//=============================================================================
