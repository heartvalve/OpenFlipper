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
//  CLASS FastMeshNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "OBJNode.hh"
#include "../GL/gl.hh"

#include <cstdio>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ========================================================== 


void
OBJNode::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax)
{
  Vec3f bbMin(FLT_MAX,FLT_MAX,FLT_MAX);
  Vec3f bbMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
  
  for(unsigned int i=0; i<vertices_.size(); ++i)
  {
    bbMin.minimize(vertices_[i]);
    bbMax.maximize(vertices_[i]);
  }
  
  Vec3d bbMind = ACG::Vec3d(bbMin);
  Vec3d bbMaxd = ACG::Vec3d(bbMax);
  
  _bbMin.minimize(bbMind);
  _bbMax.maximize(bbMaxd);  

}


//----------------------------------------------------------------------------

  
  DrawModes::DrawMode 
OBJNode::
availableDrawModes() const
{
  return ( DrawModes::POINTS                 |
	   DrawModes::WIREFRAME              |
	   DrawModes::HIDDENLINE             |
	   DrawModes::SOLID_FLAT_SHADED      |
	   DrawModes::SOLID_TEXTURED         );
}


//----------------------------------------------------------------------------


void
OBJNode::
draw(GLState& _state, const DrawModes::DrawMode& _drawMode)
{
  if (_drawMode & DrawModes::POINTS)
  {
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    draw_obj();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  if (_drawMode & DrawModes::WIREFRAME)
  {
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    draw_obj();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  else if (_drawMode & DrawModes::HIDDENLINE)
  {
    Vec4f base_color_backup = _state.base_color();

    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor(_state.clear_color());    
    ACG::GLState::depthRange(0.01, 1.0);
    draw_obj();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor(base_color_backup);
    ACG::GLState::depthRange(0.0, 1.0);
    draw_obj();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  else if (_drawMode & DrawModes::SOLID_FLAT_SHADED)
  {
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    draw_obj();
  }


  else if (_drawMode & DrawModes::SOLID_TEXTURED)
  {
    ACG::GLState::enable( GL_TEXTURE_2D );    
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    draw_obj_tex();
    ACG::GLState::disable( GL_TEXTURE_2D );    
  }
}


//----------------------------------------------------------------------------


void
OBJNode::draw_obj() const
{
  glBegin(GL_TRIANGLES);

  for(unsigned int i=0; i<faces_.size(); ++i)
  {
    glNormal(normals_[i]);      
    glVertex(vertices_[faces_[i].i0]);    
    glVertex(vertices_[faces_[i].i1]);            
    glVertex(vertices_[faces_[i].i2]);
  }

  glEnd();
}


//----------------------------------------------------------------------------


void
OBJNode::draw_obj_tex() const
{
  glBegin(GL_TRIANGLES);

  for(unsigned int i=0; i<faces_.size(); ++i)
  {
    glNormal(normals_[i]);      
    if (faces_[i].t0 != -1)
    {
      glTexCoord(texCoords_[faces_[i].t0]);
      glVertex(vertices_[faces_[i].i0]);    
      glTexCoord(texCoords_[faces_[i].t1]);
      glVertex(vertices_[faces_[i].i1]);            
      glTexCoord(texCoords_[faces_[i].t2]);
      glVertex(vertices_[faces_[i].i2]);
    }
    else
    {
      glVertex(vertices_[faces_[i].i0]);    
      glVertex(vertices_[faces_[i].i1]);            
      glVertex(vertices_[faces_[i].i2]);
    }
  }

  glEnd();
}


//----------------------------------------------------------------------------


void
OBJNode::pick(GLState& _state, PickTarget _target)
{
  switch (_target)
  {
    case PICK_VERTEX:
    {
      break;
    }
    case PICK_EDGE:
    {
      break;
    }
    case PICK_ANYTHING:
    case PICK_FACE:
    {
      _state.pick_set_maximum (1);
      _state.pick_set_name (0);
      draw_obj();
      break; 
    }

    default: // avoid warning
      break;
  }      
}


//----------------------------------------------------------------------------


bool 
OBJNode::read(const std::string& _filename)
{
  FILE* in = fopen(_filename.c_str(), "r");
  if (!in) return false;


  char                                  s[200];
  float                                 x, y, z, u, v;
  

  // clear mesh
  vertices_.clear();
  texCoords_.clear();
  faces_.clear();



  while(!feof(in) && fgets(s, 200, in))
  {
    // comment
    if (s[0] == '#') continue;

    
    // vertex
    else if (strncmp(s, "v ", 2) == 0)
    {
      if (sscanf(s, "v %100f %100f %100f", &x, &y, &z))
      {
        vertices_.push_back(Vec3f(x,y,z));
      }
    }


    // texture coord
    else if (strncmp(s, "vt ", 3) == 0)
    {
      if (sscanf(s, "vt %100f %100f", &u, &v))
	texCoords_.push_back(Vec2f(u, v));
    }

    
    // face
    else if (strncmp(s, "f ", 2) == 0)
    {
      std::vector<unsigned int>  vIdx, tIdx;
      int component(0), nV(0);
      bool endOfVertex(false);
      char *p0, *p1(s+2);
      
      vIdx.clear();
      tIdx.clear();

    
      while(p1)
      {
        p0 = p1;

        // overwrite next separator
        if (p1)
        {
	  while (*p1 != '/' && *p1 != '\r' && *p1 != '\n' && 
		 *p1 != ' ' && *p1 != '\0')
	    p1++;
            
	  // detect end of vertex
	  if (*p1 != '/') endOfVertex = true;
        
	  // replace separator by '\0'
	  if (*p1 != '\0') 
	  {
	    *p1 = '\0';
	    p1++; // point to next token
	  }
	  
	  // detect end of line and break
	  if (*p1 == '\0' || *p1 == '\n')
	    p1 = 0;
        }

        if (*p0 != '\0')
        {
	  switch (component)
	  {
	    case 0: vIdx.push_back(atoi(p0)-1); break;
	    case 1: tIdx.push_back(atoi(p0)-1); break;
	    case 2: /* ignore vertex normals */ break;
	  }

	  component++;

	  if (endOfVertex)
	  {
	    component = 0;
	    nV++;
	    endOfVertex = false;
	  }
        }
      }

      
      if (vIdx.size() >= 3)
      {
	// texture
	if (vIdx.size() == tIdx.size())
	{
	  for (unsigned int i1=1, i2=2; i2 < vIdx.size(); ++i1, ++i2)
	    faces_.push_back(Face(vIdx[0], vIdx[i1], vIdx[i2],
				  tIdx[0], tIdx[i1], tIdx[i2]));
	}

	// no texture
	else
	{
	  for (unsigned int i1=1, i2=2; i2 < vIdx.size(); ++i1, ++i2)
	  {
	    faces_.push_back(Face(vIdx[0], vIdx[i1], vIdx[i2]));
	  }
	}
      }
    }
    

    s[0]=' ';
  }

  
  fclose(in);

  update_face_normals();

  return true;
}


//----------------------------------------------------------------------------


void
OBJNode::update_face_normals()
{
  normals_.clear();
  normals_.reserve(faces_.size());

  std::vector<Face>::const_iterator f_it(faces_.begin()), f_end(faces_.end());
  for (; f_it!=f_end; ++f_it)
  {
    const Vec3f& v0(vertex(f_it->i0));
    const Vec3f& v1(vertex(f_it->i1));
    const Vec3f& v2(vertex(f_it->i2));
    normals_.push_back(((v1-v0) % (v2-v0)).normalize());
  }
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
