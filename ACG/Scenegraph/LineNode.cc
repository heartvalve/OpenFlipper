/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
#include <ACG/GL/acg_glew.hh>
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
   updateVBO_(true),
   lineNodeName_("")
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

void LineNode::createVBO()
{
  if (!updateVBO_)
    return;

  // create vbo if it does not exist
  if (!vbo_)
    glGenBuffersARB(1, &vbo_);

  vertexDecl_.clear();
  vertexDecl_.addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION);

  //3 coordinates per vertex
  std::vector<float> vboData(3*points_.size(),0.f);

  if (line_mode_ == LineSegmentsMode)
  {
    if( (points_.size()/2 == colors4f_.size()) )
    {
      //   === One color entry per line segment (alpha channel available ) ===
      vertexDecl_.addElement(GL_FLOAT, 4, VERTEX_USAGE_COLOR);
      vboData.resize(vboData.size() + 4 * points_.size());
      float* vboPtr = &vboData[0];

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
        //add position information
        *(vboPtr++) = (*p_it)[0];
        *(vboPtr++) = (*p_it)[1];
        *(vboPtr++) = (*p_it)[2];

        //add color information
        *(vboPtr++) = c[0];
        *(vboPtr++) = c[1];
        *(vboPtr++) = c[2];
        *(vboPtr++) = c[3];

        ++cnt;
      }

      //====================
    } else if ( points_.size()/2 == colors_.size() )
    {
      //=== One color entry per line segment (no alpha channel available and uchars as colors) ===
      vertexDecl_.addElement(GL_FLOAT, 4, VERTEX_USAGE_COLOR);
      //add 4 colors for each vertex
      vboData.resize(vboData.size() + 4 * points_.size());
      float* vboPtr = &vboData[0];

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

        //add position information
        *(vboPtr++) = (*p_it)[0];
        *(vboPtr++) = (*p_it)[1];
        *(vboPtr++) = (*p_it)[2];

        //add color information
        *(vboPtr++) = c[0]/255.f;
        *(vboPtr++) = c[1]/255.f;
        *(vboPtr++) = c[2]/255.f;
        *(vboPtr++) = 1.f;

        ++cnt;
      }

      //===========
    } else
    {
      //=== No colors. Just draw the segments ===
      ConstPointIter p_it=points_.begin(), p_end=points_.end();
      float* vboPtr = &vboData[0];

      for (; p_it!=p_end; ++p_it)
      {
        *(vboPtr++) = (*p_it)[0];
        *(vboPtr++) = (*p_it)[1];
        *(vboPtr++) = (*p_it)[2];
      }
      //===========
    }


  }
  else
  {
    // === No colors (Use material) and one continuous line ===
    // Pointer to it for easier copy operation
    float* pPoints = &vboData[0];

    // Copy from internal storage to vbo in memory
    for (unsigned int  i = 0 ; i < points_.size(); ++i) {
      for ( unsigned int j = 0 ; j < 3 ; ++j) {
        *(pPoints++) = points_[i][j];
      }
    }
  }

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_);
  glBufferDataARB(GL_ARRAY_BUFFER_ARB, vboData.size()*sizeof(float) , &vboData[0] , GL_STATIC_DRAW_ARB);

  // Update done.
  updateVBO_ = false;

}

void
LineNode::
getRenderObjects(IRenderer* _renderer, GLState&  _state , const DrawModes::DrawMode&  _drawMode , const ACG::SceneGraph::Material* _mat) {

  if (points_.empty())
    return;

  // init base render object

  RenderObject ro;
  ro.initFromState(&_state);
  ro.setMaterial(_mat);

  lineNodeName_ = std::string("LineNode: ")+name();
  ro.debugName = lineNodeName_.c_str();

  // draw after scene-meshes
  if (draw_always_on_top)
  {
    ro.priority = 1;
    ro.depthTest = false;
    ro.depthWrite = false;
  }
  else
  {
    ro.depthTest = true;
    ro.depthWrite = true;
  }

  //set blending
  if ((line_mode_ == LineSegmentsMode) && (points_.size()/2 == colors4f_.size()))
  {
    ro.blending = true;
    ro.blendSrc = GL_SRC_ALPHA;
    ro.blendDest = GL_ONE_MINUS_SRC_ALPHA;
  }

  // simulate line width via quad extrusion in geometry shader
  QString geomTemplate = ShaderProgGenerator::getShaderDir();
  geomTemplate += "Wireframe/geom_line2quad.tpl";

  ro.shaderDesc.geometryTemplateFile = geomTemplate;

  ro.setUniform("screenSize", Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));
  ro.setUniform("lineWidth", _state.line_width());

  createVBO();
  ro.vertexBuffer = vbo_;
  // vertexDecl is defined in createVBO
  ro.vertexDecl = &vertexDecl_;

  //besides of the position, colors are saved so we can show them
  if (vertexDecl_.getNumElements() > 1)
    ro.shaderDesc.vertexColors = true;


  if (line_mode_ == LineSegmentsMode)
    ro.glDrawArrays(GL_LINES, 0, int( points_.size() ));
  else
    ro.glDrawArrays(GL_LINE_STRIP, 0, int(points_.size()) );

  _renderer->addRenderObject(&ro);

}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
