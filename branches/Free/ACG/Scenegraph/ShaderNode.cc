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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




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
hasShader( unsigned int _drawmode, bool _pick ) {

  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if (_pick)
        return pickShaders[i].initialized;
      else
        return shaders[i].initialized;
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

void
ShaderNode::enterPick(GLState& /*_state*/, PickTarget /*_target*/, unsigned int _drawmode  )
{
  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if ( pickShaders[i].initialized ) {
        pickShaders[i].program->use();
     }
    }

    mode >>= 1;
  }
}

//----------------------------------------------------------------------------
std::string
ShaderNode::vertexShaderName(unsigned int _drawmode, bool _pick) {
  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if (_pick)
      {
        if ( pickShaders[i].initialized ) {
          return pickShaders[i].vertexShaderFile;
        }
      }
      else
      {
        if ( shaders[i].initialized ) {
          return shaders[i].vertexShaderFile;
        }
      }
    }

    mode >>= 1;
  }

  return std::string("");
}

//----------------------------------------------------------------------------
std::string
ShaderNode::fragmentShaderName(unsigned int _drawmode, bool _pick) {
  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if (_pick)
      {
        if ( pickShaders[i].initialized ) {
          return pickShaders[i].fragmentShaderFile;
        }
      }
      else
      {
        if ( shaders[i].initialized ) {
          return shaders[i].fragmentShaderFile;
        }
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


void ShaderNode::leavePick(GLState& /*_state*/, PickTarget /*_target*/, unsigned int _drawmode )
{
  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if ( pickShaders[i].initialized ) {
        pickShaders[i].program->disable();
      }
    }

    mode >>= 1;
  }
}

//----------------------------------------------------------------------------

/// Get the shader for the given drawMode
GLSL::PtrProgram
ShaderNode::
getShader( unsigned int _drawmode, bool _pick ) {
  unsigned int mode = _drawmode;
  for (size_t i = 0; i < sizeof(unsigned int)*8 ;  ++i) {
    if (mode & 1) {
      if (_pick)
      {
        if ( pickShaders[i].initialized ) {
          return pickShaders[i].program;
        }
      }
      else
      {
        if ( shaders[i].initialized ) {
          return shaders[i].program;
        }
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
           std::string _fragmentShader,
           std::string _pickVertexShader,
           std::string _pickFragmentShader) {

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

      if ( pickShaders[i].initialized ) {
        if ( pickShaders[i].program != 0 )
           delete pickShaders[i].program;

         if ( pickShaders[i].vertexShader != 0 )
           delete pickShaders[i].vertexShader;

         if ( pickShaders[i].fragmentShader != 0 )
           delete pickShaders[i].fragmentShader;

         pickShaders[i].initialized    = false;
      }

      if (_pickVertexShader.length () > 0 && _pickFragmentShader.length () > 0)
      {
        pickShaders[i].vertexShaderFile   = shaderDir_ + _pickVertexShader;
        pickShaders[i].fragmentShaderFile = shaderDir_ + _pickFragmentShader;

        const char* vertexShaderFilePath   = pickShaders[i].vertexShaderFile.c_str();
        const char* fragmentShaderFilePath = pickShaders[i].fragmentShaderFile.c_str();
        pickShaders[i].vertexShader        = GLSL::loadVertexShader(vertexShaderFilePath);
        pickShaders[i].fragmentShader      = GLSL::loadFragmentShader(fragmentShaderFilePath);
        pickShaders[i].program             = GLSL::PtrProgram(new GLSL::Program());

        if ( (pickShaders[i].vertexShader == 0) ||
             (pickShaders[i].fragmentShader == 0) ||
             (pickShaders[i].program == 0) ) {
          std::cerr << "Unable to load pick shaders" << pickShaders[i].vertexShaderFile <<
                       " or " << pickShaders[i].fragmentShaderFile << std::endl;
          pickShaders[i].vertexShader   = 0;
          pickShaders[i].fragmentShader = 0;
          pickShaders[i].program        = 0;
          pickShaders[i].initialized    = false;
          return;
        }

        pickShaders[i].program->attach(pickShaders[i].vertexShader);
        pickShaders[i].program->attach(pickShaders[i].fragmentShader);
        pickShaders[i].program->link();

        pickShaders[i].initialized = true;
      }
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
