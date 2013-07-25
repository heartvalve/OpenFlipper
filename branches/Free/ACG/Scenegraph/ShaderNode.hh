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
//  CLASS ShaderNode
//
//=============================================================================

#ifndef ACG_SHADER_NODE_HH
#define ACG_SHADER_NODE_HH


//== INCLUDES =================================================================

#include "BaseNode.hh"
#include <string>

#include "../ShaderUtils/GLSLShader.hh"
#include <ACG/Scenegraph/DrawModes.hh>

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
  void enter(GLState& /*_state*/, const DrawModes::DrawMode& _drawmode);
  /// disable shader
  void leave(GLState& /*_state*/, const DrawModes::DrawMode& _drawmode);

  /// set shader
  void enterPick(GLState& /*_state*/, PickTarget _target, const DrawModes::DrawMode& _drawmode);
  /// disable shader
  void leavePick(GLState& /*_state*/, PickTarget _target, const DrawModes::DrawMode& _drawmode);


  /// Sets the shader dir.
  void setShaderDir( std::string _shaderDir);

  std::string shaderDir() { return shaderDir_; };

  std::string vertexShaderName(DrawModes::DrawMode _drawmode, bool _pick = false);

  std::string fragmentShaderName(DrawModes::DrawMode _drawmode, bool _pick = false);
  
  /** Removes the shader for the given draw mode
   * @param _drawmode Set the drawmode for which the shader should be deactivated
  */
  void disableShader (DrawModes::DrawMode _drawmode);
    
  /** Sets a Shader for the given draw mode
   * @param _drawmode           Set the drawmode for which the shader should be activated
   * @param _vertexShader       filename of the Vertex Shader within the shader directory
   * @param _fragmentShader     filename of the Fragment Shader within the shader directory
   * @param _pickVertexShader   Vertex shader during picking
   * @param _pickFragmentShader Fragment shader during picking
  */

  void setShader( DrawModes::DrawMode _drawmode ,
                  std::string         _vertexShader,
                  std::string         _fragmentShader,
                  std::string         _pickVertexShader = "",
                  std::string         _pickFragmentShader = "");

  /// Get the shader for the given drawMode
  GLSL::PtrProgram getShader( DrawModes::DrawMode _drawmode, bool _pick = false);

  /// Check if a shader is available for the given drawMode
  bool hasShader( DrawModes::DrawMode _drawmode, bool _pick = false);

  /// return available draw modes
  DrawModes::DrawMode availableDrawModes() const;

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

  std::map< size_t, ShaderInfo> shaders;
  std::map< size_t, ShaderInfo> pickShaders;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_SHADER_NODE_HH defined
//=============================================================================

