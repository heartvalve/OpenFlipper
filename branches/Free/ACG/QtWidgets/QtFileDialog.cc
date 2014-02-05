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
//  CLASS QtFileDialog - IMPLEMENTATION
//
//=============================================================================

//== INCLUDES =================================================================

#include "QtFileDialog.hh"

#include <OpenMesh/Core/IO/IOManager.hh>

#include <QFileDialog>
#include <QMessageBox>
#include <QString>


//== NAMESPACES ===============================================================

namespace ACG {

//== IMPLEMENTATION ========================================================== 


QString
getOpenFileName(QWidget*        _parent, 
		const QString&  _caption,
		const QString&  _filter,
		const QString&  _start)
{
  return  
    QFileDialog::getOpenFileName( _parent,  // parent
				  _caption, // caption
				  _start,   // dir
				  _filter,  // filter
				  0,        // selected filter
				  0        // options
				  );
}


QString
getOpenMeshName(QWidget*        _parent, 
		const QString&  _caption,
		const QString&  _start)
{
  return  
    ACG::getOpenFileName(_parent, 
			 _caption,
			 OpenMesh::IO::IOManager().qt_read_filters().c_str(),
			 _start);
}


//-----------------------------------------------------------------------------


QStringList
getOpenFileNames(QWidget*        _parent,
		 const QString&  _caption,
		 const QString&  _filter,
		 const QString&  _start)
{
  return
    QFileDialog::getOpenFileNames( _parent, // parent
				   _caption, // caption
				   _start, // dir
				   _filter, //_filter
				   0, // selected filter
				   0 // options
				   );
}


QStringList
getOpenMeshNames(QWidget*        _parent,
		 const QString&  _caption,
		 const QString&  _start)
{
  return
    ACG::getOpenFileNames(_parent, 
			  _caption,
			  OpenMesh::IO::IOManager().qt_read_filters().c_str(),
			  _start);
}


//-----------------------------------------------------------------------------


QString
getSaveFileName(QWidget*        _parent, 
		const QString&  _caption, 
		const QString&  _filter, 
		bool            _askOW,
		const QString&  _start)
{
  QString filename = 
    QFileDialog::getSaveFileName ( _parent, // parent
				   _caption, // caption
				   _start, // dir
				   _filter, // filter,
				   0, // selected filter
				   0 // options
				   );

  if (_askOW && !filename.isEmpty() && QFile(filename).exists())
  {
    QString s;
    s += QString("The file\n  ");
    s += filename;
    s += QString("\nalready exists.\n\n");
    s += QString("Do you want to overwrite it?");

    if (QMessageBox::warning(_parent, "Overwrite", s,
			     QMessageBox::Yes | QMessageBox::Default,
			     QMessageBox::No  | QMessageBox::Escape)
	!= QMessageBox::Yes)
      return QString::null;
  }

  return filename;
}


QString
getSaveMeshName(QWidget*        _parent, 
		const QString&  _caption, 
		bool            _askOW,
		const QString&  _start)
{
  return
    ACG::getSaveFileName(_parent, 
			 _caption,
			 OpenMesh::IO::IOManager().
			 qt_write_filters().c_str(),
			 _askOW,
			 _start);
}


//=============================================================================
} // namespace ACG
//=============================================================================
