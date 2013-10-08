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

#pragma once

#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/RenderInterface.hh>

#include <ACG/GL/IRenderer.hh>

class NormalRenderer : public QObject, BaseInterface, RenderInterface, ACG::IRenderer
{
  Q_OBJECT
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(RenderInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Render-Normals")
#endif

public:
  NormalRenderer();
  ~NormalRenderer();

  QString name() { return (QString("Normal Renderer Plugin")); };
  QString description( ) { return (QString(tr("Render colors based on normals (only if they are available!)"))); };

public slots:
  QString version() { return QString("1.0"); };
  QString renderObjectsInfo(bool _outputShaderInfo) { return dumpCurrentRenderObjectsToString(&sortedObjects_[0],_outputShaderInfo); };

private slots:

  //BaseInterface
  void initializePlugin();
  void exit(){}

  // RenderInterface
  void render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties);
  QString rendererName() {return QString("Normal_Renderer");}
  void supportedDrawModes(ACG::SceneGraph::DrawModes::DrawMode& _mode) {_mode = ACG::SceneGraph::DrawModes::DEFAULT;}
  
  QString checkOpenGL();
};

