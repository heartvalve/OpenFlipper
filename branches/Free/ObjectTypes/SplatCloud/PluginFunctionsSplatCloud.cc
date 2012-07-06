
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
//  Plugin Functions for SplatClouds
//
//================================================================


//== INCLUDES ====================================================


#include "PluginFunctionsSplatCloud.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>


//== NAMESPACES ==================================================


namespace PluginFunctions {


//== IMPLEMENTATION ==============================================


//================================================================
// Get objects
//================================================================


bool getObject( int _identifier, SplatCloudObject *&_object )
{
  if( _identifier == -1 )
  {
    _object = 0;
    return false;
  }

  BaseObject *object = objectRoot()->childExists( _identifier );
  _object = dynamic_cast<SplatCloudObject *>( object );
  return (_object != 0);
}


//================================================================
// Getting data from objects and casting between them
//================================================================


ShaderNode* splatShaderNode( BaseObjectData *_object )
{
  if( !_object )
    return 0;

  if( !_object->dataType( DATA_SPLATCLOUD ) )
    return 0;

  SplatCloudObject *object = dynamic_cast<SplatCloudObject *>( _object );

  if ( object == 0 )
    return 0;

  return object->shaderNode();
}


//----------------------------------------------------------------


SplatCloudNode* splatCloudNode( BaseObjectData *_object )
{
  if( !_object )
    return 0;

  if( !_object->dataType( DATA_SPLATCLOUD ) )
    return 0;

  SplatCloudObject *object = dynamic_cast<SplatCloudObject *>( _object );

  if ( object == 0 )
    return 0;

  return object->splatCloudNode();
}


//----------------------------------------------------------------


SplatCloud* splatCloud( BaseObjectData *_object )
{
  if( !_object )
    return 0;

  if( !_object->dataType( DATA_SPLATCLOUD ) )
    return 0;

  SplatCloudObject *object = dynamic_cast<SplatCloudObject *>( _object );

  if ( object == 0 )
    return 0;

  return object->splatCloud();
}


//----------------------------------------------------------------


SplatCloudObject* splatCloudObject( BaseObjectData *_object )
{
  if( !_object )
    return 0;

  if( !_object->dataType( DATA_SPLATCLOUD ) )
    return 0;

  return dynamic_cast<SplatCloudObject *>( _object );
}


//----------------------------------------------------------------


SplatCloudObject* splatCloudObject(  int _objectId ) {

  if  (_objectId == -1)
    return 0;

  BaseObject* object = objectRoot()->childExists( _objectId );

  if ( object == 0 )
    return 0;

  SplatCloudObject* splatCloudObject = dynamic_cast< SplatCloudObject* >(object);

  return splatCloudObject;
}


//================================================================


} // namespace PluginFunctions
