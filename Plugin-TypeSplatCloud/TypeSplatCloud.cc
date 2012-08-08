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

//================================================================
//
//  CLASS TypeSplatCloudPlugin - IMPLEMENTATION
//
//================================================================


//== INCLUDES ====================================================


#include "TypeSplatCloud.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <OpenFlipper/common/BackupData.hh>
#include "SplatCloudBackup.hh"

#include <ACG/GL/GLState.hh>

#include <ObjectTypes/Camera/Camera.hh>


//== IMPLEMENTATION ==============================================


void TypeSplatCloudPlugin::slotViewChanged()
{
  // get drawmodes
  ACG::SceneGraph::DrawModes::DrawMode splatsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode( "Splats" );
  ACG::SceneGraph::DrawModes::DrawMode dotsDrawMode   = ACG::SceneGraph::DrawModes::getDrawMode( "Dots"   );

  // if drawmodes don't exist something went wrong
  if( splatsDrawMode == ACG::SceneGraph::DrawModes::NONE || 
      dotsDrawMode   == ACG::SceneGraph::DrawModes::NONE )
  {
    emit log(LOGERR,tr("Shader DrawModes for SplatCloud not existent!"));
    return;
  }

  // get current glState
  const ACG::GLState &glstate = PluginFunctions::viewerProperties().glState();

  // get viewport
  int left, bottom, width, height;
  glstate.get_viewport( left, bottom, width, height );

  float x = (float) left;
  float y = (float) bottom;
  float w = (float) width;
  float h = (float) height;

  // get depthrange
  // TODO: use glstate.get_depth_range when implemented
  GLfloat depthRange[2];
  glGetFloatv( GL_DEPTH_RANGE, depthRange );
  float z = (float) depthRange[0];
  float d = (float) depthRange[1] - z;

  // check if we are safe
  if( w<=0.0f || h<=0.0f || d<=0.0f )
    return;

  // calculate window-coordinates to normalized-device-coordinates scale
  ACG::Vec4f invVPs;
  invVPs[0] = 2.0f / w;
  invVPs[1] = 2.0f / h;
  invVPs[2] = 2.0f / d;
  invVPs[3] = 0.0f;

  // calculate window-coordinates to normalized-device-coordinates transpose
  ACG::Vec4f invVPt;
  invVPt[0] = - ( x * invVPs[0] + 1.0f );
  invVPt[1] = - ( y * invVPs[1] + 1.0f );
  invVPt[2] = - ( z * invVPs[2] + 1.0f );
  invVPt[3] = 1.0f;

  // calculate normalized-device-coordinates to window-coordinates scale and transpose
  GLfloat VPs_z = 0.5f * d;
  GLfloat VPt_z = z + VPs_z;

  // calculate scaling factor of modelview matrix
  static const double RCP_3 = 1.0 / 3.0;
  const ACG::GLMatrixd &mv = glstate.modelview();
  double detMV = mv(0,0) * (mv(1,1)*mv(2,2) - mv(1,2)*mv(2,1)) 
               + mv(0,1) * (mv(1,2)*mv(2,0) - mv(1,0)*mv(2,2)) 
               + mv(0,2) * (mv(1,0)*mv(2,1) - mv(1,1)*mv(2,0));
  GLfloat MVs = (GLfloat) pow( fabs( detMV ), RCP_3 );

  // calculate scale for pointsizes in eye-coordinates according to fovy and transformation to window-coordinates
  GLfloat VPsFov_y = glstate.projection()(1,1) * (0.5f * h);

  // for all splatcloud-objects...
  PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
  for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
  {
    // get scenegraph shader-node
    ShaderNode *shaderNode = PluginFunctions::splatShaderNode( *objIter );

    // get standard shaders and picking shaders
    GLSL::PtrProgram splatsShader     = shaderNode->getShader( splatsDrawMode, false );
    GLSL::PtrProgram splatsPickShader = shaderNode->getShader( splatsDrawMode, true  );
    GLSL::PtrProgram dotsShader       = shaderNode->getShader( dotsDrawMode,   false );
    GLSL::PtrProgram dotsPickShader   = shaderNode->getShader( dotsDrawMode,   true  );

    // update shader uniforms concerning viewport and depthrange

    if( splatsShader )
    {
      splatsShader->use();
      splatsShader->setUniform( "invViewportScale",   invVPs   );
      splatsShader->setUniform( "invViewportTransp",  invVPt   );
      splatsShader->setUniform( "viewportScale_z",    VPs_z    );
      splatsShader->setUniform( "viewportTransp_z",   VPt_z    );
      splatsShader->setUniform( "modelviewScale",     MVs      );
      splatsShader->setUniform( "viewportScaleFov_y", VPsFov_y );
      splatsShader->disable();
    }

    if( splatsPickShader )
    {
      splatsPickShader->use();
      splatsPickShader->setUniform( "invViewportScale",   invVPs   );
      splatsPickShader->setUniform( "invViewportTransp",  invVPt   );
      splatsPickShader->setUniform( "viewportScale_z",    VPs_z    );
      splatsPickShader->setUniform( "viewportTransp_z",   VPt_z    );
      splatsPickShader->setUniform( "modelviewScale",     MVs      );
      splatsPickShader->setUniform( "viewportScaleFov_y", VPsFov_y );
      splatsPickShader->disable();
    }

    if( dotsShader )
    {
      dotsShader->use();
      dotsShader->setUniform( "modelviewScale",     MVs      );
      dotsShader->setUniform( "viewportScaleFov_y", VPsFov_y );
      dotsShader->disable();
    }

    if( dotsPickShader )
    {
      dotsPickShader->use();
      dotsPickShader->setUniform( "modelviewScale",     MVs      );
      dotsPickShader->setUniform( "viewportScaleFov_y", VPsFov_y );
      dotsPickShader->disable();
    }
  }
}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::slotObjectPropertiesChanged( int _objectId )
{
  // get splatcloud-object by id
  SplatCloudObject *splatCloudObject = 0;
  if( PluginFunctions::getObject( _objectId, splatCloudObject ) )
  {
    // get parent and check if parent is a group
    BaseObject *parent = splatCloudObject->parent();
    if( (parent != 0) && parent->isGroup() )
    {
      // update name of group
      parent->setName( splatCloudObject->name() );
    }
  }
}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::objectDeleted( int _objectId )
{
  // get object by id
  BaseObject *object = 0;
  if( !PluginFunctions::getObject( _objectId, object ) )
    return;

  // check if object is a splatcloud-object
  /*const*/ SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( static_cast<BaseObjectData *>( object ) );
  if( splatCloudObject != 0 )
  {
    // get splatcloud
    /*const*/ SplatCloud *splatCloud = PluginFunctions::splatCloud( splatCloudObject );
    if( splatCloud != 0 )
    {
      // remove all camera-objects
      /*const*/ SplatCloud_CamerasProperty *camerasProp = splatCloud->getCloudProperty( SPLATCLOUD_CAMERAS_HANDLE );
      if( camerasProp != 0 )
      {
        // get cameras
        /*const*/ SplatCloud_Cameras &cameras = camerasProp->data();

        // iterate over all cameras
        SplatCloud_Cameras::/*const_*/iterator cameraIter;
        for( cameraIter = cameras.begin(); cameraIter != cameras.end(); ++cameraIter )
        {
          // get camera-object by id
          CameraObject *cameraObject = 0;
          if( PluginFunctions::getObject( cameraIter->objectId_, cameraObject ) )
          {
            // remove splatcloud-reference from current camera-object
            cameraObject->clearObjectData( "SplatCloudObjectId" );
          }
        }
      }
    }
  }

  // check if object is a camera-object
  CameraObject *cameraObject = PluginFunctions::cameraObject( static_cast<BaseObjectData *>( object ) );
  if( cameraObject != 0 )
  {
    // get per-object-data
    IntPerObjectData *pod = dynamic_cast<IntPerObjectData *>( cameraObject->objectData( "SplatCloudObjectId" ) );
    if( pod != 0 )
    {
      // get splatcloud-object id
      int splatCloudObjectId = pod->data();

      // get splatcloud-object by id
      SplatCloudObject *splatCloudObject = 0;
      if( PluginFunctions::getObject( splatCloudObjectId, splatCloudObject ) )
      {
        // get splatcloud
        SplatCloud *splatCloud = PluginFunctions::splatCloud( splatCloudObject );
        if( splatCloud != 0 )
        {
          // delete camera from cameras vector
          SplatCloud_CamerasProperty *camerasProp = splatCloud->getCloudProperty( SPLATCLOUD_CAMERAS_HANDLE );
          if( camerasProp != 0 )
          {
            // get cameras
            SplatCloud_Cameras &cameras = camerasProp->data();

            // iterate over all cameras
            SplatCloud_Cameras::iterator cameraIter;
            for( cameraIter = cameras.begin(); cameraIter != cameras.end(); ++cameraIter )
            {
              // check if camera should be deleted
              if( cameraIter->objectId_ == _objectId )
              {
                // delete current camera
                cameras.erase( cameraIter );
                break; // there is only one camera with this object id, so break
              }
            }
          }

          // delete camera from viewlists
          if( splatCloud->hasViewlists() )
          {
            // iterate over all splats
            unsigned int splatIdx, numSplats = splatCloud->numSplats();
            for( splatIdx = 0; splatIdx < numSplats; ++splatIdx )
            {
              // get viewlist
              SplatCloud::Viewlist &viewlist = splatCloud->viewlists( splatIdx );

              // iterate over all views
              SplatCloud::Viewlist::iterator viewIter;
              for( viewIter = viewlist.begin(); viewIter != viewlist.end(); ++viewIter )
              {
                // check if view should be deleted
                if( viewIter->cameraObjectId_ == _objectId )
                {
                  // delete current view
                  viewlist.erase( viewIter );
                  break; // there is only one view with this object id, so break
                }
              }
            }
          }
        }
      }
    }
  }
}


//----------------------------------------------------------------


bool TypeSplatCloudPlugin::registerType()
{
  addDataType( "SplatCloud", tr( "SplatCloud" ) );
  setTypeIcon( "SplatCloud", "SplatCloudType.png" );
  return true;
}


//----------------------------------------------------------------


int TypeSplatCloudPlugin::addEmpty()
{
  // new object data struct
  SplatCloudObject *object = new SplatCloudObject();

  if( object == 0 )
    return -1;

  if( PluginFunctions::objectCount() == 1 )
    object->target( true );

  if( PluginFunctions::targetCount() == 0 )
    object->target( true );

  QString name = QString(tr("New Splat Cloud %1.spl").arg( object->id() ));

  // call the local function to update names
  QFileInfo f( name );
  object->setName( f.fileName() );

  object->update();

  object->show();

  emit emptyObjectAdded( object->id() );

  return object->id();
}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::generateBackup( int _objectId, QString _name, UpdateType _type )
{
  if( _objectId == -1 )
    return;

  BaseObjectData *object = 0;
  PluginFunctions::getObject( _objectId, object );

  SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( object );

  if( splatCloudObject )
  {
    // get backup object data
    BackupData *backupData = 0;

    if( object->hasObjectData( OBJECT_BACKUPS ) )
    {
      backupData = dynamic_cast<BackupData *>( object->objectData( OBJECT_BACKUPS ) );
    }
    else
    {
      // add backup data
      backupData = new BackupData( object );
      object->setObjectData( OBJECT_BACKUPS, backupData );
    }

    // store a new backup
    SplatCloudBackup *backup = new SplatCloudBackup( splatCloudObject, _name, _type );

    backupData->storeBackup( backup );
  }
}


//================================================================


Q_EXPORT_PLUGIN2( typesplatcloudplugin, TypeSplatCloudPlugin );
