
#include <QtGui>
#include <QFileInfo>

#include "FileSkeleton.hh"

#include <iostream>
#include <fstream>
#include <ACG/GL/GLState.hh>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

void FileSkeletonPlugin::initializePlugin() {
}

QString FileSkeletonPlugin::getLoadFilters() {
  return QString( "Skeleton files ( *.skl )" );
};

QString FileSkeletonPlugin::getSaveFilters() {
  return QString( "Skeleton files ( *.skl )" );
};

DataType  FileSkeletonPlugin::supportedType() {
  DataType type = DATA_SKELETON;
  return type;
}


int FileSkeletonPlugin::addEmpty( ){
  // new object data struct
  SkeletonObject * object = new SkeletonObject();

  if ( PluginFunctions::objectCount() == 1 )
    object->target(true);

  if (PluginFunctions::targetCount() == 0 )
    object->target(true);

  QString name = get_unique_name(object);

  // call the local function to update names
  QFileInfo f(name);
  object->setName( f.fileName() );

  object->update();

  object->show();

  emit log(LOGINFO,object->getObjectinfo());

  emit emptyObjectAdded ( object->id() );

  return object->id();
}

template<typename Skeleton>
bool FileSkeletonPlugin::LoadSkeleton(Skeleton *_pSkeleton, QString _filename)
{
  typedef JointT<typename Skeleton::Point> Joint;
  typedef  PoseT<typename Skeleton::Point> Pose;
  typedef ACG::Matrix4x4T<typename Skeleton::Scalar> Matrix;

  unsigned int nJoints = 0;

  ifstream in(_filename.toStdString().c_str(), ofstream::in);

  // read number of joints
  in >> nJoints;

  Pose *ref = _pSkeleton->referencePose();
  // remember parent joints
  map<unsigned int, Joint*> parents;

  map<unsigned int, unsigned int> jointMap;

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
  
  //read animation
  unsigned int frameCount;
  in >> frameCount;
  
  if ( frameCount > 0 ){
    
    FrameAnimationT<ACG::Vec3d>* animation = new FrameAnimationT<ACG::Vec3d>(_pSkeleton, frameCount);
    AnimationHandle animHandle = _pSkeleton->addAnimation("Animation1", animation);
    
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

  in.close();

  return true;
}

int FileSkeletonPlugin::loadObject(QString _filename)
{
  int id = addEmpty();
  BaseObjectData *obj(0);
  if(PluginFunctions::getObject(id, obj))
  {
    SkeletonObject* skel = PluginFunctions::skeletonObject(obj);

    LoadSkeleton(skel->skeleton(), _filename);

    //general stuff
    obj->source(true);
    emit updatedObject( obj->id(), UPDATE_ALL );
    emit openedFile( obj->id() );
    PluginFunctions::viewAll();
  }

  return id;
};

template<typename Skeleton>
bool FileSkeletonPlugin::SaveSkeleton(Skeleton *_pSkeleton, QString _filename)
{
  typedef JointT<typename Skeleton::Point> Joint;
  typedef  PoseT<typename Skeleton::Point>  Pose;
  typedef ACG::Matrix4x4T<typename Skeleton::Scalar> Matrix;

  ofstream out(_filename.toStdString().c_str(), ofstream::out);

  // write the number of joints
  out << _pSkeleton->jointCount() << endl;

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

    out << endl;
  }
  
  //now store animation
  AnimationT<ACG::Vec3d>* animation = 0;
  unsigned int iAnimation;
  //get first animation with name
  for (unsigned int i = 0; i < _pSkeleton->animationCount(); i++){
    animation = _pSkeleton->animation( AnimationHandle(i, 0 ) );

    if (animation->name() == "")
      animation = 0;
    else{
      iAnimation = i;
      break;
    }
  }

  if (animation != 0){

    out << animation->frameCount() << endl;
    
    std::string name = _pSkeleton->animationName(iAnimation);
    AnimationHandle animHandle = _pSkeleton->animationHandle(name);

    // every frame of that animation
    for(unsigned int k = 0; k < animation->frameCount(); ++k){
      
      animHandle.setFrame(k);
      typename Skeleton::Pose* pose = _pSkeleton->pose( animHandle );
      
      for (typename Skeleton::Iterator it = _pSkeleton->begin(); it != _pSkeleton->end(); ++it ){

        unsigned int i = (*it)->id();
        Joint *pJoint   = *it;

        // write this joints id
        out << pJoint->id() << " ";

        // write its position
        const Matrix &mat = pose->globalMatrix(i);
        for(int y = 0; y < 3; ++y)
          for(int x = 0; x < 3; ++x)
            out << mat(y, x) << " ";
        for(int y = 0; y < 3; ++y)
          out << mat(y, 3) << " ";
      }

      out << endl;
    }
  } else {
    out << "0" << endl; 
  }

  out.close();
  return !out.fail();
}

bool FileSkeletonPlugin::saveObject(int _id, QString _filename)
{
	BaseObjectData *obj(0);
	if(PluginFunctions::getObject(_id, obj))
	{
		SkeletonObject *skel = PluginFunctions::skeletonObject(obj);
		if(skel)
		{
			obj->setName(_filename.section(OpenFlipper::Options::dirSeparator(), -1));
			obj->setPath(_filename.section(OpenFlipper::Options::dirSeparator(), 0, -2));

			SaveSkeleton(skel->skeleton(), _filename);
		}
	}

	return true;
}

void FileSkeletonPlugin::loadIniFile( INIFile& _ini ,int _id ) {
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

void FileSkeletonPlugin::saveIniFile( INIFile& _ini ,int _id) {
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


QString FileSkeletonPlugin::get_unique_name(SkeletonObject* _object)
{
  bool name_unique = false;

  int cur_idx = _object->id();

  while(!name_unique)
  {
    name_unique = true;

    QString cur_name = QString("Skeleton " + QString::number( cur_idx ) + ".skl");

    PluginFunctions::ObjectIterator o_it(PluginFunctions::ALL_OBJECTS, DATA_SKELETON );
    for(; o_it != PluginFunctions::objectsEnd(); ++o_it)
    {
      if( o_it->name() == cur_name)
      {
        name_unique = false;
        cur_idx += 10;
        break;
      }
    }
  }

  return QString("Skeleton " + QString::number( cur_idx ) + ".skl");
}

Q_EXPORT_PLUGIN2( fileskeletonplugin , FileSkeletonPlugin );

