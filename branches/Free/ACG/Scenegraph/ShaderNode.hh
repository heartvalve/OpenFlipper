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
//  CLASS ShaderNode
//
//=============================================================================

#ifndef ACG_SHADER_NODE_HH
#define ACG_SHADER_NODE_HH


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include <string>

#include "../ShaderUtils/GLSLShader.hh"

//== NAMESPACES ===============================================================

namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class ShaderNode ShaderNode.hh <ACG/Scenegraph/ShaderNode.hh>

    Set shaders for this node and all its children.
    All changes will be done in the enter() method undonecd ..cd
    in the leave() method.
**/

class ACGDLLEXPORT ShaderNode : public BaseNode
{
public:

  /// Default constructor. Applies all properties.
  ShaderNode( BaseNode*           _parent = 0,
              const std::string&  _name = "<ShaderNode>" );

  /// Destructor.
  virtual ~ShaderNode();


  ACG_CLASSNAME(ShaderNode);

  /// set shader
  void enter(GLState& /*_state*/, unsigned int _drawmode);
  /// disable shader
  void leave(GLState& /*_state*/, unsigned int _drawmode);

  /// Sets the shader dir.
  void setShaderDir( std::string _shaderDir);

  std::string shaderDir() { return shaderDir_; };

  std::string vertexShaderName(unsigned int _drawmode);

  std::string fragmentShaderName(unsigned int _drawmode);

  /** Sets a Shader for this draw Mode
   * @param _drawmode Set the drawmode for which the shader should be activated
   * @param _vertexShader filename of the Vertex Shader within the shader directory
   * @param _fragmentShader filename of the Fragment Shader within the shader directory
  */
  void setShader( unsigned int _drawmode ,
                  std::string _vertexShader,
                  std::string _fragmentShader);

  /// Get the shader for the given drawMode
  GLSL::PtrProgram getShader( unsigned int _drawmode );

  /// Check if a shader is available for the given drawMode
  bool hasShader( unsigned int _drawmode );

  /// return available draw modes
  unsigned int availableDrawModes() const;

private :
  // Path to the shaders ( if set ). If empty shaders will not be used.
  std::string shaderDir_;

  class ShaderInfo {

    public :
      ShaderInfo() :
        vertexShader(0),
        vertexShaderFile(""),
        fragmentShader(0),
        fragmentShaderFile(""),
        program(0),
        initialized(false)
      {
      };

    GLSL::PtrVertexShader   vertexShader;
    std::string             vertexShaderFile;

    GLSL::PtrFragmentShader fragmentShader;
    std::string             fragmentShaderFile;

    GLSL::PtrProgram        program;

    bool                    initialized;
  };

  ShaderInfo shaders[32];
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_SHADER_NODE_HH defined
//=============================================================================

