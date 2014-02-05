/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
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
*   $Date$                    *
*                                                                            *
\*===========================================================================*/
#include "SkeletonObjectInfoPlugin.hh"
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ObjectTypes/Skeleton/Skeleton.hh>
#include <ACG/Geometry/Algorithms.hh>
#include <limits>

//-----------------------------------------------------------------------------
/** \brief set the descriptions for scripting slots
 *
 */
void InfoSkeletonObjectPlugin::setDescriptions()
{
	emit setSlotDescription("jointCount(int)",tr("get total number of joints for a given skeleton"),
			QStringList(tr("skeletontID")), QStringList(tr("id of a skeleton")));

	emit setSlotDescription("branchCount(int)",tr("get total number of branches for a given skeleton"),
			QStringList(tr("skeletonID")), QStringList(tr("id of a skeleton")));

	emit setSlotDescription("leafCount(int)",tr("get total number of leaves for a given skeleton"),
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

/** \brief get total number of vertices for a given object
 *
 * @param _skeletonID skeleton id
 * @return number of joints or -1 if an error occured
 */
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

/** \brief get total number of vertices for a given object
 *
 * @param _skeletonID skeleton id
 * @return number of branches (joints with multiple childs) or -1 if an error occured
 */
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
//safsdf
}

//-----------------------------------------------------------------------------

/** \brief get total number of vertices for a given object
 *
 * @param _skeletonID skeleton id
 * @return number of leaves or -1 if an error occured
 */
int InfoSkeletonObjectPlugin::leafCount(int _skeletonID)
{
	Skeleton* skeleton = PluginFunctions::skeleton(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return -1;
	}

	int result = 0;
	for (Skeleton::Iterator iter = skeleton->begin(); iter != skeleton->end(); ++iter)
		if (iter->size() == 0)
			++result;

	return result;
}

//-----------------------------------------------------------------------------

/** \brief get total number of vertices for a given object
 *
 * @param _skeletonID skeleton id
 * @return minimum point of the bounding box
 */
Vector InfoSkeletonObjectPlugin::boundingBoxMin(int _skeletonID)
{
	SkeletonObject* skeleton = PluginFunctions::skeletonObject(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return Vector();
	}

	Vector min,max;
	skeleton->skeletonNode()->boundingBox(min,max);
	return min;
}

//-----------------------------------------------------------------------------

/** \brief get total number of vertices for a given object
 *
 * @param _skeletonID skeleton id
 * @return maximum point of the bounding box
 */
Vector InfoSkeletonObjectPlugin::boundingBoxMax(int _skeletonID)
{
	SkeletonObject* skeleton = PluginFunctions::skeletonObject(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return Vector();
	}

	Vector min,max;
	skeleton->skeletonNode()->boundingBox(min,max);
	return max;
}

//-----------------------------------------------------------------------------

/** \brief get total number of vertices for a given object
 *
 * @param _skeletonID skeleton id
 * @return size of the bounding box
 */
Vector InfoSkeletonObjectPlugin::boundingBoxSize(int _skeletonID)
{
	SkeletonObject* skeleton = PluginFunctions::skeletonObject(_skeletonID);
	if ( !skeleton )
	{
		emit log(LOGERR, tr("Unable to get skeleton"));
		return Vector();
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
	}

	Skeleton::Joint* parent = skeleton->joint(_jointID)->parent();
	if (!parent)
	{
		emit log(LOGERR, tr ("Unable to get parent joint"));
	}

	unsigned int parentID = parent->id();

	//length of bones is defined in the reference pose and cannot be modified in any animation
	Skeleton::Pose* pose = skeleton->referencePose();

	return (pose->globalTranslation(_jointID) - pose->globalTranslation(parentID)).length();
}

//-----------------------------------------------------------------------------

/** \brief get total number of vertices for a given object
 *
 * @param _skeletonID skeleton id
 * @return minimal bone length or -1 if an error occured
 */
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
		if (!iter->isRoot())
		{
			double length = boneLength(_skeletonID,iter->id());
			if (length < min)
				min = length;
		}
	}
	return min;
}

//-----------------------------------------------------------------------------

/** \brief get total number of vertices for a given object
 *
 * @param _skeletonID skeleton id
 * @return maximal bone length or -1 if an error occured
 */
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
		if (!iter->isRoot())
		{
			double length = boneLength(_skeletonID,iter->id());
			if (length > max)
				max = length;
		}
	}
	return max;
}

//-----------------------------------------------------------------------------

/** \brief get total number of vertices for a given object
 *
 * @param _skeletonID skeleton id
 * @return mean bone length or -1 if an error occured
 */
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
		if (!iter->isRoot())
		{
			sum += boneLength(_skeletonID,iter->id());
			++count;
		}
	}

	return (sum/static_cast<double>(count));
}
