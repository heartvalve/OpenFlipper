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
*   $Revision: 13374 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2012-01-13 09:38:16 +0100 (Fri, 13 Jan 2012) $                     *
*                                                                            *
\*===========================================================================*/


#pragma once


#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/RenderInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>

#include <ACG/Scenegraph/SceneGraph.hh>
#include <ACG/Scenegraph/MaterialNode.hh>
#include <ObjectTypes/Light/LightNode.hh>

#include <ACG/GL/IRenderer.hh>


#include <map>


class Renderer : public QObject, BaseInterface, RenderInterface, LoggingInterface
{
  Q_OBJECT
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(RenderInterface)
    Q_INTERFACES(LoggingInterface)

signals:
  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);


public:
  Renderer();
  ~Renderer();

  QString name() { return (QString("Default Classical Renderer Plugin")); };
  QString description( ) { return (QString(tr("Implementation of OpenFlippers classical rendering pipeline in a plugin"))); };

public slots:
  QString version() { return QString("1.0"); };

private slots:

  //BaseInterface
  void initializePlugin();

  // RenderInterface
  void render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);
  QString rendererName() {return QString("Default Classical Renderer");}
  void supportedDrawModes(ACG::SceneGraph::DrawModes::DrawMode& _mode) {_mode = ACG::SceneGraph::DrawModes::DEFAULT;}
  
  QString checkOpenGL();

private:
  void drawScene_mono(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);

  //===========================================================================
   /** @name Stereo
    * @{ */
   //===========================================================================

   private:

     /// helper called by drawScene() when stereo viewing is active.
     void drawScene_stereo(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);

     /// helper called by drawScene_stereo() when opengl stereo viewing is active.
     void drawScene_glStereo(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);

     /// helper called by drawScene_stereo() when anaglyph stereo viewing is active.
     void drawScene_anaglyphStereo(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);

     /// helper called by drawScene_stereo() when custom anaglyph stereo viewing is active.
     void drawScene_customAnaglyphStereo(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);

     /// helper called to initialize/update custom anaglyph stereo
     void updateCustomAnaglyphStereo(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);

     /// helper called to cleanup custom anaglyph stereo
     void finiCustomAnaglyphStereo();

     /** @} */

private:

     // custom anaglyph stuff
     int    agTexWidth_;
     int    agTexHeight_;
     GLuint agTexture_[2];
     GLuint agProgram_;
     bool   customAnaglyphSupported_;

};

