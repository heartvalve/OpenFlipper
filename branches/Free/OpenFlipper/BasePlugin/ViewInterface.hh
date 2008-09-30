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




//
// C++ Interface: BasePlugin
//
// Description: 
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef VIEWINTERFACE_HH 
#define VIEWINTERFACE_HH 

 #include <QtGui>
 #include <QMenuBar>
 #include <ACG/QtWidgets/QtExaminerViewer.hh>
 #include <OpenFlipper/common/Types.hh>
 
 /**
   This Interface should be used by plugins which will provide an additional view widget.
 */
class ViewInterface {
      
   signals :
     
     /** Get a widget form the Stackwidget by Name. The mainwidget is named "3D Examiner Widget".
      * 
      * @param _name Name of the Widget
      * @param _widget The requested widget or 0 if not found
      */
     virtual void getStackWidget( QString /*_name*/, QWidget*& /*_widget*/ ) {};
     
    /** Update a widget form the Stackwidget with Name.       * 
      * @param _name Name of the Widget
      * @param _widget The requested widget or 0 if not found
      */
     virtual void updateStackWidget( QString /*_name*/, QWidget* /*_widget*/ ) {};
     
    /** Add a widget to the Stackwidget with a Name.
      * 
      * @param _name Name of the Widget
      * @param _widget The new widget
    */
     virtual void addStackWidget( QString /*_name*/, QWidget* /*_widget*/ ) {};
      
   public : 
       
      /// Destructor
      virtual ~ViewInterface() {};
};

Q_DECLARE_INTERFACE(ViewInterface,"OpenFlipper.ViewInterface/1.0")
      
#endif // VIEWINTERFACE_HH
