/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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



#pragma once


#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/RenderInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>


#include <ACG/GL/IRenderer.hh>
#include <ACG/GL/FBO.hh>

class DepthPeeling : public QObject, BaseInterface, RenderInterface, LoggingInterface, ACG::IRenderer
{
  Q_OBJECT
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(RenderInterface)
    Q_INTERFACES(LoggingInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Render-DepthPeeling")
#endif

signals:
  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);


public:
  DepthPeeling();
  ~DepthPeeling();

  QString name() { return (QString("DepthPeeling Plugin")); };
  QString description( ) { return (QString(tr("DepthPeeling Rendering Pipeline (Alpha Version!)"))); };


  /// overide addRenderObject function to include OIT check
  void addRenderObject(ACG::RenderObject* _renderObject);


public slots:
  QString version() { return QString("1.0"); };

  QString renderObjectsInfo(bool _outputShaderInfo) { return dumpCurrentRenderObjectsToString(&sortedObjects_[0],_outputShaderInfo); };

  QAction* optionsAction();

private slots:

  //BaseInterface
  void initializePlugin();
  void exit(){}

  // RenderInterface
  void render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);
  QString rendererName() {return QString("Alpha_Version_DepthPeeling");}
  void supportedDrawModes(ACG::SceneGraph::DrawModes::DrawMode& _mode) {_mode = ACG::SceneGraph::DrawModes::DEFAULT;}

  QString checkOpenGL();

  void slotModeChanged( QAction *  );


private:


  /// peel the scene from front to back, one layer per pass
  void renderFrontPeeling(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);

  /// peel the scene with dual depth peeling, two layers per pass
  void renderDualPeeling(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);

  /// mode: 0 -> front to back peeling,  1 -> dual peeling
  int peelMode_;

  bool copyFrontDepth_;

  /// max peel count
  int maxPeelCount_;

  /// blends one depth-layer into the current scene target
  GLSL::Program* peelBlend_;

  /// final copy into back-buffer
  GLSL::Program* peelFinal_;

  /// occlusion query determining end of peeling (last layer)
  GLuint peelQueryID_;


  /// dual depth peeling shaders
  GLSL::Program* peelBlendDual_;
  GLSL::Program* peelFinalDual_;


  /// Collection of framebuffers for each viewport
  struct ViewerResources
  {
    ViewerResources();
    ~ViewerResources();

    // resize textures
    void resize(bool _dualPeeling, unsigned int _width, unsigned int _height);

    /// viewer window width
    unsigned int width_;

    /// viewer window height
    unsigned int height_;


    // single layer depth peeling textures (front to back peeling)
    //  ping-pong buffer for (depth, front) targets
    GLuint singleDepthTex_[2]; // float1: minDepth
    GLuint singleFrontTex_[2]; // rgba: color of front-peeled layer
    GLuint singleBlendTex_;    // rgba: color accumulation buffer

    ACG::FBO* singleFbo_; // targets: {depth0, front0,  depth1, front1, blend}




    // dual depth peeling textures
    //  ping-pong buffer for (depth, front, back) targets
    GLuint dualDepthTex_[2]; // float2: (-minDepth, maxDepth)
    GLuint dualFrontTex_[2]; // rgba: color of front-peeled layer
    GLuint dualBackTex_[2];  // rgba: color of back-peeled layer
    GLuint dualBlendTex_;    // rgb:  color accumulation buffer

    ACG::FBO* dualFbo_; // targets: {depth0, front0, back0,  depth1, front1, back1,  blend}
  };

  /// Allocate framebuffers and load shaders for depth-peeling.
  void initDepthPeeling();

  /// Allocate framebuffers and load shaders for dual-depth-peeling.
  void initDualDepthPeeling();

  /**
  * Stores framebuffer resources for each viewport.
  * Mapping: viewerID -> ViewerResources
  */
  std::map<int, ViewerResources> viewerRes_;




  // debug functions
//   GLSL::Program* dbgProg_;
// 
//   void dbgDrawTex(GLuint _texID);
};
