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

//================================================================
//
//  SplatCloudObject
//
//================================================================


#define SPLATCLOUDOBJECT_C


//== INCLUDES ====================================================


#include "SplatCloud.hh"
#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <ACG/Scenegraph/DrawModes.hh>
#include <QString>


//== DEFINES =====================================================


//#define REPORT_UPDATE_TYPE


//== IMPLEMENTATION ==============================================


/** Constructor for SplatCloud Objects. This object class gets a Separator Node giving
 *  the root node to which it should be connected. The SplatCloud is generated internally
 *  and all nodes for visualization will be added below the scenegraph node.\n
 *  You dont need to create an object of this type manually. Use
 *  PluginFunctions::addSplatCloud instead. ( see Types.hh::DataType )
 */
SplatCloudObject::SplatCloudObject() : 
BaseObjectData         (       ), 
backfaceCullingEnabled_( false ), 
pointsizeScale_        ( 1.0f  ), 
shaderNode_            ( 0     ), 
splatCloudNode_        ( 0     ) 
{
  // allocate memory for splat cloud
  splatCloud_ = new SplatCloud;
  if( !splatCloud_ )
  {
    std::cerr << "SplatCloudObject::SplatCloudObject() : Out of memory." << std::endl;
  }

  setDataType( DATA_SPLATCLOUD );
  setTypeIcon( DATA_SPLATCLOUD, "SplatCloudType.png" );
  init();
}


//----------------------------------------------------------------


/**
 * Copy Constructor - generates a copy of the given object
 */
SplatCloudObject::SplatCloudObject( const SplatCloudObject &_object ) : BaseObjectData( _object )
{
  init( _object.splatCloud_ );
  setName( name() );
}


//----------------------------------------------------------------


/** Destructor for SplatCloud Objects. The destructor deletes the Line and all
 *  Scenegraph nodes associated with the SplatCloud or the object.
 */
SplatCloudObject::~SplatCloudObject()
{
  // Delete the data attached to this object ( this will remove all perObject data)
  // Not the best way to do it but it will work.
  // This is only necessary if people use references to the SplatCloud below and
  // they do something with the SplatCloud in the destructor of their
  // perObjectData.
  deleteData();

  // No need to delete the scenegraph Nodes as this will be managed by baseplugin
  shaderNode_     = 0;
  splatCloudNode_ = 0;

  // free memory of splat cloud
  delete splatCloud_;
  splatCloud_ = 0;
}


//----------------------------------------------------------------


/** Cleanup Function for SplatCloud Objects. Deletes the contents of the whole object and
 * calls SplatCloudObject::init afterwards.
 */
void SplatCloudObject::cleanup()
{
  BaseObjectData::cleanup();

  shaderNode_     = 0;
  splatCloudNode_ = 0;

  init();
}


//----------------------------------------------------------------


/**
 * Generate a copy
 */
BaseObject *SplatCloudObject::copy()
{
  SplatCloudObject *object = new SplatCloudObject( *this );
  return dynamic_cast<BaseObject *>( object );
}


//----------------------------------------------------------------


/**
 * Reload shaders from file
 */
void SplatCloudObject::reloadShaders()
{
  // standard shader filenames
  static const char SPLATS_VERTEXSHADER_FILENAME[]   = "SplatCloud_Splats/Vertex.glsl";
  static const char SPLATS_FRAGMENTSHADER_FILENAME[] = "SplatCloud_Splats/Fragment.glsl";
  static const char DOTS_VERTEXSHADER_FILENAME[]     = "SplatCloud_Dots/Vertex.glsl";
  static const char DOTS_FRAGMENTSHADER_FILENAME[]   = "SplatCloud_Dots/Fragment.glsl";
  static const char POINTS_VERTEXSHADER_FILENAME[]   = "SplatCloud_Points/Vertex.glsl";
  static const char POINTS_FRAGMENTSHADER_FILENAME[] = "SplatCloud_Points/Fragment.glsl";

  // picking shader filenames
  static const char SPLATS_PICK_VERTEXSHADER_FILENAME[]   = "SplatCloud_Splats/PickVertex.glsl";
  static const char SPLATS_PICK_FRAGMENTSHADER_FILENAME[] = "SplatCloud_Splats/Fragment.glsl";
  static const char DOTS_PICK_VERTEXSHADER_FILENAME[]     = "SplatCloud_Dots/PickVertex.glsl";
  static const char DOTS_PICK_FRAGMENTSHADER_FILENAME[]   = "SplatCloud_Dots/Fragment.glsl";
  static const char POINTS_PICK_VERTEXSHADER_FILENAME[]   = "SplatCloud_Points/PickVertex.glsl";
  static const char POINTS_PICK_FRAGMENTSHADER_FILENAME[] = "SplatCloud_Points/Fragment.glsl";

  // get drawmodes
  ACG::SceneGraph::DrawModes::DrawMode splatsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode( "Splats" );
  ACG::SceneGraph::DrawModes::DrawMode dotsDrawMode   = ACG::SceneGraph::DrawModes::getDrawMode( "Dots"   );
  ACG::SceneGraph::DrawModes::DrawMode pointsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode( "Points" );

  // if drawmodes don't exist something went wrong
  if( splatsDrawMode == ACG::SceneGraph::DrawModes::NONE || 
      dotsDrawMode   == ACG::SceneGraph::DrawModes::NONE || 
      pointsDrawMode == ACG::SceneGraph::DrawModes::NONE )
  {
    std::cerr << "Shader DrawModes for SplatCloud not existent!" << std::endl;
    return;
  }

  // get shader directory
  QString shaderDir = OpenFlipper::Options::shaderDirStr() + OpenFlipper::Options::dirSeparator();
  std::string shaderDirectory = std::string( shaderDir.toUtf8() );

  // set shader directory
  shaderNode_->setShaderDir( shaderDirectory );

  // load shaders

  if( QFile( shaderDir + SPLATS_VERTEXSHADER_FILENAME        ).exists() && 
      QFile( shaderDir + SPLATS_PICK_VERTEXSHADER_FILENAME   ).exists() &&
      QFile( shaderDir + SPLATS_FRAGMENTSHADER_FILENAME      ).exists() &&
      QFile( shaderDir + SPLATS_PICK_FRAGMENTSHADER_FILENAME ).exists() )
  {
    shaderNode_->setShader( splatsDrawMode, 
      SPLATS_VERTEXSHADER_FILENAME,      SPLATS_FRAGMENTSHADER_FILENAME, 
      SPLATS_PICK_VERTEXSHADER_FILENAME, SPLATS_PICK_FRAGMENTSHADER_FILENAME );
  }
  else
  {
    std::cerr << "Shader Files for SplatCloud/Splats not found!" << std::endl;
  }

  if( QFile( shaderDir + DOTS_VERTEXSHADER_FILENAME        ).exists() && 
      QFile( shaderDir + DOTS_PICK_VERTEXSHADER_FILENAME   ).exists() &&
      QFile( shaderDir + DOTS_FRAGMENTSHADER_FILENAME      ).exists() &&
      QFile( shaderDir + DOTS_PICK_FRAGMENTSHADER_FILENAME ).exists() )
  {
    shaderNode_->setShader( dotsDrawMode, 
      DOTS_VERTEXSHADER_FILENAME,      DOTS_FRAGMENTSHADER_FILENAME, 
      DOTS_PICK_VERTEXSHADER_FILENAME, DOTS_PICK_FRAGMENTSHADER_FILENAME );
  }
  else
  {
    std::cerr << "Shader Files for SplatCloud/Dots not found!" << std::endl;
  }

  if( QFile( shaderDir + POINTS_VERTEXSHADER_FILENAME        ).exists() && 
      QFile( shaderDir + POINTS_PICK_VERTEXSHADER_FILENAME   ).exists() &&
      QFile( shaderDir + POINTS_FRAGMENTSHADER_FILENAME      ).exists() &&
      QFile( shaderDir + POINTS_PICK_FRAGMENTSHADER_FILENAME ).exists() )
  {
    shaderNode_->setShader( pointsDrawMode, 
      POINTS_VERTEXSHADER_FILENAME,      POINTS_FRAGMENTSHADER_FILENAME, 
      POINTS_PICK_VERTEXSHADER_FILENAME, POINTS_PICK_FRAGMENTSHADER_FILENAME );
  }
  else
  {
    std::cerr << "Shader Files for SplatCloud/Points not found!" << std::endl;
  }
}


//----------------------------------------------------------------


/**
 * Enable or disable backface culling
 */
void SplatCloudObject::enableBackfaceCulling( bool _enable )
{
  // get drawmodes
  ACG::SceneGraph::DrawModes::DrawMode splatsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode( "Splats" );
  ACG::SceneGraph::DrawModes::DrawMode dotsDrawMode   = ACG::SceneGraph::DrawModes::getDrawMode( "Dots"   );
  ACG::SceneGraph::DrawModes::DrawMode pointsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode( "Points" );

  // if drawmodes don't exist something went wrong
  if( splatsDrawMode == ACG::SceneGraph::DrawModes::NONE || 
      dotsDrawMode   == ACG::SceneGraph::DrawModes::NONE || 
      pointsDrawMode == ACG::SceneGraph::DrawModes::NONE )
  {
    std::cerr << "Shader DrawModes for SplatCloud not existent!" << std::endl;
    return;
  }

  // get standard and picking shaders
  GLSL::PtrProgram splatsShader     = shaderNode_->getShader( splatsDrawMode, false );
  GLSL::PtrProgram splatsPickShader = shaderNode_->getShader( splatsDrawMode, true  );
  GLSL::PtrProgram dotsShader       = shaderNode_->getShader( dotsDrawMode,   false );
  GLSL::PtrProgram dotsPickShader   = shaderNode_->getShader( dotsDrawMode,   true  );
  GLSL::PtrProgram pointsShader     = shaderNode_->getShader( pointsDrawMode, false );
  GLSL::PtrProgram pointsPickShader = shaderNode_->getShader( pointsDrawMode, true  );

  // update backface-culling uniform of shaders

  backfaceCullingEnabled_ = _enable;
  GLint backfaceCulling = (GLint) _enable;

  if( splatsShader )
  {
    splatsShader->use();
    splatsShader->setUniform( "backfaceCulling", backfaceCulling );
    splatsShader->disable();
  }

  if( splatsPickShader )
  {
    splatsPickShader->use();
    splatsPickShader->setUniform( "backfaceCulling", backfaceCulling );
    splatsPickShader->disable();
  }

  if( dotsShader )
  {
    dotsShader->use();
    dotsShader->setUniform( "backfaceCulling", backfaceCulling );
    dotsShader->disable();
  }

  if( dotsPickShader )
  {
    dotsPickShader->use();
    dotsPickShader->setUniform( "backfaceCulling", backfaceCulling );
    dotsPickShader->disable();
  }

  if( pointsShader )
  {
    pointsShader->use();
    pointsShader->setUniform( "backfaceCulling", backfaceCulling );
    pointsShader->disable();
  }

  if( pointsPickShader )
  {
    pointsPickShader->use();
    pointsPickShader->setUniform( "backfaceCulling", backfaceCulling );
    pointsPickShader->disable();
  }
}


//----------------------------------------------------------------


/**
 * Set scale for point sizes
 */
void SplatCloudObject::setPointsizeScale( float _scale )
{
  // get drawmodes
  ACG::SceneGraph::DrawModes::DrawMode splatsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode( "Splats" );
  ACG::SceneGraph::DrawModes::DrawMode dotsDrawMode   = ACG::SceneGraph::DrawModes::getDrawMode( "Dots"   );

  // if drawmodes don't exist something went wrong
  if( splatsDrawMode == ACG::SceneGraph::DrawModes::NONE || 
      dotsDrawMode   == ACG::SceneGraph::DrawModes::NONE )
  {
    std::cerr << "Shader DrawModes for SplatCloud not existent!" << std::endl;
    return;
  }

  // get standard and picking shaders
  GLSL::PtrProgram splatsShader     = shaderNode_->getShader( splatsDrawMode, false );
  GLSL::PtrProgram splatsPickShader = shaderNode_->getShader( splatsDrawMode, true  );
  GLSL::PtrProgram dotsShader       = shaderNode_->getShader( dotsDrawMode,   false );
  GLSL::PtrProgram dotsPickShader   = shaderNode_->getShader( dotsDrawMode,   true  );

  // update pointsize-scale uniform of shaders

  pointsizeScale_ = _scale;
  GLfloat pointsizeScale = (GLfloat) _scale;

  if( splatsShader )
  {
    splatsShader->use();
    splatsShader->setUniform( "pointsizeScale", pointsizeScale );
    splatsShader->disable();
  }

  if( splatsPickShader )
  {
    splatsPickShader->use();
    splatsPickShader->setUniform( "pointsizeScale", pointsizeScale );
    splatsPickShader->disable();
  }

  if( dotsShader )
  {
    dotsShader->use();
    dotsShader->setUniform( "pointsizeScale", pointsizeScale );
    dotsShader->disable();
  }

  if( dotsPickShader )
  {
    dotsPickShader->use();
    dotsPickShader->setUniform( "pointsizeScale", pointsizeScale );
    dotsPickShader->disable();
  }
}


//----------------------------------------------------------------


/** This function initalizes the SplatCloud object. It creates the scenegraph nodes.
*/
void SplatCloudObject::init( const SplatCloud *_splatCloud )
{
  if( materialNode() == NULL )
    std::cerr << "Error when creating SplatCloud Object! materialNode is NULL!" << std::endl;

  // if _splatCloud is *not* 0, copy it's contents
  if( _splatCloud )
  {
    delete splatCloud_;

    splatCloud_ = new SplatCloud( *_splatCloud );
    if( !splatCloud_ )
    {
      std::cerr << "SplatCloudObject::init() : Out of memory." << std::endl;
    }
  }

  // if something went wrong during initialization, abort
  if( !splatCloud_ )
  {
    shaderNode_     = 0;
    splatCloudNode_ = 0;
    return;
  }

  // create new scenegraph nodes
  shaderNode_     = new ShaderNode    (               materialNode(), "NEW ShaderNode for" );
  splatCloudNode_ = new SplatCloudNode( *splatCloud_, shaderNode_,    "NEW SplatCloudNode" );

  // load shaders
  reloadShaders();
}


//----------------------------------------------------------------


/**
 * Update object and scenegraph nodes
 */
void SplatCloudObject::update( UpdateType _type )
{
  if( _type == UPDATE_ALL )
  {
#   ifdef REPORT_UPDATE_TYPE
    std::cout << "SplatCloudObject::update() : UPDATE_ALL" << std::endl;
    std::cout << std::endl;
#   endif

    if( splatCloudNode_ )
      splatCloudNode_->modifiedAll();
    return;
  }

  if( _type.contains( UPDATE_GEOMETRY ) )
  {
#   ifdef REPORT_UPDATE_TYPE
    std::cout << "SplatCloudObject::update() : UPDATE_GEOMETRY" << std::endl;
#   endif

    if( splatCloudNode_ )
      splatCloudNode_->modifiedPositions();
  }

  if( _type.contains( UPDATE_COLOR ) )
  {
#   ifdef REPORT_UPDATE_TYPE
    std::cout << "SplatCloudObject::update() : UPDATE_COLOR" << std::endl;
#   endif

    if( splatCloudNode_ )
      splatCloudNode_->modifiedColors();
  }

  if( _type.contains( updateType("Normals") ) )
  {
#   ifdef REPORT_UPDATE_TYPE
    std::cout << "SplatCloudObject::update() : UPDATE_Normals" << std::endl;
#   endif

    if( splatCloudNode_ )
      splatCloudNode_->modifiedNormals();
  }

  if( _type.contains( updateType("Pointsizes") ) )
  {
#   ifdef REPORT_UPDATE_TYPE
    std::cout << "SplatCloudObject::update() : UPDATE_Pointsizes" << std::endl;
#   endif

    if( splatCloudNode_ )
      splatCloudNode_->modifiedPointsizes();
  }

  if( _type.contains( updateType("Indices") ) )
  {
#   ifdef REPORT_UPDATE_TYPE
    std::cout << "SplatCloudObject::update() : UPDATE_Indices" << std::endl;
#   endif
  }

  if( _type.contains( UPDATE_SELECTION ) )
  {
#   ifdef REPORT_UPDATE_TYPE
    std::cout << "SplatCloudObject::update() : UPDATE_SELECTION" << std::endl;
#   endif

    if( splatCloudNode_ )
      splatCloudNode_->modifiedSelections();
  }

# ifdef REPORT_UPDATE_TYPE
  std::cout << std::endl;
# endif
}


//----------------------------------------------------------------
//  Name/Path Handling
//----------------------------------------------------------------


/** Set the name of an object. All Scenegraph nodes are renamed too. It also calls
* BaseObjectData::setName.
*/
void SplatCloudObject::setName( QString _name )
{
  BaseObjectData::setName( _name );

  std::string nodename;

  nodename = std::string( "ShaderNode for SplatCloud " + _name.toUtf8() );
  shaderNode_->name( nodename );

  nodename = std::string( "SplatCloudNode for SplatCloud " + _name.toUtf8() );
  splatCloudNode_->name( nodename );
}


//----------------------------------------------------------------
//  Object information
//----------------------------------------------------------------


/** Returns a string containing all information about the current object. This also
* includes the information provided by BaseObjectData::getObjectinfo
*
* @return String containing the object information
*/
QString SplatCloudObject::getObjectinfo()
{
  QString output;

  output += "========================================================================\n";
  output += BaseObjectData::getObjectinfo();

  if( dataType( DATA_SPLATCLOUD ) )
  {
    output += "Object Contains SplatCloud: ";

    if( splatCloud_ )
    {
      output += " #Splats: " + QString::number( splatCloud_->numSplats() );

      output += ", Splat-Properties:";
      if( splatCloud_->splatProperties().empty() )
      {
        output += " none";
      }
      else
      {
        SplatCloud::SplatProperties::const_iterator splatPropertyIter;
        for( splatPropertyIter = splatCloud_->splatProperties().begin(); splatPropertyIter != splatCloud_->splatProperties().end(); ++splatPropertyIter )
          output += QString( " " ) + splatPropertyIter->first.c_str();
      }

      output += ", Cloud-Properties:";
      if( splatCloud_->cloudProperties().empty() )
      {
        output += " none";
      }
      else
      {
        SplatCloud::CloudProperties::const_iterator cloudPropertyIter;
        for( cloudPropertyIter = splatCloud_->cloudProperties().begin(); cloudPropertyIter != splatCloud_->cloudProperties().end(); ++cloudPropertyIter )
          output += QString( " " ) + cloudPropertyIter->first.c_str();
      }
    }

    output += "\n";
  }

  output += "========================================================================\n";
  return output;
}


//----------------------------------------------------------------
//  Picking
//----------------------------------------------------------------


/** Given an node index from PluginFunctions::scenegraphPick this function can be used to
* check if the splatCloudNode of the object has been picked.
*
* @param _node_idx Index of the picked splatcloud node
* @return bool if the splatCloudNode of this object is the picking target.
*/
bool SplatCloudObject::picked( uint _node_idx )
{
  return ( _node_idx == splatCloudNode_->id() );
}


//----------------------------------------------------------------


void SplatCloudObject::enablePicking( bool _enable )
{
  splatCloudNode_->enablePicking( _enable );
  shaderNode_->enablePicking( _enable );
}


//----------------------------------------------------------------


bool SplatCloudObject::pickingEnabled()
{
  return splatCloudNode_->pickingEnabled();
}
