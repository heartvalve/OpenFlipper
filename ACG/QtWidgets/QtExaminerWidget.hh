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
//  CLASS QtExaminerWidget
//
//=============================================================================

#ifndef ACG_QTEXAMINERWIDGET_HH
#define ACG_QTEXAMINERWIDGET_HH


//== INCLUDES =================================================================

#include <ACG/QtWidgets/QtExaminerViewer.hh>


//== NAMESPACES ===============================================================


namespace ACG {
namespace QtWidgets {
  

//== CLASS DEFINITION =========================================================


#ifndef ACG_HIDE_DEPRECATED
#warning QtExaminerWidget is deprecated, use QtExaminerViewer instead.
typedef QtExaminerViewer QtExaminerWidget;
#else
#error QtExaminerWidget is deprecated, use QtExaminerViewer instead.
#endif


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
#endif // ACG_QTEXAMINERWIDGET_HH defined
//=============================================================================

