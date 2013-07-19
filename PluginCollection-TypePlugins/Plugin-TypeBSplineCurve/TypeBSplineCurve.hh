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
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>

#include <ObjectTypes/BSplineCurve/BSplineCurve.hh>


class TypeBSplineCurvePlugin : public QObject, BaseInterface, TypeInterface, LoggingInterface, LoadSaveInterface, ContextMenuInterface
{
   Q_OBJECT
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(TypeInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(ContextMenuInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-TypeBSplineCurve")
#endif

  signals:

    // Logging Interface
    void log(Logtype _type, QString _message);
    void log(QString _message);
    
    // LoadSave Interface
    void emptyObjectAdded( int _id );

    // ContextMenuInterface
    void addContextMenuItem(QAction* _action , ContextMenuType _type);
    void addContextMenuItem(QAction* _action , DataType _objectType , ContextMenuType _type );

  private slots:

    // BaseInterface
    void pluginsInitialized();
    void noguiSupported( ) {} ;

    // ContextMenuInterface
    void slotUpdateContextMenu( int _objectId );

  private slots:

    /// Slot triggered from context menu, if the control polygon should be rendered
    void slotRenderControlPolygon();

    /// Slot triggered from context menu, if the curve should be rendered
    void slotRenderCurve();

    /// Slot triggered from context menu, if the selection rendering should be altered
    void slotRenderSelection(QAction* _action);

  public :

     ~TypeBSplineCurvePlugin() {};
     TypeBSplineCurvePlugin();

     QString name() { return (QString("TypeBSplineCurve")); };
     QString description( ) { return (QString(tr("Register BSpline Curve type "))); };
     
     bool registerType();
     
     DataType supportedType();
     
     int addEmpty( );

  public slots:

    // Type Interface
    void generateBackup( int _id, QString _name, UpdateType _type );
 
  public slots:

    QString version() { return QString("1.1"); };

  private:
    /// Context menu action
    QAction* renderControlPolygonAction_;

    /// Context menu action
    QAction* renderCurveAction_;

    /// Context menu action (render selection texture)
    QAction* renderCPSelectionAction_;
    QAction* renderKnotSelectionAction_;
    QAction* renderNoSelectionAction_;

};

#endif //TYPEBSPLINECURVEPLUGIN_HH
