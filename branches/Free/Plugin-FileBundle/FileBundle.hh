//================================================================
//
//  CLASS FileBundlePlugin
//
//    This class is the base class for loading and saving (reading/writing) SplatCloud objects from/to disc.
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

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/SplatCloud/SplatCloud.hh>


//== CLASS DEFINITION ============================================


class FileBundlePlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface, LoggingInterface, ScriptInterface
{
  Q_OBJECT
  Q_INTERFACES( FileInterface     )
  Q_INTERFACES( LoadSaveInterface )
  Q_INTERFACES( LoggingInterface  )
  Q_INTERFACES( BaseInterface     )
  Q_INTERFACES( ScriptInterface   )

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

  typedef struct
  {
    double      f_, k1_, k2_;
    double      r_[3][3];
    double      t_[3];
    std::string imagePath_;
  } Camera;

  typedef std::vector<Camera> CameraVector;

  typedef std::vector<std::string> StringVector;

  // read imagelist file from disc
  bool readImagelistFile( const char *_filename, StringVector &_imagePaths ) /*const*/;

  // read bundle file from disc to scenegraph node
  void readCameras( FILE *_file, CameraVector &_cameras    ) /*const*/;
  void readPoints ( FILE *_file, SplatCloud   &_splatCloud ) /*const*/;
  bool readBundleFile( const char *_filename, SplatCloud &_splatCloud ) /*const*/;

  // write bundle file from scenegraph node to disc
  bool writeBundleFile( const char *_filename, const SplatCloud &_splatCloud ) /*const*/;
};


//================================================================


#endif // FILEBUNDLEPLUGIN_HH
