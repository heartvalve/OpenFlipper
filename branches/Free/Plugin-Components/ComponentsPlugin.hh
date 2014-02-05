/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openflipper.org                             *
 *                                                                           *
 *---------------------------------------------------------------------------*
 *  This file is part of OpenFlipper.                                        *
 *                                                                           *
 *  OpenFlipper is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenFlipper is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenFlipper. If not,                                  *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/

/*===========================================================================*\
 *                                                                           *
 *   $Revision: 15115 $                                                       *
 *   $Author: moebius $                                                       *
 *   $Date: 2012-07-26 14:40:06 +0200 (Thu, 26 Jul 2012) $                   *
 *                                                                           *
\*===========================================================================*/

#ifndef COMPONENTSPLUGIN_HH
#define COMPONENTSPLUGIN_HH

#include <QObject>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

class ComponentsPlugin : public QObject, BaseInterface, MouseInterface, PickingInterface, LoggingInterface, BackupInterface, LoadSaveInterface, ToolbarInterface,ScriptInterface
{
   Q_OBJECT
   Q_INTERFACES( BaseInterface )
   Q_INTERFACES( MouseInterface )
   Q_INTERFACES( PickingInterface )
   Q_INTERFACES( LoggingInterface )
   Q_INTERFACES( BackupInterface )
   Q_INTERFACES( LoadSaveInterface )
   Q_INTERFACES( ToolbarInterface )
   Q_INTERFACES( ScriptInterface )

  signals:
    // BaseInterface
    void updateView();
    void updatedObject(int _identifier, const UpdateType _type);
    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

    // LoadSaveInterface
    void addEmptyObject( DataType _type, int& _id);
    void copyObject( int _oldId, int& _newId);

    // ToolBarInterface
    void addToolbar(QToolBar* _toolbar);
    void getToolBar( QString _name, QToolBar*& _toolbar);

    // PickingInterface
    void addPickMode( const std::string _mode );

    //ScriptInterface
    void scriptInfo( QString _functionName );

    // LoggingInterface
    void log( Logtype _type, QString _message );
    void log( QString _message );

    // BackupInterface
    void createBackup( int _objectid, QString _name, UpdateType _type);

    // LoadSaveInterface
    void deleteObject( int _id );

  private slots:

    // BaseInterface
    void initializePlugin();
    void pluginsInitialized();

    // PickingInterface
    void slotPickModeChanged( const std::string& _mode);

    // MouseInterface:
    void slotMouseEvent( QMouseEvent* _event );

  public:
    ComponentsPlugin();
    ~ComponentsPlugin() { }

    void exit() {}

    QString name() { return (QString("Components")); };
    QString description( ) { return (QString("Handle mesh components")); };


//===========================================================================
/** @name GUI Related
  * @{ */
//===========================================================================

  private slots:
    /// Split into Components Button was hit
    void slotSplitComponentsButton();
    
    void slotBiggestComponentButton();

    /// Split Components of picked object
    void splitComponents(QMouseEvent* _event);

    void biggestComponent(QMouseEvent* _event);

  private :
    //toolbar
    QAction* splitAction_;
    QAction* biggestAction_;

/** @} */

//===========================================================================
/** @name Template Functions
  * @{ */
//===========================================================================

  /// Split mesh into components
  template< class MeshT >
  void splitComponent( MeshT*   _mesh, MeshT* _copy);

  template< class MeshT >
  void selectBiggestComponent( MeshT*   _mesh);

  template< class MeshT >
  void isolateBiggestComponent( MeshT*   _mesh);

/** @} */

//===========================================================================
/** @name Scripting Functions
  * @{ */
//===========================================================================

   public slots:

      /// Split a mesh into components
      IdList splitComponents( int _objectId  );

      void biggestComponent(int _objId);

   public slots:
      QString version() { return QString("1.0"); };

/** @} */

   private:
      /// set scripting slot descriptions
      void setDescriptions();
};

#if defined(INCLUDE_TEMPLATES) && !defined(COMPONENTSPLUGIN_CC)
#define COMPONENTSPLUGIN_TEMPLATES
#include "ComponentsPluginT.cc"
#endif
//=============================================================================
#endif  //CUTPLUGIN_HH
//=============================================================================
