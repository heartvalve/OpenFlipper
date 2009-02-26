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
//   $Date: 2008-05-09 14:15:53 +0200 (Fr, 09 Mai 2008) $
//
//=============================================================================




//=============================================================================
//
//  CLASS ShaderNode - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "ShaderNode.hh"
#include "DrawModes.hh"
#include <vector>


//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== IMPLEMENTATION ==========================================================


ShaderNode::ShaderNode( BaseNode*            _parent,
			const std::string&   _name)
  : BaseNode(_parent, _name),
    shaderDir_("")
{
}

ShaderNode::~ShaderNode() {
  if ( shaderDir_ != "" ) {

    for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
      if ( shaders[i].initialized ) {

        if ( shaders[i].program != 0 )
           delete shaders[i].program;

         if ( shaders[i].vertexShader != 0 )
           delete shaders[i].vertexShader;

         if ( shaders[i].fragmentShader != 0 )
           delete shaders[i].fragmentShader;
      }

    }
  }
}

//----------------------------------------------------------------------------
bool
ShaderNode::
hasShader( unsigned int _drawmode ) {

  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if ( shaders[i].initialized ) {
        return true;
      }
    }

    mode >>= 1;
  }
  return false;
}

//----------------------------------------------------------------------------


void
ShaderNode::enter(GLState& /*_state*/, unsigned int _drawmode  )
{
  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if ( shaders[i].initialized ) {
        shaders[i].program->use();
      }
    }

    mode >>= 1;
  }
}

//----------------------------------------------------------------------------
std::string
ShaderNode::vertexShaderName(unsigned int _drawmode) {
  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if ( shaders[i].initialized ) {
        return shaders[i].vertexShaderFile;
      }
    }

    mode >>= 1;
  }

  return std::string("");
}

//----------------------------------------------------------------------------
std::string
ShaderNode::fragmentShaderName(unsigned int _drawmode) {
  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if ( shaders[i].initialized ) {
        return shaders[i].fragmentShaderFile;
      }
    }

    mode >>= 1;
  }

  return std::string("");
}


//----------------------------------------------------------------------------


void ShaderNode::leave(GLState& /*_state*/, unsigned int _drawmode )
{
  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if ( shaders[i].initialized ) {
        shaders[i].program->disable();
      }
    }

    mode >>= 1;
  }
}

//----------------------------------------------------------------------------

/// Get the shader for the given drawMode
GLSL::PtrProgram
ShaderNode::
getShader( unsigned int _drawmode ) {
  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if ( shaders[i].initialized ) {
        return shaders[i].program;
      }
    }

    mode >>= 1;

  }

  // No shader found for this mode
  return 0;
}

//----------------------------------------------------------------------------


void
ShaderNode::
setShader( unsigned int _drawmode ,
           std::string _vertexShader,
           std::string _fragmentShader) {

  // OpenGl 2.0 is needed for GLSL support
  if (!glewIsSupported("GL_VERSION_2_0")) {
    std::cerr << "No shader support... unable to load shaders!" << std::endl;
    return;

  }

  if ( shaderDir_ == "" ) {
    std::cerr << "No shader dir set for shadernode! Unable to load shaders" << std::endl;
    return;
  }

  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {

    if (mode & 1) {

      if ( shaders[i].initialized ) {
        if ( shaders[i].program != 0 )
           delete shaders[i].program;

         if ( shaders[i].vertexShader != 0 )
           delete shaders[i].vertexShader;

         if ( shaders[i].fragmentShader != 0 )
           delete shaders[i].fragmentShader;

         shaders[i].initialized    = false;
      }

      shaders[i].vertexShaderFile   = shaderDir_ + _vertexShader;
      shaders[i].fragmentShaderFile = shaderDir_ + _fragmentShader;

      const char* vertexShaderFilePath   = shaders[i].vertexShaderFile.c_str();
      const char* fragmentShaderFilePath = shaders[i].fragmentShaderFile.c_str();
      shaders[i].vertexShader            = GLSL::loadVertexShader(vertexShaderFilePath);
      shaders[i].fragmentShader          = GLSL::loadFragmentShader(fragmentShaderFilePath);
      shaders[i].program                 = GLSL::PtrProgram(new GLSL::Program());

      if ( (shaders[i].vertexShader == 0) ||
           (shaders[i].fragmentShader == 0) ||
           (shaders[i].program == 0) ) {
        std::cerr << "Unable to load shaders" << shaders[i].vertexShaderFile <<
                     " or " << shaders[i].fragmentShaderFile << std::endl;
        shaders[i].vertexShader   = 0;
        shaders[i].fragmentShader = 0;
        shaders[i].program        = 0;
        shaders[i].initialized    = false;
        return;
      }

      shaders[i].program->attach(shaders[i].vertexShader);
      shaders[i].program->attach(shaders[i].fragmentShader);
      shaders[i].program->link();

      shaders[i].initialized = true;

    }

    mode >>= 1;
  }

}

//----------------------------------------------------------------------------

void
ShaderNode::
setShaderDir( std::string _shaderDir) {
  shaderDir_ = _shaderDir;
}

unsigned int
ShaderNode::
availableDrawModes() const
{

  unsigned int drawModes(0);

  drawModes |= DrawModes::SOLID_PHONG_SHADED;
  drawModes |= DrawModes::SOLID_SHADER;

  return drawModes;

}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
