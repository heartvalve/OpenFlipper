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
 *   $Revision: 6730 $                                                       *
 *   $Author: wilden $                                                       *
 *   $Date: 2010-06-11 08:11:06 +0200 (Wed, 05 Aug 2009) $                   *
 *                                                                           *
\*===========================================================================*/

#ifdef ENABLE_SKELETON_SUPPORT

#include <Plugin-Selection/SelectionPlugin.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


#include <ObjectTypes/Skeleton/Skeleton.hh>

//-----------------------------------------------------------------------------

void SelectionPlugin::toggleSkeletonSelection(QMouseEvent* _event)
{
  if ( _event->type() != QEvent::MouseButtonPress )
    return;

  unsigned int node_idx, target_idx;
  ACG::Vec3d   hit_point;

  // toggle vertex selection
  if (selectionType_ & VERTEX){

    if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_VERTEX, _event->pos(),node_idx, target_idx, &hit_point)) {
      BaseObjectData* object;

      if ( PluginFunctions::getPickedObject(node_idx, object) )
      {
        Skeleton* skeleton = PluginFunctions::skeleton( object );

        if ( !skeleton )
          return;

        Skeleton::Joint* joint = skeleton->getJoint( target_idx );
        
        if ( joint != 0 )
          joint->setSelected( !joint->selected() );
      }
    }
  }

  emit updateView();
}


//-----------------------------------------------------------------------------

void SelectionPlugin::deleteSelection(Skeleton* _skeleton)
{
  
  if (_skeleton == 0)
    return;
  
  for (Skeleton::JointIter it = _skeleton->begin(); it != _skeleton->end(); ++it){
  
    if ( (*it)->selected() )
      _skeleton->removeJoint( *it );
  }
  
}


#endif

