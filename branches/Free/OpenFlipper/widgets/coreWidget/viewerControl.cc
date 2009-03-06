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

void CoreWidget::slotProjectionModeChanged( bool _ortho ) {
  if ( !_ortho )
    projectionButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"perspective.png") );
  else
    projectionButton_->setIcon( QIcon(OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator()+"orthogonal.png") );

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

void CoreWidget::slotFunctionMenuUpdate() {
  std::cerr << "DF" <<std::endl;
  if ( examiner_widgets_.empty() )
    return;

  QList< QAction *> allActions = functionMenu_->actions();

  for ( int i = 0 ; i <  allActions.size(); ++i ) {
    if ( allActions[i]->text()      == "Animation" )
      allActions[i]->setChecked(PluginFunctions::viewerProperties().animation());
    else if ( allActions[i]->text() == "Backface Culling" )
      allActions[i]->setChecked(PluginFunctions::viewerProperties().backFaceCulling());
    else if ( allActions[i]->text() == "Two-sided Lighting" )
      allActions[i]->setChecked(PluginFunctions::viewerProperties().twoSidedLighting());
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

void CoreWidget::slotSetLocalBackgroundColor() {
  ACG::Vec4f bc = PluginFunctions::viewerProperties().backgroundColor() * 255.0;

  QColor backCol((int)bc[0], (int)bc[1], (int)bc[2], (int)bc[3]);
  QColor c = QColorDialog::getColor(backCol,this);

  if (c != backCol && c.isValid())
    PluginFunctions::viewerProperties().backgroundColor(ACG::Vec4f(((double) c.redF())   ,
                                                                    ((double) c.greenF()) ,
                                                                    ((double) c.blueF())  ,
                                                                     1.0));
}




//=============================================================================
