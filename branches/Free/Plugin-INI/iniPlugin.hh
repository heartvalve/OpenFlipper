/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
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

#ifndef INIPLUGIN_HH
#define INIPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>

class INIPlugin : public QObject, BaseInterface, LoggingInterface, ScriptInterface, INIInterface
{
    Q_OBJECT
    Q_INTERFACES(LoggingInterface)
    Q_INTERFACES(BaseInterface)
    Q_INTERFACES(ScriptInterface)
    Q_INTERFACES(INIInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-INI")
#endif

    signals:
        void log(Logtype _type, QString _message);
        void log(QString _message);
        
    private slots:
        void loadIniFile( INIFile& _ini ,int _id);
        void saveIniFile( INIFile& _ini ,int _id);
        
        void noguiSupported( ) {} ;
        
    public :
        
        INIPlugin();
        ~INIPlugin() {};
        
    public slots:
        
        QString name() { return (QString("INIPlugin")); };
        QString description( ) { return (QString(tr("Handle INI-files."))); };
        
        QString version() { return QString("1.0"); };
        
    private:
        // Template function to parse ini file entries
        template <class Object>
        void parseIniFileT(INIFile& _ini, Object* _object);
        
        // Template function to save ini file entries
        template <class Object>
        void saveIniFileT(INIFile& _ini, Object* _object);
        
        
        // Parse data from baseObjectData derived objects
        void parseIniFile(INIFile& _ini, BaseObjectData* _object);
        
        // Save data from baseObjectData derived objects
        void saveIniFile(INIFile& _ini, BaseObjectData* _object);
};

//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(INIPLUGINT_C)
#define INIPLUGINT_TEMPLATES
#include "iniPluginT.cc"
#endif
//=============================================================================

#endif //INIPLUGIN_HH
