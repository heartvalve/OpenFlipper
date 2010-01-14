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
 *   $Revision: 83 $                                                         *
 *   $Author: moebius $                                                      *
 *   $Date: 2009-02-27 17:31:45 +0100 (Fr, 27. Feb 2009) $                   *
 *                                                                           *
\*===========================================================================*/




//=============================================================================
//
//  CLASS CoreWidget - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "CoreWidget.hh"

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>
#include <OpenFlipper/widgets/glWidget/CursorPainter.hh>

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
        moveButton_->setDown(true);
        break;
      case Viewer::LightMode:
        lightButton_->setDown(true);
        break;
      case Viewer::PickingMode:
        pickButton_->setDown(true);
        break;
      case Viewer::QuestionMode:
        questionButton_->setDown(true);
        break;
    }

    // update cursor
    switch ( _am )
    {
      case Viewer::ExamineMode:
        cursorPainter_->setCursor(QCursor( QPixmap( OpenFlipper::Options::iconDirStr() + QDir::separator() + "cursor_move.png"  )));
        break;
      case Viewer::LightMode:
        cursorPainter_->setCursor(QCursor( QPixmap( OpenFlipper::Options::iconDirStr() + QDir::separator() + "cursor_light.png"  )));
        break;
      case Viewer::PickingMode:
        cursorPainter_->setCursor(QCursor( QPixmap( OpenFlipper::Options::iconDirStr() + QDir::separator() + "cursor_arrow.png"  )));
        if (pick_mode_idx_ != -1) {
          cursorPainter_->setCursor(pick_modes_[pick_mode_idx_].cursor);
        }
        break;
      case Viewer::QuestionMode:
        cursorPainter_->setCursor(QCursor( QPixmap( OpenFlipper::Options::iconDirStr() + QDir::separator() + "cursor_whatsthis.png"  )));
        break;
    }

    //update Viewers

    for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i ) {

      examiner_widgets_[i]->sceneGraph( PluginFunctions::getSceneGraphRootNode() );
      examiner_widgets_[i]->trackMouse(false);


      switch ( _am )
      {
        case Viewer::ExamineMode:
          pickToolbar_->detachToolbar ();
          break;
        case Viewer::PickingMode:
          if (pick_mode_idx_ != -1) {
            examiner_widgets_[i]->trackMouse(pick_modes_[pick_mode_idx_].tracking);
            
            // Show the pickMode Toolbar for this picking mode if it is set
            if (pick_modes_[pick_mode_idx_].toolbar)
              pickToolbar_->attachToolbar (pick_modes_[pick_mode_idx_].toolbar);
            else
              pickToolbar_->detachToolbar ();
          }
          break;
        default:
          break;
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

    if (pick_modes_[pick_mode_idx_].toolbar)
      pickToolbar_->attachToolbar (pick_modes_[pick_mode_idx_].toolbar);
    else
      pickToolbar_->detachToolbar ();

    // adjust mouse tracking
    if ( pickingMode() )
      for ( unsigned int i = 0 ; i < OpenFlipper::Options::examinerWidgets() ; ++i )
        examiner_widgets_[i]->trackMouse(pick_modes_[pick_mode_idx_].tracking);

    // adjust Cursor
    if ( pickingMode() )
        cursorPainter_->setCursor( pick_modes_[pick_mode_idx_].cursor);

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
          cursorPainter_->setCursor(_cursor);
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

void CoreWidget::setPickModeToolbar( const std::string _mode , QToolBar * _toolbar )
{
  for (uint i=0; i < pick_modes_.size(); i++)
    if ( pick_modes_[i].name == _mode ){
      pick_modes_[i].toolbar = _toolbar;

      if (pick_mode_name_ == _mode && pickingMode() )
        pickToolbar_->attachToolbar (_toolbar);
      break;
    }
}

//-----------------------------------------------------------------------------

void CoreWidget::removePickModeToolbar( const std::string _mode )
{
  for (uint i=0; i < pick_modes_.size(); i++)
    if ( pick_modes_[i].name == _mode ){
      pick_modes_[i].toolbar = NULL;

      if (pick_mode_name_ == _mode && pickingMode() )
        pickToolbar_->detachToolbar ();
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



