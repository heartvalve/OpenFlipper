//================================================================
//
//  CLASS SplatCloud - IMPLEMENTATION
//
//================================================================


//== INCLUDES ====================================================


#include "SplatCloud.hh"


//== IMPLEMENTATION ==============================================


void SplatCloud::normalizeSize()
{
	// check if there is nothing to do
	if( points_.size() == 0 )
		return;

	// calculate center-of-gravety
	float cogX = 0.0f;
	float cogY = 0.0f;
	float cogZ = 0.0f;
	PointVector::iterator pointIter;
	for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
	{
		const Point &p = *pointIter;

		cogX += p[0];
		cogY += p[1];
		cogZ += p[2];
	}

	float rcp_count = 1.0f / (float) points_.size();
	cogX *= rcp_count;
	cogY *= rcp_count;
	cogZ *= rcp_count;

	translation_ = Point( -cogX, -cogY, -cogZ );
	translate( translation_ );
	std::cout << "SplatCloud::normalizeSize(): translating points by: " << translation_ << std::endl;

	// calculate squared length
	float sqLength = 0.0f;
	for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
	{
		const Point &p = *pointIter;
		sqLength += p[0]*p[0] + p[1]*p[1] + p[2]*p[2];
	}

	float s = (float) sqrt( sqLength * rcp_count );

	if( s == 0.0f )
	  return;

	scaleFactor_ = 1.0f / s;
	scale( scaleFactor_ );
	std::cout << "SplatCloud::normalizeSize(): scaling points by factor: " << scaleFactor_ << std::endl;
}


//----------------------------------------------------------------


void SplatCloud::translate( const Point &_t )
{
	// translate points
	PointVector::iterator pointIter;
	for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
	{
		Point &p = *pointIter;

		p[0] += _t[0];
		p[1] += _t[1];
		p[2] += _t[2];
	}
}


//----------------------------------------------------------------


void SplatCloud::scale( float _s )
{
	// scale points (and pointsizes as well)
	if( pointsizes_.size() == points_.size() )
	{

		PointsizeVector::iterator pointsizeIter = pointsizes_.begin();
		PointVector::iterator     pointIter;
		for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter, ++pointsizeIter )
		{
			Point     &p  = *pointIter;
			Pointsize &ps = *pointsizeIter;

			p[0] *= _s;
			p[1] *= _s;
			p[2] *= _s;
			ps   *= _s; // scale pointsize as well
		}

	}
	else
	{

		PointVector::iterator pointIter;
		for( pointIter = points_.begin(); pointIter != points_.end(); ++pointIter )
		{
			Point &p = *pointIter;

			p[0] *= _s;
			p[1] *= _s;
			p[2] *= _s;
		}

	}	
}
