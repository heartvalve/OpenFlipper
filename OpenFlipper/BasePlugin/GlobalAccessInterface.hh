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

#ifndef GLOBALACCESSINTERFACE_HH
#define GLOBALACCESSINTERFACE_HH

 #include <QtGui>
 #include <QMenuBar>
 #include <OpenFlipper/common/Types.hh>
 #include <OpenFlipper/widgets/glWidget/QtBaseViewer.hh>

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
      virtual void set_examiner( glViewer* /*_examiner_widget*/) = 0;

};

Q_DECLARE_INTERFACE(GlobalAccessInterface,"OpenFlipper.GlobalAccessInterface/0.3")

#endif // GLOBALACCESSINTERFACE_HH
