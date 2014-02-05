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




#ifndef DATACONTROLPLUGIN_HH
#define DATACONTROLPLUGIN_HH


#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/Light/Light.hh>
#include "Toolbox.hh"

#include "TreeModel.hh"
#include "DefaultObjectMarker.hh"


/** Plugin for controlling the Object data structures (Show,hide Object, Target, Source selection
*/
class DataControlPlugin : public QObject, BaseInterface, ToolboxInterface, KeyInterface , LoggingInterface, INIInterface, ContextMenuInterface, ScriptInterface, LoadSaveInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(INIInterface)
  Q_INTERFACES(ContextMenuInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(LoadSaveInterface)

#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-DataControl")
#endif

  signals:
    // BaseInterface
    void updateView();
    void updatedObject(int _it, const UpdateType& _type);

    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);

    // LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);
    
    // ContextMenuInterface
    void addContextMenuItem(QAction* _action , ContextMenuType _type);
    void addContextMenuItem(QAction* _action , DataType _objectType , ContextMenuType _type );
    
    // LoadSaveInterface
    void deleteObject( int _id );
    void emptyObjectAdded( int _id );
    void copyObject( int _oldId, int& _newId);
    
    // ToolboxInterface
    void addToolbox( QString _name  , QWidget* _widget, QIcon* _icon );

  private slots :
    // BaseInterface
    void initializePlugin();
    void pluginsInitialized();
    void slotVisibilityChanged      ( int _identifier );
    void slotObjectSelectionChanged ( int _identifier );
    void slotObjectPropertiesChanged( int _identifier );

    // KeyInterface
    void slotKeyEvent( QKeyEvent* _event );

    // INIInterface
    void loadIniFileOptionsLast( INIFile& _ini );
    void saveIniFileOptions( INIFile& _ini );
    void saveOnExit(INIFile& _ini );

    // LoadSaveInterface
    void fileOpened(int _id);
    void addedEmptyObject(int _id);
    void objectDeleted(int _id);

    // Tell system that this plugin runs without ui
    void noguiSupported( ) {} ;

  public :

    /// Constructor
    DataControlPlugin();

    /// Destructor
    ~DataControlPlugin() {};

    /// Name of the Plugin
    QString name(){ return (QString("DataControl")); };

    /// Description of the Plugin
    QString description() { return (QString(tr("Manages Data Objects"))); };

  private :
    /// Widget for Toolbox
    DatacontrolToolboxWidget* tool_;
    QIcon* toolIcon_;

    /// Layout for Toolbox
    QGridLayout* MeshDialogLayout_;

    /// Table containing Mesh information and status
    QTableWidget* objectList_;

    /// Flag set to true if meshlist is updated, preventing signal for list udate to be called
    bool locked;

    /// The Treemodel organizing the data
    TreeModel* model_;

    ///Tree view
    QTreeView* view_;

    /// Pointer to the header to the view widget
    QHeaderView * viewHeader_;

    /// Default marker to visualize "source" and "target" object flags
    DefaultObjectMarker objectMarker;


//===========================================================================
/** @name Slots which are called if data changed
* @{ */
//===========================================================================

  private slots:

    /// Gets called when the data in the table has changed
    void slotDataChanged (int _id, int _column, const QVariant& _value);

    /// Gets called when an object was moved via drag n drop
    void slotMoveBaseObject (int _id, int _newParentId);

  private:
    void propagateDownwards(BaseObject* _obj, int _column );
    void propagateUpwards(BaseObject* _obj, int _column );

    //variables to restrict propagation direction
    int onlyDown_;
    int onlyUp_;
    //defines, which selection column needs an update if not -1
    // 2: source 3: target
    int columnFromGUI_;

/** @} */

//===========================================================================
/** @name Popup Menu related
* @{ */
//===========================================================================
      private slots:
        /// Display a custom context window for the TreeView
        void slotCustomContextMenuRequested ( const QPoint & _pos );

        /// Called by the popup menu to remove an object/group
        void slotPopupRemove( );

        /// Called by the popup menu to ungroup items
        void slotUngroup ( );

        /// Called by the popup menu to group items
        void slotGroup ( );

        /// Called by the popup menu to copy items
        void slotCopy();

        ///Called by the popup menu to rename items
        void slotRename ( );

        ///Called by the popup menu to zoom to an object
        void slotZoomTo();
        
        /// Called by the popup menu in order to turn lights on/off
        void slotSwitchLight();
        
        /// Called by the popup menu in order to edit lights
        void slotEditLight();

        ///Called by the popup menu to set material properties
        void slotMaterialProperties();

        ///Called by the popup menu to copy material properties
        void slotCopyMaterialToTargeted();

        /** \brief Called when the material properties were changed inside the material dialog
         *
         * @param _node unused
         */
        void slotNodeChanged( ACG::SceneGraph::BaseNode* _node );
        
        /// Hide/Show all light sources if checkbox has been checked
        void slotShowLightSources( int _state );
        void slotShowLightSources();

      private :
        /// Index where a popup has been opened
        QModelIndex popupIndex_;

      private slots:

        /// Display a custom context window for the TreeViews header
        void slotHeaderCustomContextMenuRequested ( const QPoint & _pos );

      private:
        /// Defines the section of the Popup
        int headerPopupType_;

/** @} */

//===========================================================================
/** @name GL Area Context Menu related slots
* @{ */
//===========================================================================

      private slots:

        /// Hide an object
        void slotContextMenuHide( );

        /// Target Selection
        void slotContextMenuTarget( );

        /// Source Selection
        void slotContextMenuSource( );
        
        /// Remove Selected Item
        void slotContextMenuRemove( );

        /// Update the context Menus for the given id
        void slotUpdateContextMenu( int _objectId);

      private :
        QAction* targetAction_;
        QAction* sourceAction_;
        QAction* removeAction_;

/** @} */

//===========================================================================
/** @name Bounding box related slots/functions
* @{ */
//===========================================================================

      private slots:

        /// Bounding box selection changed
        void slotBoundingBoxChange( );

     private:

        /// Updates bounding box
        void updateBoundingBox(BaseObjectData* _obj);

/** @} */

//===========================================================================
/** @name Scripting functions
* @{ */
//===========================================================================

      private slots:

        /** This function sets the slot descriptions for this plugin
         *  which will be visible in the script editor
         */
        void setDescriptions();

      public slots:
        /// Get the Objects name from the id
        QString getObjectName( int objectId );

        /// Delete the given object
        void objectDelete(int objectId);

        /// Creates an object copy
        int copyObject( int objectId );

        /// Get the id of an object by its name
        int getObject( QString _name );

        /// Get the DataType of a given object
        DataType dataType( int objectId );

        /// Hide the given Object
        void hideObject( int objectId );

        /// Show the given Object
        void showObject( int objectId );

        /// set the given Object as target
        void setTarget( int objectId, bool _target );

        /// set the given Object as source
        void setSource( int objectId, bool _source );

        /// set the name of the given object
        void setObjectName( int objectId, QString _name );

        /// Makes all available objects target
        void setAllTarget();

        /// Makes all available objects source
        void setAllSource();

        /// Unselect all objects
        void clearAllTarget();

        /// remove source selection from all objects
        void clearAllSource();

        /// Shows all objects
        void showAll();

        /// Hides all objects
        void hideAll();

        /** Group objects together
        *
        * @return object id of new group or -1 if failed
        */
        int groupObjects(IdList _objectIDs, QString _groupName = "");
        
        /// add an object to an existing group
        bool addObjectToGroup(int _objectId, int _groupId);

        /** remove an object from its group and place it at the toplevel group
        *
        * @return successfull
        */
        bool unGroupObject(int _id);
        
        /// Get all target objects of given type
        IdList getTargetObjects(DataType _type);
        
        /// Get all source objects of given type
        IdList getSourceObjects(DataType _type);

        /// Returns the number of groups
        unsigned int groupCount() const;

        /// Returns a list of all available DataTypes
        QStringList availableDataTypeNames() const;

        /// Print information about all open objects to the console
        void printObjectInfoToLog();

        /// Get all elements of the given group
        IdList getGroupElements(int _groupId);

        void showReducedUi(bool reduced);

/** @} */

  public slots:
    QString version() { return QString("1.03"); };
};

#endif //DATACONTROLPLUGIN_HH
