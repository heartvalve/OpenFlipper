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

#include <iostream>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <OpenMesh/Core/IO/IOManager.hh>


//== CONSTANTS ===================================================


// constants for colorrange drop down box
static const int COLORRANGE_NONE	= 0;
static const int COLORRANGE_0_1		= 1;
static const int COLORRANGE_0_255	= 2;


//== IMPLEMENTATION ==============================================


bool FilePTSPlugin::readBinaryFile( std::ifstream &_instream, SplatCloudNode *_splatCloudNode )
{
	// set default options
	bool loadNormals    = true;
	bool loadPointsizes = true;
	int  loadColorRange = 1;

	// get options
	if( OpenFlipper::Options::gui() && loadOptions_ )
	{
		loadNormals    = loadNormals_   ->isChecked();
		loadPointsizes = loadPointsizes_->isChecked();
		loadColorRange = loadColorRange_->currentIndex();
	}

	// read file type
	int fileType;
	_instream.read( (char *) &fileType, sizeof(int) );

	// check file type
	if( fileType != 1 && fileType != 2 )
		return false; // return failure

	// read number of points
	unsigned int numPoints;
	_instream.read( (char *) &numPoints, sizeof(unsigned int) );

	// read points
	{
		unsigned int i;
		for( i=0; i<numPoints; ++i )
		{
			float pnt[3];
			_instream.read( (char *) pnt, 3 * sizeof(float) );

			SplatCloudNode::Point point;
			point[0] = pnt[0];
			point[1] = pnt[1];
			point[2] = pnt[2];

			_splatCloudNode->addPoint( point );
		}
	}

	// read normals
	if( loadNormals )
	{
		unsigned int i;
		for( i=0; i<numPoints; ++i )
		{
			float nrm[3];
			_instream.read( (char *) nrm, 3 * sizeof(float) );

			SplatCloudNode::Normal normal;
			normal[0] = nrm[0];
			normal[1] = nrm[1];
			normal[2] = nrm[2];

			_splatCloudNode->addNormal( normal );
		}
	}

	// read pointsizes
	if( loadPointsizes )
	{
		unsigned int i;
		for( i=0; i<numPoints; ++i )
		{
			float ps;
			_instream.read( (char *) &ps, sizeof(float) );

			SplatCloudNode::Pointsize pointsize;
			pointsize = ps;

			_splatCloudNode->addPointsize( pointsize );
		}
	}

	// read colors
	if( loadColorRange != COLORRANGE_NONE )
	{
		unsigned int i;
		for( i=0; i<numPoints; ++i )
		{
			unsigned int col;
			_instream.read( (char *) &col, sizeof(unsigned int) );

			SplatCloudNode::Color color; // ignore colorrange
			color[0] = (unsigned char) ( (col >> 16) & 0xFF );
			color[1] = (unsigned char) ( (col >>  8) & 0xFF );
			color[2] = (unsigned char) ( (col      ) & 0xFF );

			_splatCloudNode->addColor( color );
		}
	}

	// return success
	return true;
}


//----------------------------------------------------------------


bool FilePTSPlugin::readTextFile( std::ifstream &_instream, SplatCloudNode *_splatCloudNode )
{
	// set default options
	bool loadNormals    = true;
	bool loadPointsizes = true;
	int  loadColorRange = 1;

	// get options
	if( OpenFlipper::Options::gui() && loadOptions_ )
	{
		loadNormals    = loadNormals_   ->isChecked();
		loadPointsizes = loadPointsizes_->isChecked();
		loadColorRange = loadColorRange_->currentIndex();
	}

	char buffer[4096];

	while( !_instream.eof() )
	{
		_instream.getline( buffer, 4096, '\n' );
		std::string       strbuffer( buffer );
		std::stringstream sstream( strbuffer );

		// read point
		{
			float pnt[3];
			sstream >> pnt[0];
			sstream >> pnt[1];
			sstream >> pnt[2];

			SplatCloudNode::Point point;
			point[0] = pnt[0];
			point[1] = pnt[1];
			point[2] = pnt[2];

			_splatCloudNode->addPoint( point );
		}

		// read color
		if( loadColorRange != COLORRANGE_NONE )
		{
			float col[3];
			sstream >> col[0];
			sstream >> col[1];
			sstream >> col[2];

			SplatCloudNode::Color color;

			if( loadColorRange == COLORRANGE_0_1 )
			{
				color[0] = (unsigned char) ( 255.999f * col[0] );
				color[1] = (unsigned char) ( 255.999f * col[1] );
				color[2] = (unsigned char) ( 255.999f * col[2] );
			}
			else // loadColorRange == COLORRANGE_0_255
			{
				color[0] = (unsigned char) col[0];
				color[1] = (unsigned char) col[1];
				color[2] = (unsigned char) col[2];
			}

			_splatCloudNode->addColor( color );
		}

		// read normal
		if( loadNormals )
		{
			float nrm[3];
			sstream >> nrm[0];
			sstream >> nrm[1];
			sstream >> nrm[2];

			SplatCloudNode::Normal normal;
			normal[0] = nrm[0];
			normal[1] = nrm[1];
			normal[2] = nrm[2];

			_splatCloudNode->addNormal( normal );
		}

		// read pointsize
		if( loadPointsizes )
		{
			float ps;
			sstream >> ps;

			SplatCloudNode::Pointsize pointsize;
			pointsize = ps;

			_splatCloudNode->addPointsize( pointsize );
		}
	}

	// return success
	return true;
}


//----------------------------------------------------------------


void FilePTSPlugin::normalizeSize( SplatCloudNode *_splatCloudNode )
{
	SplatCloudNode::PointVector     &points     = _splatCloudNode->points();
	SplatCloudNode::PointsizeVector &pointsizes = _splatCloudNode->pointsizes();

	// check if there is nothing to do
	if( points.size() == 0 )
		return;

	// calculate center-of-gravety

	float cogX = 0.0f;
	float cogY = 0.0f;
	float cogZ = 0.0f;

	SplatCloudNode::PointVector::iterator pointIter;
	for( pointIter = points.begin(); pointIter != points.end(); ++pointIter )
	{
		const SplatCloudNode::Point &p = *pointIter;

		cogX += p[0];
		cogY += p[1];
		cogZ += p[2];
	}

	float rcp_count = 1.0f / (float) points.size();

	cogX *= rcp_count;
	cogY *= rcp_count;
	cogZ *= rcp_count;

	// move points' center-of-gravety to origin and calculate squared length

	float sqLength = 0.0f;

	for( pointIter = points.begin(); pointIter != points.end(); ++pointIter )
	{
		SplatCloudNode::Point &p = *pointIter;

		p[0] -= cogX;
		p[1] -= cogY;
		p[2] -= cogZ;

		sqLength += p[0]*p[0] + p[1]*p[1] + p[2]*p[2];
	}

	// calculate scale = 1.0 / sqrt( sum( ||points[i]||^2 ) / points.size )

	float scale = (float) sqrt( sqLength * rcp_count );

	if( scale == 0.0f )
		return;

	scale = 1.0f / scale;

	// scale points (and pointsizes as well) by calculated scale

	if( pointsizes.size() == points.size() )
	{
		SplatCloudNode::PointsizeVector::iterator pointsizeIter = pointsizes.begin();
		for( pointIter = points.begin(); pointIter != points.end(); ++pointIter, ++pointsizeIter )
		{
			SplatCloudNode::Point     &p  = *pointIter;
			SplatCloudNode::Pointsize &ps = *pointsizeIter;

			p[0] *= scale;
			p[1] *= scale;
			p[2] *= scale;
			ps   *= scale; // scale pointsize as well
		}
	}
	else
	{
		for( pointIter = points.begin(); pointIter != points.end(); ++pointIter )
		{
			SplatCloudNode::Point &p = *pointIter;

			p[0] *= scale;
			p[1] *= scale;
			p[2] *= scale;
		}
	}
}


//----------------------------------------------------------------


bool FilePTSPlugin::writeBinaryFile( std::ofstream &_outstream, const SplatCloudNode *_splatCloudNode )
{
	// set default options
	bool saveNormals    = true;
	bool savePointsizes = true;
	int  saveColorRange = 1;

	// get options
	if( OpenFlipper::Options::gui() && saveOptions_ )
	{
		saveNormals    = saveNormals_   ->isChecked();
		savePointsizes = savePointsizes_->isChecked();
		saveColorRange = saveColorRange_->currentIndex();
	}

	// write file type
	int fileType = 1;
	_outstream.write( (char *) &fileType, sizeof(int) );

	// write number of points
	unsigned int numPoints = _splatCloudNode->numPoints();
	_outstream.write( (char *) &numPoints, sizeof(unsigned int) );

	// write points
	{
		unsigned int i;
		for( i=0; i<numPoints; ++i )
		{
			const SplatCloudNode::Point &point = _splatCloudNode->points()[ i ];

			float pnt[3];
			pnt[0] = point[0];
			pnt[1] = point[1];
			pnt[2] = point[2];

			_outstream.write( (char *) pnt, 3 * sizeof(float) );
		}
	}

	// write normals
	if( saveNormals )
	{
		unsigned int i;
		for( i=0; i<numPoints; ++i )
		{
			const SplatCloudNode::Normal &normal = _splatCloudNode->hasNormals() 
					? _splatCloudNode->normals()[ i ] 
					: _splatCloudNode->defaultNormal();

			float nrm[3];
			nrm[0] = normal[0];
			nrm[1] = normal[1];
			nrm[2] = normal[2];

			_outstream.write( (char *) nrm, 3 * sizeof(float) );
		}
	}

	// write pointsizes
	if( savePointsizes )
	{
		unsigned int i;
		for( i=0; i<numPoints; ++i )
		{
			const SplatCloudNode::Pointsize &pointsize = _splatCloudNode->hasPointsizes() 
				? _splatCloudNode->pointsizes()[ i ] 
				: _splatCloudNode->defaultPointsize();

			float ps;
			ps = pointsize;

			_outstream.write( (char *) &ps, sizeof(float) );
		}
	}

	// write colors
	if( saveColorRange != COLORRANGE_NONE )
	{
		unsigned int i;
		for( i=0; i<numPoints; ++i )
		{
			const SplatCloudNode::Color &color = _splatCloudNode->hasColors() 
				? _splatCloudNode->colors()[ i ] 
				: _splatCloudNode->defaultColor();

			unsigned int col; // ignore colorrange
			col = (0xFF << 24) | (color[0] << 16) | (color[1] << 8) | (color[2]);

			_outstream.write( (char *) &col, sizeof(unsigned int) );
		}
	}

	// return success
	return true;
}


//----------------------------------------------------------------


bool FilePTSPlugin::writeTextFile( std::ofstream &_outstream, const SplatCloudNode *_splatCloudNode )
{
	// set default options
	bool saveNormals    = true;
	bool savePointsizes = true;
	int  saveColorRange = 1;

	// get options
	if( OpenFlipper::Options::gui() && saveOptions_ )
	{
		saveNormals    = saveNormals_   ->isChecked();
		savePointsizes = savePointsizes_->isChecked();
		saveColorRange = saveColorRange_->currentIndex();
	}

	// for all points...
	unsigned int i, numPoints = _splatCloudNode->numPoints();
	for( i=0; i<numPoints; ++i )
	{
		// write point
		{
			const SplatCloudNode::Point &point = _splatCloudNode->points()[ i ];

			float pnt[3];
			pnt[0] = point[0];
			pnt[1] = point[1];
			pnt[2] = point[2];

			_outstream <<        pnt[0];
			_outstream << " " << pnt[1];
			_outstream << " " << pnt[2];
		}

		// write color
		if( saveColorRange != COLORRANGE_NONE )
		{
			const SplatCloudNode::Color &color = _splatCloudNode->hasColors() 
				? _splatCloudNode->colors()[ i ] 
				: _splatCloudNode->defaultColor();

			if( saveColorRange == COLORRANGE_0_1 )
			{
				static const float RCP255 = 1.0f / 255.0f;

				float col[3];
				col[0] = RCP255 * color[0];
				col[1] = RCP255 * color[1];
				col[2] = RCP255 * color[2];

				_outstream << " " << col[0];
				_outstream << " " << col[1];
				_outstream << " " << col[2];
			}
			else // saveColorRange == COLORRANGE_0_255
			{
				int col[3]; // use int, *not* unsigned char !
				col[0] = color[0];
				col[1] = color[1];
				col[2] = color[2];

				_outstream << " " << col[0];
				_outstream << " " << col[1];
				_outstream << " " << col[2];
			}
		}

		// write normal
		if( saveNormals )
		{
			const SplatCloudNode::Normal &normal = _splatCloudNode->hasNormals() 
					? _splatCloudNode->normals()[ i ] 
					: _splatCloudNode->defaultNormal();

			float nrm[3];
			nrm[0] = normal[0];
			nrm[1] = normal[1];
			nrm[2] = normal[2];

			_outstream << " " << nrm[0];
			_outstream << " " << nrm[1];
			_outstream << " " << nrm[2];
		}

		// write pointsize
		if( savePointsizes )
		{
			const SplatCloudNode::Pointsize &pointsize = _splatCloudNode->hasPointsizes() 
				? _splatCloudNode->pointsizes()[ i ] 
				: _splatCloudNode->defaultPointsize();

			float ps;
			ps = pointsize;

			_outstream << " " << ps;
		}

		_outstream << std::endl;
	}

	// return success
	return true;
}


//----------------------------------------------------------------


int FilePTSPlugin::loadObject( QString _filename )
{
	// set default options
	bool loadBinaryFile    = true;
	bool loadNormalizeSize = true;

	// get options
	if( OpenFlipper::Options::gui() && loadOptions_ )
	{
		loadBinaryFile    = loadBinaryFile_   ->isChecked();
		loadNormalizeSize = loadNormalizeSize_->isChecked();
	}

	// add a new, empty splatcloud object
	int id = -1;
	emit addEmptyObject( DATA_SPLATCLOUD, id );

	// get splatcloud-object by id
	SplatCloudObject *splatCloud = 0;
	if( !PluginFunctions::getObject( id, splatCloud ) )
		return 0; // failure, but empty object was added

	// check if splatcloud-object is okay
	if( !splatCloud )
		return 0; // failure, but empty object was added

	// get scenegraph splatcloud-node
	SplatCloudNode *splatCloudNode = splatCloud->splatCloudNode();

	// open file
	char *fn = QByteArray( _filename.toLatin1() ).data();
	std::ifstream instream( fn, loadBinaryFile ? (std::ios::in | std::ios::binary) : std::ios::in );

	// check if file was opened
	if( !instream.is_open() )
		return 0; // failure, but empty object was added

	// read file
	if( loadBinaryFile )
	{
		if( !readBinaryFile( instream, splatCloudNode ) )
			return 0; // failure, but empty object was added
	}
	else
	{
		if( !readTextFile( instream, splatCloudNode ) )
			return 0; // failure, but empty object was added
	}

	// check if something went wrong
	if( instream.bad() )
		return 0; // failure, but empty object was added

	// close file
	instream.close();

	// normalize size
	if( loadNormalizeSize )
	{
		normalizeSize( splatCloudNode );
	}

	// remember filename
	splatCloud->setFromFileName( _filename );

	// emit signals that the object has to be updated and that a file was opened
	emit updatedObject( splatCloud->id(), UPDATE_ALL );
	emit openedFile( splatCloud->id() );

  // get drawmodes
  ACG::SceneGraph::DrawModes::DrawMode splatsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode( "Splats" );
  ACG::SceneGraph::DrawModes::DrawMode dotsDrawMode   = ACG::SceneGraph::DrawModes::getDrawMode( "Dots"   );
  ACG::SceneGraph::DrawModes::DrawMode pointsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode( "Points" );

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

	// return the id of the new splatcloud object
	return id;
}


//----------------------------------------------------------------


bool FilePTSPlugin::saveObject( int _id, QString _filename )
{
	// set default options
	bool saveBinaryFile = true;

	// get options
	if( OpenFlipper::Options::gui() && saveOptions_ )
	{
		saveBinaryFile = saveBinaryFile_->isChecked();
	}

	// get splatcloud-object by id
	SplatCloudObject *splatCloud = 0;
	if( !PluginFunctions::getObject( _id, splatCloud ) )
		return false; // failure

	// check if splatcloud-object is okay
	if( !splatCloud )
		return false; // failure

	// get scenegraph splatcloud-node
	const SplatCloudNode *splatCloudNode = splatCloud->splatCloudNode();

	// open file
	char *fn = QByteArray( _filename.toLatin1() ).data();
	std::ofstream outstream( fn, saveBinaryFile ? (std::ios::out | std::ios::binary) : std::ios::out );

	// check if file was opened
	if( !outstream.is_open() )
		return false; // failure

	// write file
	if( saveBinaryFile )
	{
		if( !writeBinaryFile( outstream, splatCloudNode ) )
			return false; // failure
	}
	else
	{
		if( !writeTextFile( outstream, splatCloudNode ) )
			return false; // failure
	}

	// check if something went wrong
	if( outstream.bad() )
		return false;

	// close file
	outstream.close();

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

		loadColorRange_ = new QComboBox();
		loadColorRange_->addItem( tr( "Contains No Colors"       ) );
		loadColorRange_->addItem( tr( "Contains Colors [0..1]"   ) );
		loadColorRange_->addItem( tr( "Contains Colors [0..255]" ) );

		loadNormalizeSize_ = new QCheckBox( "Normalize Size" );

		loadMakeDefaultButton_ = new QPushButton( "Make Default" );

		QVBoxLayout *layout = new QVBoxLayout();
		layout->setAlignment( Qt::AlignTop );
		layout->addWidget( loadBinaryFile_        );
		layout->addWidget( loadNormals_           );
		layout->addWidget( loadPointsizes_        );
		layout->addWidget( loadColorRange_        );
		layout->addWidget( loadNormalizeSize_     );
		layout->addWidget( loadMakeDefaultButton_ );

		loadOptions_ = new QWidget();
		loadOptions_->setLayout( layout );

		// connect events to slots
		connect( loadMakeDefaultButton_, SIGNAL( clicked() ), this, SLOT( slotLoadMakeDefaultButtonClicked() ) );

		// get Load Options from OpenFlipper (from disc)
		loadBinaryFile_->setChecked     ( OpenFlipperSettings().value( "FilePTS/Load/BinaryFile",    true ).toBool() );
		loadNormals_->setChecked        ( OpenFlipperSettings().value( "FilePTS/Load/Normals",       true ).toBool() );
		loadPointsizes_->setChecked     ( OpenFlipperSettings().value( "FilePTS/Load/Pointsizes",    true ).toBool() );
		loadColorRange_->setCurrentIndex( OpenFlipperSettings().value( "FilePTS/Load/ColorRange",    1    ).toInt()  );
		loadNormalizeSize_->setChecked  ( OpenFlipperSettings().value( "FilePTS/Load/NormalizeSize", true ).toBool() );
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

		saveColorRange_ = new QComboBox();
		saveColorRange_->addItem( tr( "Save No Colors"       ) );
		saveColorRange_->addItem( tr( "Save Colors [0..1]"   ) );
		saveColorRange_->addItem( tr( "Save Colors [0..255]" ) );

		saveMakeDefaultButton_ = new QPushButton( "Make Default" );

		QVBoxLayout *layout = new QVBoxLayout();
		layout->setAlignment( Qt::AlignTop );
		layout->addWidget( saveBinaryFile_        );
		layout->addWidget( saveNormals_           );
		layout->addWidget( savePointsizes_        );
		layout->addWidget( saveColorRange_        );
		layout->addWidget( saveMakeDefaultButton_ );

		saveOptions_ = new QWidget();
		saveOptions_->setLayout( layout );

		// connect events to slots
		connect( saveMakeDefaultButton_, SIGNAL( clicked() ), this, SLOT( slotSaveMakeDefaultButtonClicked() ) );

		// get Save Options from OpenFlipper (from disc)
		saveBinaryFile_->setChecked     ( OpenFlipperSettings().value( "FilePTS/Save/BinaryFile", true ).toBool() );
		saveNormals_->setChecked        ( OpenFlipperSettings().value( "FilePTS/Save/Normals",    true ).toBool() );
		savePointsizes_->setChecked     ( OpenFlipperSettings().value( "FilePTS/Save/Pointsizes", true ).toBool() );
		saveColorRange_->setCurrentIndex( OpenFlipperSettings().value( "FilePTS/Save/ColorRange", 1    ).toInt()  );
	}

	return saveOptions_;
}


//----------------------------------------------------------------


void FilePTSPlugin::slotLoadMakeDefaultButtonClicked()
{
	// pass our Load Options to OpenFlipper (to disc)
	OpenFlipperSettings().setValue( "FilePTS/Load/BinaryFile",    loadBinaryFile_   ->isChecked()    );
	OpenFlipperSettings().setValue( "FilePTS/Load/Normals",       loadNormals_      ->isChecked()    );
	OpenFlipperSettings().setValue( "FilePTS/Load/Pointsizes",    loadPointsizes_   ->isChecked()    );
	OpenFlipperSettings().setValue( "FilePTS/Load/ColorRange",    loadColorRange_   ->currentIndex() );
	OpenFlipperSettings().setValue( "FilePTS/Load/NormalizeSize", loadNormalizeSize_->isChecked()    );

//	OpenFlipperSettings().setValue( "Core/File/UseLoadDefaults", true );
}


//----------------------------------------------------------------


void FilePTSPlugin::slotSaveMakeDefaultButtonClicked()
{
	// pass our Save Options to OpenFlipper (to disc)
	OpenFlipperSettings().setValue( "FilePTS/Save/BinaryFile", saveBinaryFile_->isChecked()    );
	OpenFlipperSettings().setValue( "FilePTS/Save/Normals",    saveNormals_   ->isChecked()    );
	OpenFlipperSettings().setValue( "FilePTS/Save/Pointsizes", savePointsizes_->isChecked()    );
	OpenFlipperSettings().setValue( "FilePTS/Save/ColorRange", saveColorRange_->currentIndex() );
}


//================================================================


Q_EXPORT_PLUGIN2( fileptsplugin , FilePTSPlugin );
