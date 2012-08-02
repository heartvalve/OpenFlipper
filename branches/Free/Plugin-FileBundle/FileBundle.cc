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
//  CLASS FileBundlePlugin - IMPLEMENTATION
//
//================================================================


//== INCLUDES ====================================================


#include "FileBundle.hh"

#include <QtGui>
#include <QFileInfo>
#include <QSettings>

#include <fstream>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <OpenMesh/Core/IO/IOManager.hh>

#include <ObjectTypes/Camera/Camera.hh>

#include <string>

#include <vector>


//== CONSTANTS ===================================================


static const char IMAGELIST_SUFFIX[]   = ".txt";
static const char IMAGELIST_FALLBACK[] = "image_list.txt";


//== IMPLEMENTATION ==============================================


static std::string readLine( FILE *_file )
{
  // create empty string
  std::string result = "";

  while( true )
  {
    // read next char
    char c;
    fscanf( _file, "%c", &c );

    // if end of file or line is reached, break
    if( feof( _file ) || c == '\0' || c == '\n' || c == '\r' )
      break;

    // copy char to string
    result += c;
  }

  // return string
  return result;
}


//----------------------------------------------------------------


static void splitFilename( const std::string &_str, std::string &_path, std::string &_name, std::string &_ext )
{
  size_t i = _str.find_last_of( "/\\" );

  std::string rest;

  if( i == std::string::npos )
  {
    _path = "";
    rest  = _str;
  }
  else
  {
    _path = _str.substr( 0, i+1 ); // 0 .. i
    rest  = _str.substr( i+1 );    // i+1 .. end
  }

  size_t j = rest.find_last_of( '.' );

  if( j == std::string::npos )
  {
    _name = rest;
    _ext  = "";
  }
  else
  {
    _name = rest.substr( 0, j ); // 0 .. j-1
    _ext  = rest.substr( j );    // j .. end
  }
}


//----------------------------------------------------------------


bool FileBundlePlugin::readImagelistFile( const char *_filename, StringVector &_imagePaths ) /*const*/
{
  _imagePaths.clear();

  FILE *file = fopen( _filename, "rt" );
  if( !file )
  {
    emit log( LOGINFO, QString( "Could not open imagelist file \"" ) + _filename + QString( "\".\n" ) );
    return false;
  }

  char path[4096];
  char temp[32];

  while( true )
  {
    fscanf( file, "%4096s", path );
    fscanf( file, "%32s",   temp );
    fscanf( file, "%32s",   temp );

    if( feof( file ) )
      break;

    _imagePaths.push_back( std::string( path ) );
  }

  fclose( file );

  emit log( LOGINFO, QString( "Using image list file \"" ) + _filename + QString( "\".\n" ) );
  return true;
}


//----------------------------------------------------------------


void FileBundlePlugin::readCameras( FILE *_file, CameraVector &_cameras ) /*const*/
{
  char str[256];

  CameraVector::iterator cameraIter;
  for( cameraIter = _cameras.begin(); cameraIter != _cameras.end(); ++cameraIter )
  {
    Camera &camera = *cameraIter;

    fscanf( _file, "%32s", str ); camera.f_       = atof( str );
    fscanf( _file, "%32s", str ); camera.k1_      = atof( str );
    fscanf( _file, "%32s", str ); camera.k2_      = atof( str );
    fscanf( _file, "%32s", str ); camera.r_[0][0] = atof( str );
    fscanf( _file, "%32s", str ); camera.r_[0][1] = atof( str );
    fscanf( _file, "%32s", str ); camera.r_[0][2] = atof( str );
    fscanf( _file, "%32s", str ); camera.r_[1][0] = atof( str );
    fscanf( _file, "%32s", str ); camera.r_[1][1] = atof( str );
    fscanf( _file, "%32s", str ); camera.r_[1][2] = atof( str );
    fscanf( _file, "%32s", str ); camera.r_[2][0] = atof( str );
    fscanf( _file, "%32s", str ); camera.r_[2][1] = atof( str );
    fscanf( _file, "%32s", str ); camera.r_[2][2] = atof( str );
    fscanf( _file, "%32s", str ); camera.t_[0]    = atof( str );
    fscanf( _file, "%32s", str ); camera.t_[1]    = atof( str );
    fscanf( _file, "%32s", str ); camera.t_[2]    = atof( str );

    camera.imagePath_ = "";
  }
}


//----------------------------------------------------------------


void FileBundlePlugin::readPoints( FILE *_file, SplatCloud &_splatCloud ) /*const*/
{
  char str[256];

  unsigned int splatIdx;
  for( splatIdx = 0; splatIdx < _splatCloud.numSplats(); ++splatIdx )
  {
    {
      SplatCloud::Position &position = _splatCloud.positions( splatIdx );

      fscanf( _file, "%32s", str ); position[0] = atof( str );
      fscanf( _file, "%32s", str ); position[1] = atof( str );
      fscanf( _file, "%32s", str ); position[2] = atof( str );
    }

    {
      SplatCloud::Color &color = _splatCloud.colors( splatIdx );

      unsigned int r=0, g=0, b=0;
      fscanf( _file, "%16u", &r ); color[0] = r;
      fscanf( _file, "%16u", &g ); color[1] = g;
      fscanf( _file, "%16u", &b ); color[2] = b;
    }

    {
      SplatCloud::Viewlist &viewlist = _splatCloud.viewlists( splatIdx );

      unsigned int numEntries = 0;
      fscanf( _file, "%16u", &numEntries );

      viewlist.resize( numEntries );

      SplatCloud::Viewlist::iterator viewIter;
      for( viewIter = viewlist.begin(); viewIter != viewlist.end(); ++viewIter )
      {
        int i = -1;
        int j = -1;
        fscanf( _file, "%16i", &i  ); viewIter->cameraIdx_  = i;
        fscanf( _file, "%16i", &j  ); viewIter->featureIdx_ = j;
        fscanf( _file, "%32s", str ); viewIter->x_          = atof( str );
        fscanf( _file, "%32s", str ); viewIter->y_          = atof( str );
      }
    }
  }
}


//----------------------------------------------------------------


bool FileBundlePlugin::readBundleFile( const char *_filename, SplatCloud &_splatCloud ) /*const*/
{
  // clear splatcloud
  _splatCloud.clear();

  // open file
  FILE *file = fopen( _filename, "rt" );
  if( !file )
  {
    emit log( LOGERR, QString( "Could not open input file \"" ) + _filename + QString( "\".\n" ) );
    return false; // return failure
  }

  // read and check first line
  std::string magicAndVersion = readLine( file );
  if( magicAndVersion.compare( "# Bundle file v0.3" ) != 0 )
  {
    emit log( LOGERR, QString( "Bad magic/version \"" ) + magicAndVersion.c_str() + QString( "\" in input file \"" ) + _filename + QString( "\".\n" ) );
    fclose( file );
    return false; // return failure
  }

  // read counts
  unsigned int numCameras = 0;
  unsigned int numPoints  = 0;
  fscanf( file, "%16u", &numCameras );
  fscanf( file, "%16u", &numPoints  );

  // read cameras data block
  if( numCameras != 0 )
  {
    CameraVector &cameras = _splatCloud.requestCloudProperty<CameraVector>( "Cameras" )->data();

    cameras.resize( numCameras );

    readCameras( file, cameras );

    // set image paths
    {
      std::vector<std::string> imagePaths;

      std::string path, name, ext;
      splitFilename( _filename, path, name, ext );

      if( !readImagelistFile( (path + name + IMAGELIST_SUFFIX).c_str(), imagePaths ) )
           readImagelistFile( (path + IMAGELIST_FALLBACK     ).c_str(), imagePaths );

      bool hasImg = (cameras.size() <= imagePaths.size());

      if( hasImg )
      {
        unsigned int cameraIdx;
        for( cameraIdx = 0; cameraIdx < cameras.size(); ++cameraIdx )
          cameras[ cameraIdx ].imagePath_ = imagePaths[ cameraIdx ];
      }

      _splatCloud.requestCloudProperty<bool>( "CameraHasImagepath" )->data() = hasImg;
    }
  }

  // read points data block
  if( numPoints != 0 )
  {
    _splatCloud.resizeSplats( numPoints );

    _splatCloud.requestPositions();
    _splatCloud.requestColors();
    _splatCloud.requestViewlists();

    readPoints( file, _splatCloud );

    _splatCloud.requestCloudProperty<bool>( "SplatViewlistHasFeatureindices"     )->data() = true;
    _splatCloud.requestCloudProperty<bool>( "SplatViewlistCoordinatesNormalized" )->data() = false;
  }

  // check if reading error occured
  if( feof( file ) )
  {
    emit log( LOGERR, QString( "Unexpected end in input file \"" ) + _filename + QString( "\".\n" ) );
    fclose( file );
    return false; // return failure
  }

  // close file
  fclose( file );

  // return success
  return true;
}


//----------------------------------------------------------------


bool FileBundlePlugin::writeBundleFile( const char *_filename, const SplatCloud &_splatCloud ) /*const*/
{
  return false;
}


//----------------------------------------------------------------


int FileBundlePlugin::addCameras( const CameraVector &_cameras )
{
  // create a list of ids
  IdList objectIDs;
  objectIDs.reserve( _cameras.size() );

  // disable updating of scenegraph
  OpenFlipper::Options::blockSceneGraphUpdates();

  // add cameras to list of ids
  CameraVector::const_iterator cameraIter;
  for( cameraIter = _cameras.begin(); cameraIter != _cameras.end(); ++cameraIter )
  {
    const Camera &camera = *cameraIter;

    // set up matrix
    ACG::GLMatrixd matrix;
    matrix(0,0) = camera.r_[0][0]; matrix(0,1) = camera.r_[1][0]; matrix(0,2) = camera.r_[2][0];
    matrix(1,0) = camera.r_[0][1]; matrix(1,1) = camera.r_[1][1]; matrix(1,2) = camera.r_[2][1];
    matrix(2,0) = camera.r_[0][2]; matrix(2,1) = camera.r_[1][2]; matrix(2,2) = camera.r_[2][2];
    matrix(0,3) = -(camera.r_[0][0]*camera.t_[0] + camera.r_[1][0]*camera.t_[1] + camera.r_[2][0]*camera.t_[2]);
    matrix(1,3) = -(camera.r_[0][1]*camera.t_[0] + camera.r_[1][1]*camera.t_[1] + camera.r_[2][1]*camera.t_[2]);
    matrix(2,3) = -(camera.r_[0][2]*camera.t_[0] + camera.r_[1][2]*camera.t_[1] + camera.r_[2][2]*camera.t_[2]);
    matrix(3,0) = 0.0; matrix(3,1) = 0.0; matrix(3,2) = 0.0; matrix(3,3) = 1.0;

    // create a new, empty camera-object
    int cameraObjectId = -1;
    emit addEmptyObject( DATA_CAMERA, cameraObjectId );
    if( cameraObjectId != -1 )
    {
      // add id of camera-object to list of ids
      objectIDs.push_back( cameraObjectId );

      // get camera-object by id
      CameraObject *cameraObject = 0;
      if( PluginFunctions::getObject( cameraObjectId, cameraObject ) )
      {
        // set name of camera-object
        cameraObject->setName( camera.imagePath_.c_str() );

        // get camera
        CameraNode *cameraNode = cameraObject->cameraNode();
        if( cameraNode != 0 )
        {

          // set camera parameters
          cameraNode->setModelView( matrix );
          cameraNode->setSize( 1, 1 ); // FIXME

          // emit signal that the camera-object has to be updated
          emit updatedObject( cameraObjectId, UPDATE_ALL );

          // everything is okay, so continue with next camera
          continue;
        }
      }
    }

    // something went wrong, so break
    break;
  }

  // enable updating of scenegraph
  OpenFlipper::Options::unblockSceneGraphUpdates();

  // check if everything is okay so far
  if( cameraIter == _cameras.end() )
  {
    // group objects
    int groupObjectId = RPC::callFunctionValue<int>( "datacontrol", "groupObjects", objectIDs, QString( "Cameras" ) );
    if( groupObjectId != -1 )
    {
      // everything is okay, so return id of group-object
      return groupObjectId;
    }
  }

  // something went wrong, so delete objects
  unsigned int i, num = objectIDs.size();
  for( i=0; i<num; ++i )
    emit deleteObject( objectIDs[ i ] );

  // return failure
  return -1;
}


//----------------------------------------------------------------


int FileBundlePlugin::loadObject( QString _filename )
{
  // add a new, empty splatcloud-object
  int splatCloudObjectId = -1;
  emit addEmptyObject( DATA_SPLATCLOUD, splatCloudObjectId );
  if( splatCloudObjectId != -1 )
  {
    // create list of ids and add id of splatcloud-object
    IdList objectIDs;
    objectIDs.push_back( splatCloudObjectId );

    // get splatcloud-object by id
    SplatCloudObject *splatCloudObject = 0;
    if( PluginFunctions::getObject( splatCloudObjectId, splatCloudObject ) )
    {
      // set name of splatcloud-object to filename
      splatCloudObject->setFromFileName( _filename );
      splatCloudObject->setName( splatCloudObject->filename() );

      // get splatcloud and scenegraph splatcloud-node
      SplatCloud     *splatCloud     = splatCloudObject->splatCloud();
      SplatCloudNode *splatCloudNode = splatCloudObject->splatCloudNode();
      if( (splatCloud != 0) && (splatCloudNode != 0) )
      {
        // read splatcloud from disk
        if( readBundleFile( _filename.toLatin1(), *splatCloud ) )
        {
          // emit signals that the splatcloud-object has to be updated and that a file was opened
          emit updatedObject( splatCloudObjectId, UPDATE_ALL );
          emit openedFile( splatCloudObjectId );

          // get drawmodes
          ACG::SceneGraph::DrawModes::DrawMode splatsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode( "Splats" );
          ACG::SceneGraph::DrawModes::DrawMode dotsDrawMode   = ACG::SceneGraph::DrawModes::getDrawMode( "Dots"   );
          ACG::SceneGraph::DrawModes::DrawMode pointsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode( "Points" );

          // if drawmodes don't exist something went wrong
          if( splatsDrawMode == ACG::SceneGraph::DrawModes::NONE || 
              dotsDrawMode   == ACG::SceneGraph::DrawModes::NONE || 
              pointsDrawMode == ACG::SceneGraph::DrawModes::NONE )
          {
            emit log( LOGERR, tr("Shader DrawModes for SplatCloud not existent!") );
          }
          else
          {
            // get global drawmode
            ACG::SceneGraph::DrawModes::DrawMode drawmode = PluginFunctions::drawMode();

            // if global drawmode does *not* contain any of 'Splats', 'Dots' or 'Points' drawmode, add 'Points'
            if( !drawmode.containsAtomicDrawMode( splatsDrawMode ) && 
                !drawmode.containsAtomicDrawMode( dotsDrawMode   ) && 
                !drawmode.containsAtomicDrawMode( pointsDrawMode ) )
            {
              drawmode |= pointsDrawMode;
              PluginFunctions::setDrawMode( drawmode );
            }
          }

          // add cameras-object
          const SplatCloud::CloudPropertyT<CameraVector> *camerasProp = splatCloud->getCloudProperty<CameraVector>( "Cameras" );
          int camerasObjectId = (camerasProp == 0) ? -1 : addCameras( camerasProp->data() );
          if( (camerasProp == 0) || (camerasObjectId != -1) )
          {
            // add id of cameras-object to list of ids
            if( camerasObjectId != -1 )
              objectIDs.push_back( camerasObjectId );

            // group objects
            int groupObjectId = RPC::callFunctionValue<int>( "datacontrol", "groupObjects", objectIDs );
            if( groupObjectId != -1 )
            {
              // everything is okay, so return id of group-object
              return groupObjectId;
            }
          }
        }
      }
    }

    // something went wrong, so delete objects
    unsigned int i, num = objectIDs.size();
    for( i=0; i<num; ++i )
      emit deleteObject( objectIDs[ i ] );
  }

  // return failure
  return -1;
}


//----------------------------------------------------------------


bool FileBundlePlugin::saveObject( int _objectId, QString _filename )
{
  // get splatcloud-object by id
  SplatCloudObject *splatCloudObject = 0;
  if( PluginFunctions::getObject( _objectId, splatCloudObject ) )
  {
    // change name of splatcloud-object to filename
    splatCloudObject->setFromFileName( _filename );
    splatCloudObject->setName( splatCloudObject->filename() );

    // get splatcloud
    SplatCloud *splatCloud = splatCloudObject->splatCloud();
    if( splatCloud != 0 )
    {
      // write splatcloud to disk
      if( writeBundleFile( _filename.toLatin1(), *splatCloud ) )
      {
        // return success
        return true;
      }
    }
  }

  // return failure
  return false;
}


//----------------------------------------------------------------


QWidget *FileBundlePlugin::saveOptionsWidget( QString /*_currentFilter*/ )
{
  return 0;
}


//----------------------------------------------------------------


QWidget *FileBundlePlugin::loadOptionsWidget( QString /*_currentFilter*/ )
{
  return 0;
}


//================================================================

Q_EXPORT_PLUGIN2( filebundleplugin, FileBundlePlugin );
