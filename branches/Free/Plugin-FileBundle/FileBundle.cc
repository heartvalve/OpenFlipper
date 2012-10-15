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


bool FileBundlePlugin::addEmptyObjects( unsigned int _num, const DataType &_dataType, std::vector<int> &_objectIDs )
{
  deleteObjects( _objectIDs );
  _objectIDs.reserve( _num );

  OpenFlipper::Options::blockSceneGraphUpdates();

  unsigned int i;
  for( i=0; i<_num; ++i )
  {
    int objectId = -1;
    emit addEmptyObject( _dataType, objectId );

    if( objectId == -1 )
      break;

    _objectIDs.push_back( objectId );
  }

  OpenFlipper::Options::unblockSceneGraphUpdates();

  if( i == _num )
    return true;

  deleteObjects( _objectIDs );
  return false;
}


//----------------------------------------------------------------


void FileBundlePlugin::deleteObjects( std::vector<int> &_objectIDs )
{
  unsigned int i, num = _objectIDs.size();
  for( i=0; i<num; ++i )
    emit deleteObject( _objectIDs[ i ] );

  _objectIDs.clear();
}


//----------------------------------------------------------------


bool FileBundlePlugin::readImagelistFile( const char *_filename, std::vector<std::string> &_imagePaths ) /*const*/
{
  _imagePaths.clear();

  FILE *file = fopen( _filename, "rt" );
  if( !file )
  {
    emit log( LOGINFO, tr("Could not open imagelist file \"%1\".\n").arg( _filename ) );
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

  emit log( LOGINFO, tr("Using imagelist file \"%1\".\n").arg( _filename ) );
  return true;
}


//----------------------------------------------------------------


void FileBundlePlugin::readCameras( FILE *_file, const std::vector<int> &_cameraObjectIDs, SplatCloud_Cameras &_cameras ) /*const*/
{
  char str[256];

  unsigned int cameraIdx = 0;
  SplatCloud_Cameras::iterator cameraIter;
  for( cameraIter = _cameras.begin(); cameraIter != _cameras.end(); ++cameraIter, ++cameraIdx )
  {
    SplatCloud_Camera &camera = *cameraIter;

    camera.objectId_ = _cameraObjectIDs[ cameraIdx ];

    SplatCloud_Projection &proj = camera.projection_;
    fscanf( _file, "%32s", str ); proj.f_       = atof( str );
    fscanf( _file, "%32s", str ); proj.k1_      = atof( str );
    fscanf( _file, "%32s", str ); proj.k2_      = atof( str );
    fscanf( _file, "%32s", str ); proj.r_[0][0] = atof( str );
    fscanf( _file, "%32s", str ); proj.r_[0][1] = atof( str );
    fscanf( _file, "%32s", str ); proj.r_[0][2] = atof( str );
    fscanf( _file, "%32s", str ); proj.r_[1][0] = atof( str );
    fscanf( _file, "%32s", str ); proj.r_[1][1] = atof( str );
    fscanf( _file, "%32s", str ); proj.r_[1][2] = atof( str );
    fscanf( _file, "%32s", str ); proj.r_[2][0] = atof( str );
    fscanf( _file, "%32s", str ); proj.r_[2][1] = atof( str );
    fscanf( _file, "%32s", str ); proj.r_[2][2] = atof( str );
    fscanf( _file, "%32s", str ); proj.t_[0]    = atof( str );
    fscanf( _file, "%32s", str ); proj.t_[1]    = atof( str );
    fscanf( _file, "%32s", str ); proj.t_[2]    = atof( str );

    camera.imagePath_ = "";

    camera.imageWidth_  = 0;
    camera.imageHeight_ = 0;
  }
}


//----------------------------------------------------------------


void FileBundlePlugin::readPoints( FILE *_file, const std::vector<int> &_cameraObjectIDs, SplatCloud &_splatCloud ) /*const*/
{
  char str[256];

  int maxCamObjId = _cameraObjectIDs.size() - 1;

  unsigned int splatIdx;
  for( splatIdx = 0; splatIdx < _splatCloud.numSplats(); ++splatIdx )
  {
    {
      SplatCloud::Position &pos = _splatCloud.positions( splatIdx );
      fscanf( _file, "%32s", str ); pos[0] = atof( str );
      fscanf( _file, "%32s", str ); pos[1] = atof( str );
      fscanf( _file, "%32s", str ); pos[2] = atof( str );
    }

    {
      SplatCloud::Color &col = _splatCloud.colors( splatIdx );
      unsigned int r=0, g=0, b=0;
      fscanf( _file, "%16u", &r ); col[0] = r;
      fscanf( _file, "%16u", &g ); col[1] = g;
      fscanf( _file, "%16u", &b ); col[2] = b;
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
        fscanf( _file, "%16i", &i  ); viewIter->cameraObjectId_ = ((i >= 0) && (i <= maxCamObjId)) ? _cameraObjectIDs[ i ] : -1;
        fscanf( _file, "%16i", &j  ); viewIter->featureIdx_     = j;
        fscanf( _file, "%32s", str ); viewIter->x_              = atof( str );
        fscanf( _file, "%32s", str ); viewIter->y_              = atof( str );
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
    emit log( LOGERR, tr("Could not open input file \"%1\".\n").arg( _filename ) );
    return false; // return failure
  }

  // read and check first line
  std::string magicAndVersion = readLine( file );
  if( magicAndVersion.compare( "# Bundle file v0.3" ) != 0 )
  {
    emit log( LOGERR, tr("Bad magic/version \"%1\" in input file \"%2\".\n").arg( magicAndVersion.c_str(), _filename ) );
    fclose( file );
    return false; // return failure
  }

  // read counts
  unsigned int numCameras = 0;
  unsigned int numPoints  = 0;
  fscanf( file, "%16u", &numCameras );
  fscanf( file, "%16u", &numPoints  );

  // create camera object IDs
  std::vector<int> cameraObjectIDs;
  if( !addEmptyObjects( numCameras, DATA_CAMERA, cameraObjectIDs ) )
  {
    emit log( LOGERR, tr("Unable to add %1 cameras for input file \"%2\".\n").arg( QString::number( numCameras ), _filename ) );
    fclose( file );
    return false; // return failure
  }

  // read cameras data block
  if( numCameras != 0 )
  {
    SplatCloud_CameraManager &cameraManager = _splatCloud.requestCloudProperty( SPLATCLOUD_CAMERAMANAGER_HANDLE )->data();

    cameraManager.cameras_.resize( numCameras );
    readCameras( file, cameraObjectIDs, cameraManager.cameras_ );

    // set image paths
    {
      std::vector<std::string> imagePaths;

      std::string path, name, ext;
      splitFilename( _filename, path, name, ext );

      if( !readImagelistFile( (path + name + IMAGELIST_SUFFIX).c_str(), imagePaths ) )
           readImagelistFile( (path + IMAGELIST_FALLBACK     ).c_str(), imagePaths );

      bool hasImg = (cameraManager.cameras_.size() <= imagePaths.size());

      if( hasImg )
      {
        unsigned int cameraIdx = 0;
        SplatCloud_Cameras::iterator cameraIter;
        for( cameraIter = cameraManager.cameras_.begin(); cameraIter != cameraManager.cameras_.end(); ++cameraIter, ++cameraIdx )
          cameraIter->imagePath_ = imagePaths[ cameraIdx ];
      }

      _splatCloud.requestCloudProperty( SPLATCLOUD_GENERALMANAGER_HANDLE )->data().flags_.set( SPLATCLOUD_CAMERA_HAS_IMAGEPATH_FLAG, hasImg );
    }
  }

  // read points data block
  if( numPoints != 0 )
  {
    _splatCloud.resizeSplats( numPoints );
    _splatCloud.requestPositions();
    _splatCloud.requestColors();
    _splatCloud.requestViewlists();
    readPoints( file, cameraObjectIDs, _splatCloud );

    _splatCloud.requestCloudProperty( SPLATCLOUD_GENERALMANAGER_HANDLE )->data().flags_.set( SPLATCLOUD_SPLAT_VIEWLIST_HAS_FEATURE_INDICES_FLAG, true  );
    _splatCloud.requestCloudProperty( SPLATCLOUD_GENERALMANAGER_HANDLE )->data().flags_.set( SPLATCLOUD_SPLAT_VIEWLIST_COORDS_NORMALIZED_FLAG,   false );
  }

  // check if reading error occured
  if( feof( file ) )
  {
    emit log( LOGERR, tr("Unexpected end in input file \"%1\".\n" ).arg( _filename ) );
    fclose( file );
    return false; // return failure
  }

  // close file
  fclose( file );

  // return success
  return true;
}


//----------------------------------------------------------------


bool FileBundlePlugin::writeBundleFile( const char *_filename, const SplatCloud &_splatCloud, std::streamsize _precision ) /*const*/
{
  return false;
}


//----------------------------------------------------------------


int FileBundlePlugin::loadObject( QString _filename )
{
  // add a new, empty splatcloud-object
  int splatCloudObjectId = -1;
  emit addEmptyObject( DATA_SPLATCLOUD, splatCloudObjectId );
  if( splatCloudObjectId != -1 )
  {
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

          // return success
          return splatCloudObjectId;
        }
      }
    }
  }

  // return failure
  return -1;
}


//----------------------------------------------------------------


bool FileBundlePlugin::saveObject( int _objectId, QString _filename, std::streamsize _precision )
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
      if( writeBundleFile( _filename.toLatin1(), *splatCloud, _precision ) )
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
