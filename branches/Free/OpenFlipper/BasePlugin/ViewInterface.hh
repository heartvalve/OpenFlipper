/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/

#ifndef VIEWINTERFACE_HH
#define VIEWINTERFACE_HH

 
#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <QMenuBar>
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
     virtual void getStackWidget( QString _name, QWidget*& _widget ) {};

    /** Update a widget form the Stackwidget with Name.       *
      * @param _name Name of the Widget
      * @param _widget The requested widget or 0 if not found
      */
     virtual void updateStackWidget( QString _name, QWidget* _widget ) {};

    /** Add a widget to the Stackwidget with a Name.
      *
      * @param _name Name of the Widget
      * @param _widget The new widget
    */
     virtual void addStackWidget( QString _name, QWidget* _widget ) {};

   public :

      /// Destructor
      virtual ~ViewInterface() {};
};

Q_DECLARE_INTERFACE(ViewInterface,"OpenFlipper.ViewInterface/1.0")

#endif // VIEWINTERFACE_HH
