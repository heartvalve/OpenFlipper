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

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#include "RenderPickingPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <QGLFormat>
#include <QMenu>

#if QT_VERSION >= 0x050000 
#include <QtWidgets>
#else
#include <QtGui>
#endif

void RenderPickingPlugin::initializePlugin()
{
  pickRendererMode_ = ACG::SceneGraph::PICK_ANYTHING;
}

QAction* RenderPickingPlugin::optionsAction() {

  QMenu* menu = new QMenu("Picking Renderer Target");

  // Recreate actionGroup
  QActionGroup* pickingTargetsGroup = new QActionGroup( this );
  pickingTargetsGroup->setExclusive( true );

  // Always set PickAnything ( will be overridden by others)
  QAction * action = new QAction("PICK_ANYTHING" , pickingTargetsGroup );
  action->setCheckable( true );
  action->setChecked(true);

  action = new QAction("PICK_VERTEX" , pickingTargetsGroup );
  action->setCheckable( true );
  if (pickRendererMode_ == ACG::SceneGraph::PICK_VERTEX)
    action->setChecked(true);

  action = new QAction("PICK_EDGE" , pickingTargetsGroup );
  action->setCheckable( true );
  if (pickRendererMode_ == ACG::SceneGraph::PICK_EDGE)
    action->setChecked(true);

  action = new QAction("PICK_SPLINE" , pickingTargetsGroup );
  action->setCheckable( true );
  if (pickRendererMode_ == ACG::SceneGraph::PICK_SPLINE)
    action->setChecked(true);

  action = new QAction("PICK_FACE" , pickingTargetsGroup );
  action->setCheckable( true );
  if (pickRendererMode_ == ACG::SceneGraph::PICK_FACE)
    action->setChecked(true);

  action = new QAction("PICK_FRONT_VERTEX" , pickingTargetsGroup );
  action->setCheckable( true );
  if (pickRendererMode_ == ACG::SceneGraph::PICK_FRONT_VERTEX)
    action->setChecked(true);

  action = new QAction("PICK_FRONT_EDGE" , pickingTargetsGroup );
  action->setCheckable( true );
  if (pickRendererMode_ == ACG::SceneGraph::PICK_FRONT_EDGE)
    action->setChecked(true);

  action = new QAction("PICK_CELL" , pickingTargetsGroup );
  action->setCheckable( true );
  if (pickRendererMode_ == ACG::SceneGraph::PICK_CELL)
    action->setChecked(true);

  menu->addActions(pickingTargetsGroup->actions());

  connect(pickingTargetsGroup,SIGNAL(triggered( QAction * )),this,SLOT(slotPickTargetChanged( QAction * )));

  return menu->menuAction();
}

void RenderPickingPlugin::slotPickTargetChanged( QAction *  _action) {

  // Prepare Picking Debugger Flag
  if ( _action->text() == "PICK_ANYTHING") {
    pickRendererMode_ = ACG::SceneGraph::PICK_ANYTHING;
  } else if ( _action->text() == "PICK_VERTEX") {
    pickRendererMode_ = ACG::SceneGraph::PICK_VERTEX;
  } else if ( _action->text() == "PICK_EDGE") {
    pickRendererMode_ = ACG::SceneGraph::PICK_EDGE;
  } else if ( _action->text() == "PICK_SPLINE") {
    pickRendererMode_ = ACG::SceneGraph::PICK_SPLINE;
  } else if ( _action->text() == "PICK_FACE") {
    pickRendererMode_ = ACG::SceneGraph::PICK_FACE;
  } else if ( _action->text() == "PICK_FRONT_VERTEX") {
    pickRendererMode_ = ACG::SceneGraph::PICK_FRONT_VERTEX;
  } else if ( _action->text() == "PICK_FRONT_EDGE") {
    pickRendererMode_ = ACG::SceneGraph::PICK_FRONT_EDGE;
  } else if ( _action->text() == "PICK_CELL") {
    pickRendererMode_ = ACG::SceneGraph::PICK_CELL;
  } else {
    std::cerr << "Error : optionHandling unable to find pick mode!!! " << _action->text().toStdString() << std::endl;
    pickRendererMode_ = ACG::SceneGraph::PICK_ANYTHING;
  }

}

QString RenderPickingPlugin::rendererName() {
  return QString("Picking renderer");
}

void RenderPickingPlugin::supportedDrawModes(ACG::SceneGraph::DrawModes::DrawMode& _mode) {
  _mode =  ACG::SceneGraph::DrawModes::DEFAULT;
}


void RenderPickingPlugin::render(ACG::GLState* _glState, Viewer::ViewerProperties& _properties) {

  ACG::GLState::disable(GL_LIGHTING);
  ACG::GLState::disable(GL_BLEND);
  glClear(GL_DEPTH_BUFFER_BIT);

  // do the picking
  _glState->pick_init (true);
  ACG::SceneGraph::PickAction action(*_glState, pickRendererMode_, _properties.drawMode());
  ACG::SceneGraph::traverse_multipass( PluginFunctions::getSceneGraphRootNode() , action,*_glState);

  ACG::GLState::enable(GL_LIGHTING);
  ACG::GLState::enable(GL_BLEND);

}

QString RenderPickingPlugin::checkOpenGL() {

  // TODO: Correctly configure the following requirements!

  // Get version and check
  QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
  if ( ! flags.testFlag(QGLFormat::OpenGL_Version_2_0) )
    return QString("Insufficient OpenGL Version! OpenGL 2.0 or higher required");

  //Get OpenGL extensions
  QString glExtensions = QString((const char*)glGetString(GL_EXTENSIONS));

  // Collect missing extension
  QString missing = "";

  return missing;

}


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( renderpickingplugin , RenderPickingPlugin );
#endif

