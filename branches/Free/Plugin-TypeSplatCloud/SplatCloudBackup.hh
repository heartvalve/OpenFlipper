#ifndef SPLATCLOUDBACKUP_HH
#define SPLATCLOUDBACKUP_HH


//== INCLUDES ====================================================


#include <OpenFlipper/common/BaseBackup.hh>

#include <ObjectTypes/SplatCloud/SplatCloud.hh>


//== CLASS DEFINITION ============================================


/**
 * @brief Class that encapsulates a backup
 */
class SplatCloudBackup : public BaseBackup
{
public:

	SplatCloudBackup( SplatCloudObject *_splatCloud, QString _name, UpdateType _type );
	~SplatCloudBackup();

	//-- BaseBackup --
	void apply();

private:

	SplatCloudObject                *splatCloud_;
	SplatCloudNode::NormalVector    *normalsBackup_;
	SplatCloudNode::PointsizeVector *pointsizesBackup_;
};


//================================================================


#endif // SPLATCLOUDBACKUP_HH
