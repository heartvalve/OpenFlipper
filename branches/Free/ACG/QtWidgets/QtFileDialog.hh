/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS QtFileDialog
//
//=============================================================================


#ifndef ACG_QTFILEDIALOG_HH
#define ACG_QTFILEDIALOG_HH


//== INCLUDES =================================================================

#include "../Config/ACGDefines.hh"
#include <QString>
#include <QWidget>

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================

ACGDLLEXPORT	      
QString
getOpenFileName(QWidget*        _parent  = 0, 
		const QString&  _caption = QString::null,
		const QString&  _filter  = QString::null,
		const QString&  _start   = QString::null);

ACGDLLEXPORT
QString
getOpenMeshName(QWidget*        _parent  = 0, 
		const QString&  _caption = QString::null,
		const QString&  _start   = QString::null);

ACGDLLEXPORT
QStringList
getOpenFileNames(QWidget*        _parent  = 0,
		 const QString&  _caption = QString::null,
		 const QString&  _filter  = QString::null,
		 const QString&  _start   = QString::null);

ACGDLLEXPORT
QStringList
getOpenMeshNames(QWidget*        _parent  = 0,
		 const QString&  _caption = QString::null,
		 const QString&  _start   = QString::null);

ACGDLLEXPORT
QString
getSaveFileName(QWidget*        _parent  = 0,
		const QString&  _caption = QString::null,
		const QString&  _filter  = QString::null,
		bool            _askOverwrite=false,
		const QString&  _start   = QString::null);

ACGDLLEXPORT
QString
getSaveMeshName(QWidget*        _parent  = 0,
		const QString&  _caption = QString::null,
		bool            _askOverwrite=false,
		const QString&  _start   = QString::null);


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_QTFILEDIALOG_HH defined
//=============================================================================

