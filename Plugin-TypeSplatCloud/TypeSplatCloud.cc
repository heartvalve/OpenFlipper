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
//  CLASS TypeSplatCloudPlugin - IMPLEMENTATION
//
//================================================================


//== INCLUDES ====================================================


#include "TypeSplatCloud.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <OpenFlipper/common/BackupData.hh>
#include "SplatCloudBackup.hh"

#include <ACG/GL/GLState.hh>


//== DEFINES =====================================================


//#define REPORT_UPDATE_TYPE


//== IMPLEMENTATION ==============================================


void TypeSplatCloudPlugin::slotViewChanged()
{
	// get drawmodes
	ACG::SceneGraph::DrawModes::DrawMode splatsDrawMode = ACG::SceneGraph::DrawModes::getDrawMode( "Splats" );
	ACG::SceneGraph::DrawModes::DrawMode dotsDrawMode   = ACG::SceneGraph::DrawModes::getDrawMode( "Dots"   );

	// if drawmodes don't exist something went wrong
	if( splatsDrawMode == ACG::SceneGraph::DrawModes::NONE || 
		dotsDrawMode   == ACG::SceneGraph::DrawModes::NONE )
	{
		emit log(LOGERR,tr("Shader DrawModes for SplatCloud not existent!"));
		return;
	}

	// get current glState
	const ACG::GLState &glstate = PluginFunctions::viewerProperties().glState();

	// get viewport
	int left, bottom, width, height;
	glstate.get_viewport( left, bottom, width, height );

	float x = (float) left;
	float y = (float) bottom;
	float w = (float) width;
	float h = (float) height;

	// get depthrange
	// TODO: use glstate.get_depth_range when implemented
	GLfloat	depthRange[2];
	glGetFloatv( GL_DEPTH_RANGE, depthRange );
	float z = (float) depthRange[0];
	float d = (float) depthRange[1] - z;

	// check if we are safe
	if( w<=0.0f || h<=0.0f || d<=0.0f )
		return;

	// calculate window-coordinates to normalized-device-coordinates scale
	ACG::Vec4f invVPs;
	invVPs[0] = 2.0f / w;
	invVPs[1] = 2.0f / h;
	invVPs[2] = 2.0f / d;
	invVPs[3] = 0.0f;

	// calculate window-coordinates to normalized-device-coordinates transpose
	ACG::Vec4f invVPt;
	invVPt[0] = - ( x * invVPs[0] + 1.0f );
	invVPt[1] = - ( y * invVPs[1] + 1.0f );
	invVPt[2] = - ( z * invVPs[2] + 1.0f );
	invVPt[3] = 1.0f;

	// calculate normalized-device-coordinates to window-coordinates scale and transpose
	GLfloat VPs_z = 0.5f * d;
	GLfloat VPt_z = z + VPs_z;

	// calculate scaling factor of modelview matrix
	static const double RCP_3 = 1.0 / 3.0;
	const ACG::GLMatrixd &mv = glstate.modelview();
	double detMV = mv(0,0) * (mv(1,1)*mv(2,2) - mv(1,2)*mv(2,1)) 
	             + mv(0,1) * (mv(1,2)*mv(2,0) - mv(1,0)*mv(2,2)) 
	             + mv(0,2) * (mv(1,0)*mv(2,1) - mv(1,1)*mv(2,0));
	GLfloat MVs = (GLfloat) pow( fabs( detMV ), RCP_3 );

	// calculate scale for pointsizes in eye-coordinates according to fovy and transformation to window-coordinates
	GLfloat VPsFov_y = glstate.projection()(1,1) * (0.5f * h);

	// for all splatcloud-objects...
	PluginFunctions::ObjectIterator objIter( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
	for( ; objIter != PluginFunctions::objectsEnd(); ++objIter )
	{
		// get scenegraph shader-node
		ShaderNode *shaderNode = PluginFunctions::splatShaderNode( *objIter );

		// get standard shaders and picking shaders
		GLSL::PtrProgram splatsShader     = shaderNode->getShader( splatsDrawMode, false );
		GLSL::PtrProgram splatsPickShader = shaderNode->getShader( splatsDrawMode, true  );
		GLSL::PtrProgram dotsShader       = shaderNode->getShader( dotsDrawMode,   false );
		GLSL::PtrProgram dotsPickShader   = shaderNode->getShader( dotsDrawMode,   true  );

		// update shader uniforms concerning viewport and depthrange

		if( splatsShader )
		{
			splatsShader->use();
			splatsShader->setUniform( "invViewportScale",   invVPs   );
			splatsShader->setUniform( "invViewportTransp",  invVPt   );
			splatsShader->setUniform( "viewportScale_z",    VPs_z    );
			splatsShader->setUniform( "viewportTransp_z",   VPt_z    );
			splatsShader->setUniform( "modelviewScale",     MVs      );
			splatsShader->setUniform( "viewportScaleFov_y", VPsFov_y );
			splatsShader->disable();
		}

		if( splatsPickShader )
		{
			splatsPickShader->use();
			splatsPickShader->setUniform( "invViewportScale",   invVPs   );
			splatsPickShader->setUniform( "invViewportTransp",  invVPt   );
			splatsPickShader->setUniform( "viewportScale_z",    VPs_z    );
			splatsPickShader->setUniform( "viewportTransp_z",   VPt_z    );
			splatsPickShader->setUniform( "modelviewScale",     MVs      );
			splatsPickShader->setUniform( "viewportScaleFov_y", VPsFov_y );
			splatsPickShader->disable();
		}

		if( dotsShader )
		{
			dotsShader->use();
			dotsShader->setUniform( "modelviewScale",     MVs      );
			dotsShader->setUniform( "viewportScaleFov_y", VPsFov_y );
			dotsShader->disable();
		}

		if( dotsPickShader )
		{
			dotsPickShader->use();
			dotsPickShader->setUniform( "modelviewScale",     MVs      );
			dotsPickShader->setUniform( "viewportScaleFov_y", VPsFov_y );
			dotsPickShader->disable();
		}
	}
}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::slotObjectUpdated( int _objectId, const UpdateType& _type )
{
	if( _objectId == -1 )
		return;

	BaseObjectData *object = 0;
	PluginFunctions::getObject( _objectId, object );

	// Groups are not BaseObjectData, so this will fail.
	if ( !object )
	  return;

	SplatCloudNode *splatCloudNode = PluginFunctions::splatCloudNode( object );

	if( splatCloudNode )
	{

		if( _type == UPDATE_ALL )
		{
#			ifdef REPORT_UPDATE_TYPE
			std::cout << "TypeSplatCloudPlugin::slotObjectUpdated() : UPDATE_ALL" << std::endl;
			std::cout << std::endl;
#			endif

			splatCloudNode->modifiedAll();
			return;
		}

		if( _type.contains( UPDATE_GEOMETRY ) )
		{
#			ifdef REPORT_UPDATE_TYPE
			std::cout << "TypeSplatCloudPlugin::slotObjectUpdated() : UPDATE_GEOMETRY" << std::endl;
#			endif

			splatCloudNode->modifiedPoints();
		}

		if( _type.contains( updateType("Normals") ) )
		{
#			ifdef REPORT_UPDATE_TYPE
			std::cout << "TypeSplatCloudPlugin::slotObjectUpdated() : UPDATE_Normals" << std::endl;
#			endif

			splatCloudNode->modifiedNormals();
		}

		if( _type.contains( updateType("Pointsizes") ) )
		{
#			ifdef REPORT_UPDATE_TYPE
			std::cout << "TypeSplatCloudPlugin::slotObjectUpdated() : UPDATE_Pointsizes" << std::endl;
#			endif

			splatCloudNode->modifiedPointsizes();
		}

		if( _type.contains( UPDATE_COLOR ) )
		{
#			ifdef REPORT_UPDATE_TYPE
			std::cout << "TypeSplatCloudPlugin::slotObjectUpdated() : UPDATE_COLOR" << std::endl;
#			endif

			splatCloudNode->modifiedColors();
		}

		if( _type.contains( UPDATE_SELECTION ) )
		{
#			ifdef REPORT_UPDATE_TYPE
			std::cout << "TypeSplatCloudPlugin::slotObjectUpdated() : UPDATE_SELECTION" << std::endl;
#			endif

			splatCloudNode->modifiedSelections();
		}

#		ifdef REPORT_UPDATE_TYPE
		std::cout << std::endl;
#		endif
	}
}


//----------------------------------------------------------------


bool TypeSplatCloudPlugin::registerType()
{
	addDataType( "SplatCloud", tr( "SplatCloud" ) );
	setTypeIcon( "SplatCloud", "SplatCloudType.png" );
	return true;
}


//----------------------------------------------------------------


int TypeSplatCloudPlugin::addEmpty()
{
	// new object data struct
	SplatCloudObject *object = new SplatCloudObject();

	if( PluginFunctions::objectCount() == 1 )
		object->target( true );

	if( PluginFunctions::targetCount() == 0 )
		object->target( true );

	QString name = get_unique_name( object );

	// call the local function to update names
	QFileInfo f( name );
	object->setName( f.fileName() );

	object->update();

	object->show();

	emit emptyObjectAdded( object->id() );

	return object->id();
}


//----------------------------------------------------------------


QString TypeSplatCloudPlugin::get_unique_name( SplatCloudObject *_object )
{
	bool name_unique = false;

	int cur_idx = _object->id();

	while( !name_unique )
	{
		name_unique = true;

		QString cur_name = QString( tr( "SplatCloud %1.spl" ).arg( cur_idx ) );

		PluginFunctions::ObjectIterator o_it( PluginFunctions::ALL_OBJECTS, DATA_SPLATCLOUD );
		for( ; o_it != PluginFunctions::objectsEnd(); ++o_it )
		{
			if( o_it->name() == cur_name )
			{
				name_unique = false;
				cur_idx += 10;
				break;
			}
		}
	}

	return QString( tr( "SplatCloud %1.spl" ).arg( cur_idx ) );
}


//----------------------------------------------------------------


void TypeSplatCloudPlugin::generateBackup( int _objectId, QString _name, UpdateType _type )
{
	if( _objectId == -1 )
		return;

	BaseObjectData *object = 0;
	PluginFunctions::getObject( _objectId, object );

	SplatCloudObject *splatCloudObject = PluginFunctions::splatCloudObject( object );

	if( splatCloudObject )
	{
		// get backup object data
		BackupData *backupData = 0;

		if( object->hasObjectData( OBJECT_BACKUPS ) )
		{
			backupData = dynamic_cast<BackupData *>( object->objectData( OBJECT_BACKUPS ) );
		}
		else
		{
			// add backup data
			backupData = new BackupData( object );
			object->setObjectData( OBJECT_BACKUPS, backupData );
		}

		// store a new backup
		SplatCloudBackup *backup = new SplatCloudBackup( splatCloudObject, _name, _type );

		backupData->storeBackup( backup );
	}
}


//================================================================


Q_EXPORT_PLUGIN2( typesplatcloudplugin, TypeSplatCloudPlugin );
