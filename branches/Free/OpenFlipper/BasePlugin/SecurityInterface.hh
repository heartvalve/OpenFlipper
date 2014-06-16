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

#ifndef SECURITYINTERFACE_HH
#define SECURITYINTERFACE_HH


#if QT_VERSION >= 0x050000 
  #include <QtWidgets>
#else
  #include <QtGui>
#endif

#include <OpenFlipper/common/Types.hh>


/** \file SecurityInterface.hh
*
* This interface is used to add copy protection to plugins. \ref securityInterfacePage
* Usually you don't include this file but OpenFlipper/LicenseManager/LicenseManager.hh
*/


/** \brief Interface class for adding copy protection and license management to a plugin.

\n
\ref securityInterfacePage "Detailed description"
\n

This interface is used to add copy protection to plugins.
*/
class SecurityInterface {

  public:

    virtual ~SecurityInterface(){};
    SecurityInterface(){};

  public slots:
    /// Function starting the authentication process
    bool    authenticate();
    
    /// If authenticate returns fails, this string will contain the license info
    QString licenseError();

  private :
    bool authenticated();

};

/** \page securityInterfacePage Security/License Interface
\image html SecurityInterface.png
\n

\section Functionality
The security Interface can be used to add a license management to your plugin. When it is used,
plugins will only start, if a valid license is available. The license is bound to a machine
(Processor,network,...) and expires after a specified date.

\section security_interface_usage Usage
To use this interface do the following steps:

\subsection license_includes Includes
Add the LicenseManager include to your plugin header file.

\code
#include <OpenFlipper/LicenseManager/LicenseManager.hh>
\endcode

\subsection license_licensemanager_integration QObject Replacement
Don't derive your Plugin from QObject anymore but from LicenseManager
which has to be the first one in your class definition.

\code
// Before:
class ExamplePlugin : public QObject, BaseInterface, MenuInterface, ScriptInterface, ToolbarInterface, StatusbarInterface

// After:
class ExamplePlugin : public LicenseManager, BaseInterface, MenuInterface, ScriptInterface, ToolbarInterface, StatusbarInterface
\endcode

\subsection cmake_license_changes Cmake Change
Additionally add the option LICENSEMANAGER to your CMakeLists generator e.g.:

\code
openflipper_plugin (LICENSEMANAGER ) )
\endcode

\subsection salt_file_setup Salt File
Copy the example sat file from OpenFlipper/LicenseManager/salt.hh.example to your plugin directory and rename it
to salt.hh ( e.g. Plugin-Example/salt.hh ). Modify the contents of your salt file to match your plugin:

The contact line will be presented to the user when a license has to be retrieved. The user gets the option
to start the mail program with this destination address and the license request.
\code
#define CONTACTMAIL QString("contact@openflipper.org")
\endcode

Afterwards modify the ADD_SALT_PRE and ADD_SALT_POST macros. Set the characters to some numbers you want.
These salts will basically define a password that will be added to the hashes. It has to be kept !!secret!! as
everybody who knows these salts, can generate licenses for your plugin. So don't give away your salt file. It is
only used when compiling your plugin and the corresponding license manager.

The last thing to change is the ADD_PLUGIN_FILENAME macro. Specify the filename of your plugin here, omitting the ending
as this will be added platform dependent by OpenFlipper.


\section license_genration License generation
When the plugin is now build, an additional LicenseManager is generated which shows
a widget on execution. It will be in your build directory in the subdirectory LicenseManagement.

To generate a license, copy the license request you received into the widget. Remove all text lines.
The manager will split the request into its separate strings. Enter a date, until the license should be valid
and generate it.

The generated license has to be copied into OpenFlippers License subdirectory.

*/

Q_DECLARE_INTERFACE(SecurityInterface,"OpenFlipper.SecurityInterface/1.0")

#endif // SECURITYINTERFACE_HH
