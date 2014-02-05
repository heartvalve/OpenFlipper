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




#ifndef VIEWMODEWIDGET_HH
#define VIEWMODEWIDGET_HH

#include "ui_viewMode.hh"
#include <QStringList>
#include <QVector>

struct ViewMode;

class viewModeWidget : public QDialog, public Ui::viewMode
{
  Q_OBJECT
  private:
    // Vector holding list of all available modes
    const QVector< ViewMode* >& modes_;
  public:
    viewModeWidget(const QVector< ViewMode* >& _modes, QWidget *parent = 0 );
    void show(QString _lastMode);
    //void toolbarList(Qt::ContextMenuPolicy arg1);
  private slots:

    void slotModeChanged(QString _mode);
    void slotModeClicked(QModelIndex _id);
   
    
    // ============================================
    // ViewMode Buttons
    // ============================================   
      /** \brief Button slot to remove the selected view mode
      *
      * This slot removes the currently selected view mode 
      */    
      void slotRemoveMode();
      
      /** \brief Button slot to copy the selected view mode
      *
      * This slot copies the currently selected view mode 
      */    
      void slotCopyMode();
      
      /** \brief Button slot to add a new empty mode
      *
      * This slot creates an new empty view mode
      */    
      void slotAddMode();
      

    
    
    // ============================================
    // ViewMode Context Menu
    // ============================================
      /** \brief Context Menu View Modes
      *
      * This slot shows the used toolbar Context Menu
      */
      void slotModeContextMenu ( const QPoint & _pos );
      
      /** \brief Context menu slot to change the icon for a view mode
      *
      * This slot asks the user to change the icon of the given view mode
      */    
      void slotSetIcon();      
    
    // ============================================
    // ToolBox and ToolBar Lists update functions
    // ============================================
      /** \brief Update list views
      *
      * This slot updates all list views depending on the currently selected view mode
      */
      void slotSetAllWidgets();
 
    // ============================================
    // ToolBar Views Context Menu
    // ============================================
      /** \brief Context Menu Used Toolbars
      *
      * This slot shows the used toolbar Context Menu
      */
      void slotUsedToolbarContextMenu ( const QPoint & _pos );  
      
      /** \brief Context Menu Available Toolbars
      *
      * This slot shows the available toolbar Context Menu
      */
      void slotAvailableToolbarContextMenu ( const QPoint & _pos );    
    
    // ============================================
    // ToolBox Views Context Menu
    // ============================================
      /** \brief Context Menu Used Toolboxes
      *
      * This slot shows the used toolbox Context Menu
      */
      void slotUsedToolboxContextMenu ( const QPoint & _pos );  
      
      /** \brief Context Menu Available Toolboxes
      *
      * This slot shows the available toolbox Context Menu
      */
      void slotAvailableToolboxContextMenu ( const QPoint & _pos ); 
    
    // ============================================
    // ContextMenu Views Context Menu
    // ============================================
      /** \brief Context Menu Used ContextMenus
      *
      * This slot shows the used ContextMenu Context Menu
      */
      void slotUsedContextMenuContextMenu ( const QPoint & _pos );  
      
      /** \brief Context Menu Available ContextMenus
      *
      * This slot shows the available ContextMenu Context Menu
      */
      void slotAvailableContextMenuContextMenu ( const QPoint & _pos );
  
  
    // ============================================
    // ToolBar Buttons
    // ============================================
      /** \brief remove Toolbars from Mode
      *
      * This slot removes the selected toolbars from the current view Mode
      */
      void slotRightArrowToolbar();
      
      /** \brief add Toolbars to Mode
      *
      * This slot adds the selected toolbars to the current view Mode
      */
      void slotLeftArrowToolbar();
    
    // ============================================
    //ToolBox Buttons
    // ============================================
      /** \brief remove Toolboxes from Mode
      *
      * This slot removes the selected widgets from the current view Mode
      */
      void slotRightArrowToolbox();
      
      /** \brief add Toolboxes to Mode
      *
      * This slot adds the selected widgets to the current view Mode
      */
      void slotLeftArrowToolbox();
    
      /** \brief Move Toolbox up
      *
      * This slot moves the widget upward in the list of used modes
      */
      void slotMoveToolboxUp();
      
      /** \brief Move Toolbox down
      *
      * This slot moves the widget downward in the list of used modes
      */
      void slotMoveToolboxDown();
      
    // ============================================
    //ContextMenu Buttons
    // ============================================
    /** \brief remove ContextMenu from Mode
      *
      * This slot removes the selected widgets from the current view Mode
      */
      void slotRightArrowContextMenu();
      
      /** \brief add ContextMenu to Mode
      *
      * This slot adds the selected widgets to the current view Mode
      */
      void slotLeftArrowContextMenu();
    
      /** \brief Move ContextMenu up
      *
      * This slot moves the widget upward in the list of used modes
      */
      void slotMoveContextMenuUp();
      
      /** \brief Move Toolbox down
      *
      * This slot moves the widget downward in the list of used modes
      */
      void slotMoveContextMenuDown();
      
      
    // ============================================
    // External Communication
    // ============================================     
    
    private slots:
      /// Slot for changing the current view to currently configured one
      void slotChangeView();
      
      /** \brief Save the current view mode configuration
      *
      * Takes the given configuration and saves the mode
      */
      void slotSaveMode();
    
    signals:
      /// Changes the view mode to the currently configured one
      void changeView(QString _mode, QStringList _toolboxWidgets, QStringList _toolbars, QStringList _contextmenus);
      
      /// saves the given mode
      void saveMode(QString _name, bool _custom, QStringList _toolboxWidgets, QStringList _toolbars, QStringList _contextmenus);
      
      /// This signal is emitted to remove a mode
      void removeMode(QString _name);
      
      
};

#endif //VIEWMODEWIDGET_HH
