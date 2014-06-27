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

//=============================================================================
//
//  CLASS InfoMeshObjectPlugin - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


#include "SkeletonObjectInfoPlugin.hh"

#include <iostream>
#include <ACG/GL/GLState.hh>
#include <QStringList>
#include <QLocale>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <MeshTools/MeshInfoT.hh>
#include <OpenFlipper/common/BaseObject.hh>
#include <ACG/Geometry/Algorithms.hh>

#include <Math_Tools/Math_Tools.hh>

#include <cfloat>

#if QT_VERSION >= 0x050000 
#include <QtWidgets>
#else
#include <QtGui>
#endif

//== IMPLEMENTATION ==========================================================

void InfoSkeletonObjectPlugin::initializePlugin() {

  if ( OpenFlipper::Options::gui()) {
    // Create info dialog
    info_ = new InfoDialog();
  }

}

/// initialize the plugin
void InfoSkeletonObjectPlugin::pluginsInitialized() {
	//set the slot descriptions
	setDescriptions();

}

//-----------------------------------------------------------------------------

DataType InfoSkeletonObjectPlugin::supportedDataTypes() {
    return DataType(DATA_SKELETON);
}

//-----------------------------------------------------------------------------

void InfoSkeletonObjectPlugin::printSkeletonInfo( Skeleton* _skeleton,  unsigned int _objectId, unsigned int _index, ACG::Vec3d& _hitPoint ) {

  QLocale locale;
  QString name;

  // name
  BaseObject* obj = 0;
  if ( PluginFunctions::getObject(_objectId, obj) )
    info_->generalBox->setTitle( tr("General object information for %1").arg( obj->name() ) );



  // ID
  info_->id->setText( locale.toString(_objectId) );
  // Joints
  info_->joints->setText( locale.toString( _skeleton->jointCount() ) );


  // animation list with animation names and the frame count
  info_->comboBoxAnimations->clear();
  QString animationInfo;
  unsigned int aniCount = _skeleton->animationCount();

  for (unsigned int i = 0; i < aniCount; ++i) {
    std::string aniName = _skeleton->animationName(i);
    animationInfo = "Name: " + QString(aniName.c_str())
                     + " : Frames: " + locale.toString(_skeleton->animation(aniName)->frameCount());
    info_->comboBoxAnimations->addItem(animationInfo);
  }


  // Clicked:
  info_->jointHandle->setText( locale.toString( _index  ) );

  QString adjacentHandles;

  // Check if we have a parent joint
  if ( _skeleton->joint(_index)->parent() !=0 ) {
    adjacentHandles = adjacentHandles + "Parent: " + locale.toString( _skeleton->joint(_index)->parent()->id()  ) + " ;";
  }

  // Check for children
  if ( _skeleton->joint(_index)->size() != 0 ) {

    adjacentHandles = adjacentHandles + "Children:";

    for ( Skeleton::Joint::ChildIter it = _skeleton->joint(_index)->begin(); it != _skeleton->joint(_index)->end(); ++it) {
        Skeleton::Joint *joint = *it;

        adjacentHandles = adjacentHandles + " " + locale.toString(joint->id());
      }
  }



  info_->adjacentJointsHandles->setText(adjacentHandles);


  Skeleton::Point bbMin( FLT_MAX,  FLT_MAX,  FLT_MAX);
  Skeleton::Point bbMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  Skeleton::Point cog(0.0,0.0,0.0);

  Skeleton::Pose* pose = _skeleton->referencePose();
  for (Skeleton::Iterator it = _skeleton->begin(); it != _skeleton->end(); ++it) {
    Skeleton::Joint *joint = *it;

    Skeleton::Point p = pose->globalTranslation(joint->id());

    cog += p;

    bbMin.minimize(p);
    bbMax.maximize(p);

  }

  //Bounding Box Size
  Skeleton::Point diff = bbMax-bbMin;

  info_->bbMinX->setText( QString::number(bbMin[0],'f') );
  info_->bbMinY->setText( QString::number(bbMin[1],'f') );
  info_->bbMinZ->setText( QString::number(bbMin[2],'f') );

  info_->bbMaxX->setText( QString::number(bbMax[0],'f') );
  info_->bbMaxY->setText( QString::number(bbMax[1],'f') );
  info_->bbMaxZ->setText( QString::number(bbMax[2],'f') );

  info_->bbSizeX->setText( QString::number(diff[0],'f') );
  info_->bbSizeY->setText( QString::number(diff[1],'f') );
  info_->bbSizeZ->setText( QString::number(diff[2],'f') );


  //COG
  cog = cog / _skeleton->jointCount() ;

  info_->cogX->setText( QString::number(cog[0],'f') );
  info_->cogY->setText( QString::number(cog[1],'f') );
  info_->cogZ->setText( QString::number(cog[2],'f') );

  //hitpoint
  info_->pointX->setText( QString::number( _hitPoint[0],'f' ) );
  info_->pointY->setText( QString::number( _hitPoint[1],'f' ) );
  info_->pointZ->setText( QString::number( _hitPoint[2],'f' ) );

  info_->setWindowFlags(info_->windowFlags() | Qt::WindowStaysOnTopHint);

  info_->show();
}

//----------------------------------------------------------------------------------------------

void InfoSkeletonObjectPlugin::slotInformationRequested(const QPoint _clickedPoint, DataType _type) {

    // Only respond on skeleton objects
    if( _type != DATA_SKELETON ) return;

    ACG::SceneGraph::PickTarget target = ACG::SceneGraph::PICK_ANYTHING;

    unsigned int   node_idx, target_idx;
    ACG::Vec3d     hit_point;

    if (PluginFunctions::scenegraphPick(target, _clickedPoint, node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) ) {

         emit log( LOGINFO , object->getObjectinfo() );

         if ( object->picked(node_idx) && object->dataType(DATA_SKELETON) )
            printSkeletonInfo( PluginFunctions::skeleton(object) , object->id(), target_idx, hit_point );

      } else return;
    }
}

#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( infoskeletonobjectplugin , InfoSkeletonObjectPlugin );
#endif

