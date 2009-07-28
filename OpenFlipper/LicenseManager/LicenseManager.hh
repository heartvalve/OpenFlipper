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
//   $Revision: 4736 $
//   $Author: kremer $
//   $Date: 2009-02-10 11:34:14 +0100 (Di, 10. Feb 2009) $
//
//=============================================================================




//
// C++ Interface: SecurityInterface
//
// Description:
//
//
// Author: Jan Moebius <jan_moebius@web.de>, (C) 2007
//

#ifndef LICENSEMANAGER_HH
#define LICENSEMANAGER_HH

#include <OpenFlipper/BasePlugin/SecurityInterface.hh>

/** The salt file has to be provided for each plugin. It can be the same
  for all plugins. See example for details on how this file has to be setup
*/
#include "salt.hh"

class LicenseManager : public QObject, SecurityInterface  {

Q_OBJECT
Q_INTERFACES(SecurityInterface)

  public:

    virtual ~LicenseManager();
    LicenseManager();

  public :

    /** This function is overloaded and will not allow to unblock signals
        if the plugin is not authenticated
    */
    void blockSignals( bool _state);
    
    /** Return if the plugin has successfully passed the authentication 
    */
    bool authenticated();

  public slots:
    /** Call this function for plugin authentication. If it returns true,
        the authentication has been successfull. Otherwise the core will 
        stop loading the plugin. Additionally the plugin will make itself
        unusable by not allowing any signal slot connections.
    */
    bool authenticate();

  private:
    /** This is used to get the plugins Name from derived classes
        The glugin name is the usual name of the glugin
    */
    virtual QString name() = 0;

    /** This function is special to the LicenseManager. It is used to
        find the plugin when checking its hash value
    */
    virtual QString pluginFileName();

    /// This flag is true if authentication was successfull
    bool authenticated_;


  protected :
    /** This function is overloaded in the security interface. If the
        plugin is not authenticated, all connections will be automatically
        removed again.
    */
    void connectNotify ( const char * /*signal*/ );

};

#endif // LICENSEMANAGER_HH