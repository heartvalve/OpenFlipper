
#include "SkeletonObjectInfoPlugin.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ObjectTypes/Skeleton/Skeleton.hh>
#include <ACG/Geometry/Algorithms.hh>
#include <limits>

//-----------------------------------------------------------------------------

void InfoSkeletonObjectPlugin::setDescriptions()
{
	/// get total number of joints for a given skeleton
  int jointCount(int _skeletonID);
  /// get total number of branches for a given skeleton
  int branchCount(int _skeletonID);

  /// get minumum bounding box point
  Vector boundingBoxMin(int _skeletonID);
  /// get maximum bounding box point
  Vector boundingBoxMax(int _skeletonID);
  /// get the size of the bounding box
  Vector boundingBoxSize(int _skeletonID);

  /// get the length of a bone
  double boneLength(int _skeletonID, int _jointID);

  /// get the minimal bone length
  double minBoneLength(int _skeletonID);
  /// get the maximal bone length
  double maxBoneLength(int _skeletonID);
  /// get the mean bone length
  double meanBoneLength(int _skeletonID);

	emit setSlotDescription("jointCount(int)",tr("get total number of joints for a given skeleton"),
			QStringList(tr("skeletontID")), QStringList(tr("id of a skeleton")));

	emit setSlotDescription("branchCount(int)",tr("get total number of branches for a given skeleton"),
			QStringList(tr("skeletonID")), QStringList(tr("id of a skeleton")));

	emit setSlotDescription("boundingBoxMin(int)",tr("get minimum point of the axis-aligned bounding box"),
			QStringList(tr("skeletonID")), QStringList(tr("id of a skeleton")));

	emit setSlotDescription("boundingBoxMax(int)",tr("get maximum point of the axis-aligned bounding box"),
			QStringList(tr("skeletonID")), QStringList(tr("id of a skeleton")));

	emit setSlotDescription("boundingBoxSize(int)",tr("get the size of the axis-aligned bounding box"),
			QStringList(tr("skeletonID")), QStringList(tr("id of a skeleton")));


	emit setSlotDescription("boneLength(int,int)",tr("Get the length of a bone from given joint to his parent"),
			QString(tr("SkeletonID,jointID")).split(","),
			QString(tr("id of the skeleton, id of the joint")).split(","));

	emit setSlotDescription("minBoneLength(int)",tr("Get the minimal bone length of a skeleton"),
			QStringList(tr("SkeletonID")), QStringList(tr("id of the skeleton")));

	emit setSlotDescription("maxBoneLength(int)",tr("Get the maximal bone length of a skeleton"),
			QStringList(tr("SkeletonID")), QStringList(tr("id of the skeleton")));

	emit setSlotDescription("meanBoneLength(int)",tr("Get the mean bone length of a skeleton"),
			QStringList(tr("SkeletonID")), QStringList(tr("id of the skeleton")));
}

//-----------------------------------------------------------------------------

int InfoSkeletonObjectPlugin::jointCount(int _skeletonID)
{
	Skeleton* skeleton = PluginFunctions::skeleton(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return -1;
	}

	return skeleton->jointCount();
}

//-----------------------------------------------------------------------------

int InfoSkeletonObjectPlugin::branchCount(int _skeletonID)
{
	Skeleton* skeleton = PluginFunctions::skeleton(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return -1;
	}

	int result = 0;
	for (Skeleton::Iterator iter = skeleton->begin(); iter != skeleton->end(); ++iter)
		if (iter->size() > 1)
			++result;

	return result;

}

//-----------------------------------------------------------------------------

Vector InfoSkeletonObjectPlugin::boundingBoxMin(int _skeletonID)
{
	SkeletonObject* skeleton = PluginFunctions::skeletonObject(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return -1;
	}

	Vector min,max;
	skeleton->skeletonNode()->boundingBox(min,max);
	return min;
}

//-----------------------------------------------------------------------------

Vector InfoSkeletonObjectPlugin::boundingBoxMax(int _skeletonID)
{
	SkeletonObject* skeleton = PluginFunctions::skeletonObject(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return -1;
	}

	Vector min,max;
	skeleton->skeletonNode()->boundingBox(min,max);
	return max;
}

//-----------------------------------------------------------------------------

Vector InfoSkeletonObjectPlugin::boundingBoxSize(int _skeletonID)
{
	SkeletonObject* skeleton = PluginFunctions::skeletonObject(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return -1;
	}

	Vector min,max;
	skeleton->skeletonNode()->boundingBox(min,max);
	return (max - min);
}

//-----------------------------------------------------------------------------
/** \brief get the length of a bone from a given joint to his parent
 *
 * @param _skeletonID id of a skeleton
 * @param _jointID id of the joint
 * @return bone length (from _jointID to the parent of _jointID)
 */
double InfoSkeletonObjectPlugin::boneLength(int _skeletonID, int _jointID)
{
	Skeleton* skeleton = PluginFunctions::skeleton(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return -1;
	}

	Skeleton::Joint* parent = skeleton->joint(_jointID)->parent();
	if (!parent)
	{
		emit log(LOGERR, tr ("Unable to get parent joint"));
		return -1;
	}

	unsigned int parentID = parent->id();

	//length of bones is defined in the reference pose and cannot be modified in any animation
	Skeleton::Pose* pose = skeleton->referencePose();

	return (pose->globalTranslation(_jointID) - pose->globalTranslation(parentID)).length();
}

//-----------------------------------------------------------------------------
double InfoSkeletonObjectPlugin::minBoneLength(int _skeletonID)
{
	Skeleton* skeleton = PluginFunctions::skeleton(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return -1;
	}


	double min = std::numeric_limits<double>::max();
	for (Skeleton::Iterator iter = skeleton->begin(); iter != skeleton->end(); ++iter)
	{
		double length = boneLength(_skeletonID,iter->id());
		if (length < min)
			min = length;
	}
	return min;
}

//-----------------------------------------------------------------------------

double InfoSkeletonObjectPlugin::maxBoneLength(int _skeletonID)
{
	Skeleton* skeleton = PluginFunctions::skeleton(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return -1;
	}


	double max = std::numeric_limits<double>::min();
	for (Skeleton::Iterator iter = skeleton->begin(); iter != skeleton->end(); ++iter)
	{
		double length = boneLength(_skeletonID,iter->id());
		if (length > max)
			max = length;
	}
	return max;
}

//-----------------------------------------------------------------------------

double InfoSkeletonObjectPlugin::meanBoneLength(int _skeletonID)
{
	Skeleton* skeleton = PluginFunctions::skeleton(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return -1;
	}


	double sum = 0;
	unsigned int count = 0;
	for (Skeleton::Iterator iter = skeleton->begin(); iter != skeleton->end(); ++iter)
	{
		sum += boneLength(_skeletonID,iter->id());
		++count;
	}

	return (sum/static_cast<double>(count));
}
