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
//   $Revision$
//   $Author$
//   $Date$
//
//=============================================================================




#ifndef COLORPLUGIN_HH
#define COLORPLUGIN_HH

#include <QObject>
#include <QMenuBar>
#include "DataControlPlugin.hh"

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/GlobalAccessInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <ACG/QtWidgets/QtExaminerViewer.hh>
#include <OpenFlipper/common/Types.hh>
#include "Toolbox.hh"

//#include "QCheckBoxDelegate.hh"

#include "TreeModel.hh"


/** Plugin for controlling the Object data structures (Show,hide Object, Target, Source selection
*/
class DataControlPlugin : public QObject, BaseInterface, ToolboxInterface, GlobalAccessInterface, KeyInterface , LoggingInterface, INIInterface, ContextMenuInterface, ScriptInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(GlobalAccessInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(INIInterface)
  Q_INTERFACES(ContextMenuInterface)
  Q_INTERFACES(ScriptInterface)

  signals:
    /// Force Examiner widget to update their views
    void update_view();

    /// Emit this Signal, if object list has changed (e.g. Source or Target changed)
    void updated_objects(int);

    void activeObjectChanged();

    void log(Logtype _type, QString _message);
    void log(QString _message);

    void addContextMenu(QMenu* _menu );
    void addContextMenu(QMenu* _menu ,DataType _objectType , ContextMenuType _type );

    void setSlotDescription(QString     _slotName,   QString     _slotDescription,
                            QStringList _parameters, QStringList _descriptions);


   private slots :
      /// Updates the table widget
      void slotObjectUpdated( int _identifier );
      void slotActiveObjectChanged();

      void slotKeyEvent( QKeyEvent* _event );

      void loadIniFileOptions( INIFile& _ini );
      void saveIniFileOptions( INIFile& _ini );

      void pluginsInitialized();
      void initializePlugin();
   public :

     ~DataControlPlugin() {};


     /** Initialize the toolbar (create a widget in the right side toolbox)\n
      *   Creates Table and buttons */
     bool initializeToolbox(QWidget*& _widget);

     /// Sets examiner Widget (dummy here, using update_view signal here)
     void set_examiner(ACG::QtWidgets::QtExaminerViewer*  /* _examiner_widget */ ) { };

     /// Name of the Plugin
     QString name(){ return (QString("DataControl")); };

     /// Description of the Plugin
     QString description() { return (QString("Manages Data Objects")); };

     /// User selected plugins Toolbox
      void toolboxActivated( bool /* _activated */   ) { };

      /// Update the visualization of the active objects
      void update_active( );

   private :
      /// Widget for Toolbox
      DatacontrolToolboxWidget* tool_;

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

      /// Delegate for boolean values
      //QCheckBoxDelegate* checkboxDelegate_;



      ACG::QtWidgets::QtExaminerViewer*  examiner_widget_;

   private slots:

      /// This slot is called when the mesh selection changed in the table
      void slotCellClicked(int _row, int _col);

      /// Vertical header of the list has been clicked
      void verticalHeaderClicked( int _row );

      void verticalCountClicked( int id , int old , int newc );


      //===========================================================================
      /** @name Slots which are called if data changed
      * @{ */
      //===========================================================================

      /// Gets called when the data in the table has changed
      void slotDataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight );

      /// called when an object is removed from the view
      void slotRowsRemoved ( const QModelIndex & _parent, int _start, int _end );

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

        ///Called by the popup menu to rename items
        void slotRename ( );

        ///Called by the popup menu to zoom to an object
        void slotZoomTo();

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

        /// Update the context Menus for the given id
        void slotUpdateContextMenu( int _objectId);

      private :
        QAction* targetAction_;
        QAction* sourceAction_;

      /** @} */

      //===========================================================================
      /** @name Scripting functions
      * @{ */
      //===========================================================================

      private slots:
        void setDescriptions();

      public slots:
        /// Get the Objects name from the id
        QString getObjectName( int objectId );

        /// Get the id of an object by its name
        int getObject( QString _name );

        /// Hide the given Object
        void hideObject( int objectId );

        /// Show the given Object
        void showObject( int objectId );

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

        /// Group objects together
        void groupObjects(idList _objectIDs, QString _groupName = "");

      /** @} */

  public slots:
    QString version() { return QString("1.01"); };
};

#endif //COLORPLUGIN_HH
