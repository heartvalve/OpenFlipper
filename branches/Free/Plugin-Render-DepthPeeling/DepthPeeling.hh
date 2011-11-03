/*===========================================================================*\
*                                                                            *
*   $Revision:  $                                                       *
*   $LastChangedBy:  $                                                *
*   $Date:  $                     *
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
    bool _textured, bool _flatShaded, bool _phong);

  /// draw the current scene
  void drawScenePass(ACG::GLState* _glState, Viewer::ViewerProperties& _properties, BaseNode* _sceneGraphRoot);


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

    /// current shader state based on the Viewer DrawMode
    bool peelShaderTextured_,  peelShaderFlat_,  peelShaderPhong_;

    /// peel vertex shader
    GLSL::Shader* peelVertexShader_;
    
    /// peel fragment shader
    GLSL::Shader* peelFragmentShader_;

    /// peel geometry shader
    GLSL::Shader* peelGeometryShader_;
    
    /// peel program
    GLSL::Program* peelProg_;
  };

  std::map<int, ViewerResources> viewerRes_;

  /// shader resources
  GLSL::Shader* blendShaders_[8];

  /// depth peeling programs
  GLSL::Program* blendDualPeelProg_[4];

  /// fragment query
  GLuint blendQueryID_;

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
