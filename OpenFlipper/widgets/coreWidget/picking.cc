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
//   $Revision: 5359 $
//   $Author: wilden $
//   $Date: 2009-03-19 16:43:39 +0100 (Thu, 19 Mar 2009) $
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

//-----------------------------------------------------------------------------

void CoreWidget::setActionMode(const Viewer::ActionMode _am){
  if (_am != actionMode_) {
    lastActionMode_ = actionMode_;
    actionMode_ = _am;


    // update Buttons

    moveButton_->setDown(false);
    lightButton_->setDown(false);
    pickButton_->setDown(false);
    questionButton_->setDown(false);

    switch (_am)
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

    //update Viewers

    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {

      examiner_widgets_[i]->sceneGraph( PluginFunctions::getSceneGraphRootNode() );

      examiner_widgets_[i]->trackMouse(false);


      switch ( _am )
      {
        case Viewer::ExamineMode:
        {
          examiner_widgets_[i]->setCursor(Qt::PointingHandCursor);
          break;
        }


        case Viewer::LightMode:
        {
          examiner_widgets_[i]->setCursor(Qt::PointingHandCursor);
          break;
        }


        case Viewer::PickingMode:
        {
          examiner_widgets_[i]->setCursor(Qt::ArrowCursor);
          if (pick_mode_idx_ != -1) {
            examiner_widgets_[i]->trackMouse(pick_modes_[pick_mode_idx_].tracking);
            examiner_widgets_[i]->setCursor(pick_modes_[pick_mode_idx_].cursor);
          }

          break;
        }


        case Viewer::QuestionMode:
        {
          examiner_widgets_[i]->setCursor(Qt::WhatsThisCursor);
          break;
        }
      }
    }

    //emit pickmodeChanged with either the name of the current pickmode or an empty string
    if( pickingMode() )
      emit(signalPickModeChanged(pick_mode_name_));
    else
      emit(signalPickModeChanged(""));

  }
}

//-----------------------------------------------------------------------------

void CoreWidget::getActionMode(Viewer::ActionMode& _am){
  _am = actionMode_;
}

//-----------------------------------------------------------------------------

void CoreWidget::setPickMode(const std::string _mode){

  for (unsigned int i=0; i<pick_modes_.size(); ++i)
  {
    if (pick_modes_[i].name == _mode)
    {
      pickMode( i );
      updatePickMenu();
      return;
    }
  }
}

//-----------------------------------------------------------------------------

void CoreWidget::getPickMode(std::string& _mode){
  _mode = pick_mode_name_;
}

//-----------------------------------------------------------------------------

void CoreWidget::updatePickMenu()
{
  if (pickMenu_ != 0)
    delete pickMenu_;

  pickMenu_ = new QMenu( 0 );
  connect( pickMenu_, SIGNAL( aboutToHide() ),
     this, SLOT( hidePopupMenus() ) );

  QActionGroup * ag = new QActionGroup( pickMenu_ );
  ag->setExclusive( true );

  for (unsigned int i=0; i<pick_modes_.size(); ++i) {
    if ( !pick_modes_[i].visible )
      continue;

    if (pick_modes_[i].name == "Separator")
    {
      if ((i > 0) && (i<pick_modes_.size()-1)) // not first, not last
        pickMenu_->addSeparator();
    }
    else
    {
      QAction* ac = new QAction( pick_modes_[i].name.c_str(), ag );
      ac->setData( QVariant( i ) );
      ac->setCheckable( true );

      if ((int)i == pick_mode_idx_)
        ac->setChecked( true );

      pickMenu_->addAction( ac );
    }
  }

  connect( ag, SIGNAL( triggered( QAction * ) ),
     this, SLOT( actionPickMenu( QAction * ) ));
}

//-----------------------------------------------------------------------------


void CoreWidget::actionPickMenu( QAction * _action )
{
  int _id = _action->data().toInt();
  if (_id < (int) pick_modes_.size() )
  {
    pickMode( _id );
  }

  setPickingMode();

  hidePopupMenus();
}

//-----------------------------------------------------------------------------


void CoreWidget::hidePopupMenus()
{

  if ( pickMenu_ )
  {
    pickMenu_->blockSignals(true);
    pickMenu_->hide();
    pickMenu_->blockSignals(false);
  }
}

//-----------------------------------------------------------------------------


void CoreWidget::pickMode( int _id )
{
  if (_id < (int) pick_modes_.size() )
  {
    pick_mode_idx_  = _id;
    pick_mode_name_ = pick_modes_[pick_mode_idx_].name;

    // adjust mouse tracking
    if ( pickingMode() )
      for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        examiner_widgets_[i]->trackMouse(pick_modes_[pick_mode_idx_].tracking);

    // adjust Cursor
    if ( pickingMode() )
      for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        examiner_widgets_[i]->setCursor( pick_modes_[pick_mode_idx_].cursor);
      

    // emit signal
    emit(signalPickModeChanged(pick_mode_name_));
  }
}


//-----------------------------------------------------------------------------


void CoreWidget::addPickMode(const std::string& _name,
                               bool _tracking,
                               int  _pos,
                               bool _visible,
                               QCursor _cursor)
{

  if ((unsigned int)_pos < pick_modes_.size())
  {
    std::vector<PickMode>::iterator it = pick_modes_.begin();
    it += _pos+1;
    pick_modes_.insert(it, PickMode(_name, _tracking, _visible, _cursor));
  }
  else
    pick_modes_.push_back(PickMode(_name, _tracking, _visible, _cursor));

  updatePickMenu();
}

//-----------------------------------------------------------------------------

void CoreWidget::setPickModeCursor(const std::string& _name, QCursor _cursor)
{
  for (uint i=0; i < pick_modes_.size(); i++)
    if ( pick_modes_[i].name == _name ){
      pick_modes_[i].cursor = _cursor;

      //switch cursor if pickMode is active
      if (pick_mode_name_ == _name && pickingMode() )
        for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
          examiner_widgets_[i]->setCursor(_cursor);
      break;
    }
}

//-----------------------------------------------------------------------------

void CoreWidget::setPickModeMouseTracking(const std::string& _name, bool _mouseTracking)
{
  for (uint i=0; i < pick_modes_.size(); i++)
    if ( pick_modes_[i].name == _name ){
      pick_modes_[i].tracking = _mouseTracking;

      //switch cursor if pickMode is active
      if (pick_mode_name_ == _name && pickingMode() )
        for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
          examiner_widgets_[i]->trackMouse(_mouseTracking);
      break;
    }
}

//-----------------------------------------------------------------------------


void CoreWidget::clearPickModes()
{
  pick_modes_.clear();
  pick_mode_idx_  = -1;
  pick_mode_name_ = "";
  updatePickMenu();
}


//-----------------------------------------------------------------------------


const std::string& CoreWidget::pickMode() const
{
  return pick_mode_name_;
}



