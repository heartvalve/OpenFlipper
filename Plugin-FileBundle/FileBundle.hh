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
//  CLASS FileBundlePlugin
//
//    This class is the base class for loading and saving
//    (reading/writing) SplatCloud objects from/to bundler files
//    on disc.
//
//================================================================


#ifndef FILEBUNDLEPLUGIN_HH
#define FILEBUNDLEPLUGIN_HH


//== INCLUDES ====================================================


#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/RPCInterface.hh>

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/SplatCloud/SplatCloud.hh>

#include <Plugin-TypeSplatCloud/TypeSplatCloud.hh>


//== CLASS DEFINITION ============================================


class FileBundlePlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface, LoggingInterface, ScriptInterface, RPCInterface
{
  Q_OBJECT
  Q_INTERFACES( FileInterface     )
  Q_INTERFACES( LoadSaveInterface )
  Q_INTERFACES( LoggingInterface  )
  Q_INTERFACES( BaseInterface     )
  Q_INTERFACES( ScriptInterface   )
  Q_INTERFACES( RPCInterface      )

signals:

  // -- File Interface --
  void openedFile( int _objectId );

  // -- LoadSave Interface --
  void addEmptyObject( DataType _type, int &_objectId );
  void deleteObject ( int _objectId );
  void updatedObject( int _objectId, const UpdateType &_type );

  //-- Logging Interface --
  void log(                QString _message );
  void log( Logtype _type, QString _message );

private slots:

  // -- Base Interface --
  void noguiSupported() { }

public:

  //-- Base Interface --
  QString name()         { return QString(     "FileBundle"                          ); }
  QString description( ) { return QString( tr( "Load/Save SplatCloud Bundler format files" ) ); }

  // -- File Interface --
  DataType supportedType() { return DATA_SPLATCLOUD; }

  // -- File Interface --
  QString getSaveFilters() { return QString( tr( "Bundler SplatCloud files ( *.out )" ) ); }
  QString getLoadFilters() { return QString( tr( "Bundler SplatCloud files ( *.out )" ) ); }
  QWidget *saveOptionsWidget( QString /*_currentFilter*/ );
  QWidget *loadOptionsWidget( QString /*_currentFilter*/ );

public slots:

  // -- Base Interface --
  QString version() { return QString( "1.0" ); }

  // -- File Interface --
  int  loadObject(                QString _filename );
  bool saveObject( int _objectId, QString _filename );

private:

  // add/remove multiple objects
  bool addEmptyObjects( unsigned int _num, const DataType &_datatype, std::vector<int> &_objectIDs );
  void deleteObjects( std::vector<int> &_objectIDs );

  // read image list file from disc
  bool readImagelistFile( const char *_filename, std::vector<std::string> &_imagePaths ) /*const*/;

  // read bundle file from disc to scenegraph node
  void readCameras( FILE *_file, const std::vector<int> &_cameraObjectIDs, SplatCloud_Cameras &_cameras    ) /*const*/;
  void readPoints ( FILE *_file, const std::vector<int> &_cameraObjectIDs, SplatCloud         &_splatCloud ) /*const*/;
  bool readBundleFile( const char *_filename, SplatCloud &_splatCloud ) /*const*/;

  // write bundle file from scenegraph node to disc
  bool writeBundleFile( const char *_filename, const SplatCloud &_splatCloud ) /*const*/;

  // add cameras to object
  int addCameras( const SplatCloud_Cameras &_cameras, int _splatCloudObjectId );
};


//================================================================


#endif // FILEBUNDLEPLUGIN_HH
