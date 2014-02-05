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
 *   $Revision: 17990 $                                                       *
 *   $LastChangedBy: moeller $                                                *
 *   $Date: 2014-01-07 17:36:34 +0100 (Di, 07. Jan 2014) $                     *
 *                                                                            *
 \*===========================================================================*/

#include <QtGui>

#include <QFileInfo>

#include "ComponentsPlugin.hh"

#include <iostream>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <MeshTools/MeshInfoT.hh>

const char *SPLIT_COMPONENTS = "SplitComponents";
const char *BIGGEST_COMPONENT = "ComponentsPluginBiggestComponent";

//------------------------------------------------------------------------------

/** \brief Constructor
 *
 */
ComponentsPlugin::ComponentsPlugin() {}

/*******************************************************************************
        BaseInterface implementation
 *******************************************************************************/

/** \brief Initialize the plugin
 *
 */
void ComponentsPlugin::initializePlugin() {
}


//------------------------------------------------------------------------------

/** \brief Second initialization phase
 *
 */
void ComponentsPlugin::pluginsInitialized()
{
  setDescriptions();

  emit addPickMode( SPLIT_COMPONENTS );
  emit addPickMode( BIGGEST_COMPONENT );
  
  // Add a scissor Toolbar
  QToolBar* toolbar = new QToolBar("Components Toolbar");
  //Split components
  splitAction_ = new QAction(tr("&Split into components"), this);
  splitAction_->setCheckable( true );
  splitAction_->setStatusTip(tr("Clicked objects are splitted into components"));
  splitAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"components_split_components.png") );
  connect(splitAction_, SIGNAL(triggered()), this, SLOT(slotSplitComponentsButton()) );
  toolbar->addAction(splitAction_);
  
  biggestAction_ = new QAction(tr("&Get biggest component"), this);
  biggestAction_->setCheckable( true );
  biggestAction_->setStatusTip(tr("Get the biggest component of the clicked object and delete the other components."));
  biggestAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"components_biggest_component.png") );
  connect(biggestAction_, SIGNAL(triggered()), this, SLOT(slotBiggestComponentButton()) );
  toolbar->addAction(biggestAction_);

  emit addToolbar( toolbar );
}

/*******************************************************************************
    PickingInterface implementation
 *******************************************************************************/

/** \brief the pickMode changed
 *
 * @param _mode the new pickMode
 */
void ComponentsPlugin::slotPickModeChanged( const std::string& _mode){
  splitAction_->setChecked( _mode == SPLIT_COMPONENTS );
  biggestAction_->setChecked( _mode == BIGGEST_COMPONENT );
}

/*******************************************************************************
    MouseInterface implementation
 *******************************************************************************/

/** \brief a mouse event occured
 *
 * @param _event the event that occured
 */
void ComponentsPlugin::slotMouseEvent( QMouseEvent* _event )
{
  if( PluginFunctions::pickMode() == SPLIT_COMPONENTS )
    splitComponents( _event );
  if (PluginFunctions::pickMode() == BIGGEST_COMPONENT)
    biggestComponent( _event );
}

/*******************************************************************************
         ComponentsPlugin Implementation
 *******************************************************************************/

/** \brief Split into Components Button was hit
 *
 */
void ComponentsPlugin::slotSplitComponentsButton( )
{
  PluginFunctions::actionMode( Viewer::PickingMode );
  PluginFunctions::pickMode( SPLIT_COMPONENTS );
}


//------------------------------------------------------------------------------

/** \brief Split components (triggered by pickMode)
 *
 * @param _event the mouse event that occured
 */
void ComponentsPlugin::splitComponents(QMouseEvent * _event)
{
  if (_event->type() == QEvent::MouseButtonPress )
  {
    unsigned int     node_idx, target_idx;
    ACG::Vec3d*      sourcePoint3D = 0;

    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE,
                                                    _event->pos(),
                                                    node_idx,
                                                    target_idx,
                                                    sourcePoint3D))
    {
      BaseObjectData *obj;
      PluginFunctions::getPickedObject(node_idx, obj);
      splitComponents( obj->id() );
    }
  }
}


void ComponentsPlugin::slotBiggestComponentButton()
{
  PluginFunctions::actionMode( Viewer::PickingMode );
  PluginFunctions::pickMode( BIGGEST_COMPONENT );
}

void ComponentsPlugin::biggestComponent(QMouseEvent* _event)
{
  if (_event->type() == QEvent::MouseButtonPress )
  {
    unsigned int     node_idx, target_idx;
    ACG::Vec3d*      sourcePoint3D = 0;

    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_FACE,
        _event->pos(),
        node_idx,
        target_idx,
        sourcePoint3D))
    {
      BaseObjectData *obj;
      PluginFunctions::getPickedObject(node_idx, obj);

      if (!obj)
      {
        emit log(LOGERR,tr("Unable to pick Object."));
        return;
      }

      biggestComponent(obj->id());

      emit createBackup(obj->id(),"GetBiggestComponents",UPDATE_ALL);
      emit updatedObject( obj->id() , UPDATE_ALL );
    }
  }
}

Q_EXPORT_PLUGIN2( componentsplugin, ComponentsPlugin );

