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
//  CLASS Type BSpline Surface Plugin
//
//  Author:  Ellen Dekkers <dekkers@cs.rwth-aachen.de>
//
//  Date: 2009-12-14 19:18:57 +0100 (Mo, 14. Dez 2009)
//
//=============================================================================


#ifndef TYPEBSPLINESURFACEPLUGIN_HH
#define TYPEBSPLINESURFACEPLUGIN_HH

#include <QObject>


#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/TypeInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>

#include <ObjectTypes/BSplineSurface/BSplineSurface.hh>

class TypeBSplineSurfacePlugin : public QObject, BaseInterface, TypeInterface, LoggingInterface, LoadSaveInterface, ContextMenuInterface
{
   Q_OBJECT
   Q_INTERFACES(BaseInterface)
   Q_INTERFACES(TypeInterface)
   Q_INTERFACES(LoggingInterface)
   Q_INTERFACES(LoadSaveInterface)
   Q_INTERFACES(ContextMenuInterface)

  signals:

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

    /// Slot triggered from context menu, if the control net should be rendered
    void slotRenderControlNet();

    /// Slot triggered from context menu, if the surface should be rendered
    void slotRenderSurface();

    /// Slot triggered from context menu, if the selection rendering should be altered
    void slotRenderSelection(QAction* _action);

  public :

     ~TypeBSplineSurfacePlugin() {};
     TypeBSplineSurfacePlugin();

     QString name() { return (QString("TypeBSplineSurface")); };
     QString description( ) { return (QString(tr("Register BSpline Surface type "))); };
     
     bool registerType();

     DataType supportedType();
     
     int addEmpty();
     
  public slots:

    QString version() { return QString("1.1"); };

  private:
    /// Context menu action
    QAction* renderControlNetAction_;

    /// Context menu action
    QAction* renderSurfaceAction_;

    /// Context menu action (render selection texture)
    QAction* renderCPSelectionAction_;
    QAction* renderKnotSelectionAction_;
    QAction* renderNoSelectionAction_;

};

#endif //TYPEBSPLINESURFACEPLUGIN_HH
