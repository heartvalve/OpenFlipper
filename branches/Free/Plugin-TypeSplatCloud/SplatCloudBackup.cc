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

//== INCLUDES ====================================================


#include "SplatCloudBackup.hh"


//== DEFINES =====================================================


//#define REPORT_BACKUP


//== IMPLEMENTATION ==============================================


SplatCloudBackup::SplatCloudBackup( SplatCloudObject *_object, QString _name, UpdateType _type ) : 
	BaseBackup       ( _object, _name, _type ), 
	splatCloudObject_( _object ), 
	pointsBackup_    ( 0 ), 
	normalsBackup_   ( 0 ), 
	pointsizesBackup_( 0 ), 
	colorsBackup_    ( 0 ), 
	indicesBackup_   ( 0 ), 
	selectionsBackup_( 0 ) 
{
#	ifdef REPORT_BACKUP
	std::cerr << "Create SplatCloudBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
#	endif

	const SplatCloud *splatCloud = splatCloudObject_->splatCloud();

	if( splatCloud )
	{
		if( _type.contains( UPDATE_GEOMETRY          ) ) pointsBackup_     = new SplatCloud::PointVector    ( splatCloud->points()     );
		if( _type.contains( updateType("Normals")    ) ) normalsBackup_    = new SplatCloud::NormalVector   ( splatCloud->normals()    );
		if( _type.contains( updateType("Pointsizes") ) ) pointsizesBackup_ = new SplatCloud::PointsizeVector( splatCloud->pointsizes() );
		if( _type.contains( UPDATE_COLOR             ) ) colorsBackup_     = new SplatCloud::ColorVector    ( splatCloud->colors()     );
		if( _type.contains( updateType("Indices")    ) ) indicesBackup_    = new SplatCloud::IndexVector    ( splatCloud->indices()    );
		if( _type.contains( UPDATE_SELECTION         ) ) selectionsBackup_ = new SplatCloud::SelectionVector( splatCloud->selections() );
	}
}


//----------------------------------------------------------------


SplatCloudBackup::~SplatCloudBackup()
{
#	ifdef REPORT_BACKUP
	std::cerr << "Delete SplatCloudBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
#	endif

	delete pointsBackup_;
	delete normalsBackup_;
	delete pointsizesBackup_;
	delete colorsBackup_;
	delete indicesBackup_;
	delete selectionsBackup_;
}


//----------------------------------------------------------------


void SplatCloudBackup::apply()
{
	// first apply the baseBackup
	BaseBackup::apply();

#	ifdef REPORT_BACKUP
	std::cerr << "Apply SplatCloudBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;
#	endif

	SplatCloud *splatCloud = splatCloudObject_->splatCloud();

	if( splatCloud )
	{
		if( pointsBackup_     ) splatCloud->points()     = *pointsBackup_;
		if( normalsBackup_    ) splatCloud->normals()    = *normalsBackup_;
		if( pointsizesBackup_ ) splatCloud->pointsizes() = *pointsizesBackup_;
		if( colorsBackup_     ) splatCloud->colors()     = *colorsBackup_;
		if( indicesBackup_    ) splatCloud->indices()    = *indicesBackup_;
		if( selectionsBackup_ ) splatCloud->selections() = *selectionsBackup_;
	}
}
