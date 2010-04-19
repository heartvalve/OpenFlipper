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
    base_color_backup_ = _state.base_color();
    _state.set_base_color(material_.base_color());
  }

  if (applyProperties_ & Material)
  {
    ambient_color_backup_  = _state.ambient_color();
    diffuse_color_backup_  = _state.diffuse_color();
    specular_color_backup_ = _state.specular_color();
    shininess_backup_      = _state.shininess();

    _state.set_ambient_color(material_.ambient_color());
    _state.set_diffuse_color(material_.diffuse_color());
    _state.set_specular_color(material_.specular_color());
    _state.set_shininess(material_.shininess());
  }

  if (applyProperties_ & PointSize)
  {
    point_size_backup_ = _state.point_size();
    _state.set_point_size(material_.point_size());
  }

  if (applyProperties_ & LineWidth)
  {
    line_width_backup_ = _state.line_width();
    _state.set_line_width(material_.line_width());
  }

  if (applyProperties_ & RoundPoints)
  {
    round_points_backup_ = glIsEnabled(GL_POINT_SMOOTH) &&
                           glIsEnabled(GL_ALPHA_TEST);

    if( material_.round_points() ) {
      glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
      glEnable(GL_POINT_SMOOTH);
    } else
      glDisable(GL_POINT_SMOOTH);
  }

  if (applyProperties_ & LineSmooth)
  {
    lines_smooth_backup_ = glIsEnabled(GL_LINE_SMOOTH) &&
                           glIsEnabled(GL_ALPHA_TEST);

    if( material_.line_smooth() ) {
      glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
      glEnable(GL_LINE_SMOOTH);
    } else
      glDisable(GL_LINE_SMOOTH);
  }

  if (applyProperties_ & MultiSampling)
  {
    multiSampling_backup_ = _state.multisampling();
    _state.set_multisampling( material_.multi_sampling() );
  }

  if (applyProperties_ & AlphaTest)
  {
    alpha_test_backup_ = glIsEnabled(GL_ALPHA_TEST);
    glGetFloatv(GL_ALPHA_TEST_REF, &alpha_clip_backup_);

    if(material_.alpha_test())
    {
      glAlphaFunc(GL_GREATER, material_.alpha_value() );
      glEnable(GL_ALPHA_TEST);
    }
    else
    {
      glDisable(GL_ALPHA_TEST);
    }
  }


  if (applyProperties_ & Blending)
  {
    blending_backup_ = _state.blending();
    glGetIntegerv( GL_BLEND_SRC, (GLint*) &blend_param1_backup_);
    glGetIntegerv( GL_BLEND_DST, (GLint*) &blend_param2_backup_);

    _state.set_blending(material_.blending());

    if (material_.blending())
    {
      glDepthFunc(GL_LEQUAL);
      glBlendFunc(material_.blending_param1(), material_.blending_param2());
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
    backface_culling_backup_ = glIsEnabled(GL_CULL_FACE);

    if ( material_.backface_culling() )
      glEnable( GL_CULL_FACE );
    else
      glDisable( GL_CULL_FACE );

  }

  if ( ( applyProperties_ & ColorMaterial ) && ( (_drawmode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) ||
                                                 (_drawmode & DrawModes::SOLID_2DTEXTURED_FACE_SHADED) ) )
  {
    color_material_backup_ = glIsEnabled(GL_COLOR_MATERIAL);

    if (material_.color_material() ) {
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
    _state.set_base_color(base_color_backup_);
  }


  if (applyProperties_ & Material)
  {
    _state.set_ambient_color(ambient_color_backup_);
    _state.set_diffuse_color(diffuse_color_backup_);
    _state.set_specular_color(specular_color_backup_);
    _state.set_shininess(shininess_backup_);
  }


  if (applyProperties_ & PointSize)
  {
    _state.set_point_size(point_size_backup_);
  }


  if (applyProperties_ & LineWidth)
  {
    _state.set_line_width(line_width_backup_);
  }


  if (applyProperties_ & RoundPoints)
  {
    if( round_points_backup_)
      glEnable(GL_POINT_SMOOTH);
    else
      glDisable(GL_POINT_SMOOTH);
  }

  if (applyProperties_ & LineSmooth)
  {
    if( lines_smooth_backup_)
      glEnable(GL_LINE_SMOOTH);
    else
      glDisable(GL_LINE_SMOOTH);
  }

  if (applyProperties_ & MultiSampling)
    _state.set_multisampling( multiSampling_backup_ );

  if (applyProperties_ & AlphaTest)
  {
    if (alpha_test_backup_)
    {
      glAlphaFunc(GL_GREATER, alpha_clip_backup_);
      glEnable(GL_ALPHA_TEST);
    }
    else
    {
      glDisable(GL_ALPHA_TEST);
    }
  }


  if (applyProperties_ & Blending)
  {
    _state.set_blending(blending_backup_);

    if (blending_backup_)
    {
      glDepthFunc(GL_LEQUAL);
      glBlendFunc(blend_param1_backup_, blend_param2_backup_);
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
    if (backface_culling_backup_)
      glEnable( GL_CULL_FACE );
    else
      glDisable( GL_CULL_FACE );
 }

  if ( ( applyProperties_ & ColorMaterial ) && ( (_drawmode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) ||
                                                 (_drawmode & DrawModes::SOLID_2DTEXTURED_FACE_SHADED) ) )
  {
    if (color_material_backup_ ) {
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
