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
//  CLASS LineNode - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "LineNode.hh"
#include <ACG/GL/gl.hh>
#include <ACG/GL/IRenderer.hh>
#include <ACG/GL/VertexDeclaration.hh>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {

//== IMPLEMENTATION ==========================================================

LineNode::LineNode( LineMode     _mode,
                    BaseNode*    _parent,
                     std::string  _name ) :
   MaterialNode(_parent, _name, MaterialNode::BaseColor | MaterialNode::LineWidth),
   line_mode_(_mode),
   draw_always_on_top (false),
   prev_depth_(GL_LESS),
   vbo_(0),
   updateVBO_(true)
{
  drawMode(DrawModes::WIREFRAME);
}

//----------------------------------------------------------------------------

LineNode::~LineNode() {
  if (vbo_)
    glDeleteBuffersARB(1, &vbo_);

}

//----------------------------------------------------------------------------

void LineNode::set_line_mode(LineMode _mode)
{
  // Set the new line mode
  line_mode_ = _mode;

  // Force an update of the vbo
  updateVBO_ = true;
}

//----------------------------------------------------------------------------

void LineNode::clear()
{
  clear_points();
  clear_colors();
}

//----------------------------------------------------------------------------

void LineNode::clear_points()
{
  points_.clear();

  // Force an update of the vbo
  updateVBO_ = true;
}

//----------------------------------------------------------------------------

void LineNode::clear_colors()
{
  colors_.clear();
  colors4f_.clear();

  // Force an update of the vbo
  updateVBO_ = true;
}

//----------------------------------------------------------------------------

void LineNode::set_color(const Vec4f& _c)
{
  clear_colors();
  add_color(ACG::Vec3uc((char) (((int) _c[0]) * 255),
                        (char) (((int) _c[1]) * 255),
                        (char) (((int) _c[2]) * 255)));
  MaterialNode::set_color(_c);
}

//----------------------------------------------------------------------------

void LineNode::add_point(const Vec3d& _v)
{
  points_.push_back(_v);

  // Force an update of the vbo
  updateVBO_ = true;
}

//----------------------------------------------------------------------------

void LineNode::add_line(const Vec3d& _v0, const Vec3d& _v1)
{
  add_point(_v0);
  add_point(_v1);

  // Force an update of the vbo
  updateVBO_ = true;
}

//----------------------------------------------------------------------------

void LineNode::add_color(const ACG::Vec3uc& _c)
{
  colors_.push_back(_c);

  // Force an update of the vbo
  updateVBO_ = true;
}

//----------------------------------------------------------------------------

void LineNode::add_color(const Color4f _c)
{
  colors4f_.push_back(_c);

  // Force an update of the vbo
  updateVBO_ = true;
}

//----------------------------------------------------------------------------

void
LineNode::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  ConstPointIter p_it=points_.begin(), p_end=points_.end();
  for (; p_it!=p_end; ++p_it)
  {
      _bbMax.maximize(*p_it);
      _bbMin.minimize(*p_it);
  }
}


//----------------------------------------------------------------------------


DrawModes::DrawMode
LineNode::
availableDrawModes() const
{
  return DrawModes::WIREFRAME;
}


//----------------------------------------------------------------------------

void
LineNode::
enter(GLState& _state , const DrawModes::DrawMode& _drawMode)
{
    MaterialNode::enter(_state, _drawMode);
    
    if (alwaysOnTop()) {
	//store current depth comparison function (needed for lasso selection)
	prev_depth_ = _state.depthFunc();
	
	//set depth function and change GLState accordingly
	_state.set_depthFunc(GL_ALWAYS);
    }
}

//----------------------------------------------------------------------------

void
LineNode::
draw(GLState&  _state  , const DrawModes::DrawMode& _drawMode)
{
  if (_drawMode & DrawModes::WIREFRAME)
  {
    ACG::GLState::disable(GL_LIGHTING);


   // if (line_mode_ == LineSegmentsMode)
   //   glBegin(GL_LINES);
   // else
   //   glBegin(GL_LINE_STRIP);


    if (line_mode_ == LineSegmentsMode)
    {
      // first check if (new standard) 4-channel colors are specified
      if( (points_.size()/2 == colors4f_.size()) )
      {
        // enable blending of lines
        GLboolean blendb;
        glGetBooleanv( GL_BLEND, &blendb);
        glEnable(GL_BLEND);

        // blend func to blend alpha values of lines
        GLint sblendfunc;
        GLint dblendfunc;
        glGetIntegerv( GL_BLEND_SRC_ALPHA, &sblendfunc );
        glGetIntegerv( GL_BLEND_DST_ALPHA, &dblendfunc );
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // blend ontop of prev. drawn mesh
        GLboolean depthmaskb;
        glGetBooleanv( GL_DEPTH_WRITEMASK, &depthmaskb);
        glDepthMask(GL_FALSE);

        glBegin(GL_LINES);

        ConstPointIter p_it=points_.begin(), p_end=points_.end();
        ConstColor4fIter c_it=colors4f_.begin();

        Color4f c(1.0f,1.0f,1.0f,1.0f);
        if(c_it != colors4f_.end()) {
          c = *c_it;
        }

        int cnt = 0;
        for (; p_it!=p_end; ++p_it)
        {
          if ((cnt > 0) && (cnt % 2 == 0) && (c_it+1) != colors4f_.end()) {
            ++c_it;
            c = *c_it;
          }

          glColor(c);
          glVertex(*p_it);

          ++cnt;
        }

        glEnd();

        // disable blending of lines
        if( blendb == GL_FALSE )
          glDisable(GL_BLEND);

        // reset blend func
        glBlendFunc( sblendfunc, dblendfunc );

        // enable depth mask
        if( depthmaskb == GL_TRUE )
          glDepthMask(GL_TRUE);

      }
      else if ((line_mode_ == LineSegmentsMode) && (points_.size()/2 == colors_.size()) )
      {
        glBegin(GL_LINES);
        ConstPointIter p_it=points_.begin(), p_end=points_.end();
        ConstColorIter c_it=colors_.begin();

        Color c((char)255, (char)255, (char)255);
        if(c_it != colors_.end()) {
          c = *c_it;
        }

        int cnt = 0;
        for (; p_it!=p_end; ++p_it)
        {
          if ((cnt > 0) && (cnt % 2 == 0) && (c_it+1) != colors_.end()) {
            ++c_it;
            c = *c_it;
          }

          glColor(c);
          glVertex(*p_it);

          ++cnt;
        }
        glEnd();
      }
      else
      {
        glBegin(GL_LINES);

        ConstPointIter p_it=points_.begin(), p_end=points_.end();

        for (; p_it!=p_end; ++p_it)
        {
          glVertex(*p_it);
        }

        glEnd();
      }
    }
    else
    {
      _state.set_color(_state.base_color());
      glBegin(GL_LINE_STRIP);
      ConstPointIter p_it=points_.begin(), p_end=points_.end();
      for (; p_it!=p_end; ++p_it)
        glVertex(*p_it);
      glEnd();
    }

    //glEnd();
  }
}
  
//----------------------------------------------------------------------------

void
LineNode::
leave(GLState& _state , const DrawModes::DrawMode& _drawMode)
{
  if (alwaysOnTop()) {
    //restore depth function and change GLState accordingly
    _state.set_depthFunc(prev_depth_);
  }

  MaterialNode::leave(_state, _drawMode);
}

//----------------------------------------------------------------------------

void
LineNode::
getRenderObjects(IRenderer* _renderer, GLState&  _state , const DrawModes::DrawMode&  _drawMode , const ACG::SceneGraph::Material* _mat) {

  // TODO: Implement in Line Node

  // init base render object
  RenderObject ro;
  ro.initFromState(&_state);
  ro.setMaterial(_mat);

  ro.debugName = "LineNode";

  // draw after scene-meshes
  ro.priority = 1;

  if (line_mode_ == LineSegmentsMode)
  {

    if( (points_.size()/2 == colors4f_.size()) ) {
      // One color entry per line segment (alpha channel available )

      // TODO : Implement this mode
    } else if ( points_.size()/2 == colors_.size() ) {
      // One color entry per line segment (no alpha channel available and uchars as colors)

      std::cerr << "Todo with old style colors" <<std::endl;
      // TODO : Implement this mode
    } else {
      // No colors. Just draw the segments

      std::cerr << "Todo without colors" <<std::endl;

      // TODO : Implement this mode
    }


  } else {
    // No colors (Use material) and one continuous line


    // create vbo if it does not exist
    if (!vbo_)
      glGenBuffersARB(1, &vbo_);

    float*       vboData_ = NULL;

    // Update the vbo only if required.
    if ( updateVBO_ ) {

      // Create the required array
      vboData_ = new float[3 * points_.size() * 4];

      // Pointer to it for easier copy operation
      float* pPoints = &vboData_[0];

      // Copy from internal storage to vbo in memory
      for (unsigned int  i = 0 ; i < points_.size(); ++i) {
        for ( unsigned int j = 0 ; j < 3 ; ++j) {
          *(pPoints++) = points_[i][j];
        }
      }

      // Move data to the buffer
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_);
      glBufferDataARB(GL_ARRAY_BUFFER_ARB, 3 * points_.size() * 4 , vboData_ , GL_STATIC_DRAW_ARB);

      // Remove the local storage
      delete[] vboData_;

      // Update done.
      updateVBO_ = false;
    }

    ro.vertexBuffer = vbo_;

    // decl must be static or member,  renderer does not make a copy
    static VertexDeclaration vertexDecl;
    vertexDecl.clear();
    vertexDecl.addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION);

    ro.vertexDecl = &vertexDecl;

    ro.glDrawArrays(GL_LINE_STRIP, 0, points_.size() );

    _renderer->addRenderObject(&ro);


  }

}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
