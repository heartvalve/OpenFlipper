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
  for ( std::map<size_t,ShaderInfo>::iterator it = shaders.begin(); it != shaders.end(); ++it) {
    if ( it->second.initialized ) {
      
      if ( it->second.program != 0 )
        delete it->second.program;
      
      if ( it->second.vertexShader != 0 )
        delete it->second.vertexShader;
      
      if ( it->second.fragmentShader != 0 )
        delete it->second.fragmentShader;
    }
  }
}

//----------------------------------------------------------------------------
bool
ShaderNode::
hasShader( DrawModes::DrawMode _drawmode, bool _pick ) {
  
  if ( !_drawmode.isAtomic() ) {
    std::cerr << "hasShader: Error, draw mode not atomic!" << std::endl;
    return false;
  }
  
  std::map<size_t,ShaderInfo>::iterator it;
  
  if ( _pick ) {
    it = pickShaders.find(_drawmode.getIndex());
    if ( it == pickShaders.end() )
      return false;
  } else {
    it = shaders.find(_drawmode.getIndex());
    if ( it == shaders.end() )
      return false;
  }
  
  return it->second.initialized;
}

//----------------------------------------------------------------------------


void
ShaderNode::enter(GLState& /*_state*/, const DrawModes::DrawMode& _drawmode  )
{
  for ( std::map<size_t,ShaderInfo>::iterator it = shaders.begin(); it != shaders.end(); ++it) {
    if ( _drawmode.containsAtomicDrawMode(it->first) && it->second.initialized ) {
      it->second.program->use();
    }
  }
}

//----------------------------------------------------------------------------

void
ShaderNode::enterPick(GLState& /*_state*/, PickTarget /*_target*/, const DrawModes::DrawMode& _drawmode  )
{
  for ( std::map<size_t,ShaderInfo>::iterator it = pickShaders.begin(); it != pickShaders.end(); ++it) {
    if ( _drawmode.containsAtomicDrawMode(it->first) && it->second.initialized ) {
      it->second.program->use();
    }
  }  
}

//----------------------------------------------------------------------------
std::string
ShaderNode::vertexShaderName(DrawModes::DrawMode _drawmode, bool _pick) {
  
  if ( !_drawmode.isAtomic() ) {
    std::cerr << "vertexShaderName: Error, draw mode not atomic!" << std::endl;
    return std::string("");
  }
  
  std::map<size_t,ShaderInfo>::iterator it;
  
  if ( _pick ) {
    it = pickShaders.find(_drawmode.getIndex());
    if ( it == pickShaders.end() )
      return  std::string("");
  } else {
    it = shaders.find(_drawmode.getIndex());
    if ( it == shaders.end() )
      return  std::string("");
  }
  
  if ( it->second.initialized )
    return it->second.vertexShaderFile;
  
  return  std::string("");
}

//----------------------------------------------------------------------------
std::string
ShaderNode::fragmentShaderName(DrawModes::DrawMode _drawmode, bool _pick) {
  
  if ( !_drawmode.isAtomic() ) {
    std::cerr << "fragmentShaderName: Error, draw mode not atomic!" << std::endl;
    return std::string("");
  }
  
  std::map<size_t,ShaderInfo>::iterator it;
  
  if ( _pick ) {
    it = pickShaders.find(_drawmode.getIndex());
    if ( it == pickShaders.end() )
      return  std::string("");
  } else {
    it = shaders.find(_drawmode.getIndex());
    if ( it == shaders.end() )
      return  std::string("");
  }
  
  if ( it->second.initialized )
    return it->second.fragmentShaderFile;
  
  return  std::string("");
}


//----------------------------------------------------------------------------


void ShaderNode::leave(GLState& /*_state*/, const DrawModes::DrawMode& _drawmode )
{
  for ( std::map<size_t,ShaderInfo>::iterator it = shaders.begin(); it != shaders.end(); ++it)
    if ( _drawmode.containsAtomicDrawMode(it->first) && it->second.initialized ) 
      it->second.program->disable();
}

//----------------------------------------------------------------------------


void ShaderNode::leavePick(GLState& /*_state*/, PickTarget /*_target*/, const DrawModes::DrawMode& _drawmode )
{
  for ( std::map<size_t,ShaderInfo>::iterator it = pickShaders.begin(); it != pickShaders.end(); ++it)
    if ( _drawmode.containsAtomicDrawMode(it->first) && it->second.initialized ) 
      it->second.program->disable();
}

//----------------------------------------------------------------------------

/// Get the shader for the given drawMode
GLSL::PtrProgram
ShaderNode::
getShader( DrawModes::DrawMode _drawmode, bool _pick ) {
  
  if ( !_drawmode.isAtomic() ) {
    std::cerr << "getShader: Error, draw mode not atomic!" << std::endl;
    return 0;
  }
  
  std::map<size_t,ShaderInfo>::iterator it;
  
  if ( _pick ) {
    it = pickShaders.find(_drawmode.getIndex());
    
    if ( it == pickShaders.end() )
      return  0;
    
    if ( it->second.initialized )
      return it->second.program;
    else 
      return 0;
    
  } else {
    it = shaders.find(_drawmode.getIndex());
    
    if ( it == shaders.end() )
      return  0;
    
    if ( it->second.initialized )
      return it->second.program;
    else 
      return 0;
  }

  // No shader found for this mode
  return  0;
}

//----------------------------------------------------------------------------

void
ShaderNode::
disableShader (DrawModes::DrawMode _drawmode) {


  if ( !_drawmode.isAtomic() ) {
    std::cerr << "disableShader: Error, draw mode not atomic!" << std::endl;
    return;
  }

  size_t index = _drawmode.getIndex();

  // Cleanup old shaders for this mode, if they exist
  if ( shaders[index].initialized ) {
    if ( shaders[index].program != 0 )
      delete shaders[index].program;

    if ( shaders[index].vertexShader != 0 )
      delete shaders[index].vertexShader;

    if ( shaders[index].fragmentShader != 0 )
      delete shaders[index].fragmentShader;

    shaders[index].initialized    = false;
  }
}

//----------------------------------------------------------------------------

void
ShaderNode::
setShader( DrawModes::DrawMode _drawmode ,
           std::string _vertexShader,
           std::string _fragmentShader,
           std::string _pickVertexShader,
           std::string _pickFragmentShader) {

  if ( !ACG::openGLVersion(2,0) ) {
    std::cerr << "Shaders not supported with OpenGL Version less than 2.0" << std::endl;
    return;
  }

  if ( shaderDir_ == "" ) {
    std::cerr << "No shader dir set for shadernode. Unable to load shaders!" << std::endl;
    return;
  }
  
  if ( !_drawmode.isAtomic() ) {
    std::cerr << "setShader: Error, draw mode not atomic!" << std::endl;
    return;
  }

  disableShader (_drawmode);
  size_t index = _drawmode.getIndex();

  shaders[index].vertexShaderFile   = shaderDir_ + _vertexShader;
  shaders[index].fragmentShaderFile = shaderDir_ + _fragmentShader;

  const char* vertexShaderFilePath   = shaders[index].vertexShaderFile.c_str();
  const char* fragmentShaderFilePath = shaders[index].fragmentShaderFile.c_str();
  shaders[index].vertexShader            = GLSL::loadVertexShader(vertexShaderFilePath);
  shaders[index].fragmentShader          = GLSL::loadFragmentShader(fragmentShaderFilePath);
  shaders[index].program                 = GLSL::PtrProgram(new GLSL::Program());

  if ( (shaders[index].vertexShader == 0) ||
        (shaders[index].fragmentShader == 0) ||
        (shaders[index].program == 0) ) {
    std::cerr << "Unable to load shaders" << shaders[index].vertexShaderFile <<
                  " or " << shaders[index].fragmentShaderFile << std::endl;
    shaders[index].vertexShader   = 0;
    shaders[index].fragmentShader = 0;
    shaders[index].program        = 0;
    shaders[index].initialized    = false;
    return;
  }

  shaders[index].program->attach(shaders[index].vertexShader);
  shaders[index].program->attach(shaders[index].fragmentShader);
  shaders[index].program->link();

  shaders[index].initialized = true;

  
  // Cleanup old shaders for this mode, if they exist
  if ( pickShaders[index].initialized ) {
    if ( pickShaders[index].program != 0 )
        delete pickShaders[index].program;

    if ( pickShaders[index].vertexShader != 0 )
      delete pickShaders[index].vertexShader;

    if ( pickShaders[index].fragmentShader != 0 )
      delete pickShaders[index].fragmentShader;

    pickShaders[index].initialized    = false;
  }

  if (_pickVertexShader.length () > 0 && _pickFragmentShader.length () > 0)
  {
    pickShaders[index].vertexShaderFile   = shaderDir_ + _pickVertexShader;
    pickShaders[index].fragmentShaderFile = shaderDir_ + _pickFragmentShader;

    const char* vertexShaderFilePath   = pickShaders[index].vertexShaderFile.c_str();
    const char* fragmentShaderFilePath = pickShaders[index].fragmentShaderFile.c_str();
    pickShaders[index].vertexShader        = GLSL::loadVertexShader(vertexShaderFilePath);
    pickShaders[index].fragmentShader      = GLSL::loadFragmentShader(fragmentShaderFilePath);
    pickShaders[index].program             = GLSL::PtrProgram(new GLSL::Program());

    if ( (pickShaders[index].vertexShader == 0) ||
          (pickShaders[index].fragmentShader == 0) ||
          (pickShaders[index].program == 0) ) {
      std::cerr << "Unable to load pick shaders" << pickShaders[index].vertexShaderFile <<
                    " or " << pickShaders[index].fragmentShaderFile << std::endl;
      pickShaders[index].vertexShader   = 0;
      pickShaders[index].fragmentShader = 0;
      pickShaders[index].program        = 0;
      pickShaders[index].initialized    = false;
      return;
    }

    pickShaders[index].program->attach(pickShaders[index].vertexShader);
    pickShaders[index].program->attach(pickShaders[index].fragmentShader);
    pickShaders[index].program->link();

    pickShaders[index].initialized = true;
  }
}

//----------------------------------------------------------------------------

void
ShaderNode::
setShaderDir( std::string _shaderDir) {
  shaderDir_ = _shaderDir;
}

DrawModes::DrawMode
ShaderNode::
availableDrawModes() const
{
  DrawModes::DrawMode drawModes(DrawModes::NONE);

  for ( std::map<size_t,ShaderInfo>::const_iterator it = shaders.begin(); it != shaders.end(); ++it) {
    // If the shader for this drawmode is initialized, this node supports the given draw mode.
    // Then we add it to the list of supported draw modes
    if ( it->second.initialized) {
      drawModes |= DrawModes::DrawMode(it->first);
    }
  }

  return drawModes;

}

//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
