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




//=============================================================================
//
//  CLASS CoreWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "CoreWidget.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>



//== IMPLEMENTATION ==========================================================

//=============================================================================

void CoreWidget::slotToggleStereoMode()
{
  stereoActive_ = !stereoActive_;

  if ( stereoActive_ ) {
    statusBar_->showMessage("Stereo enabled");
    stereoButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"mono.png") );
  } else {
    statusBar_->showMessage("Stereo disabled");
    stereoButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"stereo.png") );
  }

  for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    examiner_widgets_[i]->setStereoMode(stereoActive_);
}

void CoreWidget::slotActionModeChanged( Viewer::ActionMode _mode ) {
  moveButton_->setDown(false);
  lightButton_->setDown(false);
  pickButton_->setDown(false);
  questionButton_->setDown(false);

  switch (_mode)
  {
    case Viewer::ExamineMode:
    {
      moveButton_->setDown(true);
      break;
    }


    case Viewer::LightMode:
    {
      lightButton_->setDown(true);
      break;
    }


    case Viewer::PickingMode:
    {
      pickButton_->setDown(true);
      break;
    }


    case Viewer::QuestionMode:
    {
      questionButton_->setDown(true);
      break;
    }
  }
}

void CoreWidget::slotSetGlobalBackgroundColor() {
  ACG::Vec4f bc = PluginFunctions::viewerProperties().backgroundColor() * 255.0;

  QColor backCol((int)bc[0], (int)bc[1], (int)bc[2], (int)bc[3]);
  QColor c = QColorDialog::getColor(backCol,this);

  if (c != backCol && c.isValid())
    for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets(); ++i ) 
      PluginFunctions::viewerProperties(i).backgroundColor(ACG::Vec4f(((double) c.redF())   ,
                                                                      ((double) c.greenF()) ,
                                                                      ((double) c.blueF())  ,
                                                                        1.0));

  OpenFlipper::Options::defaultBackgroundColor( c );

}

void CoreWidget::slotSetContextBackgroundColor() {
  ACG::Vec4f bc = PluginFunctions::viewerProperties().backgroundColor() * 255.0;

  QColor backCol((int)bc[0], (int)bc[1], (int)bc[2], (int)bc[3]);
  QColor c = QColorDialog::getColor(backCol,this);

  if (c != backCol && c.isValid())
    PluginFunctions::viewerProperties().backgroundColor(ACG::Vec4f(((double) c.redF())   ,
                                                                    ((double) c.greenF()) ,
                                                                    ((double) c.blueF())  ,
                                                                     1.0));
}


/// Set the viewer to home position
void CoreWidget::slotContextHomeView() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->home();
}


/// Set the viewer to home position
void CoreWidget::slotGlobalHomeView() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->home();
}

/// Set the viewers home position
void CoreWidget::slotContextSetHomeView() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->setHome();
}


/// Set the home position for all viewers
void CoreWidget::slotGlobalSetHomeView() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->setHome();
}

/// Change view on active viewer to view complete scene
void CoreWidget::slotContextViewAll() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->viewAll();
}


/// Change view on all viewers to view complete scene
void CoreWidget::slotGlobalViewAll() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->viewAll();
}

/// Toggle projection Mode of the active viewer
void CoreWidget::slotContextSwitchProjection() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->toggleProjectionMode();
}

/// Toggle projection Mode of all viewers to perspective projection
void CoreWidget::slotGlobalPerspectiveProjection() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->perspectiveProjection();
}

/// Toggle projection Mode of all viewers to orthogonal projection
void CoreWidget::slotGlobalOrthographicProjection() {
  for ( int i = 0 ; i < PluginFunctions::viewers() ; ++i )
    examiner_widgets_[i]->orthographicProjection();
}

/// Set the animation Mode for all viewers
void CoreWidget::slotGlobalChangeAnimation(bool _animation){
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    PluginFunctions::viewerProperties(i).animation(_animation);
}

/// Set the animation Mode for active viewer
void CoreWidget::slotLocalChangeAnimation(bool _animation){
  PluginFunctions::viewerProperties().animation(_animation);
}

/// Set Backface culling for all viewers
void CoreWidget::slotGlobalChangeBackFaceCulling(bool _backFaceCulling){
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    PluginFunctions::viewerProperties(i).backFaceCulling(_backFaceCulling);

}

/// Set Backface culling for active viewer
void CoreWidget::slotLocalChangeBackFaceCulling(bool _backFaceCulling){
  PluginFunctions::viewerProperties().backFaceCulling(_backFaceCulling);
}


/// Set two sided lighting for all viewers
void CoreWidget::slotGlobalChangeTwoSidedLighting(bool _lighting) {
  for ( uint i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
    PluginFunctions::viewerProperties(i).twoSidedLighting(_lighting);
}

/// Set two sided lighting for active viewer
void CoreWidget::slotLocalChangeTwoSidedLighting(bool _lighting) {
  PluginFunctions::viewerProperties().twoSidedLighting(_lighting);
}


void CoreWidget::slotSnapshot() {
  examiner_widgets_[PluginFunctions::activeExaminer()]->snapshot();
}

void CoreWidget::slotPasteView( ) {
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionPasteView();
}

void CoreWidget::slotCopyView( ) {
  examiner_widgets_[PluginFunctions::activeExaminer()]->actionCopyView();
}

   


//=============================================================================
