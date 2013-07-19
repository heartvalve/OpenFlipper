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
//  CLASS Type BSpline Curve Plugin - IMPLEMENTATION
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  Date: 2010-02-02 11:02:14 +0100 (Di, 02. Feb 2010) 
//
//=============================================================================


#include "TypeBSplineCurve.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <OpenFlipper/common/BackupData.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include "BSplineCurveBackup.hh"

//-----------------------------------------------------------------------------

TypeBSplineCurvePlugin::
TypeBSplineCurvePlugin():
renderControlPolygonAction_(0),
renderCurveAction_(0),
renderCPSelectionAction_(0),
renderKnotSelectionAction_(0),
renderNoSelectionAction_(0)
{
}


//-----------------------------------------------------------------------------

/** \brief Second initialization phase
 *
 */
void TypeBSplineCurvePlugin::pluginsInitialized()
{

  if ( OpenFlipper::Options::gui() ){

    QMenu* contextMenu = new QMenu("Rendering");

    QString iconPath = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();

    // Render Control Net
    renderControlPolygonAction_ = new QAction(tr("Render Control Polygon"), this);
    renderControlPolygonAction_->setStatusTip(tr("Render Control Polygon"));
//    renderControlPolygonAction_->setIcon( QIcon(iconPath + "showIndices.png") );
    renderControlPolygonAction_->setCheckable(true);
    renderControlPolygonAction_->setChecked(false);

    // Render Surface
    renderCurveAction_ = new QAction(tr("Render Curve"), this);
    renderCurveAction_->setStatusTip(tr("Render Curve"));
//    renderCurveAction_->setIcon( QIcon(iconPath + "coordsys.png") );
    renderCurveAction_->setCheckable(true);
    renderCurveAction_->setChecked(true);

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


    connect(renderControlPolygonAction_, SIGNAL(triggered()), this, SLOT(slotRenderControlPolygon()) );
    connect(renderCurveAction_,          SIGNAL(triggered()), this, SLOT(slotRenderCurve()) );

    connect(group, SIGNAL(triggered(QAction*)), this, SLOT(slotRenderSelection(QAction*)));

    contextMenu->addAction(renderControlPolygonAction_);
    contextMenu->addAction(renderCurveAction_);
    contextMenu->addSeparator();
    contextMenu->addAction(renderCPSelectionAction_);
    contextMenu->addAction(renderKnotSelectionAction_);
    contextMenu->addAction(renderNoSelectionAction_);

    emit addContextMenuItem(contextMenu->menuAction(), DATA_BSPLINE_CURVE, CONTEXTOBJECTMENU);
  }
}

//-----------------------------------------------------------------------------

void
TypeBSplineCurvePlugin::
slotUpdateContextMenu( int _objectId ) {
  if ( _objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(_objectId,object) )
    return;

  BSplineCurveObject* bsplineCurveObject = dynamic_cast<BSplineCurveObject*>(object);

  if(bsplineCurveObject != 0){
    renderControlPolygonAction_->setChecked( bsplineCurveObject->splineCurveNode()->render_control_polygon() );
    renderCurveAction_->setChecked( bsplineCurveObject->splineCurveNode()->render_bspline_curve() );
    renderCPSelectionAction_->setChecked( bsplineCurveObject->splineCurveNode()->get_selection_draw_mode() == ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::CONTROLPOINT );
    renderKnotSelectionAction_->setChecked( bsplineCurveObject->splineCurveNode()->get_selection_draw_mode() == ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::KNOTVECTOR );
    renderNoSelectionAction_->setChecked( bsplineCurveObject->splineCurveNode()->get_selection_draw_mode() == ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::NONE );
  }
}

//-----------------------------------------------------------------------------

void TypeBSplineCurvePlugin::slotRenderControlPolygon(){

  QVariant contextObject = renderControlPolygonAction_->data();
  int objectId = contextObject.toInt();

  if ( objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(objectId,object) )
    return;

  BSplineCurveObject* bsplineCurveObject = dynamic_cast<BSplineCurveObject*>(object);

  if(bsplineCurveObject != 0){
    bsplineCurveObject->splineCurveNode()->render_control_polygon(renderControlPolygonAction_->isChecked());
    emit updatedObject( objectId, UPDATE_ALL );
  }
}

//-----------------------------------------------------------------------------

void TypeBSplineCurvePlugin::slotRenderCurve(){

  QVariant contextObject = renderCurveAction_->data();
  int objectId = contextObject.toInt();

  if ( objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(objectId,object) )
    return;

  BSplineCurveObject* bsplineCurveObject = dynamic_cast<BSplineCurveObject*>(object);

  if(bsplineCurveObject != 0){
    bsplineCurveObject->splineCurveNode()->render_bspline_curve(renderCurveAction_->isChecked());
    emit updatedObject( objectId, UPDATE_ALL );
  }
}

//-----------------------------------------------------------------------------

void TypeBSplineCurvePlugin::slotRenderSelection(QAction* _action) {

  QVariant contextObject = _action->data();
  int objectId = contextObject.toInt();

  if ( objectId == -1)
    return;

  BaseObjectData* object;
  if ( !PluginFunctions::getObject(objectId,object) )
    return;

  BSplineCurveObject* bsplineCurveObject = dynamic_cast<BSplineCurveObject*>(object);

  if(bsplineCurveObject != 0){
    if(_action == renderCPSelectionAction_) {
      bsplineCurveObject->splineCurveNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::CONTROLPOINT);
      emit updatedObject( objectId, UPDATE_ALL );
    } else if(_action == renderKnotSelectionAction_) {
      bsplineCurveObject->splineCurveNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::KNOTVECTOR);
      emit updatedObject( objectId, UPDATE_ALL );
    } else if(_action == renderNoSelectionAction_) {
      bsplineCurveObject->splineCurveNode()->set_selection_draw_mode(ACG::SceneGraph::BSplineCurveNodeT<BSplineCurve>::NONE);
      emit updatedObject( objectId, UPDATE_ALL );
    }
  }
}

//-----------------------------------------------------------------------------

bool
TypeBSplineCurvePlugin::
registerType() 
{
  addDataType("BSplineCurve",tr("B-Spline Curve"));
  setTypeIcon("BSplineCurve", "BSplineCurveType.png");
  return true;
}

//-----------------------------------------------------------------------------

DataType
TypeBSplineCurvePlugin::
supportedType()
{
  DataType type = DATA_BSPLINE_CURVE;
  return type;
}

//-----------------------------------------------------------------------------

int
TypeBSplineCurvePlugin::
addEmpty()
{
  // new object data struct
  BSplineCurveObject * object = new BSplineCurveObject();

  if ( PluginFunctions::objectCount() == 1 )
    object->target(true);

  if (PluginFunctions::targetCount() == 0 )
    object->target(true);

  QString name = "BSplineCurve_" + QString::number( object->id() ) + ".bsc";

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  object->update();

  object->show();

  emit log(LOGINFO,object->getObjectinfo());

  emit emptyObjectAdded (object->id() );

  return object->id();
}

//-----------------------------------------------------------------------------

void TypeBSplineCurvePlugin::generateBackup( int _id, QString _name, UpdateType _type ){
  
  BaseObjectData* object = 0;
  PluginFunctions::getObject(_id, object);
  
  BSplineCurveObject* splineObj = PluginFunctions::bsplineCurveObject(object);
  
  if ( splineObj != 0 ){

    //get backup object data
    BackupData* backupData = 0;

    if ( object->hasObjectData( OBJECT_BACKUPS ) )
      backupData = dynamic_cast< BackupData* >(object->objectData(OBJECT_BACKUPS));
    else{
      //add backup data
      backupData = new BackupData(object);
      object->setObjectData(OBJECT_BACKUPS, backupData);
    }
    
    //store a new backup
    BSplineCurveBackup* backup = new BSplineCurveBackup(splineObj, _name, _type);
    backupData->storeBackup( backup );
  }
}

//-----------------------------------------------------------------------------

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( typebsplinecurveplugin , TypeBSplineCurvePlugin );
#endif


