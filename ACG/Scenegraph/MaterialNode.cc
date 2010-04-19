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
//  CLASS MaterialNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "MaterialNode.hh"
#include "DrawModes.hh"

#include <cstdio>

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


MaterialNode::MaterialNode( BaseNode*            _parent,
			    const std::string&   _name,
			    unsigned int         _applyProperties )
  : BaseNode(_parent, _name),
    applyProperties_(_applyProperties)
{}


//----------------------------------------------------------------------------


void MaterialNode::enter(GLState& _state, DrawModes::DrawMode  _drawmode  )
{
  if (applyProperties_ & BaseColor)
  {
    materialBackup_.base_color_ = _state.base_color();
    _state.set_base_color(material_.base_color_);
  }

  if (applyProperties_ & Material)
  {
    materialBackup_.ambient_color_  = _state.ambient_color();
    materialBackup_.diffuse_color_  = _state.diffuse_color();
    materialBackup_.specular_color_ = _state.specular_color();
    materialBackup_.shininess_      = _state.shininess();

    _state.set_ambient_color(material_.ambient_color_);
    _state.set_diffuse_color(material_.diffuse_color_);
    _state.set_specular_color(material_.specular_color_);
    _state.set_shininess(material_.shininess_);
  }

  if (applyProperties_ & PointSize)
  {
    materialBackup_.point_size_ = _state.point_size();
    _state.set_point_size(material_.point_size_);
  }

  if (applyProperties_ & LineWidth)
  {
    materialBackup_.line_width_ = _state.line_width();
    _state.set_line_width(material_.line_width_);
  }

  if (applyProperties_ & RoundPoints)
  {
    materialBackup_.round_points_ = glIsEnabled(GL_POINT_SMOOTH) &&
                           glIsEnabled(GL_ALPHA_TEST);

    if( material_.round_points_ ) {
      glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
      glEnable(GL_POINT_SMOOTH);
    } else
      glDisable(GL_POINT_SMOOTH);
  }

  if (applyProperties_ & LineSmooth)
  {
    materialBackup_.lines_smooth_ = glIsEnabled(GL_LINE_SMOOTH) &&
                           glIsEnabled(GL_ALPHA_TEST);

    if( material_.lines_smooth_ ) {
      glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
      glEnable(GL_LINE_SMOOTH);
    } else
      glDisable(GL_LINE_SMOOTH);
  }

  if (applyProperties_ & MultiSampling)
  {
    materialBackup_.multi_sampling_ = _state.multisampling();
    _state.set_multisampling( material_.multi_sampling_ );
  }

  if (applyProperties_ & AlphaTest)
  {
    materialBackup_.alpha_test_ = glIsEnabled(GL_ALPHA_TEST);
    glGetFloatv(GL_ALPHA_TEST_REF, &materialBackup_.alpha_clip_);

    if(material_.alpha_test_)
    {
      glAlphaFunc(GL_GREATER, material_.alpha_clip_ );
      glEnable(GL_ALPHA_TEST);
    }
    else
    {
      glDisable(GL_ALPHA_TEST);
    }
  }


  if (applyProperties_ & Blending)
  {
    materialBackup_.blending_ = _state.blending();
    glGetIntegerv( GL_BLEND_SRC, (GLint*) &materialBackup_.blend_param1_);
    glGetIntegerv( GL_BLEND_DST, (GLint*) &materialBackup_.blend_param2_);

    _state.set_blending(material_.blending_);

    if (material_.blending_)
    {
      glDepthFunc(GL_LEQUAL);
      glBlendFunc(material_.blend_param1_, material_.blend_param2_);
      glEnable(GL_BLEND);
    }
    else
    {
      glDepthFunc(GL_LESS);
      glDisable(GL_BLEND);
    }
  }


  if (applyProperties_ & BackFaceCulling)
  {
    materialBackup_.backface_culling_ = glIsEnabled(GL_CULL_FACE);

    if ( material_.backface_culling_ )
      glEnable( GL_CULL_FACE );
    else
      glDisable( GL_CULL_FACE );

  }

  if ( ( applyProperties_ & ColorMaterial ) && ( (_drawmode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) ||
                                                 (_drawmode & DrawModes::SOLID_2DTEXTURED_FACE_SHADED) ) )
  {
    materialBackup_.color_material_ = glIsEnabled(GL_COLOR_MATERIAL);

    if (material_.color_material_ ) {
      glDisable( GL_COLOR_MATERIAL );
      glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
      glEnable( GL_COLOR_MATERIAL );
    } else
      glDisable( GL_COLOR_MATERIAL );
  }

}

//----------------------------------------------------------------------------

void MaterialNode::enterPick(GLState& /*_state*/ ,
                             PickTarget /*_target*/,
                             DrawModes::DrawMode /*_drawMode*/ ) {

}

void MaterialNode::leavePick(GLState& /*_state*/,
                            PickTarget /*_target*/,
                             DrawModes::DrawMode /*_drawMode*/ ) {
}


//----------------------------------------------------------------------------


void MaterialNode::leave(GLState& _state, DrawModes::DrawMode _drawmode )
{
  if (applyProperties_ & BaseColor)
  {
    _state.set_base_color(materialBackup_.base_color_);
  }


  if (applyProperties_ & Material)
  {
    _state.set_ambient_color(materialBackup_.ambient_color_);
    _state.set_diffuse_color(materialBackup_.diffuse_color_);
    _state.set_specular_color(materialBackup_.specular_color_);
    _state.set_shininess(materialBackup_.shininess_);
  }


  if (applyProperties_ & PointSize)
  {
    _state.set_point_size(materialBackup_.point_size_);
  }


  if (applyProperties_ & LineWidth)
  {
    _state.set_line_width(materialBackup_.line_width_);
  }


  if (applyProperties_ & RoundPoints)
  {
    if( materialBackup_.round_points_)
      glEnable(GL_POINT_SMOOTH);
    else
      glDisable(GL_POINT_SMOOTH);
  }

  if (applyProperties_ & LineSmooth)
  {
    if( materialBackup_.lines_smooth_)
      glEnable(GL_LINE_SMOOTH);
    else
      glDisable(GL_LINE_SMOOTH);
  }

  if (applyProperties_ & MultiSampling)
    _state.set_multisampling( materialBackup_.multi_sampling_ );

  if (applyProperties_ & AlphaTest)
  {
    if (materialBackup_.alpha_test_)
    {
      glAlphaFunc(GL_GREATER, materialBackup_.alpha_clip_);
      glEnable(GL_ALPHA_TEST);
    }
    else
    {
      glDisable(GL_ALPHA_TEST);
    }
  }


  if (applyProperties_ & Blending)
  {
    _state.set_blending(materialBackup_.blending_);

    if (materialBackup_.blending_)
    {
      glDepthFunc(GL_LEQUAL);
      glBlendFunc(materialBackup_.blend_param1_, materialBackup_.blend_param2_);
      glEnable(GL_BLEND);
    }
    else
    {
      glDepthFunc(GL_LESS);
      glDisable(GL_BLEND);
    }
  }


  if (applyProperties_ & BackFaceCulling)
  {
    if (materialBackup_.backface_culling_)
      glEnable( GL_CULL_FACE );
    else
      glDisable( GL_CULL_FACE );
 }

  if ( ( applyProperties_ & ColorMaterial ) && ( (_drawmode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) ||
                                                 (_drawmode & DrawModes::SOLID_2DTEXTURED_FACE_SHADED) ) )
  {
    if (materialBackup_.color_material_ ) {
      glEnable( GL_COLOR_MATERIAL );
    } else
      glDisable( GL_COLOR_MATERIAL );
  }

}


//----------------------------------------------------------------------------


void
MaterialNode::read(std::istream& _is)
{

  char                                  s[200];
  float                                 x, y, z, u;


  while(_is && (!_is.eof()) && _is.getline(s, 200))
  {
    // comment
    if (s[0] == '#') continue;


    // BaseColor
    else if (strncmp(s, "BaseColor ", 10) == 0)
    {
      if (sscanf(s, "BaseColor %f %f %f %f", &x, &y, &z, &u))
      {
        material_.set_base_color(Vec4f(x,y,z,u));
      }
    }
    // AmbientColor
    else if (strncmp(s, "AmbientColor ", 13) == 0)
    {
      if (sscanf(s, "AmbientColor %f %f %f %f", &x, &y, &z, &u))
      {
        material_.set_ambient_color(Vec4f(x,y,z,u));
      }
    }
    // DiffuseColor
    else if (strncmp(s, "DiffuseColor ", 13) == 0)
    {
      if (sscanf(s, "DiffuseColor %f %f %f %f", &x, &y, &z, &u))
      {
        material_.set_diffuse_color(Vec4f(x,y,z,u));
      }
    }
    // SpecularColor
    else if (strncmp(s, "SpecularColor ", 14) == 0)
    {
      if (sscanf(s, "SpecularColor %f %f %f %f", &x, &y, &z, &u))
      {
        material_.set_specular_color(Vec4f(x,y,z,u));
      }
    }
    // Shininess
    else if (strncmp(s, "Shininess ", 10) == 0)
    {
      if (sscanf(s, "Shininess %f", &x))
      {
        material_.set_shininess(x);
      }
    }
    // PointSize
    else if (strncmp(s, "PointSize ", 10) == 0)
    {
      if (sscanf(s, "PointSize %f", &x))
      {
        material_.set_point_size(x);
      }
    }
    // LineWidth
    else if (strncmp(s, "LineWidth ", 10) == 0)
    {
      if (sscanf(s, "LineWidth %f", &x))
      {
        material_.set_line_width(x);
      }
    }


    s[0]=' ';
  }
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
