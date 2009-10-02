/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS Core - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "Core.hh"

//== IMPLEMENTATION ==========================================================


// A job has been started by a plugin
void Core::slotStartJob( QString _jobId, QString _description , int _min , int _max ) {
  std::cerr << "slotStartJob" << std::endl;
}

//-----------------------------------------------------------------------------

// A job state has been updated by a plugin
void Core::slotSetJobState(QString _jobId, int _value ) {
  std::cerr << "slotSetJobState" << std::endl;
}

//-----------------------------------------------------------------------------

// A job state has been canceled by a plugin
void Core::slotCancelJob(QString _jobId ) {
  std::cerr << "slotCancelJob" << std::endl;
}

//-----------------------------------------------------------------------------

// A job state has been finished by a plugin
void Core::slotFinishJob(QString _jobId ) {
  std::cerr << "slotFinishJob" << std::endl;
}




//=============================================================================
