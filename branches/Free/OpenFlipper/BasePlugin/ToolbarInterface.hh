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
//   $Revision: 1909 $
//   $Author: wilden $
//   $Date: 2008-06-03 18:45:21 +0200 (Tue, 03 Jun 2008) $
//
//=============================================================================

//
// C++ Interface: ToolbarInterface
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef TOOLBARINTERFACE_HH
#define TOOLBARINTERFACE_HH

 #include <QtGui>
 #include <QMenuBar>
 #include <QStringList>
 #include <ACG/QtWidgets/QtExaminerViewer.hh>
 #include <OpenFlipper/common/Types.hh>
 #include <QToolBar>

 /**
 * This Interface should be used by plugins which will provide a toolbar.
 * Each Plugin can create own Toolbars. Emit 
 */
class ToolbarInterface {

   private slots :

   public :

      /// Destructor
      virtual ~ToolbarInterface() {};

    signals :

      /** \brief Adds a Toolbar to the main widget
      *
      * To create your own Toolbar, emit this signal with a pointer to your Toolbar.
      * @param _toolbar Your Toolbar
      */
      virtual void addToolbar(QToolBar* /*_toolbar*/) {};
      
      
      /** \brief Remove a Toolbar from the main widget
      *
      * @param _toolbar Toolbar to be removed
      */
      virtual void removeToolbar(QToolBar* /*_toolbar*/) {};


};

Q_DECLARE_INTERFACE(ToolbarInterface,"OpenFlipper.ToolbarInterface/1.0")

#endif // TOOLBARINTERFACE_HH
