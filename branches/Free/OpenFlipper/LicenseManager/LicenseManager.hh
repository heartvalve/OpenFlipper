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

#include "OpenFlipper/BasePlugin/SecurityInterface.hh"

class LicenseManager : public QObject, SecurityInterface  {

Q_OBJECT
Q_INTERFACES(SecurityInterface)

  public:

    virtual ~LicenseManager();
    LicenseManager();

  public :

    void blockSignals( bool _state);
    bool authenticated();

    virtual QString name() = 0;

  public slots:
    bool authenticate();

  private:
    bool authenticated_;

    virtual QString pluginFileName();


  protected :
    void connectNotify ( const char * /*signal*/ );

};

#endif // LICENSEMANAGER_HH