/*===========================================================================*\
*                                                                           *
*                              OpenFlipper                                  *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
*                           www.openflipper.org                             *
*                                                                           *
*---------------------------------------------------------------------------*
*  This file is part of OpenFlipper.                                        *
*                                                                           *
*  OpenFlipper is free software: you can redistribute it and/or modify      *
*  it under the terms of the GNU Lesser General Public License as           *
*  published by the Free Software Foundation, either version 3 of           *
*  the License, or (at your option) any later version with the              *
*  following exceptions:                                                    *
*                                                                           *
*  If other files instantiate templates or use macros                       *
*  or inline functions from this file, or you compile this file and         *
*  link it with other files to produce an executable, this file does        *
*  not by itself cause the resulting executable to be covered by the        *
*  GNU Lesser General Public License. This exception does not however       *
*  invalidate any other reasons why the executable file might be            *
*  covered by the GNU Lesser General Public License.                        *
*                                                                           *
*  OpenFlipper is distributed in the hope that it will be useful,           *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU LesserGeneral Public          *
*  License along with OpenFlipper. If not,                                  *
*  see <http://www.gnu.org/licenses/>.                                      *
*                                                                           *
\*===========================================================================*/

/*===========================================================================*\
*                                                                           *
*   $Revision: 7936 $                                                       *
*   $Author: kremer $                                                      *
*   $Date: 2009-12-15 14:01:19 +0100 (Tue, 15 Dec 2009) $                   *
*                                                                           *
\*===========================================================================*/

#include <QtGui>
#include <QFileInfo>

#include "FileBVH.hh"

#include <iostream>
#include <fstream>
#include <sstream>

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"
#include "OpenFlipper/common/GlobalOptions.hh"

#include <ACG/Geometry/AlgorithmsAngleT.hh>

#include <bitset>

const std::bitset<4>  HIERARCHY			(static_cast<int>(0));
const std::bitset<4>  ROOT_DEFINITION	(static_cast<int>(1));
const std::bitset<4>  OPENED_BRACKET	(static_cast<int>(2));
const std::bitset<4>  CLOSED_BRACKET	(static_cast<int>(3));
const std::bitset<4>  OFFSET			(static_cast<int>(4));
const std::bitset<4>  CHANNELS			(static_cast<int>(5));
const std::bitset<4>  JOINT				(static_cast<int>(6));
const std::bitset<4>  ENDSITE			(static_cast<int>(7));
const std::bitset<4>  MOTION			(static_cast<int>(8));
const std::bitset<4>  FRAMES			(static_cast<int>(9));
const std::bitset<4>  FRAME_TIME		(static_cast<int>(10));
const std::bitset<4>  CHANNEL_DATA		(static_cast<int>(11));

//-----------------------------------------------------------------------------
//data structures used for parsing
enum ChannelType{NotGiven,XP,YP,ZP,XR,YR,ZR}; //contains the informations about the use of this datafield

class JointInfo{ //for every Joint we will create one such instance during Hirachy parsing. It will contain the informations needed to generate the Transformation from the frame values
  public:
    ChannelType dataChannels[6]; //will state which channels information is stored in the current datafields value.
    unsigned int dataOffset[6]; //will contain the index of the channel in the frame data

    unsigned int channelOffset; //used top keep track of the highest channel currently used

    //constructor
    //will set proper default values (as the dataChannels will only be written if an according option is found in the file)
    JointInfo(){
      channelOffset=0; //we assign the first channelType to the first entry
      for(int i=0;i<6;i++){ //all channels that are not overwritten are Unused
          dataChannels[i]=NotGiven;
          dataOffset[i]=0;
      }
    } //end ctor()
}; //end class

//end of data structure
//-----------------------------------------------------------------------------



/// Constructor
FileBVHPlugin::FileBVHPlugin()
  : ignoreJointScaling_(true),
    loadOptions_(0),
    checkJointScaling_(0),
    loadDefaultButton_(0)
{
}

//-----------------------------------------------------------------------------------------------------

void FileBVHPlugin::initializePlugin() {
}

//-----------------------------------------------------------------------------------------------------

QString FileBVHPlugin::getLoadFilters() {
  return QString( tr("Biovision Format files ( *.bvh )") );
}

//-----------------------------------------------------------------------------------------------------

QString FileBVHPlugin::getSaveFilters() {
  return QString( tr("Biovision Format files ( *.bvh )") );
}

//-----------------------------------------------------------------------------------------------------

DataType  FileBVHPlugin::supportedType() {
    DataType type = DATA_SKELETON;
    return type;
}

//-----------------------------------------------------------------------------------------------------

void trimString( std::string& _string) {
  // Trim Both leading and trailing spaces

  size_t start = _string.find_first_not_of(" \t\r\n");
  size_t end   = _string.find_last_not_of(" \t\r\n");

  if(( std::string::npos == start ) || ( std::string::npos == end))
    _string = "";
  else
    _string = _string.substr( start, end-start+1 );
}

//-----------------------------------------------------------------------------------------------------

int FileBVHPlugin::loadObject(QString _filename) {
  
  if ( checkJointScaling_ != 0 )
    ignoreJointScaling_ = checkJointScaling_->isChecked();
  else
    ignoreJointScaling_ = OpenFlipperSettings().value("FileBVH/Load/JointScaling",true).toBool();
  
  //setup filestream
  
  std::fstream input( _filename.toUtf8(), std::ios_base::in );
  
  if ( !input.is_open() || !input.good() ){
    emit log(LOGERR, tr("Error: cannot open file %1").arg(_filename) );
    return -1;
  }
  
  //add a skeleton
  int id = -1;
  emit addEmptyObject(DATA_SKELETON, id);
  
  BaseObjectData* object = 0;
  Skeleton* skeleton = 0;
  
  if(PluginFunctions::getObject( id, object)){
    skeleton = PluginFunctions::skeleton( object );
    object->setFromFileName(_filename);
    object->setName(object->filename());
  }
  
  if (skeleton == 0){
    emit log(LOGERR, tr("Error: Unable to add skeleton!"));
    return -1;
  }

  Skeleton::Joint* currentParent = 0;
  Skeleton::Pose* refPose = skeleton->referencePose();
  

  std::string line;
  std::string keyWrd;
  std::bitset<4> waitingFor = HIERARCHY;
  
  std::map< Skeleton::Joint* , JointInfo> jointInfos;

  uint dataOffset = 0; //Offset of the current channel in the frame data
  
  AnimationHandle animHandle;
  uint currentFrame = 0;
  uint frameCount = 0;
  
  while( input && !input.eof() )
  {
    std::getline(input,line);
    if ( input.bad() ){
      emit log(LOGERR, tr("Error: could not read file properly!"));
      return -1;
    }

    // Trim Both leading and trailing spaces
    trimString(line);
    
    // ignore empty lines
    if ( line.size() == 0 )
      continue;

    std::stringstream stream(line);

    //read keyword from stream
    stream >> keyWrd;

    //HIERARCHY
    if ( (waitingFor == HIERARCHY) && (keyWrd == "HIERARCHY") ){
      waitingFor = ROOT_DEFINITION;
      continue;
    }
    
    //ROOT_DEFINITION
    if ( (waitingFor == ROOT_DEFINITION) && (keyWrd == "ROOT") ){
      
      std::string name;
      stream >> name;

      Skeleton::Joint* rootJoint = new Skeleton::Joint(0, name);
      skeleton->addJoint(0, rootJoint);
      JointInfo info; //we found a new Joint, hence we need an ne JointInfo to store the channel inforamtions.
      jointInfos[rootJoint]=info; //store Joint info for later use in case of CHANNELS
      currentParent = rootJoint;

      waitingFor = OPENED_BRACKET;
      continue;
    }
    
    //OPENED_BRACKET
    if ( (waitingFor == OPENED_BRACKET) && (keyWrd == "{") ){

      waitingFor = OFFSET | CHANNELS | JOINT | ENDSITE | CLOSED_BRACKET;

      continue;
    }

    //CLOSED_BRACKET
    if ( (!(waitingFor&CLOSED_BRACKET).none()) && (keyWrd == "}") ){

      if ( currentParent->parent() == 0 )
        waitingFor = MOTION;
      else{
        waitingFor = JOINT | CLOSED_BRACKET;
      }

      currentParent = currentParent->parent();
      continue;
    }
    
    //JOINT
    if ( (!(waitingFor&JOINT).none()) && (keyWrd == "JOINT") ){

      std::string name;
      stream >> name;
      
      Skeleton::Joint* newJoint = new Skeleton::Joint(currentParent, name);
      skeleton->addJoint(currentParent, newJoint);
      JointInfo info; //we found a new Joint, hence we need an ne JointInfo to store the channel inforamtions.
      jointInfos[newJoint]=info; //store Joint info for later use in case of CHANNELS
      currentParent = newJoint;
      
      waitingFor = OPENED_BRACKET;
      continue;
    }
    
    //OFFSET
    if ( (!(waitingFor&OFFSET).none()) && (keyWrd == "OFFSET") ){

      ACG::Vec3d translation;

      stream >> translation[0];
      stream >> translation[1];
      stream >> translation[2];
      
      refPose->setLocalTranslation(currentParent->id(), translation );

      continue;
    }
    
    //CHANNELS
    if ( (!(waitingFor&CHANNELS).none()) && (keyWrd == "CHANNELS") ){

      uint channelCount;
      stream >> channelCount;
      
      JointInfo& info=jointInfos[ currentParent ];

      if(channelCount>6) //well somethings wrong here...
        std::cerr << "Error: channel count to big, will crash very soon" << std::endl;

      for (uint i=0; i < channelCount; i++){ //parse channel informations
        std::string channelType;
        stream >> channelType;

        //extract Channel position info and store in info.dataChannels
        if (channelType == "Xposition")
          info.dataChannels[info.channelOffset]=XP;
        else if (channelType == "Yposition")
         info.dataChannels[info.channelOffset]=YP;
        else if (channelType == "Zposition")
         info.dataChannels[info.channelOffset]=ZP;
        else if (channelType == "Xrotation")
         info.dataChannels[info.channelOffset]=XR;
        else if (channelType == "Yrotation")
         info.dataChannels[info.channelOffset]=YR;
        else if (channelType == "Zrotation")
         info.dataChannels[info.channelOffset]=ZR;
        else 
          {std::cerr << "Error: Unknown channelType. Ignoring." << std::endl;}
       
        if(info.dataChannels[info.channelOffset]!=NotGiven){  //if there is a channel assigned
            info.dataOffset[info.channelOffset]=dataOffset; //the value for this channel will be found this data position
            info.channelOffset++;                           //write next info into the next index
        }
        dataOffset++;                                       // the data will be read even if there is no channel assigned to this data (e.g. unknown channel type)
      }
      continue;
    }
    
    // ENDSITE
    if ( (!(waitingFor&ENDSITE).none()) && (keyWrd == "End") ){

       std::string site;
       stream >> site;

      std::string name = "End";
      
      Skeleton::Joint* newJoint = new Skeleton::Joint(currentParent, currentParent->name() + name);
      skeleton->addJoint(currentParent, newJoint);
      currentParent = newJoint;
      
      waitingFor = OPENED_BRACKET;
      continue;
    }
    
    //MOTION
    if ( (waitingFor == MOTION) && (keyWrd == "MOTION") ){
      waitingFor = FRAMES;
      continue;
    }
    
    //Frames
    if ( (waitingFor == FRAMES) && (keyWrd == "Frames:") ){
      
      stream >> frameCount;
      
      if (frameCount > 0){
        FrameAnimationT<ACG::Vec3d>* animation = new FrameAnimationT<ACG::Vec3d>(skeleton, frameCount);
        animHandle = skeleton->addAnimation(object->filename().toStdString(), animation);
      }
      
      waitingFor = FRAME_TIME;
      continue;
    }
    
    //Frame Time
    if ( (waitingFor == FRAME_TIME) && (keyWrd == "Frame") ){
      
      std::string time;
      stream >> time;

      double frameTime;
      stream >> frameTime;

      if (frameCount > 0)
        skeleton->animation(animHandle)->setFps( frameTime * 1000 );

      waitingFor = CHANNEL_DATA;
      continue;
    }

    //Channel Data
    if ( (waitingFor == CHANNEL_DATA) ){
      
      // a vector to store all the data for this frame
      vector<double> data(dataOffset,0.0);

      Skeleton::Pose* pose = 0;
      
      if ( currentFrame < frameCount ){
        animHandle.setFrame( currentFrame );
        pose = skeleton->pose(animHandle);
      }
      else
        std::cerr << "Warning: Too many frames specified in file." << std::endl;

      //since we dont have a keyWrd here
      // keyWrd is already the first data
      data[0] = QString( keyWrd.c_str() ).toDouble();
      //read the data for this frame
      for (uint i=1; i < data.size(); i++){
          stream >> data[i];
      }

      //now the channel for this timeFrame is complete
      //apply them to the joints

      if ( currentFrame < frameCount )
      for (unsigned long jointID=0; jointID < skeleton->jointCount(); jointID++ ){
                
        Skeleton::Joint* joint = skeleton->joint( jointID );
        
        // special case: end-effector joints
        // they don't have animation data
        if ( jointInfos.find(joint) == jointInfos.end() ){
          ACG::Matrix4x4d matRef = refPose->localMatrix(jointID);
          pose->setLocalMatrix(jointID, matRef);
          continue;
        }

        JointInfo& info=jointInfos[joint]; //get the cahnnels info for the current joint
         
        ACG::Vec3d translation(0.0,0.0,0.0); //setup translation

        ACG::GLMatrixd matRot; //setup rotation
        matRot.identity();

        for(int i=0;i<6;i++)    //iterate over the cannels, applying the transformations in order
          {
            if(info.dataChannels[i]==NotGiven) break; //stop at the first empty channel

            double val=data[info.dataOffset[i]];  //read one value from the data
            
            switch(info.dataChannels[i]){         //apply the transformation
              case XP: translation[0]=val; break; //translation (order doesnt matter)
              case YP: translation[1]=val; break;
              case ZP: translation[2]=val; break;
              case XR: matRot.rotateX(val); break; //rotation (order does matter)
              case YR: matRot.rotateY(val); break;
              case ZR: matRot.rotateZ(val); break;
              default: break;                      //stop at the first empty channel 
            }
          }

        ACG::GLMatrixd matTrans;
        matTrans.identity();
        
        if (  (!ignoreJointScaling_) || //translate only if there is no need to preserve the scale
              (joint->parent() == 0) ) //or if the joint is the rootjoint
          matTrans.translate(translation);

        ACG::Matrix4x4d matRef = refPose->localMatrix(jointID); 
        pose->setLocalMatrix(jointID, matRef * matTrans * matRot);
      }
      
      currentFrame++;
      
      continue;

    }

    std::cerr << "Error: No match for keyword '" << keyWrd << "' ";
    std::cerr << "waiting for : " << waitingFor.to_string<char,char_traits<char>,allocator<char> >() << std::endl;
  }
  

  //general stuff
  object->source( PluginFunctions::objectCount() > 4 );
  emit updatedObject( object->id(), UPDATE_ALL );
  emit openedFile( object->id() );
  PluginFunctions::viewAll();

  return object->id();
}

//-----------------------------------------------------------------------------------------------------

bool FileBVHPlugin::saveObject(int _id, QString _filename)
{

  BaseObjectData* object;
  PluginFunctions::getObject(_id,object);


  //open output stream
  std::string filename = std::string( _filename.toUtf8() );

  std::fstream stream( filename.c_str(), std::ios_base::out );

  if ( !stream ){
    emit log(LOGERR, tr("saveObject : cannot not open file %1").arg(_filename) );
    QFile( QString(filename.c_str()) ).remove();
    return false;
  }

  //write object
  if ( object->dataType( DATA_SKELETON ) ) {

    object->setFromFileName(_filename);
    object->setName(object->filename());

    Skeleton* skeleton = PluginFunctions::skeleton(object);

    if ( writeSkeleton( stream, *skeleton ) ){
      
      emit log(LOGINFO, tr("Saved object to ") + _filename );
      stream.close();
      return true;

    } else {
      
      emit log(LOGERR, tr("Unable to save ") + _filename);
      stream.close();
      QFile( QString(filename.c_str()) ).remove();
      return false;
    }
    
  } else {
    
    emit log(LOGERR, tr("Unable to save (object is not a skeleton)"));
    stream.close();
    QFile( QString(filename.c_str()) ).remove();
    return false;
  }
}

//-----------------------------------------------------------------------------------------------------


ACG::Vec3d MatrixToEuler(ACG::Matrix4x4d _matrix){
  ACG::Vec3d v(0.0, 0.0, 0.0);

  if(_matrix(1,0) > 0.998) { // singularity at north pole
      v[0] = atan2(_matrix(0,2), _matrix(2,2));
      v[1] = M_PI / 2.0;
      v[2] = 0;
      return v;
  }

  if(_matrix(1,0) < -0.998) { // singularity at south pole
      v[0] = atan2(_matrix(0,2), _matrix(2,2));
      v[1] = - M_PI / 2.0;
      v[2] = 0;
      return v;
  }

  v[0] = atan2(-_matrix(2,0), _matrix(0,0));
  v[1] = atan2(-_matrix(1,2), _matrix(1,1));
  v[2] = asin(_matrix(1,0));
  return v;
}

//-----------------------------------------------------------------------------------------------------

bool FileBVHPlugin::writeSkeleton( std::ostream& _out, Skeleton& _skeleton ) {
  
  Skeleton::Pose* refPose = _skeleton.referencePose();
  
  _out << "HIERARCHY" << std::endl;
  
  std::string indent = "";
  
  Skeleton::Joint* lastJoint = 0;
  
  for (Skeleton::Iterator it = _skeleton.begin(); it != _skeleton.end(); ++it )
  {

    //close brackets
    while ( (*it)->parent() != lastJoint ){
      indent = indent.substr(0, indent.size()-1);
      _out << indent << "}" << std::endl;

      lastJoint = lastJoint->parent();
    }
    
    ACG::Vec3d translation;
    
    if ( (*it)->parent() == 0 ){
      //ROOT Joint
      _out << "ROOT " << (*it)->name() << std::endl;
      
      translation = refPose->globalTranslation( (*it)->id() );
      
    } else if ( (*it)->size() > 0 ){
      
      //normal joint
      _out << indent << "JOINT " << (*it)->name() << std::endl;

      translation = refPose->globalTranslation( (*it)->id() ) - refPose->globalTranslation( (*it)->parent()->id() );
      
    } else {
      
      //end-effector
      _out << indent << "End Site" << std::endl;
      
      translation = refPose->globalTranslation( (*it)->id() ) - refPose->globalTranslation( (*it)->parent()->id() );
    }

    _out << indent << "{" << std::endl;
    indent += "\t";

    _out << indent << "OFFSET " << translation[0] << " " << translation[1] << " " << translation[2] << std::endl;
    
    if ( (*it)->size() > 0 ){ //end-effectors have no channel
      
      if ( (*it)->parent() == 0)
        _out << indent << "CHANNELS 6 Xposition Yposition Zposition Zrotation Yrotation Xrotation" << std::endl;
      else
        _out << indent << "CHANNELS 3 Zrotation Yrotation Xrotation" << std::endl;

      lastJoint = *it;
      
    } else {
      indent = indent.substr(0, indent.size()-1);
      _out << indent << "}" << std::endl;
      lastJoint = (*it)->parent();
    }
  }
  
  //close brackets
  while ( lastJoint->parent() != 0 ){
    indent = indent.substr(0, indent.size()-1);
    _out << indent << "}" << std::endl;

    lastJoint = lastJoint->parent();
  }
  _out << "}" << std::endl;
  
  //now hierarchy is set up
  // save the motion
  AnimationT<ACG::Vec3d>* animation = 0;
  uint iAnimation;
  //get first animation with name
  for (uint i = 0; i < _skeleton.animationCount(); i++){
    animation = _skeleton.animation( AnimationHandle(i, 0 ) );
    
    if (animation->name() == "")
      animation = 0;
    else{
      iAnimation = i;
      break;
    }
  }

  if (animation == 0){

    _out << "MOTION"          << std::endl;
    _out << "Frames: 0"       << std::endl;
    _out << "Frame Time: 0.1" << std::endl;
    
    return true;
  }
  
  _out << "MOTION" << std::endl;
  _out << "Frames: " << animation->frameCount() << std::endl;
  _out << "Frame Time: " << animation->fps() / 1000.0 << std::endl;

  std::string name = _skeleton.animationName(iAnimation);
  AnimationHandle animHandle = _skeleton.animationHandle(name);

 
  // and every frame of that animation
  for(unsigned long k = 0; k < animation->frameCount(); ++k)
  {
    
    animHandle.setFrame(k);
    
    Skeleton::Pose* pose = _skeleton.pose( animHandle );
    
    for (Skeleton::Iterator it = _skeleton.begin(); it != _skeleton.end(); ++it )
    {
      //skip end-effectors
      if ( (*it)->size() == 0)
        continue;

      if ( (*it)->parent() == 0 ){
        //root joint
        ACG::Vec3d translation = pose->globalTranslation( (*it)->id() ) - refPose->globalTranslation( (*it)->id() );

        _out << translation[0] << " " << translation[1] << " " << translation[2];
      }
      
      ACG::Matrix4x4d rotMat = _skeleton.referencePose()->localMatrixInv( (*it)->id() ) * pose->localMatrix( (*it)->id() );

      ACG::Vec3d angles = convertToAxisRotation(rotMat);
      
      _out << " " << angles[2] << " " << angles[1] << " " << angles[0];
      
//       ACG::GLMatrixd testMat( _skeleton.referencePose()->localMatrix( (*it)->id() ).raw() );
//       
//       if ( (*it)->isRoot() )
//         testMat.translate( pose->globalTranslation( (*it)->id() ) );
//       
//       testMat.rotateZ( angles[2] );
//       testMat.rotateY( angles[1] );
//       testMat.rotateX( angles[0] );
//       
//       
//       if ( testMat != pose->localMatrix( (*it)->id() ) ){
//         std::cerr << "Decomposition failed (frame : " << k << " joint: " << (*it)->id() << ")" << std::endl;
//         std::cerr << "Original:" << pose->localMatrix( (*it)->id() ) << std::endl;
//         std::cerr << "Test    :" << testMat << std::endl;
//       }
    }
    _out << std::endl;
  }
  
  return true;
}

//-----------------------------------------------------------------------------------------------------

ACG::Vec3d FileBVHPlugin::convertToAxisRotation(ACG::Matrix4x4d& _rotationMatrix)
{
  //  conversion to quaternion
  ACG::Quaterniond quat(_rotationMatrix);

  double x,y,z,w;
  w = quat[0];
  x = quat[1];
  y = quat[2];
  z = quat[3];

  //  convert quaternion to euler
  //  create special rotation matrix
  ACG::GLMatrixT<double> matrix;

  matrix(0,0) = 1.0f - 2.0f * (y * y +z * z);
  matrix(0,1) = 2.0f * (x * y +z * w);
  matrix(0,2) = 2.0f * (z * x -y * w);

  matrix(1,0) = 2.0f * (x * y -z * w);
  matrix(1,1) = 1.0f - 2.0f * (z * z +x * x);
  matrix(1,2) = 2.0f * (y * z +x * w);

  matrix(2,0) = 2.0f * (z * x +y * w);
  matrix(2,1) = 2.0f * (y * z -x * w);
  matrix(2,2) = 1.0f - 2.0f * (y * y +x * x);

  matrix.transpose();

  //  decompose the rotation matrix
  double cosY = sqrt(matrix(0,0)*matrix(0,0)+matrix(1,0)*matrix(1,0));

  ACG::Vec3d result;
  
  if (cosY > 16 * FLT_EPSILON) {
    result[0] = atan2( 1.0*matrix(2,1), matrix(2,2));
    result[1] = atan2(-1.0*matrix(2,0), cosY);
    result[2] = atan2( 1.0*matrix(1,0), matrix(0,0));
  } else {
    result[0] = atan2(-1.0*matrix(1,2), matrix(1,1));
    result[1] = atan2(-1.0*matrix(2,0), cosY);
    result[2] = 0.0;
  }

  //  finally convert angles to degree
  result[0] *= 180/M_PI;
  result[1] *= 180/M_PI;
  result[2] *= 180/M_PI;

  return result;
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileBVHPlugin::saveOptionsWidget(QString /*_currentFilter*/) {
  return 0;
}

//-----------------------------------------------------------------------------------------------------

QWidget* FileBVHPlugin::loadOptionsWidget(QString /*_currentFilter*/) {
    
    if (loadOptions_ == 0){
        //generate widget
        loadOptions_ = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setAlignment(Qt::AlignTop);
        
        checkJointScaling_ = new QCheckBox("Ignore Joint Scaling");
        layout->addWidget(checkJointScaling_);

        loadDefaultButton_ = new QPushButton("Make Default");
        layout->addWidget(loadDefaultButton_);
        
        loadOptions_->setLayout(layout);
        
        connect(loadDefaultButton_, SIGNAL(clicked()), this, SLOT(slotLoadDefault()));
        
        checkJointScaling_->setChecked( OpenFlipperSettings().value("FileBVH/Load/JointScaling",true).toBool() );
    }
    
    return loadOptions_;
}

//-----------------------------------------------------------------------------------------------------

void FileBVHPlugin::slotLoadDefault() {
    
    OpenFlipperSettings().setValue( "FileBVH/Load/JointScaling", checkJointScaling_->isChecked() );

    OpenFlipperSettings().setValue( "Core/File/UseLoadDefaults", true );
}

//-----------------------------------------------------------------------------------------------------

Q_EXPORT_PLUGIN2( filebvhplugin , FileBVHPlugin );
