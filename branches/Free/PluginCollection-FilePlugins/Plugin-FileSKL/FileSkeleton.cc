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
*   $Revision: 10274 $                                                       *
*   $LastChangedBy: wilden $                                                 *
*   $Date: 2010-11-24 17:49:09 +0100 (Wed, 24 Nov 2010) $                    *
*                                                                            *
\*===========================================================================*/

#include <QtGui>
#include <QFileInfo>

#include "FileSkeleton.hh"

#include <iostream>
#include <fstream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

void FileSKLPlugin::initializePlugin() {
}

QString FileSKLPlugin::getLoadFilters() {
  return QString( "Skeleton files ( *.skl )" );
};

QString FileSKLPlugin::getSaveFilters() {
  return QString( "Skeleton files ( *.skl )" );
};

DataType  FileSKLPlugin::supportedType() {
  DataType type = DATA_SKELETON;
  return type;
}

template<typename Skeleton>
bool FileSKLPlugin::LoadSkeleton(Skeleton *_pSkeleton, QString _filename)
{
  typedef JointT<typename Skeleton::Point> Joint;
  typedef  PoseT<typename Skeleton::Point> Pose;
  typedef ACG::Matrix4x4T<typename Skeleton::Scalar> Matrix;

  unsigned int nJoints = 0;

  std::ifstream in(_filename.toStdString().c_str(), std::ofstream::in);

  // read number of joints
  in >> nJoints;

  Pose *ref = _pSkeleton->referencePose();
  // remember parent joints
  std::map<unsigned int, Joint*> parents;

  std::map<unsigned int, unsigned int> jointMap;

  for(unsigned int i = 0; i < nJoints; ++i)
  {
    // id not stored in skeleton; the read order is increasing for joints
    unsigned int id;

    // read joint id
    in >> id;

    // read its matrix
    Matrix mat;
    mat.identity();
    for(int y = 0; y < 3; ++y)
      for(int x = 0; x < 3; ++x)
        in >> mat(y, x);
    for(int y = 0; y < 3; ++y)
      in >> mat(y, 3);

    // try to find the parent joint
    Joint *pParent = 0;
    if(parents.find(id) != parents.end())
      pParent = parents[id];

    // setup the new joint
    Joint *pJoint = new Joint(pParent);
    _pSkeleton->addJoint(pParent, pJoint);

    jointMap[ id ] = pJoint->id();
    
    // save the joints position
    ref->setGlobalMatrix(jointMap[ id ], mat);

    // how many child nodes
    unsigned int nChildren;
    in >> nChildren;

    for(unsigned int j = 0; j < nChildren; ++j)
    {
      // remember to attach this child joint once its being load
      unsigned int idChild;
      in >> idChild;

      parents[idChild] = pJoint;
    }
  }
  
  unsigned int num_anim = 0;
  while(in.good()) {

      num_anim++;

      // Test whether animation name is provided
      std::string identifier;
      in >> identifier;
      std::string animationName = (QString("Animation") + QString::number(num_anim)).toStdString();

	  //read animation
      unsigned int frameCount = 0;
	  

      if(identifier == "animation") {
          std::getline(in, animationName);
          // Trim string
          animationName = QString(animationName.c_str()).trimmed().toStdString();

		  in >> frameCount;
      } else {

		  std::istringstream tmp(identifier);
		  tmp >> frameCount;

      }

      if ( frameCount > 0 ){

        FrameAnimationT<ACG::Vec3d>* animation = new FrameAnimationT<ACG::Vec3d>(_pSkeleton, frameCount);
        AnimationHandle animHandle = _pSkeleton->addAnimation(animationName, animation);

        for (unsigned int k = 0; k < frameCount; k++){

          animHandle.setFrame(k);
          typename Skeleton::Pose* pose = _pSkeleton->pose( animHandle );

          for(unsigned int i = 0; i < nJoints; ++i)
          {
            unsigned int id; // id not stored in skeleton; the read order is increasing for joints

            // read joint id
            in >> id;

            // read its matrix
            Matrix mat;
            mat.identity();
            for(int y = 0; y < 3; ++y)
              for(int x = 0; x < 3; ++x)
                in >> mat(y, x);
            for(int y = 0; y < 3; ++y)
              in >> mat(y, 3);

            //set matrix
            pose->setGlobalMatrix(jointMap[ id ], mat);
          }
        }
      }
  }

  in.close();

  return true;
}

int FileSKLPlugin::loadObject(QString _filename)
{
  int id = -1;
  emit addEmptyObject(DATA_SKELETON, id);
  
  BaseObjectData *obj(0);
  if(PluginFunctions::getObject(id, obj))
  {
    SkeletonObject* skel = PluginFunctions::skeletonObject(obj);

    LoadSkeleton(skel->skeleton(), _filename);

    //general stuff
    obj->source( PluginFunctions::objectCount() > 4 );
    obj->setFromFileName(_filename);
    obj->setName(obj->filename());
    emit updatedObject( obj->id(), UPDATE_ALL );
    emit openedFile( obj->id() );
    PluginFunctions::viewAll();
  } else {
    emit log(LOGERR,tr("Unable to add empty skeleton"));
  }
    

  return id;
};

template<typename Skeleton>
bool FileSKLPlugin::SaveSkeleton(Skeleton *_pSkeleton, QString _filename)
{
  typedef JointT<typename Skeleton::Point> Joint;
  typedef  PoseT<typename Skeleton::Point>  Pose;
  typedef ACG::Matrix4x4T<typename Skeleton::Scalar> Matrix;

  std::ofstream out(_filename.toStdString().c_str(), std::ofstream::out);

  // write the number of joints
  out << _pSkeleton->jointCount() << std::endl;

  Pose *ref = _pSkeleton->referencePose();
  // write all the joints
  for (typename Skeleton::Iterator it = _pSkeleton->begin(); it != _pSkeleton->end(); ++it ){
  
    unsigned int i = (*it)->id();
    Joint *pJoint   = *it;

    // write this joints id
    out << pJoint->id() << " ";

    // write its position
    const Matrix &mat = ref->globalMatrix(i);
    for(int y = 0; y < 3; ++y)
      for(int x = 0; x < 3; ++x)
        out << mat(y, x) << " ";
    for(int y = 0; y < 3; ++y)
      out << mat(y, 3) << " ";

    // write this joints number of children
    out << pJoint->size() << " ";

    // write the children
    for(unsigned int j = 0; j < pJoint->size(); ++j)
      out << pJoint->child(j)->id() << " ";

    out << std::endl;
  }
  
  // now store animations
  AnimationT<ACG::Vec3d>* animation = 0;

  for (unsigned int i = 0; i < _pSkeleton->animationCount(); i++) {

      animation = _pSkeleton->animation(AnimationHandle(i, 0));

        if (animation != 0) {

            std::string name = animation->name();

            out << "animation " << name << std::endl;

            out << animation->frameCount() << std::endl;

            AnimationHandle animHandle = _pSkeleton->animationHandle(name);

            // every frame of that animation
            for (unsigned int k = 0; k < animation->frameCount(); ++k) {

                animHandle.setFrame(k);
                typename Skeleton::Pose* pose = _pSkeleton->pose(animHandle);

                for (typename Skeleton::Iterator it = _pSkeleton->begin(); it != _pSkeleton->end(); ++it) {

                    unsigned int i = (*it)->id();
                    Joint *pJoint = *it;

                    // write this joints id
                    out << pJoint->id() << " ";

                    // write its position
                    const Matrix &mat = pose->globalMatrix(i);
                    for (int y = 0; y < 3; ++y)
                        for (int x = 0; x < 3; ++x)
                            out << mat(y, x) << " ";
                    for (int y = 0; y < 3; ++y)
                        out << mat(y, 3) << " ";
                }

                out << std::endl;
            }
        } else {
            out << "0" << std::endl;
        }
    }

  out.close();
  return !out.fail();
}

bool FileSKLPlugin::saveObject(int _id, QString _filename)
{
	BaseObjectData *obj(0);
	if(PluginFunctions::getObject(_id, obj))
	{
		SkeletonObject *skel = PluginFunctions::skeletonObject(obj);
		if(skel)
		{
		  obj->setFromFileName(_filename);
		  obj->setName(obj->filename());
			SaveSkeleton(skel->skeleton(), _filename);
		}
	}

	return true;
}

void FileSKLPlugin::loadIniFile( INIFile& _ini ,int _id ) {
  BaseObjectData* baseObject;
  if ( !PluginFunctions::getObject(_id,baseObject) ) {
    emit log(LOGERR,"Cannot find object for id " + QString::number(_id) + " in saveFile" );
    return;
  }

  if ( baseObject->materialNode() != 0 ) {
     ACG::Vec4f col(0.0,0.0,0.0,0.0);

    if ( _ini.get_entryVecf( col, baseObject->name() , "BaseColor" ) )
      baseObject->materialNode()->set_base_color(col);
  }

}

void FileSKLPlugin::saveIniFile( INIFile& _ini ,int _id) {
  BaseObjectData* baseObject;
  if ( !PluginFunctions::getObject(_id,baseObject) ) {
    emit log(LOGERR,"Cannot find object for id " + QString::number(_id) + " in saveFile" );
    return;
  }


  if ( baseObject->materialNode() != 0 ) {
      _ini.add_entryVec( baseObject->name() ,
                         "BaseColor" ,
                         baseObject->materialNode()->base_color() );
  }
}


Q_EXPORT_PLUGIN2( filesklplugin , FileSKLPlugin );

