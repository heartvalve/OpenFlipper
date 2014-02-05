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

#ifndef LICENSEMANAGER_HH
#define LICENSEMANAGER_HH

#include <OpenFlipper/BasePlugin/SecurityInterface.hh>

/** \file LicenseManager.hh
*
* This interface is used to add copy protection to plugins. \ref securityInterfacePage
*
*/

/* The salt file has to be provided for each plugin. It can be the same
  for all plugins. See example for details on how this file has to be setup
*/
#include "salt.hh"

/** \brief License management base class
 *
 * See \ref securityInterfacePage for Details on how to use it.
 *
 * The class is used by plugins to integrate license management. It will check the license,
 * generate license requests and prevent the plugin from loading if an invalid license or no license
 * is found.
 */
class LicenseManager : public QObject, SecurityInterface  {

Q_OBJECT
Q_INTERFACES(SecurityInterface)

  public:

    virtual ~LicenseManager();
    LicenseManager();

  public :

    /** This function is overloaded and will not allow to unblock signals
        if the plugin is not authenticated.
    */
    void blockSignals( bool _state);
    
    /** Return if the plugin has successfully passed the authentication.
    */
    bool authenticated();

  public slots:
    /** Call this function for plugin authentication. If it returns true,
        the authentication has been successful. Otherwise the core will
        stop loading the plugin. Additionally the plugin will make itself
        unusable by not allowing any signal slot connections.

    */
    bool authenticate();
    
    /** if authenticate returns false, this string will contain the license information required
     *  to generate a license request and the error that caused the failure.
    */
    QString licenseError();

  private:
    /** This is used to get the plugins Name from derived classes
        The plugin name is the usual name of the plugin
    */
    virtual QString name() = 0;

    /** This function is special to the LicenseManager. It is used to
        find the plugin when checking its hash value.
    */
    virtual QString pluginFileName();

    /// This flag is true if authentication was successful
    bool authenticated_;
    
    /// License information string
    QString authstring_;


  protected :
    /** This function is overloaded in the security interface. If the
        plugin is not authenticated, all connections will be automatically
        removed again.
    */
    void connectNotify ( const char * signal );

};

#endif // LICENSEMANAGER_HH
