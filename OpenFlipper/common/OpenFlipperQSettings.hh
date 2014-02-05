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
*   $Revision: 10413 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2010-12-09 15:40:04 +0100 (Thu, 09 Dec 2010) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef OPENLFLIPPERQSETTINGS_HH
#define OPENLFLIPPERQSETTINGS_HH

#include <QSettings>
#include <OpenFlipper/common/GlobalDefines.hh>

class DLLEXPORT OpenFlipperQSettings : public QSettings
{
public:
    explicit OpenFlipperQSettings(const QString &organization,
                                  const QString &application = QString(),
                                  QObject *parent = 0) : QSettings(organization, application, parent) {};

    OpenFlipperQSettings(QSettings::Scope scope,
                         const QString &organization,
                         const QString &application = QString(),
                         QObject *parent = 0) : QSettings(scope, organization, application, parent) {};

    OpenFlipperQSettings(QSettings::Format format,
                         QSettings::Scope scope, const QString &organization,
	                       const QString &application = QString(),
	                       QObject *parent = 0) : QSettings (format, scope, organization, application, parent) {};

    OpenFlipperQSettings(const QString &fileName,
                         QSettings::Format format,
                         QObject *parent = 0) : QSettings(fileName, format, parent) {};

    explicit OpenFlipperQSettings(QObject *parent = 0) : QSettings(parent) {};
    
    /// Wrapper function for QSettings::value() that outputs an error message to console if the key was not found and no default value given.
    /// This is supposed to prevent Plugin authors from forgetting to specify the defaultValue. ;)
    /// Additionally it is possible to enable Debugging output with -DOPENFLIPPER_SETTINGS_DEBUG
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

    /// Wrapper function which makes it possible to enable Debugging output with -DOPENFLIPPER_SETTINGS_DEBUG
    void setValue ( const QString & key, const QVariant & value );

};

#endif // OPENFLIPPERQSETTINGS_HH
