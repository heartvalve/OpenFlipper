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

//  CLASS FilePTSPlugin
//
//    This class is the base class for loading and saving (reading/writing) SplatCloud objects from/to disc.
//
//================================================================


#ifndef FILEPTSPLUGIN_HH
#define FILEPTSPLUGIN_HH


//== INCLUDES ====================================================


#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/FileInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/SplatCloud/SplatCloud.hh>


//== CLASS DEFINITION ============================================


class FilePTSPlugin : public QObject, BaseInterface, FileInterface, LoadSaveInterface, LoggingInterface, ScriptInterface, INIInterface
{
	Q_OBJECT
	Q_INTERFACES( FileInterface     )
	Q_INTERFACES( LoadSaveInterface )
	Q_INTERFACES( LoggingInterface  )
	Q_INTERFACES( BaseInterface     )
	Q_INTERFACES( ScriptInterface   )
	Q_INTERFACES( INIInterface      )

signals:

	// -- File Interface --
	void openedFile( int _id );

	// -- LoadSave Interface --
	void addEmptyObject( DataType _type, int &_id );
	void updatedObject( int _identifier, const UpdateType _type);

	//-- Logging Interface --
	void log( Logtype _type, QString _message );
	void log( QString _message );

private slots:

  // -- Base Interface --
  void noguiSupported() { }

  // slots called when user wants to save the given Load/Save Options as default
  void slotLoadMakeDefaultButtonClicked();
  void slotSaveMakeDefaultButtonClicked();

public:

	// standard constructor/destructor
	FilePTSPlugin() : loadOptions_( 0 ), saveOptions_( 0 ) { }
	~FilePTSPlugin() { }

	//-- Base Interface --
	QString name() { return QString( "FilePTS" ); }
	QString description( ) { return QString( tr( "Load/Save SplatCloud format files" ) ); }

  // -- File Interface --
  DataType supportedType() { return DATA_SPLATCLOUD; }

	// -- File Interface --
	QString getSaveFilters() { return QString( tr( "SplatCloud format files ( *.pts *.bin )" ) ); }
	QString getLoadFilters() { return QString( tr( "SplatCloud format files ( *.pts *.bin )" ) ); }
	QWidget *saveOptionsWidget( QString /*_currentFilter*/ );
	QWidget *loadOptionsWidget( QString /*_currentFilter*/ );

public slots:

	// -- Base Interface --
	QString version() { return QString( "1.0" ); }

	// -- File Interface --
	int loadObject( QString _filename );
	bool saveObject( int _id, QString _filename );

private:

	// read binary/text file from disc to scenegraph node
	bool readBinaryFile( std::ifstream &_instream, SplatCloudNode *_splatCloudNode );
	bool readTextFile  ( std::ifstream &_instream, SplatCloudNode *_splatCloudNode );

	// normalize the overall size of the SplatCloud (move points to center of gravity and then shrink or expand the points)
	void normalizeSize( SplatCloudNode *_splatCloudNode );

	// write binary/text file from scenegraph node to disc
	bool writeBinaryFile( std::ofstream &_outstream, const SplatCloudNode *_splatCloudNode );
	bool writeTextFile  ( std::ofstream &_outstream, const SplatCloudNode *_splatCloudNode );

	// widgets
	QWidget* loadOptions_;
	QWidget* saveOptions_;

	// options in the loading menu
	QCheckBox* loadBinaryFile_;
	QCheckBox* loadNormals_;
	QCheckBox* loadPointsizes_;
	QComboBox* loadColorRange_;
	QCheckBox* loadNormalizeSize_;

	// options in the saving menu
	QCheckBox* saveBinaryFile_;
	QCheckBox* saveNormals_;
	QCheckBox* savePointsizes_;
	QComboBox* saveColorRange_;

	// buttons
	QPushButton* loadMakeDefaultButton_;
	QPushButton* saveMakeDefaultButton_;
};


//================================================================


#endif // FILEPTSPLUGIN_HH
