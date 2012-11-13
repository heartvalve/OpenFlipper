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

//== INCLUDES ====================================================


#include "SplatCloudBackup.hh"

#include <iostream>


//== DEFINES =====================================================


//#define REPORT_BACKUP


//== IMPLEMENTATION ==============================================


SplatCloudBackup::SplatCloudBackup( SplatCloudObject *_object, QString _name, UpdateType _type ) :
  BaseBackup       ( _object, _name, _type ),
  splatCloudObject_( _object ),
  splatCloudBackup_( 0 )
{
# ifdef REPORT_BACKUP
  std::cerr << "Create SplatCloudBackup with name: \"" << name_.toStdString() << "\" (id: " << id_ << ")." << std::endl;
# endif

  const SplatCloud *splatCloud = PluginFunctions::splatCloud( _object );

  if( splatCloud == 0 )
  {
    std::cerr << "Could not create SplatCloudBackup with name: \"" << name_.toStdString() << "\" (id: " << id_ << ") : SplatCloud not found." << std::endl;
    return;
  }

  splatCloudBackup_ = new SplatCloud( *splatCloud );

  if( splatCloudBackup_ == 0 )
  {
    std::cerr << "Could not create SplatCloudBackup with name: \"" << name_.toStdString() << "\" (id: " << id_ << ") : Out of memory." << std::endl;
    return;
  }
}


//----------------------------------------------------------------


SplatCloudBackup::~SplatCloudBackup()
{
# ifdef REPORT_BACKUP
  std::cerr << "Delete SplatCloudBackup with name: \"" << name_.toStdString() << "\" (id: " << id_ << ")." << std::endl;
# endif

  delete splatCloudBackup_;
}


//----------------------------------------------------------------


void SplatCloudBackup::apply()
{
  // first apply the baseBackup
  BaseBackup::apply();

# ifdef REPORT_BACKUP
  std::cerr << "Apply SplatCloudBackup with name: \"" << name_.toStdString() << "\" (id: " << id_ << ")." << std::endl;
# endif

  SplatCloud *splatCloud = PluginFunctions::splatCloud( splatCloudObject_ );

  if( splatCloud == 0 )
  {
    std::cerr << "Could not apply SplatCloudBackup with name: \"" << name_.toStdString() << "\" (id: " << id_ << ") : SplatCloud not found." << std::endl;
    return;
  }

  if( splatCloudBackup_ == 0 )
  {
    std::cerr << "Could not apply SplatCloudBackup with name: \"" << name_.toStdString() << "\" (id: " << id_ << ") : No backup available." << std::endl;
    *splatCloud = SplatCloud(); // restore empty SpaltCloud
    return;
  }

  *splatCloud = *splatCloudBackup_;
}
