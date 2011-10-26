//== INCLUDES ====================================================


#include "SplatCloudBackup.hh"


//== IMPLEMENTATION ==============================================


SplatCloudBackup::SplatCloudBackup( SplatCloudObject *_splatCloud, QString _name, UpdateType _type ) : 
	BaseBackup       ( _splatCloud, _name, _type ), 
	splatCloud_      ( _splatCloud ), 
	normalsBackup_   ( 0 ), 
	pointsizesBackup_( 0 ) 
{
//	std::cerr << "Create SplatCloudBackup with name:" << name_.toStdString() << "(id : " << id_ << ")" << std::endl;

	if( _type == updateType( "Normals" ) || _type == UPDATE_ALL )
		normalsBackup_ = new SplatCloudNode::NormalVector( _splatCloud->splatCloudNode()->normals() );
	else if ( _type == updateType( "Pointsizes" ) || _type == UPDATE_ALL )
		pointsizesBackup_ = new SplatCloudNode::PointsizeVector( _splatCloud->splatCloudNode()->pointsizes() );
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

	if( normalsBackup_ )
		splatCloud_->splatCloudNode()->normals() = *normalsBackup_;

	if( pointsizesBackup_ )
		splatCloud_->splatCloudNode()->pointsizes() = *pointsizesBackup_;
}
