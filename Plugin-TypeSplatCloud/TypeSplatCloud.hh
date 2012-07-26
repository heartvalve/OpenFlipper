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

//================================================================
//
//  CLASS TypeSplatCloudPlugin
//
//    This class represents an OpenFlipper plugin for a new node named SplatCloud.
//
//================================================================


#ifndef TYPESPLATCLOUDPLUGIN_HH
#define TYPESPLATCLOUDPLUGIN_HH


//== INCLUDES ====================================================


#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>

#include <OpenFlipper/common/Types.hh>

#include <ObjectTypes/SplatCloud/SplatCloud.hh>


//== CLASS DEFINITION ============================================


class TypeSplatCloudPlugin : public QObject, BaseInterface, LoggingInterface, TypeInterface
{
  Q_OBJECT
  Q_INTERFACES( BaseInterface     )
  Q_INTERFACES( LoggingInterface  )
  Q_INTERFACES( TypeInterface     )

signals:

  //-- Type Interface --
  void emptyObjectAdded( int _objectId );

  //-- Logging Interface --
  void log(                QString _message );
  void log( Logtype _type, QString _message );

private slots:

  // -- Base Inferface --
  void noguiSupported() { }
  void slotViewChanged();

public:

  // standard constructor/destructor
  TypeSplatCloudPlugin() { }
  ~TypeSplatCloudPlugin() { }

  //-- Base Interface --
  QString name()        { return QString(     "TypeSplatCloud"             ); };
  QString description() { return QString( tr( "Register SplatCloud type" ) ); };

  // -- Type Interface --
  bool registerType();

public slots:

  //-- Base Interface --
  QString version() { return QString( "1.0" ); };

  //-- Type Interface --
  int addEmpty();
  DataType supportedType() { return DATA_SPLATCLOUD; }; 
  void generateBackup( int _objectId, QString _name, UpdateType _type );

};


//================================================================


#endif // TYPESPLATCLOUDPLUGIN_HH
