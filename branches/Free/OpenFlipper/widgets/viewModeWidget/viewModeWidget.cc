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




#include "viewModeWidget.hh"
#include <OpenFlipper/widgets/coreWidget/CoreWidget.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>

/// Constructor
viewModeWidget::viewModeWidget(const QVector< ViewMode* >& _modes, QWidget *_parent)
  : QDialog(_parent),
    modes_(_modes)
{
  setupUi(this);


  connect(viewModeList, SIGNAL(itemSelectionChanged()), this, SLOT(slotSetAllWidgets()));

  
  
  
  
  connect(viewModeList, SIGNAL(currentTextChanged (QString)), this, SLOT(slotModeChanged(QString)) );
  connect(viewModeList, SIGNAL(clicked (QModelIndex)), this, SLOT(slotModeClicked(QModelIndex)) );

  
  
  // View Mode buttons
  connect(removeButton, SIGNAL(clicked()), this, SLOT(slotRemoveMode()));
  connect(copyButton, SIGNAL(clicked()), this, SLOT(slotCopyMode()));
  connect(addButton, SIGNAL(clicked()), this, SLOT(slotAddMode()));
  
  // View Mode List context Menu
  viewModeList->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(viewModeList ,SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
          this         ,SLOT(slotModeContextMenu ( const QPoint & ) ));
  
  
  // Context Menus for Toolbars
  toolbarList->setContextMenuPolicy(Qt::CustomContextMenu);
  availableToolbars->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(toolbarList ,SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
          this        ,SLOT(slotUsedToolbarContextMenu ( const QPoint & ) )); 
  connect(availableToolbars ,SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
          this        ,SLOT(slotAvailableToolbarContextMenu ( const QPoint & ) )); 
  
  
  // Context Menus for Toolboxes
  toolboxList->setContextMenuPolicy(Qt::CustomContextMenu);
  availableToolboxes->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(toolboxList ,SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
          this        ,SLOT(slotUsedToolboxContextMenu ( const QPoint & ) )); 
  connect(availableToolboxes ,SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
          this        ,SLOT(slotAvailableToolboxContextMenu ( const QPoint & ) ));


  // Context Menus for Context Menus
  contextMenuList->setContextMenuPolicy(Qt::CustomContextMenu);
  availableContextMenus->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(contextMenuList ,SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
          this        ,SLOT(slotUsedContextMenuContextMenu ( const QPoint & ) )); 
  connect(availableContextMenus ,SIGNAL(customContextMenuRequested ( const QPoint &  )  ),
          this        ,SLOT(slotAvailableContextMenuContextMenu ( const QPoint & ) )); 
          
  
  // Toolbar Buttons to add and remove toolbars in the given Mode
  connect(rightArrowToolbar, SIGNAL(clicked()), this, SLOT(slotRightArrowToolbar()) );
  rightArrowToolbar->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "arrow-right.png" ) );
  connect(leftArrowToolbar, SIGNAL(clicked()), this, SLOT(slotLeftArrowToolbar()) );
  leftArrowToolbar->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "arrow-left.png" ) );
  
  
  // Toolbox Buttons to add, remove and order toolboxes in the given Mode
  connect(rightArrowToolbox, SIGNAL(clicked()), this, SLOT(slotRightArrowToolbox()) );
  rightArrowToolbox->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "arrow-right.png" ) );
  connect(leftArrowToolbox, SIGNAL(clicked()), this, SLOT(slotLeftArrowToolbox()) );
  leftArrowToolbox->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "arrow-left.png" ) );  
  
  connect(upButtonToolbox, SIGNAL(clicked()), this, SLOT(slotMoveToolboxUp()) );
  upButtonToolbox->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "arrow-up.png" ) );
  connect(downButtonToolbox, SIGNAL(clicked()), this, SLOT(slotMoveToolboxDown()) );
  downButtonToolbox->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "arrow-down.png" ) );
  
  
  // ContextMenu Buttons to add, remove and order ContextMenus in the given Mode
  connect(rightArrowContextMenu, SIGNAL(clicked()), this, SLOT(slotRightArrowContextMenu()) );
  rightArrowContextMenu->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "arrow-right.png" ) );
  connect(leftArrowContextMenu, SIGNAL(clicked()), this, SLOT(slotLeftArrowContextMenu()) );
  leftArrowContextMenu->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "arrow-left.png" ) );  
  
  connect(upButtonContextMenu, SIGNAL(clicked()), this, SLOT(slotMoveContextMenuUp()) );
  upButtonContextMenu->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "arrow-up.png" ) );
  connect(downButtonContextMenu, SIGNAL(clicked()), this, SLOT(slotMoveContextMenuDown()) );
  downButtonContextMenu->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + OpenFlipper::Options::dirSeparator() + "arrow-down.png" ) );
  
  
  
  // General Buttons
  // Apply currently configured Mode
  connect(okButton, SIGNAL(clicked()), this, SLOT(slotChangeView()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(saveButton, SIGNAL(clicked()), this, SLOT(slotSaveMode()));
}


// =======================================================================================================
// View Mode button slots
// =======================================================================================================
void viewModeWidget::slotRemoveMode(){
  emit removeMode( viewModeList->currentItem()->text() );
  QListWidgetItem* item =  viewModeList->takeItem( viewModeList->currentRow() );
  delete item;
}

void viewModeWidget::slotCopyMode(){
  // Find currently selected Mode
  // Search for current mode 
  int id = -1;
  if ( viewModeList->selectedItems().count() > 0) 
    for (int i=0; i < modes_.size(); i++)
      if (modes_[i]->name == viewModeList->currentItem()->text()){
        id = i;
        break;
      }
      
  if ( id == -1 ) {
    std::cerr << "Currently selected Mode not found?!" << std::endl; 
    return;
  }
  
  //ask for a name for the new viewmode as it is not a custom one
  bool ok;
  QString name = QInputDialog::getText(this, tr("Copy View Mode"),
                                             tr("Please enter a name for the new View Mode"), QLineEdit::Normal,
                                                "", &ok);
                     
  // Check if valid                                                
  if (!ok || name.isEmpty()) {
    QMessageBox::warning(this, tr("Copy View Mode"), tr("Please enter a Name"), QMessageBox::Ok);
    return; 
  }
  
  //check if name already exists
  for (int i=0; i < modes_.size(); i++)
    if (modes_[i]->name == name){
      QMessageBox::warning(this, tr("Copy View Mode"), tr("Cannot Copy ViewMode. \nNew Name already in use for a different mode."), QMessageBox::Ok);
      return;
    }
  
  emit saveMode(name, true, modes_[id]->visibleToolboxes, modes_[id]->visibleToolbars, modes_[id]->visibleContextMenus);      
  
  QListWidgetItem *item = new QListWidgetItem(viewModeList);
  item->setTextAlignment(Qt::AlignHCenter);
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  item->setIcon(QIcon(OpenFlipper::Options::iconDirStr() + QDir::separator () + "Unknown.png"));
  item->setText(name);
  
  item->setForeground( QBrush(QColor(0,0,150) ) );
  
  show(name);
}

void viewModeWidget::slotAddMode(){
  
 //ask for a name for the new viewmode as it is not a custom one
  bool ok;
  QString name = QInputDialog::getText(this, tr("Add View Mode"),
                                             tr("Please enter a name for the new View Mode"), QLineEdit::Normal,
                                                "", &ok);
                     
  // Check if valid                                                
  if (!ok || name.isEmpty()) {
    QMessageBox::warning(this, tr("Add View Mode"), tr("Please enter a Name"), QMessageBox::Ok);
    return; 
  }
  
  //check if name already exists
  for (int i=0; i < modes_.size(); i++)
    if (modes_[i]->name == name){
      QMessageBox::warning(this, tr("Add View Mode"), tr("Cannot Add ViewMode. \nNew Name already in use for a different mode."), QMessageBox::Ok);
      return;
    }
  
  emit saveMode(name, true, QStringList(), QStringList(), modes_[0]->visibleContextMenus);      
  
  QListWidgetItem *item = new QListWidgetItem(viewModeList);
  item->setTextAlignment(Qt::AlignHCenter);
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  item->setIcon(QIcon(OpenFlipper::Options::iconDirStr() + QDir::separator () + "Unknown.png"));
  item->setText(name);
  
  item->setForeground( QBrush(QColor(0,0,150) ) );
  
  show(name);
}

// =======================================================================================================
// View Mode Context Menu
// =======================================================================================================
void viewModeWidget::slotModeContextMenu ( const QPoint & _pos ){
  
  if (viewModeList->itemAt(_pos)){
    
    QMenu menu(0);
    
    menu.addAction(tr("Remove Mode"), this, SLOT ( slotRemoveMode() ));
    menu.addAction(tr("Copy Mode"), this, SLOT ( slotCopyMode() ));      
    menu.addAction(tr("Change Icon"), this, SLOT ( slotSetIcon() ));
    
    //check if mode is custom e.g. that it can be removed
    for (int i=0; i < modes_.size(); i++)
      if (modes_[i]->name == viewModeList->currentItem()->text()){
        menu.actions()[0]->setEnabled(modes_[i]->custom);
        menu.actions()[2]->setEnabled(modes_[i]->custom);
        break;
      }
      
    menu.exec(viewModeList->mapToGlobal( _pos) );
  }
}

void viewModeWidget::slotSetIcon() {
  // Get the correct mode
  int id = -1;
  for (int i=0; i < modes_.size(); i++)
    if (modes_[i]->name == viewModeList->currentItem()->text()){
      id = i;
      break;
    }
    
  if ( id == -1) {
    std::cerr << "Unable to find Mode viewModeWidget::slotSetIcon()" << std::endl;
    return;
  }

  // Get the filename from the user
  QString fileName = QFileDialog::getOpenFileName ( this, tr("Select Image for view Mode ( best size : 150x150px )"), QString(),  tr("Images (*.png *.xpm *.jpg)") );
  
  QFile file(fileName);
  QFileInfo fileInfo(file);
  
  if ( ! file.exists() )
    return;
  
  
  file.copy(OpenFlipper::Options::configDirStr() + QDir::separator() + "Icons" + QDir::separator() + "viewMode_" + fileInfo.fileName() );
  
  modes_[id]->icon = "viewMode_" + fileInfo.fileName();

  show(modes_[id]->name);
}

// =======================================================================================================
// ToolBox, ToolBar and ContextMenu Lists update functions
// =======================================================================================================
void viewModeWidget::slotSetAllWidgets(){
  toolboxList->clear();
  toolbarList->clear();
  contextMenuList->clear();
  availableToolboxes->clear();
  availableToolbars->clear();
  availableContextMenus->clear();
  
  QStringList toolboxes;
  QStringList toolbars;
  QStringList contextmenus;
  
  //iterate over all selected modes
  for (int m=0; m < viewModeList->selectedItems().size(); m++)
    
    // find mode in modeVector modes_
    for (int i=0; i < modes_.size(); i++)
      if ( modes_[i]->name == (viewModeList->selectedItems()[m])->text() ) {
        toolboxes = modes_[i]->visibleToolboxes;
        toolbars = modes_[i]->visibleToolbars;
	contextmenus = modes_[i]->visibleContextMenus;
	if (contextmenus.contains("ALL_THAT_EXIST"))
	  contextmenus = modes_[0]->visibleContextMenus;
        toolboxList->addItems(toolboxes); //add corresponding widgets
        toolbarList->addItems(toolbars);
	contextMenuList->addItems(contextmenus);
        break;
      }
      
      
      if ( !modes_.empty() ) {
        QStringList allToolboxes = modes_[0]->visibleToolboxes;
        QStringList allToolbars  = modes_[0]->visibleToolbars;
        QStringList allContextMenus  = modes_[0]->visibleContextMenus;
        
        QStringList availableToolboxList;
        QStringList availableToolbarList;
        QStringList availableContextMenuList;
        
        for ( int i = 0; i < allToolboxes.size(); ++i ) {
          if ( ! toolboxes.contains(allToolboxes[i]) ) 
            availableToolboxList.push_back(allToolboxes[i]);
        }
        
        for ( int i = 0; i < allToolbars.size(); ++i ) {
          if ( ! toolbars.contains(allToolbars[i]) ) 
            availableToolbarList.push_back(allToolbars[i]);
        }
        
        for ( int i = 0; i < allContextMenus.size(); ++i ) {
          if ( ! contextmenus.contains(allContextMenus[i]) ) 
            availableContextMenuList.push_back(allContextMenus[i]);
        }
        
        availableToolboxes->addItems(availableToolboxList);
        availableToolbars->addItems(availableToolbarList);
        availableContextMenus->addItems(availableContextMenuList);
        
      } else {
        std::cerr << "Mode not found!" << std::endl;
      }
      
}


// =======================================================================================================
// ToolBar Context Menus Buttons
// =======================================================================================================

/// opens custom context menu in used toolbar-List
void viewModeWidget::slotUsedToolbarContextMenu ( const QPoint & _pos ){
  
  if (toolboxList->itemAt(_pos)){
    
    QMenu menu(0);
    
    if ( toolbarList->selectedItems().count() != 0 )
      menu.addAction(tr("Remove"), this, SLOT ( slotRightArrowToolbar() ));
    
    menu.exec(toolbarList->mapToGlobal( _pos) );
  }
}

/// opens custom context menu in available toolbar-List
void viewModeWidget::slotAvailableToolbarContextMenu ( const QPoint & _pos ){
  
  if (availableToolbars->itemAt(_pos)){
    
    QMenu menu(0);
    
    if ( availableToolbars->selectedItems().count() != 0 )
      menu.addAction(tr("Add"), this, SLOT ( slotLeftArrowToolbar() ));
    
    menu.exec(availableToolbars->mapToGlobal( _pos) );
  }
}

// =======================================================================================================
// ToolBox Context Menus Buttons
// =======================================================================================================

/// opens custom context menu in used toolbox-List
void viewModeWidget::slotUsedToolboxContextMenu ( const QPoint & _pos ){
  
  if (toolboxList->itemAt(_pos)){
    
    QMenu menu(0);
    
    if (toolboxList->selectedItems().count() == 1){
      menu.addAction(tr("Move up"), this, SLOT ( slotMoveToolboxUp() ));
      menu.addAction(tr("Move down"), this, SLOT ( slotMoveToolboxDown() ));
      menu.addSeparator();
    }
    
    if ( toolboxList->selectedItems().count() != 0 )
      menu.addAction(tr("Remove"), this, SLOT ( slotRightArrowToolbox() ));
    
    menu.exec(toolboxList->mapToGlobal( _pos) );
  }
}

/// opens custom context menu in available toolbox-List
void viewModeWidget::slotAvailableToolboxContextMenu ( const QPoint & _pos ){
  
  if (availableToolboxes->itemAt(_pos)){
    
    QMenu menu(0);
    
    if ( availableToolboxes->selectedItems().count() != 0 )
      menu.addAction(tr("Add"), this, SLOT ( slotLeftArrowToolbox() ));
    
    menu.exec(availableToolboxes->mapToGlobal( _pos) );
  }
}

// =======================================================================================================
// ContextMenu Context Menus Buttons
// =======================================================================================================

/// opens custom context menu in used contextmenu-List
void viewModeWidget::slotUsedContextMenuContextMenu ( const QPoint & _pos ){
  
  if (contextMenuList->itemAt(_pos)){
    
    QMenu menu(0);
    
    if (contextMenuList->selectedItems().count() == 1){
      menu.addAction(tr("Move up"), this, SLOT ( slotMoveContextMenuUp() ));
      menu.addAction(tr("Move down"), this, SLOT ( slotMoveContextMenuDown() ));
      menu.addSeparator();
    }
    
    if ( contextMenuList->selectedItems().count() != 0 )
      menu.addAction(tr("Remove"), this, SLOT ( slotRightArrowContextMenu() ));
    
    menu.exec(contextMenuList->mapToGlobal( _pos) );
  }
}

/// opens custom context menu in available contextmenu-List
void viewModeWidget::slotAvailableContextMenuContextMenu ( const QPoint & _pos ){
  
  if (availableContextMenus->itemAt(_pos)){
    
    QMenu menu(0);
    
    if ( availableContextMenus->selectedItems().count() != 0 )
      menu.addAction(tr("Add"), this, SLOT ( slotLeftArrowContextMenu() ));
    
    menu.exec(availableContextMenus->mapToGlobal( _pos) );
  }
}

// =======================================================================================================
// ToolBar Buttons
// =======================================================================================================

void viewModeWidget::slotRightArrowToolbar() {
  QList<QListWidgetItem *> selectedItems = toolbarList->selectedItems ();
  for ( int i = 0 ; i < selectedItems.size(); ++i ) 
    availableToolbars->addItem(selectedItems[i]->text());

  qDeleteAll(selectedItems);
}

void viewModeWidget::slotLeftArrowToolbar() {
  QList<QListWidgetItem *> selectedItems = availableToolbars->selectedItems ();
  for ( int i = 0 ; i < selectedItems.size(); ++i ) 
    toolbarList->addItem(selectedItems[i]->text());
  
  qDeleteAll(selectedItems);
}

// =======================================================================================================
// ToolBox Buttons
// =======================================================================================================

void viewModeWidget::slotRightArrowToolbox() {
  QList<QListWidgetItem *> selectedItems = toolboxList->selectedItems ();
  for ( int i = 0 ; i < selectedItems.size(); ++i ) 
    availableToolboxes->addItem(selectedItems[i]->text());
  
  qDeleteAll(selectedItems);
}

void viewModeWidget::slotLeftArrowToolbox() {
  QList<QListWidgetItem *> selectedItems = availableToolboxes->selectedItems ();
  for ( int i = 0 ; i < selectedItems.size(); ++i ) 
    toolboxList->addItem(selectedItems[i]->text());
  
  qDeleteAll(selectedItems);
}


/// Move Widget up in the list
void viewModeWidget::slotMoveToolboxUp(){
  if (toolboxList->selectedItems().count() == 1){
    if (toolboxList->currentRow() == 0) return;
    //extract a list of all items
    QString item = toolboxList->currentItem()->text();
    int oldRow = toolboxList->currentRow();
    QStringList widgets;
    for (int i=0; i < toolboxList->count(); i++)
      widgets << toolboxList->item(i)->text();
    
    //reorder items
    QString last = widgets[0];
    for (int i=1; i < widgets.size(); i++){
      if (widgets[i] == item){
        widgets[i] = last;
        widgets[i-1] = item;
      }
      last = widgets[i];
    }
    // set new list as elements for the toolboxList
    toolboxList->clear();
    toolboxList->addItems(widgets);
    //highlight active item
    toolboxList->setCurrentRow(oldRow-1);
  }
}

/// Move widget down in the list
void viewModeWidget::slotMoveToolboxDown(){
  if (toolboxList->selectedItems().count() == 1){
    if (toolboxList->currentRow() == toolboxList->count()-1) return;
    //extract a list of all items
    QString item = toolboxList->currentItem()->text();
    int oldRow = toolboxList->currentRow();
    QStringList widgets;
    for (int i=0; i < toolboxList->count(); i++)
      widgets << toolboxList->item(i)->text();
    
    //reorder items
    QString last = widgets[widgets.size()-1];
    for (int i=widgets.size()-2; i >= 0; i--){
      if (widgets[i] == item){
        widgets[i] = last;
        widgets[i+1] = item;
      }
      last = widgets[i];
    }
    // set new list as elements for the toolboxList
    toolboxList->clear();
    toolboxList->addItems(widgets);
    //highlight active item
    toolboxList->setCurrentRow(oldRow+1);
  }
}

// =======================================================================================================
// ContextMenu Buttons
// =======================================================================================================

void viewModeWidget::slotRightArrowContextMenu() {
  QList<QListWidgetItem *> selectedItems = contextMenuList->selectedItems ();
  for ( int i = 0 ; i < selectedItems.size(); ++i ) 
    availableContextMenus->addItem(selectedItems[i]->text());
  
  qDeleteAll(selectedItems);
}

void viewModeWidget::slotLeftArrowContextMenu() {
  QList<QListWidgetItem *> selectedItems = availableContextMenus->selectedItems ();
  for ( int i = 0 ; i < selectedItems.size(); ++i ) 
    contextMenuList->addItem(selectedItems[i]->text());
  
  qDeleteAll(selectedItems);
}


/// Move Widget up in the list
void viewModeWidget::slotMoveContextMenuUp(){
  if (contextMenuList->selectedItems().count() == 1){
    if (contextMenuList->currentRow() == 0) return;
    //extract a list of all items
    QString item = contextMenuList->currentItem()->text();
    int oldRow = contextMenuList->currentRow();
    QStringList widgets;
    for (int i=0; i < contextMenuList->count(); i++)
      widgets << contextMenuList->item(i)->text();
    
    //reorder items
    QString last = widgets[0];
    for (int i=1; i < widgets.size(); i++){
      if (widgets[i] == item){
        widgets[i] = last;
        widgets[i-1] = item;
      }
      last = widgets[i];
    }
    // set new list as elements for the toolboxList
    contextMenuList->clear();
    contextMenuList->addItems(widgets);
    //highlight active item
    contextMenuList->setCurrentRow(oldRow-1);
  }
}

/// Move widget down in the list
void viewModeWidget::slotMoveContextMenuDown(){
  if (contextMenuList->selectedItems().count() == 1){
    if (contextMenuList->currentRow() == contextMenuList->count()-1) return;
    //extract a list of all items
    QString item = contextMenuList->currentItem()->text();
    int oldRow = contextMenuList->currentRow();
    QStringList widgets;
    for (int i=0; i < contextMenuList->count(); i++)
      widgets << contextMenuList->item(i)->text();
    
    //reorder items
    QString last = widgets[widgets.size()-1];
    for (int i=widgets.size()-2; i >= 0; i--){
      if (widgets[i] == item){
        widgets[i] = last;
        widgets[i+1] = item;
      }
      last = widgets[i];
    }
    // set new list as elements for the toolboxList
    contextMenuList->clear();
    contextMenuList->addItems(widgets);
    //highlight active item
    contextMenuList->setCurrentRow(oldRow+1);
  }
}



// =======================================================================================================
// External communication
// =======================================================================================================

/// Slot for changing View and closing widget
void viewModeWidget::slotChangeView(){
  //get toolboxes
  QStringList toolboxes;
  for (int i=0; i < toolboxList->count(); i++)
    toolboxes << toolboxList->item(i)->text();
  
  //get toolbars
  QStringList toolbars;
  for (int i=0; i < toolbarList->count(); i++)
    toolbars << toolbarList->item(i)->text();
  
  //get context menus
  QStringList contextmenus;
  for (int i=0; i < contextMenuList->count(); i++)
    contextmenus << contextMenuList->item(i)->text();
  
  //get mode
  QString mode = "";
  if (viewModeList->selectedItems().size() > 0)
    mode = viewModeList->selectedItems()[0]->text();
  
  
  // Check current configuration if it is a changed view mode
  
  // Search for current mode 
  int id = -1;
  if ( viewModeList->selectedItems().count() > 0) 
    for (int i=0; i < modes_.size(); i++)
      if (modes_[i]->name == viewModeList->currentItem()->text()){
        id = i;
        break;
      }
      
  if ( id == -1 ) {
    std::cerr << "Currently selected Mode not found?!" << std::endl; 
    return;
  }
  
  bool matching = true;
  // Check if toolbox list matches:
  if ( modes_[id]->visibleToolboxes.size() == toolboxes.size() ) {
    for ( int i = 0 ; i < modes_[id]->visibleToolboxes.size(); ++i  ) 
      if ( modes_[id]->visibleToolboxes[i] != toolboxes[i] )
        matching = false;
  } else {
      matching = false;
  }
  
  // Check if toolbar list matches:
  if ( modes_[id]->visibleToolbars.size() == toolbars.size() ) {
    for ( int i = 0 ; i < modes_[id]->visibleToolbars.size(); ++i  ) 
      if ( modes_[id]->visibleToolbars[i] != toolbars[i] )
        matching = false;
  } else {
    matching = false;
  }
  
  // Check if context menu list matches:
  if ( modes_[id]->visibleContextMenus.size() == contextmenus.size() ) {
    for ( int i = 0 ; i < modes_[id]->visibleContextMenus.size(); ++i  ) 
      if ( modes_[id]->visibleContextMenus[i] != contextmenus[i] )
        matching = false;
  } else {
    matching = false;
  }
  
  if ( !matching ) {
    int ret = QMessageBox::warning(this, 
                                   tr("Mode has been changed!"),
                                   tr("You changed the view mode configuration. Do you want to save it?"),
                                   QMessageBox::Yes|QMessageBox::No,
                                   QMessageBox::No);
    if (ret == QMessageBox::Yes) 
      slotSaveMode();
    
  }
  
  emit changeView(mode,toolboxes,toolbars,contextmenus);
  close();
}

/// Slot for saving current List of Widgets as custom mode
void viewModeWidget::slotSaveMode(){
  // Search for current mode vector
  int id = -1;
  if ( viewModeList->selectedItems().count() > 0) 
    for (int i=0; i < modes_.size(); i++)
      if (modes_[i]->name == viewModeList->currentItem()->text()){
        id = i;
        break;
      }
  
  if ( id == -1 ) {
    std::cerr << "Mode Not found in slotSaveMode" << std::endl;
    return;
  }
  
  // Get Toolboxes
  QStringList toolboxes;
  for (int i=0; i < toolboxList->count(); i++)
    toolboxes << toolboxList->item(i)->text();
  
  // Get Toolbars
  QStringList toolbars;
  for (int i=0; i < toolbarList->count(); i++)
    toolbars << toolbarList->item(i)->text();
  
  // Get Context Menus
  QStringList contextmenus;
  for (int i=0; i < contextMenuList->count(); i++)
    contextmenus << contextMenuList->item(i)->text();
   
  bool  createNewMode = false;

  QString message = tr("You cannot change predefined modes.\n"
                       "Please enter a new Name for the mode.");
  
  // Check if we want to create a new node.
  if ( ! modes_[id]->custom ) {
    createNewMode = true;
    
  } else {
    int ret = QMessageBox::warning(this, 
                                   tr("View Mode exists"),
                                   tr("View Mode already exists. Do you want to overwrite it?"),
                                   QMessageBox::Yes|QMessageBox::No,
                                   QMessageBox::No);
    if (ret == QMessageBox::No) {
      message = tr("New name for view mode:");
      createNewMode = true;
    }
  }
  
  
  if ( createNewMode ) {
    
    //ask for a name for the new viewmode as it is not a custom one
    bool ok;
    QString name = QInputDialog::getText(this, tr("Save view Mode"),
                                               message, QLineEdit::Normal,
                                                  "", &ok);
                                                    
    //Remove Spaces from name
    if (!ok || name.isEmpty()) {
      std::cerr << "Illegal or no name given!" << std::endl;
      return; 
    }
    
    //check if name already exists
    for (int i=0; i < modes_.size(); i++)
      if (modes_[i]->name == name){
        QMessageBox::warning(this, tr("Save View Mode"), tr("Cannot Save ViewMode.\nName already taken by a different mode."), QMessageBox::Ok);
        return;
      }
      
    emit saveMode(name, true, toolboxes, toolbars, contextmenus);
    show(name);      
  } else {
    emit saveMode(modes_[id]->name, true, toolboxes, toolbars, contextmenus);
    show(modes_[id]->name);    
  }
  
  slotModeChanged(QString());
}  



















/// overloaded show function
void viewModeWidget::show(QString _lastMode){
  QDialog::show();
  
  //fill viewModeList
  viewModeList->clear();
  for (int i=0; i < modes_.size(); i++){
    QListWidgetItem *item = new QListWidgetItem(viewModeList);
    item->setTextAlignment(Qt::AlignHCenter);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    
    QFile iconFile( OpenFlipper::Options::iconDirStr() + QDir::separator () + modes_[i]->icon  );
    
    if ( iconFile.exists() )
      item->setIcon( QIcon(iconFile.fileName()) );
    else {
      iconFile.setFileName( OpenFlipper::Options::configDirStr() + QDir::separator() + "Icons" + QDir::separator() + modes_[i]->icon );
      if ( iconFile.exists() )
        item->setIcon( QIcon(iconFile.fileName()) );
      else {
        item->setIcon( QIcon(OpenFlipper::Options::iconDirStr() + QDir::separator () + "Unknown.png")  );
        std::cerr << "Unable to find icon file! " << iconFile.fileName().toStdString() <<  std::endl;
      }
    }
    
    item->setText(modes_[i]->name);
    
    if (modes_[i]->custom)
      viewModeList->item(i)->setForeground( QBrush(QColor(0,0,150) ) );
    else
      viewModeList->item(i)->setForeground( QBrush(QColor(0,0,0) ) );
  }
  
  //select given mode
  viewModeList->setCurrentRow(0);
  
  for (int i=0; i < viewModeList->count(); i++)
    if (viewModeList->item(i)->text() == _lastMode)
      viewModeList->setCurrentRow(i);
    
    removeButton->setEnabled(false);
}


/// Slot for updating removeButton when new mode is selected
void viewModeWidget::slotModeClicked(QModelIndex /*_id*/){
  slotModeChanged(QString());
}

/// Slot for updating removeButton when new mode is selected
void viewModeWidget::slotModeChanged(QString /*_mode*/){
  //check if mode is custom e.g. that it can be removed
  if (viewModeList->selectedItems().count() > 0){
    for (int i=0; i < modes_.size(); i++)
      if (modes_[i]->name == viewModeList->currentItem()->text()){
        removeButton->setEnabled(modes_[i]->custom);
        break;
      }
  }
}

















