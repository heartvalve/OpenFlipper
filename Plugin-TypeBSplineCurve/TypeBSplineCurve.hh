/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen       *
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

//=============================================================================
//
//  CLASS Type BSpline Curve Plugin
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  Date: 2009-12-14 19:18:48 +0100 (Mo, 14. Dez 2009)
//
//=============================================================================


#ifndef TYPEBSPLINECURVEPLUGIN_HH
#define TYPEBSPLINECURVEPLUGIN_HH

#include <QObject>


#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>

#include <ObjectTypes/BSplineCurve/BSplineCurve.hh>


class TypeBSplineCurvePlugin : public QObject, BaseInterface, TypeInterface, LoggingInterface, LoadSaveInterface
{
   Q_OBJECT
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(TypeInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(LoadSaveInterface)

  signals:

    void log(Logtype _type, QString _message);
    void log(QString _message);
    
    // LoadSave Interface
    void emptyObjectAdded( int _id );

  private slots:

    void noguiSupported( ) {} ;

  public :

     ~TypeBSplineCurvePlugin() {};
     TypeBSplineCurvePlugin();

     QString name() { return (QString("TypeBSplineCurve")); };
     QString description( ) { return (QString(tr("Register BSpline Curve type "))); };
     
     bool registerType();
     
     DataType supportedType();
     
     int addEmpty( );

  public slots:

    QString version() { return QString("1.0"); };

};

#endif //TYPEBSPLINECURVEPLUGIN_HH
