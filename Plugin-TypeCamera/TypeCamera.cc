/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision$                                                       *
 *   $Author$                                                       *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/


#include "TypeCamera.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include <OpenFlipper/common/GlobalOptions.hh>

TypeCameraPlugin::TypeCameraPlugin() :
        contextMenu_(0)
{

}

void TypeCameraPlugin::pluginsInitialized() {

  if ( OpenFlipper::Options::gui() ){
    contextMenu_ = new QMenu(tr("Rendering"));

    QAction* lastAction;

    lastAction = contextMenu_->addAction( tr("Show viewing frustum") );
    lastAction->setCheckable(true);
    lastAction->setChecked(false);
    lastAction->setToolTip(tr("Visualize cameras viewing frustum."));
    lastAction->setStatusTip( lastAction->toolTip() );

    // Add context menu
    emit addContextMenuItem(contextMenu_->menuAction(), DATA_CAMERA, CONTEXTOBJECTMENU);

    connect(contextMenu_, SIGNAL(triggered(QAction*)), this ,SLOT(contextMenuClicked(QAction*)));
  }
}

void TypeCameraPlugin::slotUpdateContextMenuObject(int _objectId) {

}

void TypeCameraPlugin::contextMenuClicked(QAction* _contextAction) {

  QVariant contextObject = _contextAction->data();
  int objectId = contextObject.toInt();

  if (objectId == -1)
    return;

  CameraObject* object;
  if (!PluginFunctions::getObject(objectId, object))
    return;

  if (_contextAction->text() == tr("Show viewing frustum")) {

    // Set frustum flag to whether action is checked or not
    object->cameraNode()->showFrustum(_contextAction->isChecked());

  }

}

bool TypeCameraPlugin::registerType() {
    
    addDataType ( "Camera",tr ( "Camera" ) );
    setTypeIcon ( "Camera", "camera.png" );
    return true;
}

int TypeCameraPlugin::addEmpty() {

    // new object data struct
    CameraObject* object = new CameraObject ();

    if ( PluginFunctions::objectCount() == 1 ||
         PluginFunctions::targetCount() == 0) {
        object->target ( true );
    }
    
    QString name = get_unique_name ( object );

    // call the local function to update names
    object->setName ( name );

    object->update();

    object->show();

    emit log ( LOGINFO, object->getObjectinfo() );

    emit emptyObjectAdded ( object->id() );

    return object->id();
}

QString TypeCameraPlugin::get_unique_name ( CameraObject* _object ) {
    
    bool name_unique = false;

    int cur_idx = _object->id();

    while ( !name_unique ) {
        
        name_unique = true;

        QString cur_name = QString ( tr ( "Camera %1.off" ).arg ( cur_idx ) );

        PluginFunctions::ObjectIterator o_it ( PluginFunctions::ALL_OBJECTS, DATA_CAMERA );
        for ( ; o_it != PluginFunctions::objectsEnd(); ++o_it ) {
            
            if ( o_it->name() == cur_name ) {
                
                name_unique = false;
                cur_idx += 10;
                break;
            }
        }
    }

    return QString ( tr ( "Camera %1.off" ).arg ( cur_idx ) );
}

Q_EXPORT_PLUGIN2 ( typecameraplugin , TypeCameraPlugin );

