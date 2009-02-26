//=============================================================================
//
//                               OpenFlipper
//        Copyright (C) 2008 by Computer Graphics Group, RWTH Aachen
//                           www.openflipper.org
//
//-----------------------------------------------------------------------------
//
//                                License
//
//  OpenFlipper is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  OpenFlipper is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
// 
//  You should have received a copy of the GNU Lesser General Public License
//  along with OpenFlipper.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




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

