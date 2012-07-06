//================================================================
//
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
//  CLASS FilePTSPlugin - IMPLEMENTATION
//
//================================================================


//== INCLUDES ====================================================


#include "FilePTS.hh"

#include <QtGui>
#include <QFileInfo>
#include <QSettings>

#include <fstream>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <OpenMesh/Core/IO/IOManager.hh>


//== CONSTANTS ===================================================


// constants of color range drop down box
static const int COLORRANGE_0_1   = 0;
static const int COLORRANGE_0_255 = 1;


//== IMPLEMENTATION ==============================================


FilePTSPlugin::FilePTSPlugin() : 
  loadOptions_( 0 ), 
  saveOptions_( 0 ), 
  loadBinaryFile_   ( 0 ), 
  loadNormals_      ( 0 ), 
  loadPointsizes_   ( 0 ), 
  loadColors_       ( 0 ), 
  loadColorRange_   ( 0 ), 
  loadIndices_      ( 0 ), 
  saveBinaryFile_( 0 ), 
  saveNormals_   ( 0 ), 
  savePointsizes_( 0 ), 
  saveColors_    ( 0 ), 
  saveColorRange_( 0 ), 
  saveIndices_   ( 0 ), 
  loadMakeDefaultButton_( 0 ), 
  saveMakeDefaultButton_( 0 ) 
{ }


//----------------------------------------------------------------


bool FilePTSPlugin::readBinaryFile( const char *_filename, SplatCloud &_splatCloud ) /*const*/
{
  // clear splatcloud
  _splatCloud.clear();

  // set default options
  bool loadNormals    = true;
  bool loadPointsizes = false;
  bool loadColors     = false;
//int  loadColorRange = 0;
  bool loadIndices    = false;

  // get options
  if( OpenFlipper::Options::gui() && loadOptions_ )
  {
    loadNormals    = loadNormals_->   isChecked();
    loadPointsizes = loadPointsizes_->isChecked();
    loadColors     = loadColors_->    isChecked();
//  loadColorRange = loadColorRange_->currentIndex();
    loadIndices    = loadIndices_->   isChecked();
  }

  // request properties
  bool success = true;
                       { if( !_splatCloud.requestPositions()  ) success = false; }
  if( loadNormals    ) { if( !_splatCloud.requestNormals()    ) success = false; }
  if( loadPointsizes ) { if( !_splatCloud.requestPointsizes() ) success = false; }
  if( loadColors     ) { if( !_splatCloud.requestColors()     ) success = false; }
  if( loadIndices    ) { if( !_splatCloud.requestIndices()    ) success = false; }

  // check success of requests
  if( !success )
  {
    emit log( LOGERR, QString( "Out of memory for input file \"" ) + _filename + QString( "\"\n." ) );
    return false; // return failure
  }

  // open file
  FILE *file = fopen( _filename, "rb" );
  if( !file )
  {
    emit log( LOGERR, QString( "Could not open input file \"" ) + _filename + QString( "\"\n." ) );
    return false;
  }

  // read file type
  int fileType = 0;
  fread( &fileType, sizeof(int), 1, file );

  // check file type
  if( fileType != 1 && fileType != 2 )
  {
    emit log( LOGERR, QString( "Bad filetype (" ) + QString::number( fileType ) + QString( ") in input file \"" ) + _filename + QString( "\"\n." ) );
    fclose( file );
    return false; // return failure
  }

  // read number of splats
  unsigned int numSplats = 0;
  fread( &numSplats, sizeof(unsigned int), 1, file );

  // set number of splats
  _splatCloud.resizeSplats( numSplats );

  // read positions
  {
    unsigned int i;
    for( i=0; i<numSplats; ++i )
    {
      float pos[3];
      fread( pos, sizeof(float), 3, file );

      SplatCloud::Position position;
      position[0] = pos[0];
      position[1] = pos[1];
      position[2] = pos[2];

      _splatCloud.positions( i ) = position;
    }
  }

  // read normals
  if( loadNormals )
  {
    unsigned int i;
    for( i=0; i<numSplats; ++i )
    {
      float nrm[3];
      fread( nrm, sizeof(float), 3, file );

      SplatCloud::Normal normal;
      normal[0] = nrm[0];
      normal[1] = nrm[1];
      normal[2] = nrm[2];

      _splatCloud.normals( i ) = normal;
    }
  }

  // read pointsizes
  if( loadPointsizes )
  {
    unsigned int i;
    for( i=0; i<numSplats; ++i )
    {
      float ps = 0.0f;
      fread( &ps, sizeof(float), 1, file );

      SplatCloud::Pointsize pointsize;
      pointsize = ps;

      _splatCloud.pointsizes( i ) = pointsize;
    }
  }

  // read colors
  if( loadColors )
  {
    unsigned int i;
    for( i=0; i<numSplats; ++i )
    {
      unsigned int col = 0;
      fread( &col, sizeof(unsigned int), 1, file );

      SplatCloud::Color color; // ignore colorrange
      color[0] = (unsigned char) ((col >> 16) & 0xFF);
      color[1] = (unsigned char) ((col >>  8) & 0xFF);
      color[2] = (unsigned char) ((col      ) & 0xFF);

      _splatCloud.colors( i ) = color;
    }
  }

  // read indices
  if( loadIndices )
  {
    unsigned int i;
    for( i=0; i<numSplats; ++i )
    {
      int idx = -1;
      fread( &idx, sizeof(idx), 1, file );

      SplatCloud::Index index;
      index = idx;

      _splatCloud.indices( i ) = index;
    }
  }

  // check for errors
  if( ferror( file ) )
  {
    emit log( LOGERR, QString( "Could not read input file \"" ) + _filename + QString( "\"\n." ) );
    fclose( file );
    return false; // return failure
  }
  if( feof( file ) )
  {
    emit log( LOGERR, QString( "Unexpected end in input file \"" ) + _filename + QString( "\"\n." ) );
    fclose( file );
    return false; // return failure
  }

  // close file
  fclose( file );

  // return success
  return true;
}


//----------------------------------------------------------------


bool FilePTSPlugin::readTextFile( const char *_filename, SplatCloud &_splatCloud ) /*const*/
{
  // clear splatcloud
  _splatCloud.clear();

  // set default options
  bool loadNormals    = true;
  bool loadPointsizes = false;
  bool loadColors     = false;
  int  loadColorRange = 0;
  bool loadIndices    = false;

  // get options
  if( OpenFlipper::Options::gui() && loadOptions_ )
  {
    loadNormals    = loadNormals_   ->isChecked();
    loadPointsizes = loadPointsizes_->isChecked();
    loadColors     = loadColors_    ->isChecked();
    loadColorRange = loadColorRange_->currentIndex();
    loadIndices    = loadIndices_   ->isChecked();
  }

  // request properties
  bool success = true;
                       { if( !_splatCloud.requestPositions()  ) success = false; }
  if( loadNormals    ) { if( !_splatCloud.requestNormals()    ) success = false; }
  if( loadPointsizes ) { if( !_splatCloud.requestPointsizes() ) success = false; }
  if( loadColors     ) { if( !_splatCloud.requestColors()     ) success = false; }
  if( loadIndices    ) { if( !_splatCloud.requestIndices()    ) success = false; }

  // check success of requests
  if( !success )
  {
    emit log( LOGERR, QString( "Out of memory for input file \"" ) + _filename + QString( "\"\n." ) );
    return false; // return failure
  }

  // open file
  FILE *file = fopen( _filename, "rb" );
  if( !file )
  {
    emit log( LOGERR, QString( "Could not open input file \"" ) + _filename + QString( "\"\n." ) );
    return false;
  }

  int splatIdx;
  for( splatIdx = 0; ; ++splatIdx )
  {
    // read position
    {
      float pos[3];
      if( fscanf( file, "%32f %32f %32f", &pos[0], &pos[1], &pos[2] ) != 3 )
        break;

      // increase number of splats
      _splatCloud.pushbackSplat();

      SplatCloud::Position position;
      position[0] = pos[0];
      position[1] = pos[1];
      position[2] = pos[2];

      _splatCloud.positions( splatIdx ) = position;
    }

    // read color
    if( loadColors )
    {
      float col[3];
      fscanf( file, "%32f %32f %32f", &col[0], &col[1], &col[2] );

      SplatCloud::Color color;

      if( loadColorRange == COLORRANGE_0_1 )
      {
        color[0] = (unsigned char) (255.999f * col[0]);
        color[1] = (unsigned char) (255.999f * col[1]);
        color[2] = (unsigned char) (255.999f * col[2]);
      }
      else // loadColorRange == COLORRANGE_0_255
      {
        color[0] = (unsigned char) col[0];
        color[1] = (unsigned char) col[1];
        color[2] = (unsigned char) col[2];
      }

      _splatCloud.colors( splatIdx ) = color;
    }

    // read normal
    if( loadNormals )
    {
      float nrm[3];
      fscanf( file, "%32f %32f %32f", &nrm[0], &nrm[1], &nrm[2] );

      SplatCloud::Normal normal;
      normal[0] = nrm[0];
      normal[1] = nrm[1];
      normal[2] = nrm[2];

      _splatCloud.normals( splatIdx ) = normal;
    }

    // read pointsize
    if( loadPointsizes )
    {
      float ps = 0.0f;
      fscanf( file, "%32f", &ps );

      SplatCloud::Pointsize pointsize;
      pointsize = ps;

      _splatCloud.pointsizes( splatIdx ) = pointsize;
    }

    // read index
    if( loadIndices )
    {
      int idx = -1;
      fscanf( file, "%16i", &idx );

      SplatCloud::Index index;
      index = idx;

      _splatCloud.indices( splatIdx ) = index;
    }

    // check for errors
    if( ferror( file ) )
    {
      emit log( LOGERR, QString( "Could not read input file \"" ) + _filename + QString( "\"\n." ) );
      fclose( file );
      return false; // return failure
    }
    if( feof( file ) )
    {
      emit log( LOGERR, QString( "Unexpected end in input file \"" ) + _filename + QString( "\"\n." ) );
      fclose( file );
      return false; // return failure
    }
  }

  // check for errors
  if( !feof( file ) ) // if end-of-file is *not* reached, something went wrong
  {
    emit log( LOGERR, QString( "Bad file format of input file \"" ) + _filename + QString( "\"\n." ) );
    fclose( file );
    return false; // return failure
  }

  // close file
  fclose( file );

  // return success
  return true;
}


//----------------------------------------------------------------


bool FilePTSPlugin::writeBinaryFile( const char *_filename, const SplatCloudNode *_splatCloudNode ) /*const*/
{
  // set default options
  bool saveNormals    = true;
  bool savePointsizes = false;
  bool saveColors     = false;
//int  saveColorRange = 0;
  bool saveIndices    = false;

  // get options
  if( OpenFlipper::Options::gui() && saveOptions_ )
  {
    saveNormals    = saveNormals_->   isChecked();
    savePointsizes = savePointsizes_->isChecked();
    saveColors     = saveColors_->    isChecked();
//  saveColorRange = saveColorRange_->currentIndex();
    saveIndices    = saveIndices_->   isChecked();
  }

  // use default values instead of returning a failure

//// check availability
//if( (                  !_splatCloudNode->splatCloud().hasPositions() ) ||
//    (saveNormals    && !_splatCloudNode->splatCloud().hasNormals())  ) ||
//    (savePointsizes && !_splatCloudNode->splatCloud().hasPointsizes()) ||
//    (saveColors     && !_splatCloudNode->splatCloud().hasColors()    ) ||
//    (saveIndices    && !_splatCloudNode->splatCloud().hasIndices()   )
//{
//  emit log( LOGERR, QString( "Desired properties not available for output file \"" ) + _filename + QString( "\"\n." ) );
//  return false; // return failure
//}

  // open file
  FILE *file = fopen( _filename, "wb" );
  if( !file )
  {
    emit log( LOGERR, QString( "Could not open output file \"" ) + _filename + QString( "\"\n." ) );
    return false;
  }

  // write file type
  int fileType = 1;
  fwrite( &fileType, sizeof(int), 1, file );

  // write number of splats
  unsigned int numSplats = _splatCloudNode->splatCloud().numSplats();
  fwrite( &numSplats, sizeof(unsigned int), 1, file );

  // write positions
  {
    unsigned int i;
    for( i=0; i<numSplats; ++i )
    {
      const SplatCloud::Position &position = _splatCloudNode->getPosition( i );

      float pos[3];
      pos[0] = position[0];
      pos[1] = position[1];
      pos[2] = position[2];

      fwrite( pos, sizeof(float), 3, file );
    }
  }

  // write normals
  if( saveNormals )
  {
    unsigned int i;
    for( i=0; i<numSplats; ++i )
    {
      const SplatCloud::Normal &normal = _splatCloudNode->getNormal( i );

      float nrm[3];
      nrm[0] = normal[0];
      nrm[1] = normal[1];
      nrm[2] = normal[2];

      fwrite( nrm, sizeof(float), 3, file );
    }
  }

  // write pointsizes
  if( savePointsizes )
  {
    unsigned int i;
    for( i=0; i<numSplats; ++i )
    {
      const SplatCloud::Pointsize &pointsize = _splatCloudNode->getPointsize( i );

      float ps;
      ps = pointsize;

      fwrite( &ps, sizeof(float), 1, file );
    }
  }

  // write colors
  if( saveColors )
  {
    unsigned int i;
    for( i=0; i<numSplats; ++i )
    {
      const SplatCloud::Color &color = _splatCloudNode->getColor( i );

      unsigned int col; // ignore colorrange
      col = (0xFF << 24) | (color[0] << 16) | (color[1] << 8) | (color[2]);

      fwrite( &col, sizeof(unsigned int), 1, file );
    }
  }

  // write indices
  if( saveIndices )
  {
    unsigned int i;
    for( i=0; i<numSplats; ++i )
    {
      const SplatCloud::Index &index = _splatCloudNode->getIndex( i );

      int idx;
      idx = index;

      fwrite( &idx, sizeof(int), 1, file );
    }
  }

  // check for errors
  if( ferror( file ) )
  {
    emit log( LOGERR, QString( "Could not write output file \"" ) + _filename + QString( "\"\n." ) );
    fclose( file );
    return false; // return failure
  }

  // close file
  fclose( file );

  // return success
  return true;
}


//----------------------------------------------------------------


bool FilePTSPlugin::writeTextFile( const char *_filename, const SplatCloudNode *_splatCloudNode ) /*const*/
{
  // set default options
  bool saveNormals    = true;
  bool savePointsizes = false;
  bool saveColors     = false;
  int  saveColorRange = 0;
  bool saveIndices    = false;

  // get options
  if( OpenFlipper::Options::gui() && saveOptions_ )
  {
    saveNormals    = saveNormals_->   isChecked();
    savePointsizes = savePointsizes_->isChecked();
    saveColors     = saveColors_->    isChecked();
    saveColorRange = saveColorRange_->currentIndex();
    saveIndices    = saveIndices_->   isChecked();
  }

  // use default values instead of returning a failure

//// check availability
//if( (                  !_splatCloudNode->splatCloud().hasPositions() ) ||
//    (saveNormals    && !_splatCloudNode->splatCloud().hasNormals())  ) ||
//    (savePointsizes && !_splatCloudNode->splatCloud().hasPointsizes()) ||
//    (saveColors     && !_splatCloudNode->splatCloud().hasColors()    ) ||
//    (saveIndices    && !_splatCloudNode->splatCloud().hasIndices()   )
//{
//  emit log( LOGERR, QString( "Desired properties not available for output file \"" ) + _filename + QString( "\"\n." ) );
//  return false; // return failure
//}

  // open file
  FILE *file = fopen( _filename, "wt" );
  if( !file )
  {
    emit log( LOGERR, QString( "Could not open output file \"" ) + _filename + QString( "\"\n." ) );
    return false;
  }

  // for all splats...
  unsigned int i, numSplats = _splatCloudNode->splatCloud().numSplats();
  for( i=0; i<numSplats; ++i )
  {
    // write position
    {
      const SplatCloud::Position &position = _splatCloudNode->getPosition( i );

      float pos[3];
      pos[0] = position[0];
      pos[1] = position[1];
      pos[2] = position[2];

      fprintf( file, "%.6g %.6g %.6g", pos[0], pos[1], pos[2] );
    }

    // write color
    if( saveColors )
    {
      const SplatCloud::Color &color = _splatCloudNode->getColor( i );

      if( saveColorRange == COLORRANGE_0_1 )
      {
        static const float RCP255 = 1.0f / 255.0f;

        float col[3];
        col[0] = RCP255 * color[0];
        col[1] = RCP255 * color[1];
        col[2] = RCP255 * color[2];

        fprintf( file, " %.6g %.6g %.6g", col[0], col[1], col[2] );
      }
      else // saveColorRange == COLORRANGE_0_255
      {
        int col[3]; // use int, *not* unsigned char !
        col[0] = color[0];
        col[1] = color[1];
        col[2] = color[2];

        fprintf( file, " %i %i %i", col[0], col[1], col[2] );
      }
    }

    // write normal
    if( saveNormals )
    {
      const SplatCloud::Normal &normal = _splatCloudNode->getNormal( i );

      float nrm[3];
      nrm[0] = normal[0];
      nrm[1] = normal[1];
      nrm[2] = normal[2];

      fprintf( file, " %.6g %.6g %.6g", nrm[0], nrm[1], nrm[2] );
    }

    // write pointsize
    if( savePointsizes )
    {
      const SplatCloud::Pointsize &pointsize = _splatCloudNode->getPointsize( i );

      float ps;
      ps = pointsize;

      fprintf( file, " %.6g", ps );
    }

    // write index
    if( saveIndices )
    {
      const SplatCloud::Index &index = _splatCloudNode->getIndex( i );

      int idx;
      idx = index;

      fprintf( file, " %i", idx );
    }

    fprintf( file, "\n" );
  }

  // check for errors
  if( ferror( file ) )
  {
    emit log( LOGERR, QString( "Could not write output file \"" ) + _filename + QString( "\"\n." ) );
    fclose( file );
    return false; // return failure
  }

  // close file
  fclose( file );

  // return success
  return true;
}


//----------------------------------------------------------------


int FilePTSPlugin::loadObject( QString _filename )
{
  // set default options
  bool loadBinaryFile = false;

  // get options
  if( OpenFlipper::Options::gui() && loadOptions_ )
  {
    loadBinaryFile = loadBinaryFile_->   isChecked();
  }

  // add a new, empty splatcloud object
  int id = -1;
  emit addEmptyObject( DATA_SPLATCLOUD, id );

  // check id
  if( id == -1 )
    return -1; // return failure

  // get splatcloud-object by id
  SplatCloudObject *splatCloudObject = 0;
  if( !PluginFunctions::getObject( id, splatCloudObject ) )
  {
    emit deleteObject( id );
    return -1; // return failure
  }

  // check if splatcloud-object is okay
  if( !splatCloudObject )
  {
    emit deleteObject( id );
    return -1; // return failure
  }

  // set name of splatcloud-object to filename
  splatCloudObject->setFromFileName( _filename );
  splatCloudObject->setName( splatCloudObject->filename() );

  // get splatcloud and scenegraph splatcloud-node
  SplatCloud     *splatCloud     = splatCloudObject->splatCloud();
  SplatCloudNode *splatCloudNode = splatCloudObject->splatCloudNode();

  // check if splatcloud-node if okay
  if( !splatCloud || !splatCloudNode )
  {
    emit deleteObject( id );
    return -1; // return failure
  }

  // read file
  if( loadBinaryFile )
  {
    if( !readBinaryFile( _filename.toLatin1(), *splatCloud ) )
    {
      emit deleteObject( id );
      return -1; // return failure
    }
  }
  else
  {
    if( !readTextFile( _filename.toLatin1(), *splatCloud ) )
    {
      emit deleteObject( id );
      return -1; // return failure
    }
  }

  // emit signals that the object has to be updated and that a file was opened
  emit updatedObject( splatCloudObject->id(), UPDATE_ALL);
  emit openedFile   ( splatCloudObject->id()            );

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

  // return the id of the new splatcloud object
  return id;
}


//----------------------------------------------------------------


bool FilePTSPlugin::saveObject( int _objectId, QString _filename )
{
  // set default options
  bool saveBinaryFile = false;

  // get options
  if( OpenFlipper::Options::gui() && saveOptions_ )
  {
    saveBinaryFile = saveBinaryFile_->isChecked();
  }

  // get splatcloud-object by id
  SplatCloudObject *splatCloudObject = 0;
  if( !PluginFunctions::getObject( _objectId, splatCloudObject ) )
    return false; // return failure

  // check if splatcloud-object is okay
  if( !splatCloudObject )
    return false; // return failure

  // change name of splatcloud-object to filename
  splatCloudObject->setFromFileName( _filename );
  splatCloudObject->setName( splatCloudObject->filename() );

  // get scenegraph splatcloud-node
  const SplatCloudNode *splatCloudNode = splatCloudObject->splatCloudNode();

  // check if splatcloud-node if okay
  if( !splatCloudNode )
    return false; // return failure

  // write file
  if( saveBinaryFile )
  {
    if( !writeBinaryFile( _filename.toLatin1(), splatCloudNode ) )
      return false; // return failure
  }
  else
  {
    if( !writeTextFile( _filename.toLatin1(), splatCloudNode ) )
      return false; // return failure
  }

  // return success
  return true;
}


//----------------------------------------------------------------


QWidget *FilePTSPlugin::loadOptionsWidget( QString /*_currentFilter*/ )
{
  if( loadOptions_ == 0 )
  {
    // create new widget (including Load Options and buttons)

    loadBinaryFile_ = new QCheckBox( "Load as Binary File" );

    loadNormals_    = new QCheckBox( "Contains Normals"    );
    loadPointsizes_ = new QCheckBox( "Contains Pointsizes" );
    loadColors_     = new QCheckBox( "Contains Colors"     );

    loadColorRange_ = new QComboBox();
    loadColorRange_->addItem( "[0..1]"   );
    loadColorRange_->addItem( "[0..255]" );
    slotUpdateLoadColorRange();

    QHBoxLayout *loadColorsLayout = new QHBoxLayout();
    loadColorsLayout->setSpacing( 6 );
    loadColorsLayout->addWidget( loadColors_     );
    loadColorsLayout->addWidget( loadColorRange_ );

    loadIndices_ = new QCheckBox( "Contains Indices" );

    QVBoxLayout *loadStructureLayout = new QVBoxLayout();
    loadStructureLayout->setSpacing( 6 );
    loadStructureLayout->addWidget( loadNormals_     );
    loadStructureLayout->addWidget( loadPointsizes_  );
    loadStructureLayout->addItem  ( loadColorsLayout );
    loadStructureLayout->addWidget( loadIndices_     );

    QGroupBox *loadStructureGroupBox = new QGroupBox( "Internal File Structure" );
    loadStructureGroupBox->setLayout( loadStructureLayout );

    loadMakeDefaultButton_ = new QPushButton( "Make Default" );

    QVBoxLayout *loadLayout = new QVBoxLayout();
    loadLayout->setAlignment( Qt::AlignTop );
    loadLayout->setSpacing( 6 );
    loadLayout->addWidget( loadBinaryFile_        );
    loadLayout->addWidget( loadStructureGroupBox  );
    loadLayout->addWidget( loadMakeDefaultButton_ );

    loadOptions_ = new QWidget();
    loadOptions_->setLayout( loadLayout );

    // connect events to slots
    connect( loadBinaryFile_,        SIGNAL( stateChanged(int) ), this, SLOT( slotUpdateLoadColorRange()         ) );
    connect( loadColors_,            SIGNAL( stateChanged(int) ), this, SLOT( slotUpdateLoadColorRange()         ) );
    connect( loadMakeDefaultButton_, SIGNAL( clicked()         ), this, SLOT( slotLoadMakeDefaultButtonClicked() ) );

    // get Load Options from OpenFlipper (from disc)
    loadBinaryFile_->   setChecked     ( OpenFlipperSettings().value( "FilePTS/Load/BinaryFile", true ).toBool() );
    loadNormals_->      setChecked     ( OpenFlipperSettings().value( "FilePTS/Load/Normals",    true ).toBool() );
    loadPointsizes_->   setChecked     ( OpenFlipperSettings().value( "FilePTS/Load/Pointsizes", true ).toBool() );
    loadColors_->       setChecked     ( OpenFlipperSettings().value( "FilePTS/Load/Colors",     true ).toBool() );
    loadColorRange_->   setCurrentIndex( OpenFlipperSettings().value( "FilePTS/Load/ColorRange",    0 ).toInt()  );
    loadIndices_->      setChecked     ( OpenFlipperSettings().value( "FilePTS/Load/Indices",    true ).toBool() );
  }

  return loadOptions_;
}


//----------------------------------------------------------------


QWidget *FilePTSPlugin::saveOptionsWidget( QString _currentFilter )
{
  if( saveOptions_ == 0 )
  {
    // create new widget (including Save Options and buttons)

    saveBinaryFile_ = new QCheckBox( "Save as Binary File" );

    saveNormals_    = new QCheckBox( "Save Normals"        );
    savePointsizes_ = new QCheckBox( "Save Pointsizes"     );
    saveColors_     = new QCheckBox( "Save Colors"         );

    saveColorRange_ = new QComboBox();
    saveColorRange_->addItem( "[0..1]"   );
    saveColorRange_->addItem( "[0..255]" );
    slotUpdateSaveColorRange();

    QHBoxLayout *saveColorsLayout = new QHBoxLayout();
    saveColorsLayout->setSpacing( 6 );
    saveColorsLayout->addWidget( saveColors_     );
    saveColorsLayout->addWidget( saveColorRange_ );

    saveIndices_ = new QCheckBox( "Save Indices" );

    QVBoxLayout *saveStructureLayout = new QVBoxLayout();
    saveStructureLayout->setSpacing( 6 );
    saveStructureLayout->addWidget( saveNormals_     );
    saveStructureLayout->addWidget( savePointsizes_  );
    saveStructureLayout->addItem  ( saveColorsLayout );
    saveStructureLayout->addWidget( saveIndices_     );

    QGroupBox *saveStructureGroupBox = new QGroupBox( "Internal File Structure" );
    saveStructureGroupBox->setLayout( saveStructureLayout );

    saveMakeDefaultButton_ = new QPushButton( "Make Default" );

    QVBoxLayout *saveLayout = new QVBoxLayout();
    saveLayout->setAlignment( Qt::AlignTop );
    saveLayout->setSpacing( 6 );
    saveLayout->addWidget( saveBinaryFile_        );
    saveLayout->addWidget( saveStructureGroupBox  );
    saveLayout->addWidget( saveMakeDefaultButton_ );

    saveOptions_ = new QWidget();
    saveOptions_->setLayout( saveLayout );

    // connect events to slots
    connect( saveBinaryFile_,        SIGNAL( stateChanged(int) ), this, SLOT( slotUpdateSaveColorRange()         ) );
    connect( saveColors_,            SIGNAL( stateChanged(int) ), this, SLOT( slotUpdateSaveColorRange()         ) );
    connect( saveMakeDefaultButton_, SIGNAL( clicked()         ), this, SLOT( slotSaveMakeDefaultButtonClicked() ) );

    // get Save Options from OpenFlipper (from disc)
    saveBinaryFile_->setChecked     ( OpenFlipperSettings().value( "FilePTS/Save/BinaryFile", true ).toBool() );
    saveNormals_->   setChecked     ( OpenFlipperSettings().value( "FilePTS/Save/Normals",    true ).toBool() );
    savePointsizes_->setChecked     ( OpenFlipperSettings().value( "FilePTS/Save/Pointsizes", true ).toBool() );
    saveColors_->    setChecked     ( OpenFlipperSettings().value( "FilePTS/Save/Colors",     true ).toBool() );
    saveColorRange_->setCurrentIndex( OpenFlipperSettings().value( "FilePTS/Save/ColorRange",    0 ).toInt()  );
    saveIndices_->   setChecked     ( OpenFlipperSettings().value( "FilePTS/Save/Indices",    true ).toBool() );
  }

  return saveOptions_;
}


//----------------------------------------------------------------


void FilePTSPlugin::slotUpdateLoadColorRange()
{
  loadColorRange_->setEnabled( loadColors_->isChecked() && !loadBinaryFile_->isChecked() );
}


//----------------------------------------------------------------


void FilePTSPlugin::slotUpdateSaveColorRange()
{
  saveColorRange_->setEnabled( saveColors_->isChecked() && !saveBinaryFile_->isChecked() );
}


//----------------------------------------------------------------


void FilePTSPlugin::slotLoadMakeDefaultButtonClicked()
{
  // pass our Load Options to OpenFlipper (to disc)
  OpenFlipperSettings().setValue( "FilePTS/Load/BinaryFile", loadBinaryFile_->isChecked()    );
  OpenFlipperSettings().setValue( "FilePTS/Load/Normals",    loadNormals_->   isChecked()    );
  OpenFlipperSettings().setValue( "FilePTS/Load/Pointsizes", loadPointsizes_->isChecked()    );
  OpenFlipperSettings().setValue( "FilePTS/Load/Colors",     loadColors_->    isChecked()    );
  OpenFlipperSettings().setValue( "FilePTS/Load/ColorRange", loadColorRange_->currentIndex() );
  OpenFlipperSettings().setValue( "FilePTS/Load/Indices",    loadIndices_->   isChecked()    );

//  OpenFlipperSettings().setValue( "Core/File/UseLoadDefaults", true );
}


//----------------------------------------------------------------


void FilePTSPlugin::slotSaveMakeDefaultButtonClicked()
{
  // pass our Save Options to OpenFlipper (to disc)
  OpenFlipperSettings().setValue( "FilePTS/Save/BinaryFile", saveBinaryFile_->isChecked()    );
  OpenFlipperSettings().setValue( "FilePTS/Save/Normals",    saveNormals_->   isChecked()    );
  OpenFlipperSettings().setValue( "FilePTS/Save/Pointsizes", savePointsizes_->isChecked()    );
  OpenFlipperSettings().setValue( "FilePTS/Save/Colors",     saveColors_->    isChecked()    );
  OpenFlipperSettings().setValue( "FilePTS/Save/ColorRange", saveColorRange_->currentIndex() );
  OpenFlipperSettings().setValue( "FilePTS/Save/Indices",    saveIndices_->   isChecked()    );
}


//================================================================

Q_EXPORT_PLUGIN2( fileptsplugin, FilePTSPlugin );
