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


//== IMPLEMENTATION ==============================================


SplatCloudBackup::SplatCloudBackup( SplatCloudObject *_object, QString _name, UpdateType _type ) : 
	BaseBackup       ( _object, _name, _type ), 
	splatCloudObject_( _object ), 
	normalsBackup_   ( 0 ), 
	pointsizesBackup_( 0 ) 
{
//	std::cerr << "Create SplatCloudBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;

	const SplatCloud *splatCloud = splatCloudObject_->splatCloudNode()->splatCloud();

	if( splatCloud )
	{
		if( _type == updateType( "Normals" ) || _type == UPDATE_ALL )
			normalsBackup_ = new SplatCloud::NormalVector( splatCloud->normals() );

		else if ( _type == updateType( "Pointsizes" ) || _type == UPDATE_ALL )
			pointsizesBackup_ = new SplatCloud::PointsizeVector( splatCloud->pointsizes() );
	}
}


//----------------------------------------------------------------


SplatCloudBackup::~SplatCloudBackup()
{
//	std::cerr << "Delete SplatCloudBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;

	delete normalsBackup_;
	delete pointsizesBackup_;
}


//----------------------------------------------------------------


void SplatCloudBackup::apply()
{
	// first apply the baseBackup
	BaseBackup::apply();

//	std::cerr << "Apply SplatCloudBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;

	SplatCloud *splatCloud = splatCloudObject_->splatCloudNode()->splatCloud();

	if( splatCloud )
	{
		if( normalsBackup_ )
			splatCloud->normals() = *normalsBackup_;

		if( pointsizesBackup_ )
			splatCloud->pointsizes() = *pointsizesBackup_;
	}
}
