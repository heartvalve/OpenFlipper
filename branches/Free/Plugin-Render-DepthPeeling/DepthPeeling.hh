/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/RenderInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

#include <ACG/Scenegraph/SceneGraph.hh>

#include <map>


class DepthPeelingPlugin : public QObject, BaseInterface, RenderInterface, LoggingInterface
{
  Q_OBJECT
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(RenderInterface)
    Q_INTERFACES(LoggingInterface)
    
    
signals:
  // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);


public :

  DepthPeelingPlugin();
  ~DepthPeelingPlugin();

  QString name() { return (QString("Dual Depth Peeling Plugin")); };
  QString description( ) { return (QString(tr("Order independent transparency renderer"))); };

public slots:
  QString version() { return QString("1.0"); };

private slots:

  //BaseInterface
  void initializePlugin();
  void exit();

  // RenderInterface
  void render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);
  QString rendererName();
  void supportedDrawModes(ACG::SceneGraph::DrawModes::DrawMode& _mode);

private:

  /// free all gl resources
  void destroyResources();

  /// free viewer specific gl resources
  void destroyResources(int _viewerId);

  /// reload gl resources
  void reloadResources(int _viewerId);

  /// draw a quad in projection space (only positions)
  void drawQuadProj(float _x0 = -1.0f, float _y0 = 1.0f, 
    float _w = 2.0f, float _h = 2.0f);

  /// find all light nodes in the scene
  void traverseLightNodes(BaseNode* _node);

  /// peel shader generator based on lights and texture mode
  void generatePeelingShaders(GLSL::StringList* _strVertexShaderOut,
    GLSL::StringList* _strFragmentShaderOut,
    GLSL::StringList* _strGeometryShaderOut,
    bool _textured, bool _flatShaded, bool _phong, bool _vertexColor, bool _wireFrame);

  /// regenerates peeling shaders based on light nodes in scenegraph
  void updatePeelingShaderSet();

  /// draw the current scene
  void drawScenePass(ACG::GLState* _glState, ACG::SceneGraph::DrawModes::DrawMode _drawMode, BaseNode* _sceneGraphRoot);
  
  void drawScenePeelPass(ACG::GLState* _glState, ACG::SceneGraph::DrawModes::DrawMode _drawMode, BaseNode* _sceneGraphRoot, int _peelPass);
  
  template <class Action>
  void traverseDraw(BaseNode* _node, Action& _action, ACG::SceneGraph::DrawModes::DrawMode _globalDrawMode, int _pass, int _peelPass);
  
  template <class Action>
  bool traverseDrawApplyAction(BaseNode* _node, Action& _action, ACG::SceneGraph::DrawModes::DrawMode _globalDrawMode, int _pass, int _peelPass);

  /// converts a drawmode to the correct shading program index
  unsigned int getPeelShaderIndex(ACG::SceneGraph::DrawModes::DrawMode _drawMode);

private:
  
  struct ViewerResources
  {
    ViewerResources();

    /// viewer window width
    unsigned int glWidth_;

    /// viewer window height
    unsigned int glHeight_;

    /// render target width
    unsigned int rtWidth_; 
    /// render target height
    unsigned int rtHeight_;

    /// render target textures: {depth0, depth1,  front_blend0, front_blend1,   back_temp0, back_temp1,   back_blend}
    GLuint blendDualPeelTexID_[7];
    /// depth peeling fbo
    GLuint blendDualPeelFbo_;
  };

  std::map<int, ViewerResources> viewerRes_;

  /// shader resources
  GLSL::Shader* blendShaders_[8];

  /// depth peeling programs
  GLSL::Program* blendDualPeelProg_[4];

  // note that shader flags are only good easy shader indexing
  //  some combinations like phong + flat make no sense
  enum
  {
    PEEL_SHADER_WIREFRAME = 0, // wireframe is a special case here, not combinable with others
    PEEL_SHADER_HIDDENLINE = 1, // hiddenline is another special case, it renders geometry in background color
    PEEL_SHADER_TEXTURED = 0x1,
    PEEL_SHADER_PHONG = 0x2,
    PEEL_SHADER_GOURAUD = 0x4,
    PEEL_SHADER_FLAT = 0x8,
    PEEL_SHADER_VERTEXCOLORS = 0x10,
    PEEL_NUM_COMBINATIONS = 0x20,
    PEEL_SHADER_NUM_FLAGS = 5
  };
  /// generated shader set
  GLSL::Shader* peelShaders_[PEEL_NUM_COMBINATIONS*3]; // interleaved: vertex, geometry, fragment

  /// generates shader programs
  GLSL::Program* peelProgs_[PEEL_NUM_COMBINATIONS];

  /// fragment query
  GLuint blendQueryID_;

  /// current glState ptr for hiddenline rendering
  ACG::GLState* glStateTmp_;

  /// number of used lights in the scene
  GLuint numLights_;

  /// light type enumeration
  enum LightType
  {
    LIGHTTYPE_DIRECTIONAL = 0,
    LIGHTTYPE_POINT,
    LIGHTTYPE_SPOT
  };

  /// registered lights in the scene
  LightType lightTypes_[16];
  /// matching GL light id
  GLuint glLightIDs_[16];
};
