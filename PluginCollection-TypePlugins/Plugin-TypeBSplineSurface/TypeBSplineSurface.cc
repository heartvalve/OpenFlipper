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

//=============================================================================
//
//  CLASS Type BSpline Surface Plugin - IMPLEMENTATION
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  Date: 2010-02-02 11:01:53 +0100 (Di, 02. Feb 2010) 
//
//=============================================================================


#include "TypeBSplineSurface.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <OpenFlipper/common/GlobalOptions.hh>

//-----------------------------------------------------------------------------

TypeBSplineSurfacePlugin::
TypeBSplineSurfacePlugin() :
renderControlNetAction_(0),
renderSurfaceAction_(0),
renderCPSelectionAction_(0),
renderKnotSelectionAction_(0),
renderNoSelectionAction_(0)
{
}

//-----------------------------------------------------------------------------

/** \brief Second initialization phase
 *
 */
void TypeBSplineSurfacePlugin::pluginsInitialized()
{

  if ( OpenFlipper::Options::gui() ){

    QMenu* contextMenu = new QMenu("Rendering");

    QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

    // Render Control Net
    renderControlNetAction_ = new QAction(tr("Render Control Net"), this);
    renderControlNetAction_->setStatusTip(tr("Render Control Net"));
//    renderControlNetAction_->setIcon( QIcon(iconPath + "showIndices.png") );
    renderControlNetAction_->setCheckable(true);
    renderControlNetAction_->setChecked(false);

    // Render Surface
    renderSurfaceAction_ = new QAction(tr("Render Surface"), this);
    renderSurfaceAction_->setStatusTip(tr("Render Surface"));
//    renderSurfaceAction_->setIcon( QIcon(iconPath + "coordsys.png") );
    renderSurfaceAction_->setCheckable(true);
    renderSurfaceAction_->setChecked(true);

    QActionGroup* group = new QActionGroup(this);
    group->setExclusive(true);

    // Render Control Point Selection
    renderCPSelectionAction_ = new QAction(tr("Render Control Point Selection"), group);
    renderCPSelectionAction_->setStatusTip(tr("Render Control Point Selection"));
//    renderCPSelectionAction_->setIcon( QIcon(iconPath + "coordsys.png") );
    renderCPSelectionAction_->setCheckable(true);
    renderCPSelectionAction_->setChecked(true);

    // Render Knot Selection
    renderKnotSelectionAction_ = new QAction(tr("Render Knot Selection"), group);
    renderKnotSelectionAction_->setStatusTip(tr("Render Knot Selection"));
//    renderKnotSelectionAction_->setIcon( QIcon(iconPath + "coordsys.png") );
    renderKnotSelectionAction_->setCheckable(true);
    renderKnotSelectionAction_->setChecked(true);

    // Render No Selection
    renderNoSelectionAction_ = new QAction(tr("Don't Render Selection"), group);
    renderNoSelectionAction_->setStatusTip(tr("Don't Render Selection"));
//    renderNoSelectionAction_->setIcon( QIcon(iconPath + "coordsys.png") );
    renderNoSelectionAction_->setCheckable(true);
    renderNoSelectionAction_->setChecked(true);


    connect(renderControlNetAction_,     SIGNAL(triggered()), this, SLOT(slotRenderControlNet()) );
    connect(renderSurfaceAction_, SIGNAL(triggered()), this, SLOT(slotRenderSurface()) );

    connect(group, SIGNAL(triggered(QAction*)), this, SLOT(slotRenderSelection(QAction*)));

    contextMenu->addAction(renderControlNetAction_);
    contextMenu->addAction(renderSurfaceAction_);
    contextMenu->addSeparator();
    contextMenu->addAction(renderCPSelectionAction_);
    contextMenu->addAction(renderKnotSelectionAction_);
    contextMenu->addAction(renderNoSelectionAction_);

    emit addContextMenuItem(contextMenu->menuAction(), DATA_BSPLINE_SURFACE, CONTEXTOBJECTMENU);
  }
}

//-----------------------------------------------------------------------------

void
TypeBSplineSurfacePlugin::
slotUpdateContextMenu( int _objectId ) {
  if ( _objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(_objectId,object) )
    return;

  BSplineSurfaceObject* bsplineSurfaceObject = dynamic_cast<BSplineSurfaceObject*>(object);

  if(bsplineSurfaceObject != 0){
    renderControlNetAction_->setChecked( bsplineSurfaceObject->splineSurfaceNode()->render_control_net() );
    renderSurfaceAction_->setChecked( bsplineSurfaceObject->splineSurfaceNode()->render_bspline_surface() );
    renderCPSelectionAction_->setChecked( bsplineSurfaceObject->splineSurfaceNode()->get_selection_draw_mode() == ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::CONTROLPOINT );
    renderKnotSelectionAction_->setChecked( bsplineSurfaceObject->splineSurfaceNode()->get_selection_draw_mode() == ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::KNOTVECTOR );
    renderNoSelectionAction_->setChecked( bsplineSurfaceObject->splineSurfaceNode()->get_selection_draw_mode() == ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::NONE );
  }
}

//-----------------------------------------------------------------------------

void TypeBSplineSurfacePlugin::slotRenderControlNet(){

  QVariant contextObject = renderControlNetAction_->data();
  int objectId = contextObject.toInt();

  if ( objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(objectId,object) )
    return;

  BSplineSurfaceObject* bsplineSurfaceObject = dynamic_cast<BSplineSurfaceObject*>(object);

  if(bsplineSurfaceObject != 0){
    bsplineSurfaceObject->splineSurfaceNode()->render_control_net(renderControlNetAction_->isChecked());
    emit updatedObject( objectId, UPDATE_ALL );
  }
}

//-----------------------------------------------------------------------------

void TypeBSplineSurfacePlugin::slotRenderSurface(){

  QVariant contextObject = renderSurfaceAction_->data();
  int objectId = contextObject.toInt();

  if ( objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(objectId,object) )
    return;

  BSplineSurfaceObject* bsplineSurfaceObject = dynamic_cast<BSplineSurfaceObject*>(object);

  if(bsplineSurfaceObject != 0){
    bsplineSurfaceObject->splineSurfaceNode()->render_bspline_surface(renderSurfaceAction_->isChecked());
    emit updatedObject( objectId, UPDATE_ALL );
  }
}

//-----------------------------------------------------------------------------

void TypeBSplineSurfacePlugin::slotRenderSelection(QAction* _action) {

  QVariant contextObject = _action->data();
  int objectId = contextObject.toInt();

  if ( objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(objectId,object) )
    return;

  BSplineSurfaceObject* bsplineSurfaceObject = dynamic_cast<BSplineSurfaceObject*>(object);

  if(bsplineSurfaceObject != 0){
    if(_action == renderCPSelectionAction_) {
      bsplineSurfaceObject->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::CONTROLPOINT);
      emit updatedObject( objectId, UPDATE_ALL );
    } else if(_action == renderKnotSelectionAction_) {
      bsplineSurfaceObject->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::KNOTVECTOR);
      emit updatedObject( objectId, UPDATE_ALL );
    } else if(_action == renderNoSelectionAction_) {
      bsplineSurfaceObject->splineSurfaceNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineSurfaceNodeT<BSplineSurface>::NONE);
      emit updatedObject( objectId, UPDATE_ALL );

    }
  }
}

//-----------------------------------------------------------------------------

bool
TypeBSplineSurfacePlugin::
registerType() 
{
  addDataType("BSplineSurface",tr("B-Spline Surface"));
  setTypeIcon("BSplineSurface", "BSplineSurfaceType.png");
  return true;
}

//-----------------------------------------------------------------------------

DataType
TypeBSplineSurfacePlugin::
supportedType()
{
  DataType type = DATA_BSPLINE_SURFACE;
  return type;
}

//-----------------------------------------------------------------------------

int
TypeBSplineSurfacePlugin::
addEmpty()
{
  // new object data struct
  BSplineSurfaceObject * object = new BSplineSurfaceObject();

  if ( PluginFunctions::objectCount() == 1 )
    object->target(true);

  if (PluginFunctions::targetCount() == 0 )
    object->target(true);

  QString name = "BSplineSurface_" + QString::number( object->id() ) + ".bss";

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  // set the default colors
  const QColor color = OpenFlipper::Options::defaultColor();
  const ACG::Vec4f default_color(color.redF(), color.greenF(), color.blueF(), color.alphaF());
  object->materialNode()->set_color(default_color);

  object->update();

  object->show();

  emit log(LOGINFO,object->getObjectinfo());

  emit emptyObjectAdded (object->id() );

  return object->id();
}

//-----------------------------------------------------------------------------

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( typebsplinesurfaceplugin , TypeBSplineSurfacePlugin );
#endif


