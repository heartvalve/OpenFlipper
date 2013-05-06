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


static inline QVariantList col2vl(const Vec4f &col) {
    return QVariantList() << col[0] << col[1] << col[2] << col[3];
}

static inline Vec4f vl2col(const QVariantList &vl) {
    if (vl.size() < 4) return Vec4f();
    return Vec4f(vl[0].toFloat(), vl[1].toFloat(), vl[2].toFloat(), vl[3].toFloat());
}

QString Material::serializeToJson() const {
    QVariantMap matMap;

    matMap["baseColor"] = col2vl(baseColor_);
    matMap["ambientColor"] = col2vl(ambientColor_);
    matMap["diffuseColor"] = col2vl(diffuseColor_);
    matMap["specularColor"] = col2vl(specularColor_);
    matMap["overlayColor"] = col2vl(overlayColor_);
    matMap["shininess"] = shininess_;
    matMap["reflectance"] = reflectance_;
    matMap["pointSize"] = pointSize_;
    matMap["lineWidth"] = lineWidth_;
    matMap["roundPoints"] = roundPoints_;
    matMap["linesSmooth"] = linesSmooth_;
    matMap["alphaTest"] = alphaTest_;
    matMap["alphaClip"] = alphaClip_;
    matMap["blending"] = blending_;
    matMap["blendParam1"] = blendParam1_;
    matMap["blendParam2"] = blendParam2_;
    matMap["colorMaterial"] = colorMaterial_;
    matMap["backfaceCulling"] = backfaceCulling_;
    matMap["multiSampling"] = multiSampling_;

    //QJson::Serializer serializer;
    //QByteArray bytes = serializer.serialize(matMap);
    //return QString::fromUtf8(bytes.constData(), bytes.size());

    return QString("<No suitable serializer at the moment. Sorry.>");
}

void Material::deserializeFromJson(const QString &json) {
    //QJson::Parser parser;
    //bool ok;
    //QVariantMap matMap = parser.parse(json.toUtf8(), &ok).toMap();
    //if (!ok) return;
    QVariantMap matMap;

    if (matMap.contains("baseColor")) baseColor_ = vl2col(matMap["baseColor"].toList());
    if (matMap.contains("ambientColor")) ambientColor_ = vl2col(matMap["ambientColor"].toList());
    if (matMap.contains("diffuseColor")) diffuseColor_ = vl2col(matMap["diffuseColor"].toList());
    if (matMap.contains("specularColor")) specularColor_ = vl2col(matMap["specularColor"].toList());
    if (matMap.contains("overlayColor")) overlayColor_ = vl2col(matMap["overlayColor"].toList());
    if (matMap.contains("shininess")) shininess_ = matMap["shininess"].toFloat();
    if (matMap.contains("reflectance")) reflectance_ = matMap["reflectance"].toDouble();
    if (matMap.contains("pointSize")) pointSize_ = matMap["pointSize"].toFloat();
    if (matMap.contains("lineWidth")) lineWidth_ = matMap["lineWidth"].toFloat();
    if (matMap.contains("roundPoints")) roundPoints_ = matMap["roundPoints"].toBool();
    if (matMap.contains("linesSmooth")) linesSmooth_ = matMap["linesSmooth"].toBool();
    if (matMap.contains("alphaTest")) alphaTest_ = matMap["alphaTest"].toBool();
    if (matMap.contains("alphaClip")) alphaClip_ = matMap["alphaClip"].toFloat();
    if (matMap.contains("blending")) blending_ = matMap["blending"].toBool();
    if (matMap.contains("blendParam1")) blendParam1_ = matMap["blendParam1"].toUInt();
    if (matMap.contains("blendParam2")) blendParam2_ = matMap["blendParam2"].toUInt();
    if (matMap.contains("colorMaterial")) colorMaterial_ = matMap["colorMaterial"].toBool();
    if (matMap.contains("backfaceCulling")) backfaceCulling_ = matMap["backfaceCulling"].toBool();
    if (matMap.contains("multiSampling")) multiSampling_ = matMap["multiSampling"].toBool();
}

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
    materialBackup_.overlayColor_  = _state.overlay_color();
    materialBackup_.shininess_     = _state.shininess();

    _state.set_ambient_color(material_.ambientColor_);
    _state.set_diffuse_color(material_.diffuseColor_);
    _state.set_specular_color(material_.specularColor_);
    _state.set_overlay_color(material_.overlayColor_);
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
      ACG::GLState::enable(GL_POINT_SMOOTH);
    } else
      ACG::GLState::disable(GL_POINT_SMOOTH);
  }

  if (applyProperties_ & LineSmooth)
  {
    materialBackup_.linesSmooth_ = glIsEnabled(GL_LINE_SMOOTH) &&
                           glIsEnabled(GL_ALPHA_TEST);

    if( material_.linesSmooth_ ) {
      glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
      ACG::GLState::enable(GL_LINE_SMOOTH);
    } else
      ACG::GLState::disable(GL_LINE_SMOOTH);
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
      ACG::GLState::alphaFunc(GL_GREATER, material_.alphaClip_ );
      ACG::GLState::enable(GL_ALPHA_TEST);
    }
    else
    {
      ACG::GLState::disable(GL_ALPHA_TEST);
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
      ACG::GLState::blendFunc(material_.blendParam1_, material_.blendParam2_);
      ACG::GLState::enable(GL_BLEND);
    }
    else
    {
      _state.set_depthFunc(GL_LESS);
      ACG::GLState::disable(GL_BLEND);
    }
  }


  if (applyProperties_ & BackFaceCulling)
  {
    materialBackup_.backfaceCulling_ = glIsEnabled(GL_CULL_FACE);

    if ( material_.backfaceCulling_ )
      ACG::GLState::enable( GL_CULL_FACE );
    else
      ACG::GLState::disable( GL_CULL_FACE );

  }

  if ( ( applyProperties_ & ColorMaterial ) && (_drawmode & (DrawModes::SOLID_FACES_COLORED_FLAT_SHADED|DrawModes::SOLID_FACES_COLORED_SMOOTH_SHADED)) )
  {
    materialBackup_.colorMaterial_ = glIsEnabled(GL_COLOR_MATERIAL);

    if (material_.colorMaterial_ ) {
      ACG::GLState::disable( GL_COLOR_MATERIAL );
      glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
      ACG::GLState::enable( GL_COLOR_MATERIAL );
    } else
      ACG::GLState::disable( GL_COLOR_MATERIAL );
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
    _state.set_overlay_color(materialBackup_.overlayColor_);
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
      ACG::GLState::enable(GL_POINT_SMOOTH);
    else
      ACG::GLState::disable(GL_POINT_SMOOTH);
  }

  if (applyProperties_ & LineSmooth)
  {
    if( materialBackup_.linesSmooth_)
      ACG::GLState::enable(GL_LINE_SMOOTH);
    else
      ACG::GLState::disable(GL_LINE_SMOOTH);
  }

  if (applyProperties_ & MultiSampling)
    _state.set_multisampling( materialBackup_.multiSampling_ );

  if (applyProperties_ & AlphaTest)
  {
    if (materialBackup_.alphaTest_)
    {
      ACG::GLState::alphaFunc(GL_GREATER, materialBackup_.alphaClip_);
      ACG::GLState::enable(GL_ALPHA_TEST);
    }
    else
    {
      ACG::GLState::disable(GL_ALPHA_TEST);
    }
  }


  if (applyProperties_ & Blending)
  {
    _state.set_blending(materialBackup_.blending_);

    if (materialBackup_.blending_)
    {
      _state.set_depthFunc(GL_LEQUAL);
      ACG::GLState::blendFunc(materialBackup_.blendParam1_, materialBackup_.blendParam2_);
      ACG::GLState::enable(GL_BLEND);
    }
    else
    {
      _state.set_depthFunc(GL_LESS);
      ACG::GLState::disable(GL_BLEND);
    }
  }


  if (applyProperties_ & BackFaceCulling)
  {
    if (materialBackup_.backfaceCulling_)
      ACG::GLState::enable( GL_CULL_FACE );
    else
      ACG::GLState::disable( GL_CULL_FACE );
 }

  if ( ( applyProperties_ & ColorMaterial ) && ( _drawmode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED )  )
  {
    if (materialBackup_.colorMaterial_ ) {
      ACG::GLState::enable( GL_COLOR_MATERIAL );
    } else
      ACG::GLState::disable( GL_COLOR_MATERIAL );
  }

}


//----------------------------------------------------------------------------


void
MaterialNode::read(std::istream& _is)
{

  char        s[200];
  float       x, y, z, u;

  while (_is && (!_is.eof()) && _is.getline(s,200) ) {
    std::istringstream buffer(s);

    // comment or empty
    if ( s[0] == '#')
      continue;

    std::string specifier = "";

    // Read specifier from buffer
    buffer >> specifier;

    // BaseColor
    if (specifier == "BaseColor") {
      buffer >> x >> y >> z >> u;

      if (buffer.good()) {
        material_.baseColor(Vec4f(x, y, z, u));
      }
    }
    // AmbientColor
    else if (specifier == "AmbientColor") {
      buffer >> x >> y >> z >> u;

      if (buffer.good()) {
        material_.ambientColor(Vec4f(x, y, z, u));
      }
    }
    // DiffuseColor
    else if (specifier == "DiffuseColor") {
      buffer >> x >> y >> z >> u;

      if (buffer.good()) {
        material_.diffuseColor(Vec4f(x, y, z, u));
      }
    }
    // SpecularColor
    else if (specifier == "SpecularColor") {
      buffer >> x >> y >> z >> u;

      if (buffer.good()) {
        material_.specularColor(Vec4f(x, y, z, u));
      }
    }
    // OverlayColor
    else if (specifier == "OverlayColor") {
      buffer >> x >> y >> z >> u;

      if (buffer.good()) {
        material_.overlayColor(Vec4f(x, y, z, u));
      }
    }
    // Shininess
    else if (specifier == "Shininess") {
      buffer >> x;

      if (buffer.good()) {
        material_.shininess(x);
      }
    }
    // PointSize
    else if (specifier == "PointSize") {
      buffer >> x;

      if (buffer.good()) {
        material_.pointSize(x);
      }
    }
    // LineWidth
    else if (specifier == "LineWidth") {
      buffer >> x;

      if (buffer.good()) {
        material_.lineWidth(x);
      }
    }

    if (!buffer.good())
      std::cerr << "MaterialNode parse error while reading string : " << s << std::endl;

  }
}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
