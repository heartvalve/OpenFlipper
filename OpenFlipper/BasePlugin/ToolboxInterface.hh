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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/




//
// C++ Interface: BasePlugin
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef TOOLBOXINTERFACE_HH
#define TOOLBOXINTERFACE_HH

 #include <QtGui>
 #include <QMenuBar>
 #include <QStringList>
 #include <OpenFlipper/common/Types.hh>

 /**
  * \brief Plugins can add its own toolbox to the main widget's toolbox area by using this interface.
  *
  * This Interface should be used by plugins which will provide a toolbox widget.
  * Each Plugin can create own Widgets in the Toolbox area. Just create your widget and then add it with \n
  * emit addToolbox( QString  _name  , QWidget* _widget ) \n
  * to the user interface. You can create extra Signals and slots in your Plugin.
  * These can be connected between your toolbox and your plugin. Signals and slots across plugins are currently
  * not available but may be possible via an internal message system later.\n
  *
  * See our tutorials \ref ex2 and \ref ex3 for an example of to add custom toolboxes to
  * OpenFlipper.
  */
class ToolboxInterface {

   private slots :

   public :

      /// Destructor
      virtual ~ToolboxInterface() {};

    signals :
        
      /** \brief Add a toolbox widget to the gui with the given name
       *
       * This signal adds a toolbox widget to the toolbar on the right.
       */
      virtual void addToolbox( QString /* _name */ , QWidget* /*_widget*/ ) {};

};

Q_DECLARE_INTERFACE(ToolboxInterface,"OpenFlipper.ToolboxInterface/1.1")

#endif // TOOLBOXINTERFACE_HH
