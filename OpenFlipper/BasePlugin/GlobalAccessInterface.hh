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

#ifndef GLOBALACCESSINTERFACE_HH 
#define GLOBALACCESSINTERFACE_HH 

 #include <QtGui>
 #include <QMenuBar>
 #include <ACG/QtWidgets/QtExaminerViewer.hh>
 #include <OpenFlipper/common/Types.hh>
 
 /** \brief Interface class for plugins which need access to global Data
  * 
  * Do not use this Interface!!!!
 */
class GlobalAccessInterface {
      
   public : 
       
      /// Destructor
      virtual ~GlobalAccessInterface() {};
      
      /** \brief DONT USE THIS (Get a pointer to the examiner Widget from Main App)
       * 
       *  This function is called to set a pointer to the global examiner Widget.\n
       *  @param _examiner_widget Pointer to the Examiner Widget in the Main app
       */
      virtual void set_examiner(ACG::QtWidgets::QtExaminerViewer* /*_examiner_widget*/) = 0;
      
};

Q_DECLARE_INTERFACE(GlobalAccessInterface,"OpenFlipper.GlobalAccessInterface/0.3")
      
#endif // GLOBALACCESSINTERFACE_HH
