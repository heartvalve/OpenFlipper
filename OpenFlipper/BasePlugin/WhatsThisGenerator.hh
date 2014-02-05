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

#ifndef WHATSTHISGENERATOR_HH_
#define WHATSTHISGENERATOR_HH_

#include <QAction>
#include <QString>
#include <QObject>
#include <OpenFlipper/common/GlobalDefines.hh>

/** \brief a class which provides an link generator for WhatsThisMessages linking to the user doc
 *	If you have an QAction pointer called action, you can call:
 *	 \code{.cpp}action->setWhatsThis("Hello");\endcode
 *	This message "Hello" will be shown when somebody call this action in WhatsThisMode.
 *
 *	WhatsThisGenerator provides functions linking to the user documentation.
 *		e.g. assume that whatsThisGen is a valid WhatsThisGenerator object
 *		\code{.cpp}action->setWhatsThis(QString(tr("Hello"))+whatsThisGen.generateLink()); \endcode
 *	The code above will change the output for WhatsThisMessages to: "Hello Click here for more information"
 *	where "Click here for more information" is a link which will call the index.html (default parameter) of the plugin
 *	which you have to specify in the constructor of WhatsThisGenerator.
 *
 *	You can also use the WhatsThisGenerator to set the WhatsThisMessage:
 *		\code{.cpp}whatsThisGen.setWHatsThis(action,tr("Hello"));\endcode
 *	It has the same effect and options like the code above and is shorter.
 */
class DLLEXPORT WhatsThisGenerator: public QObject {

  Q_OBJECT

  const QString plugin_name_;

public:
  WhatsThisGenerator(const QString &_plugin_name);

  QString generateLink(const QString &_ref = "", const QString &_site = "index.html") const;

  void setWhatsThis(
      QAction* _action,
      const QString &_msg,
      const QString &_ref = "",
      const QString &_site = "index.html") const;

};


#endif /* WHATSTHISGENERATOR_HH_ */
