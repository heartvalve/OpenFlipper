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


//== CONSTANTS ===================================================


static const char SPLATCLOUD_OBJECT_ID_DATANAME[] = "SplatCloudObjectId";

static const double MIN_CLUSTER_BRIGHTNESS = 0.25;


//== IMPLEMENTATION ==============================================

TypeSplatCloudPlugin::TypeSplatCloudPlugin() {
}

void TypeSplatCloudPlugin::slotViewChanged()
{
  // if drawmodes don't exist we have no object of this type
  if( splatsDrawMode_ == ACG::SceneGraph::DrawModes::NONE ||
      dotsDrawMode_   == ACG::SceneGraph::DrawModes::NONE )
  {
    //emit log(LOGERR,tr("Shader DrawModes for SplatCloud not existent!"));
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
    GLSL::PtrProgram splatsShader     = shaderNode->getShader( splatsDrawMode_, false );
    GLSL::PtrProgram splatsPickShader = shaderNode->getShader( splatsDrawMode_, true  );
    GLSL::PtrProgram dotsShader       = shaderNode->getShader( dotsDrawMode_,   false );
    GLSL::PtrProgram dotsPickShader   = shaderNode->getShader( dotsDrawMode_,   true  );

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


void TypeSplatCloudPlugin::slotObjectUpdated( int _objectId, const UpdateType &_updateType )
{
  // get object by id
  BaseObject *object = 0;
  if( !PluginFunctions::getObject( _objectId, object ) )
    return;

  // check if object is a splatcloud-object
  SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( static_cast<BaseObjectData *>( object ) ); // TODO: remove cast
  if( splatCloudObject != 0 )
  {
    // check update type
    if( _updateType.contains( UPDATE_STATE ) )
    {
      // add objects
      addCameraObjects ( splatCloudObject );
      addClusterObjects( splatCloudObject );
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
  SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( static_cast<BaseObjectData *>( object ) ); // TODO: remove cast
  if( splatCloudObject != 0 )
  {
    // free objects
    freeCameraObjects ( splatCloudObject );
    freeClusterObjects( splatCloudObject );
  }

  // check if object is a camera-object
  CameraObject *cameraObject = PluginFunctions::cameraObject( static_cast<BaseObjectData *>( object ) ); // TODO: remove cast
  if( cameraObject != 0 )
  {
    // erase camera
    eraseCamera( cameraObject );
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
  // Add or get the current draw modes to make sure they exist now
  splatsDrawMode_ = ACG::SceneGraph::DrawModes::addDrawMode("Splats");
  dotsDrawMode_   = ACG::SceneGraph::DrawModes::addDrawMode("Dots");

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

  // set the default colors
  const QColor color = OpenFlipper::Options::defaultColor();
  const ACG::Vec4f default_color(color.redF(), color.greenF(), color.blueF(), color.alphaF());
  object->materialNode()->set_color(default_color);

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


//----------------------------------------------------------------


void TypeSplatCloudPlugin::ungroupGroupObject( GroupObject *_groupObject )
{
  // get new parent
  BaseObject *parent = _groupObject->parent();

  // iterate over all children
  int i;
  for( i = _groupObject->childCount() - 1; i >= 0; --i )
  {
    // get current child
    BaseObject *child = _groupObject->child( i );

    // change child's parent
    child->setParent( parent );
  }

  // delete group-object
  emit deleteObject( _groupObject->id() );
}


//----------------------------------------------------------------


GroupObject *TypeSplatCloudPlugin::getCamerasGroupObject( const SplatCloud_CameraManager &_cameraManager )
{
  // iterate over all cameras
  SplatCloud_Cameras::const_iterator cameraIter;
  for( cameraIter = _cameraManager.cameras_.begin(); cameraIter != _cameraManager.cameras_.end(); ++cameraIter )
  {
    const SplatCloud_Camera &camera = *cameraIter;

    // get current camera-object by id
    CameraObject *cameraObject = 0;
    if( PluginFunctions::getObject( camera.objectId_, cameraObject ) )
    {
      // get parent
      BaseObject *parent = cameraObject->parent();
      if( (parent != 0) && parent->isGroup() && (parent->parent() != 0) ) // parent is valid, group and *not* root-node
      {
        // cast parent to group-object
        GroupObject *groupObject = dynamic_cast<GroupObject *>( parent );
        if( groupObject != 0 )
          return groupObject;
      }
    }
  }

  // return failure
  return 0;
}


//----------------------------------------------------------------


GroupObject *TypeSplatCloudPlugin::getClustersGroupObject( const SplatCloud_ClusterManager &_clusterManager )
{
  // there is no clusters group-object yet, so return 0
  return 0;
}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::ungroupCameraObjects( const SplatCloud_CameraManager &_cameraManager )
{
  // get cameras group-object
  GroupObject *groupObject = getCamerasGroupObject( _cameraManager );
  if( groupObject != 0 )
  {
    // ungroup group-object
    ungroupGroupObject( groupObject );
  }
}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::ungroupClusterObjects( const SplatCloud_ClusterManager &_clusterManager )
{
  // get clusters group-object
  GroupObject *groupObject = getClustersGroupObject( _clusterManager );
  if( groupObject != 0 )
  {
    // ungroup group-object
    ungroupGroupObject( groupObject );
  }
}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::groupCameraObjects( const SplatCloud_CameraManager &_cameraManager )
{
  // ungroup cameras first
  ungroupCameraObjects( _cameraManager );

  // group cameras
  {
    // create new, emtpy list of object IDs
    IdList objectIDs;

    // reserve enough memory
    objectIDs.reserve( _cameraManager.cameras_.size() );

    // iterate over all cameras
    SplatCloud_Cameras::const_iterator cameraIter;
    for( cameraIter = _cameraManager.cameras_.begin(); cameraIter != _cameraManager.cameras_.end(); ++cameraIter )
    {
      const SplatCloud_Camera &camera = *cameraIter;

      // check if object id is valid
      if( camera.objectId_ != -1 )
      {
        // add object id to list
        objectIDs.push_back( camera.objectId_ );
      }
    }

    // group objects in list
    RPC::callFunction( "datacontrol", "groupObjects", objectIDs, QString( "Cameras" ) );
  }
}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::groupClusterObjects( const SplatCloud_ClusterManager &_clusterManager )
{
  // ungroup clusters first
  ungroupClusterObjects( _clusterManager );
}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::addCameraObjects( SplatCloudObject *_splatCloudObject )
{
  // get splatcloud
  SplatCloud *splatCloud = PluginFunctions::splatCloud( _splatCloudObject );
  if( splatCloud == 0 )
    return;

  // get camera-manager property
  SplatCloud_CameraManagerProperty *cameraManagerProp = splatCloud->getCloudProperty( SPLATCLOUD_CAMERAMANAGER_HANDLE );
  if( cameraManagerProp == 0 )
    return;

  // get camera-manager
  SplatCloud_CameraManager &cameraManager = cameraManagerProp->data();

  // get cameras
  SplatCloud_Cameras &cameras = cameraManager.cameras_;
  if( cameras.empty() )
    return;

  // get splatcloud-object id
  int splatCloudObjectId = _splatCloudObject->id();

  // add object(s)
  {
    // disable update of scenegraph
    OpenFlipper::Options::blockSceneGraphUpdates();

    // iterate over all cameras
    SplatCloud_Cameras::iterator cameraIter;
    for( cameraIter = cameras.begin(); cameraIter != cameras.end(); ++cameraIter )
    {
      SplatCloud_Camera &camera = *cameraIter;

      // create new, empty camera-object if not already present
      if( camera.objectId_ == -1 )
        emit addEmptyObject( DATA_CAMERA, camera.objectId_ );

      // check object id
      if( camera.objectId_ != -1 )
      {
        // get camera-object by id
        CameraObject *cameraObject = 0;
        if( PluginFunctions::getObject( camera.objectId_, cameraObject ) )
        {
          // set name of camera-object
          cameraObject->setName( camera.imagePath_.c_str() );

          // remember splatcloud-object id
          cameraObject->setObjectData( SPLATCLOUD_OBJECT_ID_DATANAME, new IntPerObjectData( splatCloudObjectId ) );

          // get camera
          CameraNode *cameraNode = cameraObject->cameraNode();
          if( cameraNode != 0 )
          {
            // set matrix
            ACG::GLMatrixd matrix;
            {
              const SplatCloud_Projection &proj = camera.projection_;
              matrix(0,0) = proj.r_[0][0]; matrix(0,1) = proj.r_[1][0]; matrix(0,2) = proj.r_[2][0];
              matrix(1,0) = proj.r_[0][1]; matrix(1,1) = proj.r_[1][1]; matrix(1,2) = proj.r_[2][1];
              matrix(2,0) = proj.r_[0][2]; matrix(2,1) = proj.r_[1][2]; matrix(2,2) = proj.r_[2][2];
              matrix(0,3) = -(proj.r_[0][0]*proj.t_[0] + proj.r_[1][0]*proj.t_[1] + proj.r_[2][0]*proj.t_[2]);
              matrix(1,3) = -(proj.r_[0][1]*proj.t_[0] + proj.r_[1][1]*proj.t_[1] + proj.r_[2][1]*proj.t_[2]);
              matrix(2,3) = -(proj.r_[0][2]*proj.t_[0] + proj.r_[1][2]*proj.t_[1] + proj.r_[2][2]*proj.t_[2]);
              matrix(3,0) = 0.0; matrix(3,1) = 0.0; matrix(3,2) = 0.0; matrix(3,3) = 1.0;
            }

            // set resolution
            unsigned int width  = camera.imageWidth_;
            unsigned int height = camera.imageHeight_;
            if( (width == 0) || (height == 0) )
            {
              width  = 1;
              height = 1;
            }

            // set camera parameters
            cameraNode->setModelView( matrix );
            cameraNode->setSize( width, height );

            // emit signal that the camera-object has to be updated
            emit updatedObject( camera.objectId_, UPDATE_ALL );

            // everything is okay, so continue with next camera
            continue;
          }

          // something went wrong
        }

        // something went wrong, so delete camera-object
        emit deleteObject( camera.objectId_ );
        camera.objectId_ = -1;
      }
    }

    // enable update of scenegraph
    OpenFlipper::Options::unblockSceneGraphUpdates();

    // group camera-objects
    groupCameraObjects( cameraManager );
  }

}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::addClusterObjects( SplatCloudObject *_splatCloudObject )
{
  // get splatcloud
  SplatCloud *splatCloud = PluginFunctions::splatCloud( _splatCloudObject );
  if( splatCloud == 0 )
    return;

  // get cluster-manager property
  SplatCloud_ClusterManagerProperty *clusterManagerProp = splatCloud->getCloudProperty( SPLATCLOUD_CLUSTERMANAGER_HANDLE );
  if( clusterManagerProp == 0 )
    return;

  // get cluster-manager
  SplatCloud_ClusterManager &clusterManager = clusterManagerProp->data();

  // get clusters
  SplatCloud_Clusters &clusters = clusterManager.clusters_;
  if( clusters.empty() )
    return;

  // get splatcloud-object id
  int splatCloudObjectId = _splatCloudObject->id();

  // add object(s)
  {
    // create new, empty (poly)mesh-object if not already present
    if( clusterManager.objectId_ == -1 )
      emit addEmptyObject( DATA_POLY_MESH, clusterManager.objectId_ );

    // check object id
    if( clusterManager.objectId_ != -1 )
    {
      // get mesh-object by id
      PolyMeshObject *meshObject = 0;
      if( PluginFunctions::getObject( clusterManager.objectId_, meshObject ) )
      {
        // set name of mesh-object
        meshObject->setName( "Clusters" );

        // remember splatcloud-object id
        meshObject->setObjectData( SPLATCLOUD_OBJECT_ID_DATANAME, new IntPerObjectData( splatCloudObjectId ) );

        // get mesh
        PolyMesh *mesh = meshObject->mesh();
        if( mesh != 0 )
        {
          // request colors
          mesh->request_vertex_colors();
          mesh->request_face_colors();

          // reset seed of random-number-generator
          srand( 0 );

          // add clusters to mesh
          SplatCloud_Clusters::const_iterator clusterIter;
          for( clusterIter = clusters.begin(); clusterIter != clusters.end(); ++clusterIter )
          {
            const SplatCloud_Cluster &cluster = *clusterIter;

            // choose random color
            PolyMesh::Color color;
            {
              static const double MIN_SQR_BRIGHTNESS = MIN_CLUSTER_BRIGHTNESS * MIN_CLUSTER_BRIGHTNESS;
              static const double RCP_RAND_MAX       = 1.0 / (double) RAND_MAX;
              static const double RCP_3              = 1.0 / 3.0;

              double r, g, b;
              double sqrBrightness;
              do
              {
                r = rand() * RCP_RAND_MAX;
                g = rand() * RCP_RAND_MAX;
                b = rand() * RCP_RAND_MAX;
                sqrBrightness = RCP_3 * (r*r + g*g + b*b);
              }
              while( sqrBrightness < MIN_SQR_BRIGHTNESS );

              color = PolyMesh::Color( r, g, b );
            }

            // get quad vertices
            const ACG::Vec3d &qv0 = cluster.quad_.vertices_[0];
            const ACG::Vec3d &qv1 = cluster.quad_.vertices_[1];
            const ACG::Vec3d &qv2 = cluster.quad_.vertices_[2];
            const ACG::Vec3d &qv3 = cluster.quad_.vertices_[3];

            // add vertices to mesh
            std::vector<PolyMesh::VertexHandle> vertexHandles( 4 );
            vertexHandles[0] = mesh->add_vertex( PolyMesh::Point( qv0[0], qv0[1], qv0[2] ) );
            vertexHandles[1] = mesh->add_vertex( PolyMesh::Point( qv1[0], qv1[1], qv1[2] ) );
            vertexHandles[2] = mesh->add_vertex( PolyMesh::Point( qv2[0], qv2[1], qv2[2] ) );
            vertexHandles[3] = mesh->add_vertex( PolyMesh::Point( qv3[0], qv3[1], qv3[2] ) );
            mesh->set_color( vertexHandles[0], color );
            mesh->set_color( vertexHandles[1], color );
            mesh->set_color( vertexHandles[2], color );
            mesh->set_color( vertexHandles[3], color );

            // add face to mesh
            PolyMesh::FaceHandle faceHandle = mesh->add_face( vertexHandles );
            mesh->set_color( faceHandle, color );
          }

          // request and calculate normals
          mesh->request_face_normals();
          mesh->request_vertex_normals();
          mesh->update_normals();

          // emit signal that the mesh-object has to be updated
          emit updatedObject( clusterManager.objectId_, UPDATE_ALL );

          // group cluster-object(s)
          groupClusterObjects( clusterManager );

          // everything is okay, so return
          return;
        }

        // something went wrong
      }

      // something went wrong, so delete mesh-object
      emit deleteObject( clusterManager.objectId_ );
      clusterManager.objectId_ = -1;
    }
  }

}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::freeCameraObjects( SplatCloudObject *_splatCloudObject )
{
  // get splatcloud
  SplatCloud *splatCloud = PluginFunctions::splatCloud( _splatCloudObject );
  if( splatCloud == 0 )
    return;

  // get camera-manager property
  SplatCloud_CameraManagerProperty *cameraManagerProp = splatCloud->getCloudProperty( SPLATCLOUD_CAMERAMANAGER_HANDLE );
  if( cameraManagerProp == 0 )
    return;

  // get camera-manager
  SplatCloud_CameraManager &cameraManager = cameraManagerProp->data();

  // get cameras
  SplatCloud_Cameras &cameras = cameraManager.cameras_;
  if( cameras.empty() )
    return;

  // remove splatcloud-reference from object(s)
  {
    // iterate over all cameras
    SplatCloud_Cameras::const_iterator cameraIter;
    for( cameraIter = cameras.begin(); cameraIter != cameras.end(); ++cameraIter )
    {
      // get camera-object by id
      CameraObject *cameraObject = 0;
      if( PluginFunctions::getObject( cameraIter->objectId_, cameraObject ) )
      {
        // delete per-object-data
        cameraObject->clearObjectData( SPLATCLOUD_OBJECT_ID_DATANAME );
      }
    }
  }

}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::freeClusterObjects( SplatCloudObject *_splatCloudObject )
{
  // get splatcloud
  SplatCloud *splatCloud = PluginFunctions::splatCloud( _splatCloudObject );
  if( splatCloud == 0 )
    return;

  // get cluster-manager property
  SplatCloud_ClusterManagerProperty *clusterManagerProp = splatCloud->getCloudProperty( SPLATCLOUD_CLUSTERMANAGER_HANDLE );
  if( clusterManagerProp == 0 )
    return;

  // get cluster-manager
  SplatCloud_ClusterManager &clusterManager = clusterManagerProp->data();

  // remove splatcloud-reference from object(s)
  {
    // get clusters mesh-object by id
    PolyMeshObject *meshObject = 0;
    if( PluginFunctions::getObject( clusterManager.objectId_, meshObject ) )
    {
      // delete per-object-data
      meshObject->clearObjectData( SPLATCLOUD_OBJECT_ID_DATANAME );
    }
  }

}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::eraseCamera( CameraObject *_cameraObject )
{
  // get per-object-data
  IntPerObjectData *splatCloudObjectIdPOD = dynamic_cast<IntPerObjectData *>( _cameraObject->objectData( SPLATCLOUD_OBJECT_ID_DATANAME ) );
  if( splatCloudObjectIdPOD == 0 )
    return;

  // get splatcloud-object by id
  SplatCloudObject *splatCloudObject = 0;
  if( !PluginFunctions::getObject( splatCloudObjectIdPOD->data(), splatCloudObject ) )
    return;

  // get splatcloud
  SplatCloud *splatCloud = PluginFunctions::splatCloud( splatCloudObject );
  if( splatCloud == 0 )
    return;

  // get camera-object id
  int cameraObjectId = _cameraObject->id();

  // delete camera from cameras
  {
    // get camera-manager property
    SplatCloud_CameraManagerProperty *cameraManagerProp = splatCloud->getCloudProperty( SPLATCLOUD_CAMERAMANAGER_HANDLE );
    if( cameraManagerProp != 0 )
    {
      // get camera-manager
      SplatCloud_CameraManager &cameraManager = cameraManagerProp->data();

      // get cameras
      SplatCloud_Cameras &cameras = cameraManager.cameras_;

      // iterate over all cameras
      SplatCloud_Cameras::iterator cameraIter;
      for( cameraIter = cameras.begin(); cameraIter != cameras.end(); ++cameraIter )
      {
        // check if camera should be deleted
        if( cameraIter->objectId_ == cameraObjectId )
        {
          // delete current camera
          cameras.erase( cameraIter );
          break; // there is only one camera with this object id, so break
        }
      }
    }
  }

  // delete camera from viewlists
  {
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
          if( viewIter->cameraObjectId_ == cameraObjectId )
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


//================================================================


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2( typesplatcloudplugin, TypeSplatCloudPlugin );
#endif

