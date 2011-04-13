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


void MaterialNode::enter(GLState& _state, const DrawModes::DrawMode&  _drawmode  )
{
  if (applyProperties_ & BaseColor)
  {
    materialBackup_.baseColor_ = _state.base_color();
    _state.set_base_color(material_.baseColor_);
  }

  if (applyProperties_ & Material)
  {
    materialBackup_.ambientColor_  = _state.ambient_color();
    materialBackup_.diffuseColor_  = _state.diffuse_color();
    materialBackup_.specularColor_ = _state.specular_color();
    materialBackup_.shininess_      = _state.shininess();

    _state.set_ambient_color(material_.ambientColor_);
    _state.set_diffuse_color(material_.diffuseColor_);
    _state.set_specular_color(material_.specularColor_);
    _state.set_shininess(material_.shininess_);
  }

  if (applyProperties_ & PointSize)
  {
    materialBackup_.pointSize_ = _state.point_size();
    _state.set_point_size(material_.pointSize_);
  }

  if (applyProperties_ & LineWidth)
  {
    materialBackup_.lineWidth_ = _state.line_width();
    _state.set_line_width(material_.lineWidth_);
  }

  if (applyProperties_ & RoundPoints)
  {
    materialBackup_.roundPoints_ = glIsEnabled(GL_POINT_SMOOTH) &&
                           glIsEnabled(GL_ALPHA_TEST);

    if( material_.roundPoints_ ) {
      glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
      glEnable(GL_POINT_SMOOTH);
    } else
      glDisable(GL_POINT_SMOOTH);
  }

  if (applyProperties_ & LineSmooth)
  {
    materialBackup_.linesSmooth_ = glIsEnabled(GL_LINE_SMOOTH) &&
                           glIsEnabled(GL_ALPHA_TEST);

    if( material_.linesSmooth_ ) {
      glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
      glEnable(GL_LINE_SMOOTH);
    } else
      glDisable(GL_LINE_SMOOTH);
  }

  if (applyProperties_ & MultiSampling)
  {
    materialBackup_.multiSampling_ = _state.multisampling();
    _state.set_multisampling( material_.multiSampling_ );
  }

  if (applyProperties_ & AlphaTest)
  {
    materialBackup_.alphaTest_ = glIsEnabled(GL_ALPHA_TEST);
    glGetFloatv(GL_ALPHA_TEST_REF, &materialBackup_.alphaClip_);

    if(material_.alphaTest_)
    {
      glAlphaFunc(GL_GREATER, material_.alphaClip_ );
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
    glGetIntegerv( GL_BLEND_SRC, (GLint*) &materialBackup_.blendParam1_);
    glGetIntegerv( GL_BLEND_DST, (GLint*) &materialBackup_.blendParam2_);

    _state.set_blending(material_.blending_);

    if (material_.blending_)
    {
      _state.set_depthFunc(GL_LEQUAL);
      glBlendFunc(material_.blendParam1_, material_.blendParam2_);
      glEnable(GL_BLEND);
    }
    else
    {
      _state.set_depthFunc(GL_LESS);
      glDisable(GL_BLEND);
    }
  }


  if (applyProperties_ & BackFaceCulling)
  {
    materialBackup_.backfaceCulling_ = glIsEnabled(GL_CULL_FACE);

    if ( material_.backfaceCulling_ )
      glEnable( GL_CULL_FACE );
    else
      glDisable( GL_CULL_FACE );

  }

  if ( ( applyProperties_ & ColorMaterial ) && ( (_drawmode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) ||
                                                 (_drawmode & DrawModes::SOLID_2DTEXTURED_FACE_SHADED) ) )
  {
    materialBackup_.colorMaterial_ = glIsEnabled(GL_COLOR_MATERIAL);

    if (material_.colorMaterial_ ) {
      glDisable( GL_COLOR_MATERIAL );
      glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
      glEnable( GL_COLOR_MATERIAL );
    } else
      glDisable( GL_COLOR_MATERIAL );
  }

}

//----------------------------------------------------------------------------

void MaterialNode::enterPick(GLState& _state , PickTarget /*_target*/, const DrawModes::DrawMode& /*_drawMode*/ ) {

  if (applyProperties_ & PointSize)
  {
    materialBackup_.pointSize_ = _state.point_size();
    _state.set_point_size(material_.pointSize_);
  }

  if (applyProperties_ & LineWidth)
  {
    materialBackup_.lineWidth_ = _state.line_width();
    _state.set_line_width(material_.lineWidth_);
  }
}

void MaterialNode::leavePick(GLState& _state, PickTarget /*_target*/, const DrawModes::DrawMode& /*_drawMode*/ ) {

  if (applyProperties_ & PointSize)
  {
    _state.set_point_size(materialBackup_.pointSize_);
  }


  if (applyProperties_ & LineWidth)
  {
    _state.set_line_width(materialBackup_.lineWidth_);
  }
}


//----------------------------------------------------------------------------


void MaterialNode::leave(GLState& _state, const DrawModes::DrawMode& _drawmode )
{
  if (applyProperties_ & BaseColor)
  {
    _state.set_base_color(materialBackup_.baseColor_);
  }


  if (applyProperties_ & Material)
  {
    _state.set_ambient_color(materialBackup_.ambientColor_);
    _state.set_diffuse_color(materialBackup_.diffuseColor_);
    _state.set_specular_color(materialBackup_.specularColor_);
    _state.set_shininess(materialBackup_.shininess_);
  }


  if (applyProperties_ & PointSize)
  {
    _state.set_point_size(materialBackup_.pointSize_);
  }


  if (applyProperties_ & LineWidth)
  {
    _state.set_line_width(materialBackup_.lineWidth_);
  }


  if (applyProperties_ & RoundPoints)
  {
    if( materialBackup_.roundPoints_)
      glEnable(GL_POINT_SMOOTH);
    else
      glDisable(GL_POINT_SMOOTH);
  }

  if (applyProperties_ & LineSmooth)
  {
    if( materialBackup_.linesSmooth_)
      glEnable(GL_LINE_SMOOTH);
    else
      glDisable(GL_LINE_SMOOTH);
  }

  if (applyProperties_ & MultiSampling)
    _state.set_multisampling( materialBackup_.multiSampling_ );

  if (applyProperties_ & AlphaTest)
  {
    if (materialBackup_.alphaTest_)
    {
      glAlphaFunc(GL_GREATER, materialBackup_.alphaClip_);
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
      _state.set_depthFunc(GL_LEQUAL);
      glBlendFunc(materialBackup_.blendParam1_, materialBackup_.blendParam2_);
      glEnable(GL_BLEND);
    }
    else
    {
      _state.set_depthFunc(GL_LESS);
      glDisable(GL_BLEND);
    }
  }


  if (applyProperties_ & BackFaceCulling)
  {
    if (materialBackup_.backfaceCulling_)
      glEnable( GL_CULL_FACE );
    else
      glDisable( GL_CULL_FACE );
 }

  if ( ( applyProperties_ & ColorMaterial ) && ( (_drawmode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) ||
                                                 (_drawmode & DrawModes::SOLID_2DTEXTURED_FACE_SHADED) ) )
  {
    if (materialBackup_.colorMaterial_ ) {
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
        material_.baseColor(Vec4f(x,y,z,u));
      }
    }
    // AmbientColor
    else if (strncmp(s, "AmbientColor ", 13) == 0)
    {
      if (sscanf(s, "AmbientColor %f %f %f %f", &x, &y, &z, &u))
      {
        material_.ambientColor(Vec4f(x,y,z,u));
      }
    }
    // DiffuseColor
    else if (strncmp(s, "DiffuseColor ", 13) == 0)
    {
      if (sscanf(s, "DiffuseColor %f %f %f %f", &x, &y, &z, &u))
      {
        material_.diffuseColor(Vec4f(x,y,z,u));
      }
    }
    // SpecularColor
    else if (strncmp(s, "SpecularColor ", 14) == 0)
    {
      if (sscanf(s, "SpecularColor %f %f %f %f", &x, &y, &z, &u))
      {
        material_.specularColor(Vec4f(x,y,z,u));
      }
    }
    // Shininess
    else if (strncmp(s, "Shininess ", 10) == 0)
    {
      if (sscanf(s, "Shininess %f", &x))
      {
        material_.shininess(x);
      }
    }
    // PointSize
    else if (strncmp(s, "PointSize ", 10) == 0)
    {
      if (sscanf(s, "PointSize %f", &x))
      {
        material_.pointSize(x);
      }
    }
    // LineWidth
    else if (strncmp(s, "LineWidth ", 10) == 0)
    {
      if (sscanf(s, "LineWidth %f", &x))
      {
        material_.lineWidth(x);
      }
    }


    s[0]=' ';
  }
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
