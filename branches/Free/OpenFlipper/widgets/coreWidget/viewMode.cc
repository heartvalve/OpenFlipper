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
 *   $Revision$                                                         *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



#include "CoreWidget.hh"

#include <OpenFlipper/widgets/viewModeWidget/viewModeWidget.hh>
#include <OpenFlipper/widgets/viewModeWidget/viewModeChangeWidget.hh>

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

void CoreWidget::slotAddViewModeToolboxes(QString _mode, QStringList _usedWidgets){
  slotAddViewModeToolboxes(_mode, false, _usedWidgets);
}

void CoreWidget::slotAddViewModeToolboxes(QString _mode, bool _custom, QStringList _usedWidgets){
  int id = -1;

  // Check if it already exists
  for ( int i = 0 ; i < viewModes_.size(); i++) {
    if ( viewModes_[i]->name == _mode ) {
      id = i;
      break;
    }
  }

  ViewMode* vm = 0;
  if ( id == -1 ) {
    vm         = new ViewMode();
    vm->name   = _mode;
    vm->custom = _custom;
    vm->icon   = "Unknown.png";

    if (_custom) {
      viewModes_.push_back(vm);
    } else {
      //insert before custom viewModes
      int i = viewModes_.size();
      for (int k=0; k < viewModes_.size(); k++)
        if (viewModes_[k]->custom == true){
          i = k;
          break;
        }
      viewModes_.insert(i,vm);
    }
  } else {
    vm = viewModes_[id];
  }

  vm->visibleToolboxes = _usedWidgets;

  initViewModes();
}

void CoreWidget::slotAddViewModeToolbars(QString _mode, QStringList _usedToolbars) {
  slotAddViewModeToolbars(_mode,false,_usedToolbars);
}

void CoreWidget::slotAddViewModeToolbars(QString _mode, bool _custom, QStringList _usedToolbars) {
  int id = -1;

  // Check if it already exists
  for ( int i = 0 ; i < viewModes_.size(); i++) {
    if ( viewModes_[i]->name == _mode ) {
      id = i;
      break;
    }
  }

  ViewMode* vm = 0;
  if ( id == -1 ) {
    vm         = new ViewMode();
    vm->name   = _mode;
    vm->custom = _custom;
    vm->icon   = "Unknown.png";

    if (_custom) {
      viewModes_.push_back(vm);
    } else {
      //insert before custom viewModes
      int i = viewModes_.size();
      for (int k=0; k < viewModes_.size(); k++)
        if (viewModes_[k]->custom == true){
          i = k;
          break;
        }
        viewModes_.insert(i,vm);
    }
  } else {
    vm = viewModes_[id];
  }

  vm->visibleToolbars = _usedToolbars;

  initViewModes();
}

/// Sets the Icon for a given View Mode
void CoreWidget::slotSetViewModeIcon(QString _mode, QString _iconName) {
  slotSetViewModeIcon(_mode,false,_iconName);
}

/// Sets the Icon for a given View Mode
void CoreWidget::slotSetViewModeIcon(QString _mode, bool _custom, QString _iconName) {

  int id = -1;

  // Check if it already exists
  for ( int i = 0 ; i < viewModes_.size(); i++) {
    if ( viewModes_[i]->name == _mode ) {
      id = i;
      break;
    }
  }

  ViewMode* vm = 0;
  if ( id == -1 ) {
    vm         = new ViewMode();
    vm->name   = _mode;
    vm->custom = _custom;
    vm->icon   = _iconName;

    if (_custom) {
      viewModes_.push_back(vm);
    } else {
      //insert before custom viewModes
      int i = viewModes_.size();
      for (int k=0; k < viewModes_.size(); k++)
        if (viewModes_[k]->custom == true){
          i = k;
          break;
        }
        viewModes_.insert(i,vm);
    }
  } else {
    vm = viewModes_[id];
  }

  vm->icon = _iconName;

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
  slotChangeView(_mode, QStringList(), QStringList());
}

void CoreWidget::slotAddViewModeComplete(QString _mode , bool _custom, QStringList _toolboxes, QStringList _toolbars) {
  slotAddViewModeToolbars(_mode,_custom,_toolbars);
  slotAddViewModeToolboxes(_mode,_custom,_toolboxes);
}

/// show dialog for changing ViewMode
void CoreWidget::slotViewModeDialog(){
  //init widget
  static viewModeWidget* widget = 0;
  if ( !widget ){
    widget = new viewModeWidget(viewModes_);
    widget->setWindowIcon( OpenFlipper::Options::OpenFlipperIcon() );
    connect(widget, SIGNAL(changeView(QString, QStringList, QStringList)), this, SLOT(slotChangeView(QString, QStringList, QStringList)) );
    connect(widget, SIGNAL(saveMode(QString, bool, QStringList, QStringList)), this, SLOT(slotAddViewModeComplete(QString, bool, QStringList, QStringList)) );
    connect(widget, SIGNAL(removeMode(QString)), this, SLOT(slotRemoveViewMode(QString)) );
  }
  widget->show( OpenFlipper::Options::defaultToolboxMode() );
}

void CoreWidget::slotViewChangeDialog() {
  //init widget
  static viewModeChangeWidget* modeChangeWidget = 0;

  if ( !modeChangeWidget ){
    modeChangeWidget = new viewModeChangeWidget(viewModes_, this);
    modeChangeWidget->setWindowIcon( OpenFlipper::Options::OpenFlipperIcon() );
    connect(modeChangeWidget, SIGNAL(changeView(QString, QStringList, QStringList)), this, SLOT(slotChangeView(QString, QStringList, QStringList)) );
  }

  // Make it look like a dialog
  modeChangeWidget->setWindowFlags(Qt::Popup);
  modeChangeWidget->show( OpenFlipper::Options::defaultToolboxMode() );

  // Move it to the position of the push button
  QPoint posButton = vmChangeButton_->mapToGlobal(vmChangeButton_->pos());
  modeChangeWidget->move( posButton);

}

/// Slot for Changing visible toolWidgets
void CoreWidget::slotChangeView(QString _mode, QStringList _toolboxWidgets, QStringList _toolbars ){

  //try to find Widgets if they aren't given
  if (_mode != "" && _toolboxWidgets.size() == 0 && _toolbars.size() == 0)
    for (int i=0; i < viewModes_.size(); i++)
      if (viewModes_[i]->name == _mode) {
        _toolboxWidgets = viewModes_[i]->visibleToolboxes;
        _toolbars       = viewModes_[i]->visibleToolbars;
      }

  // Remove all toolbox entries
  toolBox_->clear ();

  //find all widgets that should be visible
  for (int i=0; i < _toolboxWidgets.size(); i++)
    for (uint p=0; p < plugins_.size(); p++){
      for ( uint j = 0 ; j < plugins_[p].toolboxWidgets.size(); ++j )
        if (_toolboxWidgets[i] == plugins_[p].toolboxWidgets[j].first )
          toolBox_->addItem (plugins_[p].toolboxWidgets[j].second, plugins_[p].toolboxWidgets[j].first);
    }

  //find all Toolbars that should be visible and hide the others
  for (uint p=0; p < plugins_.size(); p++)
    for ( uint j = 0 ; j < plugins_[p].toolbars.size(); ++j )
      if (_toolbars.contains( plugins_[p].toolbars[j].first ) )
        plugins_[p].toolbars[j].second->show();
      else
        plugins_[p].toolbars[j].second->hide();


  // Check the Main Toolbar:
  if ( _toolbars.contains(tr("Main Toolbar")) )
    mainToolbar_->show();
  else
    mainToolbar_->hide();

  // Check the Main Toolbar:
  if ( _toolbars.contains(tr("Viewer Toolbar")) )
    viewerToolbar_->show();
  else
    viewerToolbar_->hide();


  if (_mode != "")
    OpenFlipper::Options::defaultToolboxMode(_mode);

}

void CoreWidget::stereoButtonContextMenu(const QPoint& _pos) {

    // Set values:
    stereoSettingsWidget_->stereoOpengl->setChecked(OpenFlipper::Options::stereoMode() == OpenFlipper::Options::OpenGL);
    stereoSettingsWidget_->stereoAnaglyph->setChecked(OpenFlipper::Options::stereoMode() == OpenFlipper::Options::AnaglyphRedCyan);
    stereoSettingsWidget_->stereoCustomAnaglyph->setChecked(OpenFlipper::Options::stereoMode() == OpenFlipper::Options::AnaglyphCustom);
    stereoSettingsWidget_->stereoPhilips->setChecked(OpenFlipper::Options::stereoMode() == OpenFlipper::Options::Philips);

    stereoSettingsWidget_->eyeDistance->setValue(OpenFlipper::Options::eyeDistance());
    stereoSettingsWidget_->focalDistance->setValue(OpenFlipper::Options::focalDistance() * 1000);

    // Philips stereo mode part
    stereoSettingsWidget_->headerContentType->setCurrentIndex(OpenFlipper::Options::stereoPhilipsContent());
    stereoSettingsWidget_->headerFactor->setValue(OpenFlipper::Options::stereoPhilipsFactor());
    stereoSettingsWidget_->headerOffsetCC->setValue(OpenFlipper::Options::stereoPhilipsOffset());
    stereoSettingsWidget_->factorCounter->setNum(OpenFlipper::Options::stereoPhilipsFactor());
    stereoSettingsWidget_->offsetCounter->setNum(OpenFlipper::Options::stereoPhilipsOffset());
    stereoSettingsWidget_->headerSelect->setCurrentIndex(OpenFlipper::Options::stereoPhilipsSelect());

//    // Default values are strange...
//    std::cerr << "Content type: " << OpenFlipper::Options::stereoPhilipsContent() << std::endl;
//    std::cerr << "Factor: " << OpenFlipper::Options::stereoPhilipsFactor() << std::endl;
//    std::cerr << "Offset: " << OpenFlipper::Options::stereoPhilipsOffset() << std::endl;
//    std::cerr << "Select: " << OpenFlipper::Options::stereoPhilipsSelect() << std::endl;

    // Show right stacked widget
    if (stereoSettingsWidget_->stereoPhilips->isChecked()) {
        stereoSettingsWidget_->stackedWidget->setCurrentIndex(1);
    } else {
        stereoSettingsWidget_->stackedWidget->setCurrentIndex(0);
    }

    // Move widget to the position of the cursor
    stereoSettingsWidget_->move(stereoButton_->mapToGlobal(_pos) - QPoint((int)(stereoSettingsWidget_->width()/2), 0));
    // Show widget
    stereoSettingsWidget_->show();
}

void CoreWidget::slotApplyStereoSettings(int /*_tmpParam*/) {

    // Update values
    if (stereoSettingsWidget_->stereoCustomAnaglyph->isChecked()) {
        // Update option entry
        OpenFlipper::Options::stereoMode(OpenFlipper::Options::AnaglyphCustom);
        // Show right stacked widget
        stereoSettingsWidget_->stackedWidget->setCurrentIndex(0);
    } else if (stereoSettingsWidget_->stereoAnaglyph->isChecked()) {
        OpenFlipper::Options::stereoMode(OpenFlipper::Options::AnaglyphRedCyan);
        stereoSettingsWidget_->stackedWidget->setCurrentIndex(0);
    } else if (stereoSettingsWidget_->stereoPhilips->isChecked()) {
        OpenFlipper::Options::stereoMode(OpenFlipper::Options::Philips);
        stereoSettingsWidget_->stackedWidget->setCurrentIndex(1);
    } else {
        OpenFlipper::Options::stereoMode(OpenFlipper::Options::OpenGL);
        stereoSettingsWidget_->stackedWidget->setCurrentIndex(0);
    }

    // Save everything
    OpenFlipper::Options::eyeDistance(stereoSettingsWidget_->eyeDistance->value());
    OpenFlipper::Options::focalDistance((float) stereoSettingsWidget_->focalDistance->value() / 1000);

    // Update labels that display the current values
    stereoSettingsWidget_->factorCounter->setNum(stereoSettingsWidget_->headerFactor->value());
    stereoSettingsWidget_->offsetCounter->setNum(stereoSettingsWidget_->headerOffsetCC->value());

    // Set option entries
    OpenFlipper::Options::stereoPhilipsContent(stereoSettingsWidget_->headerContentType->currentIndex());
    OpenFlipper::Options::stereoPhilipsFactor(stereoSettingsWidget_->headerFactor->value());
    OpenFlipper::Options::stereoPhilipsOffset(stereoSettingsWidget_->headerOffsetCC->value());
    OpenFlipper::Options::stereoPhilipsSelect(stereoSettingsWidget_->headerSelect->currentIndex());

    // Update all views
    for (unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i) {
        examiner_widgets_[i]->updateGL();
    }
}
