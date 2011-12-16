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
