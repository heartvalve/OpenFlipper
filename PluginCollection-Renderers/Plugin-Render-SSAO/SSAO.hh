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

class SSAOPlugin : public QObject, BaseInterface, RenderInterface, LoggingInterface
{
  Q_OBJECT
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(RenderInterface)
    Q_INTERFACES(LoggingInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Render-SSAO")
#endif

    
signals:
  // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);


public :

  SSAOPlugin();
  ~SSAOPlugin();

  QString name() { return (QString("SSAO Plugin")); };
  QString description( ) { return (QString(tr("Screen Space Ambient Occlusion"))); };

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
  QString checkOpenGL();

private:

  /// free all gl resources
  void destroyResources();

  /// free viewer specific gl resources
  void destroyResources(int _viewerId);

  /// reload gl resources
  void reloadResources(int _viewerId, unsigned int _sceneTexWidth, unsigned int _sceneTexHeight);

  /// draw a quad in projection space (only positions)
  void drawQuadProj(float _x0 = -1.0f, float _y0 = 1.0f, 
    float _w = 2.0f, float _h = 2.0f);

  /// find all light nodes in the scene
  void traverseLightNodes(BaseNode* _node);

  /// peel shader generator based on lights and texture mode
  void generatePeelingShaders(GLSL::StringList* _strVertexShaderOut,
    GLSL::StringList* _strFragmentShaderOut, bool _textured);

  /// draw the current scene
  void drawScenePass(ACG::GLState* _glState, Viewer::ViewerProperties& _properties, BaseNode* _sceneGraphRoot);

  /// gaussian blur pass
  struct ViewerResources;
  void gaussianBlurPass(const ViewerResources* _pViewer, const float* _texelSize, 
    GLenum _targetAttachement, GLuint _srcTexture);

  /// computes a hemisphere sampling kernel in [0,1] range
  void generateSamplingKernel();
private:
  
  struct ViewerResources
  {
    ViewerResources();

    /// viewer window width
    unsigned int glWidth_;

    /// viewer window height
    unsigned int glHeight_;

    /// scene render target width
    unsigned int rtSceneWidth_; 
    /// scene render target height
    unsigned int rtSceneHeight_;

    /// render target width
    unsigned int rtWidth_; 
    /// render target height
    unsigned int rtHeight_;

    /// downsampled rt width
    unsigned int rtDownWidth_;
    /// downsampled rt height
    unsigned int rtDownHeight_;

    /// depth buffer render target
    GLuint depthBufTex_;

    /// scene normal buffer render target
    /// R8G8B8 format
    GLuint sceneNormalTex_;

    /// depth renderbuffer for ssaoFbo
    GLuint depthSSAORenderBuf_;

    /// depth renderbuffer for sceneFbo
    GLuint depthSceneRenderBuf_;

    /// standard scene without a render target
    GLuint sceneBufTex_;

    /// downsampled depth render target
    GLuint downsampledTex_;

    /// downsampled temp rt for intermediate results
    GLuint downsampledTmpTex_;

    /// occlusion render target
    GLuint occlusionTex_;

    /// sceneFbo for scene color rendering only (seperated for multisampling)
    /// attachment order: only scene color texture
    GLuint sceneFbo_;

    /// ssaoFbo for deferred rendering
    /// attachment order: depth, normal, occlusion
    GLuint ssaoFbo_;

    /// blurFbo for downsampling and gaussian blur filter
    /// attachment order: downsampled, downsampledTmp, occlusion
    GLuint blurFbo_;
  };

  std::map<int, ViewerResources> viewerRes_;

  /// shader resources
  GLSL::Shader* shaders_[10];

  enum
  { 
    PROG_INIT = 0,  // early Z pass + standard scene rendering
    PROG_DOWNSAMPLING,
    PROG_BLUR,
    PROG_SSAO,
    PROG_FINAL,
    PROG_FINAL_MSAA
  };
  
  /// shader programs
  GLSL::Program* programs_[6];

  /// random vector table for sample offset rotation
  GLuint randomVecTex_;

  /// number of samples
  static const unsigned int numSamples_;

  /// ssao sampling kernel
  ACG::Vec3f samplingKernel_[128];
};
