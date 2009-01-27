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
//   $Revision: 2063 $
//   $Author: moebius $
//   $Date: 2008-06-22 17:32:25 +0200 (So, 22 Jun 2008) $
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

void CoreWidget::slotProjectionModeChanged( bool _ortho ) {
  if ( !_ortho )
    projectionButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"perspective.png") );
  else
    projectionButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"orthogonal.png") );

}

void CoreWidget::slotActionModeChanged( QtBaseViewer::ActionMode _mode ) {
  moveButton_->setDown(false);
  lightButton_->setDown(false);
  pickButton_->setDown(false);
  questionButton_->setDown(false);

  switch (_mode)
  {
    case QtBaseViewer::ExamineMode:
    {
      moveButton_->setDown(true);
      break;
    }


    case QtBaseViewer::LightMode:
    {
      lightButton_->setDown(true);
      break;
    }


    case QtBaseViewer::PickingMode:
    {
      pickButton_->setDown(true);
      break;
    }


    case QtBaseViewer::QuestionMode:
    {
      questionButton_->setDown(true);
      break;
    }
  }
}

void CoreWidget::slotFunctionMenuUpdate() {
  std::cerr << "DF" <<std::endl;
  if ( examiner_widgets_.empty() )
    return;

  QList< QAction *> allActions = functionMenu_->actions();

  for ( int i = 0 ; i <  allActions.size(); ++i ) {
    if ( allActions[i]->text() == "Synchronization" )
      allActions[i]->setChecked(examiner_widgets_[0]->synchronization());
    else if ( allActions[i]->text() == "Animation" )
      allActions[i]->setChecked(examiner_widgets_[0]->animation());
    else if ( allActions[i]->text() == "Backface Culling" )
      allActions[i]->setChecked(examiner_widgets_[0]->backFaceCulling());
    else if ( allActions[i]->text() == "Two-sided Lighting" )
      allActions[i]->setChecked(examiner_widgets_[0]->twoSidedLighting());
  }

}


//=============================================================================
