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




#include "CoreWidget.hh"

#include <OpenFlipper/widgets/viewModeWidget/viewModeWidget.hh>

#include "OpenFlipper/common/GlobalOptions.hh"

//=============================================================================

/// init ViewModes that were loaded via ini-file
void CoreWidget::initViewModes(){
  //init viewMode subMenu
  if (!viewModeMenu_){
    viewModeMenu_ = new QMenu(tr("View Modes"));
    viewMenu_->addMenu(viewModeMenu_);
    viewGroup_ = new QActionGroup(0);
    viewGroup_->setExclusive(true);
    connect( viewGroup_, SIGNAL( triggered( QAction* ) ), this, SLOT( slotSetViewMode( QAction* ) ) );
  }

  viewModeMenu_->clear();

  bool seenCustom = false;

  for (int i=0; i < viewModes_.size(); i++){

    //Add Separator above the custom widgets
    if (viewModes_[i]->custom && !seenCustom){
      viewModeMenu_->addSeparator();
      seenCustom = true;
    }

    //add Action to viewMenu
    QAction* acViewMode = new QAction(viewModes_[i]->name, this);
    acViewMode->setStatusTip(tr("Change ViewMode"));
    viewGroup_->addAction(acViewMode);
    viewModeMenu_->addAction(acViewMode);

    //add Separator after viewMode 'all'
    if (viewModes_[i]->name == "All")
      viewModeMenu_->addSeparator();
  }

}

/// Add a new viewMode (non-custom)
void CoreWidget::slotAddViewMode(QString _mode, QStringList _usedWidgets){
  slotAddViewMode(_mode, false, _usedWidgets);
}

/// Add a new viewMode
void CoreWidget::slotAddViewMode(QString _mode, bool _custom, QStringList _usedWidgets){
  ViewMode* vm = new ViewMode();
  vm->name = _mode;
  vm->custom = _custom;
  vm->visibleWidgets = _usedWidgets;
  if (_custom)
    viewModes_.push_back(vm);
  else{
    //insert before custom viewModes
    int i = viewModes_.size();
    for (int k=0; k < viewModes_.size(); k++)
      if (viewModes_[k]->custom == true){
        i = k;
        break;
      }
    viewModes_.insert(i,vm);
  }
  initViewModes();
}

/// Remove a viewMode
void CoreWidget::slotRemoveViewMode(QString _name){
  for (int i=0; i < viewModes_.size(); i++)
    if (viewModes_[i]->name == _name && viewModes_[i]->custom == true){ //remove only userdefined viewModes
      viewModes_.remove(i);
      //remove action from menu
      for (int a=0; a < viewModeMenu_->actions().size(); a++)
        if (viewModeMenu_->actions()[a]->text() == _name){
          QAction* action = viewModeMenu_->actions()[a];
          viewModeMenu_->removeAction(action);
          viewGroup_->removeAction(action);
          delete action;
        }
      break;

    }
}

/// Slot for setting the viewMode from menu
void CoreWidget::slotSetViewMode( QAction* action){
  setViewMode( action->text() );
}

/// Slot for setting the viewMode from menu
void CoreWidget::setViewMode( QString _mode ){
//   //find viewMode
//   for (int i=0; i < viewModes_.size(); i++)
//     if ( viewModes_[i]->name == _mode ){
// 
//       OpenFlipper::Options::defaultToolboxMode(_mode);
// 
//       QStringList widgets = viewModes_[i]->visibleWidgets;
//       //and find all widgets that should be visible
//       for (uint p=0; p < plugins_.size(); p++){
//         if (plugins_[p].widget == 0) continue;
//         QString name_nospace = plugins_[p].name;
//         name_nospace.remove(" ");
//         if (widgets.contains( name_nospace) )
//           plugins_[p].widget->setVisible( true );
//         else
//           plugins_[p].widget->setVisible( false );
//       }
// 
//       //tab all dockWidgets together
//       tabDockWidgets();
//       break;
// 
//     }
  slotChangeView(_mode, QStringList());
}

/// Tab all DockWidgets which belong to ToolWidgets together
void CoreWidget::tabDockWidgets(QVector< QDockWidget* > _widgets){

  int maxH      = maximumHeight();
  int minH      = minimumHeight();

  setFixedHeight(height());

  //if no widgets are given take all visible one's and ignore the order
  if (_widgets.size() == 0)
    for (uint p=0; p < plugins_.size(); p++){
      if (plugins_[p].widget == 0) continue;
      if ( plugins_[p].widget->isVisible() && !plugins_[p].widget->isFloating() )
        _widgets.push_back( plugins_[p].widget );
    }

  //tab all dockWidgets together
  if (_widgets.size() > 1){
    QDockWidget* d1 = 0;
    QDockWidget* d2 = _widgets.first();
    _widgets.pop_front();

    while (_widgets.size() > 0){
      d1 = d2;
      d2 = _widgets.first();
      _widgets.pop_front();
      tabifyDockWidget(d1, d2);
    }
  }

  setMaximumHeight(maxH);
  setMinimumHeight(minH);
}

/// show dialog for changing ViewMode
void CoreWidget::slotViewModeDialog(){
  //init widget
  static viewModeWidget* widget = 0;
  if ( !widget ){
    widget = new viewModeWidget(viewModes_);
    connect(widget, SIGNAL(changeView(QString, QStringList)), this, SLOT(slotChangeView(QString, QStringList)) );
    connect(widget, SIGNAL(saveMode(QString, bool, QStringList)), this, SLOT(slotAddViewMode(QString, bool, QStringList)) );
    connect(widget, SIGNAL(removeMode(QString)), this, SLOT(slotRemoveViewMode(QString)) );
  }
  widget->show( OpenFlipper::Options::defaultToolboxMode() );
}

/// Slot for Changing visible toolWidgets
void CoreWidget::slotChangeView(QString _mode, QStringList _toolWidgets){

  int maxH      = maximumHeight();
  int minH      = minimumHeight();

  setFixedHeight(height());

  //try to find Widgets if they aren't given
  if (_mode != "" && _toolWidgets.size() == 0)
    for (int i=0; i < viewModes_.size(); i++)
      if (viewModes_[i]->name == _mode)
        _toolWidgets = viewModes_[i]->visibleWidgets;

  //first hide all widgets
  for (uint p=0; p < plugins_.size(); p++){
    if ( plugins_[p].widget )
      plugins_[p].widget->setVisible( false );
  }

  QVector< QDockWidget* > dockWidgets;

  //find all widgets that should be visible
  for (int i=0; i < _toolWidgets.size(); i++)
    for (uint p=0; p < plugins_.size(); p++){
      if (plugins_[p].widget == 0) continue;
      QString name_nospace = plugins_[p].name;
      name_nospace.remove(" ");
      if (_toolWidgets[i] == name_nospace ){
        plugins_[p].widget->setVisible( true );
        if ( !plugins_[p].widget->isFloating() )
          dockWidgets.push_back( plugins_[p].widget );
      }
    }

  if (_mode != "")
    OpenFlipper::Options::defaultToolboxMode(_mode);

  //tab all dockWidgets together
  tabDockWidgets(dockWidgets);

  setMaximumHeight(maxH);
  setMinimumHeight(minH);
}

/// Slot is called when the visibility of a toolWidget changes
void CoreWidget::slotVisibilityChanged ( bool /*visible*/ ){
  for (uint p=0; p < plugins_.size(); p++){
    if (!plugins_[p].widget) continue;
    if (plugins_[p].widget->isVisible() && !plugins_[p].widget->isFloating()){
      dockViewMode_->setVisible(true);
      return;
    }
  }
  dockViewMode_->setVisible(false);
}
