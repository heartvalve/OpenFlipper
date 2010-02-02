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




#include "loggerWidget.hh"

#include <OpenFlipper/common/GlobalOptions.hh>
#include <iostream>
#include <math.h>

LoggerWidget::LoggerWidget( QWidget *parent)
  : QWidget(parent)
{
  QVBoxLayout* vlayout  = new QVBoxLayout();
  QHBoxLayout* hlayout  = new QHBoxLayout();
  QHBoxLayout* hlayout2 = new QHBoxLayout();
  
  list_ = new QListWidget();
  
  list_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list_->setFocusPolicy(Qt::NoFocus);
  list_->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QString path = OpenFlipper::Options::iconDirStr()+OpenFlipper::Options::dirSeparator();
  
  context_ = new QMenu(tr("Log Viewer"));
  
  QAction* copyAction  = new QAction(QIcon(path + "edit-copy.png"), tr("Copy"),0);
  copyAction->setShortcut( QKeySequence("Ctrl+C") );
  QAction* selectAction = new QAction(tr("Select All"),0);
  selectAction->setShortcut( QKeySequence("Ctrl+A") );
  
  connect (copyAction, SIGNAL(triggered()), this, SLOT(copySelected()));
  connect (selectAction, SIGNAL(triggered()), list_, SLOT(selectAll()));
  
  context_->addAction(copyAction);
  context_->addSeparator();
  context_->addAction(selectAction);
  
  scrollBar_ = new QScrollBar(Qt::Vertical);
  blockNext_ = false;
  
  connect (scrollBar_, SIGNAL(valueChanged(int)), this, SLOT(scrollTo(int)));
  connect (list_->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(mapScrollPosition(int)));
  
  hlayout2->addWidget(list_);
  hlayout2->addWidget(scrollBar_);
  
  allButton_   = new QPushButton(QIcon(path + "status_all.png"),tr("All Messages"));
  allButton_->setCheckable(true);
  allButton_->setAutoExclusive(true);
  infoButton_  = new QPushButton(QIcon(path + "status_green.png"),tr("Informations"));
  infoButton_->setCheckable(true);
  infoButton_->setAutoExclusive(true);
  warnButton_  = new QPushButton(QIcon(path + "status_yellow.png"),tr("Warnings"));
  warnButton_->setCheckable(true);
  warnButton_->setAutoExclusive(true);
  errorButton_ = new QPushButton(QIcon(path + "status_red.png"),tr("Errors"));
  errorButton_->setCheckable(true);
  errorButton_->setAutoExclusive(true);

  allButton_->setChecked(true);

  connect(allButton_,   SIGNAL(clicked()), this, SLOT(updateList()));
  connect(infoButton_,  SIGNAL(clicked()), this, SLOT(updateList()));
  connect(warnButton_,  SIGNAL(clicked()), this, SLOT(updateList()));
  connect(errorButton_, SIGNAL(clicked()), this, SLOT(updateList()));

  clearButton_ = new QPushButton(QIcon(path + "edit-clear.png"),tr("Clear Messages"));
  connect(clearButton_, SIGNAL(clicked()), list_, SLOT(clear()));
  
  hlayout->addWidget( allButton_ );
  hlayout->addWidget( infoButton_ );
  hlayout->addWidget( warnButton_ );
  hlayout->addWidget( errorButton_ );
  hlayout->addStretch();
  hlayout->addWidget( clearButton_ );
  
  hlayout->setSpacing(0);
  hlayout->setContentsMargins (0,0,0,0);
  vlayout->setSpacing(0);
  vlayout->setContentsMargins (0,0,0,0);
  
  vlayout->addLayout( hlayout2 );
  vlayout->addLayout( hlayout );
  
  setLayout( vlayout );

}

//-------------------------------------------------------------------------------------

/// workaround for scrolling
void LoggerWidget::mapScrollPosition(int _pos){
 //map list_'s internal scrollbar position to our own scrollBar position
 blockNext_ = true;
 scrollBar_->setValue( floor( 100 * _pos / (double) list_->verticalScrollBar()->maximum() ) );
}

//-------------------------------------------------------------------------------------

/// workaround for scrolling
void LoggerWidget::scrollTo(int _pos){

  //if the scrollEvent was triggered by list_ ignore it
  if (blockNext_){
    blockNext_ = false;
    return;
  }

  // now it gets ugly ;)
  if (_pos < 5)
    list_->scrollToTop();
  else if (_pos > 95)
    list_->scrollToBottom();
  else
    list_->scrollToItem( list_->item( floor((list_->count()-1) * (_pos / 100.0)) ) );

  list_->hide();
  list_->show();
}

//-------------------------------------------------------------------------------------

/// Append a new logmessage to log viewer
void LoggerWidget::append(QString _text, Logtype _type){
  
  QTime time;
  time.start();
  
  list_->addItem(_text);

  QListWidgetItem* item = list_->item( list_->count()-1 );
  
  if ( allButton_->isChecked() )
    item->setHidden(false);
  else
    item->setHidden(true);
  
  switch (_type) {
    case LOGINFO:
      item->setForeground( QBrush(QColor(Qt::darkGreen)) );
      item->setBackground( QBrush(QColor(225,255,225), Qt::Dense4Pattern) );
      
      if ( infoButton_->isChecked() )
        item->setHidden(false);
      break;
    case LOGOUT:
      item->setForeground( QBrush(QColor(Qt::black)) );
      break;
    case LOGWARN:
      item->setForeground( QBrush(QColor(160,160,0)) );
      item->setBackground( QBrush(QColor(255,240,200),Qt::Dense4Pattern) );
      
      if ( warnButton_->isChecked() )
        item->setHidden(false);
      break;
    case LOGERR:
      item->setForeground( QBrush(QColor(Qt::red)) );
      item->setBackground( QBrush(QColor(255,225,225),Qt::Dense4Pattern) );
      
      if ( errorButton_->isChecked() )
        item->setHidden(false);
      break;
  }

  if ( !isHidden() )
    list_->scrollToBottom();

}

//-------------------------------------------------------------------------------------

/// update the list if a button was pressed
void LoggerWidget::updateList(){
  
  QColor color;
  
  if ( infoButton_->isChecked() )
    color = QColor(Qt::darkGreen);
  else if ( warnButton_->isChecked() )
    color = QColor(160,160,0);
  else if ( errorButton_->isChecked() )
    color = QColor(Qt::red);
  else
    color = QColor(Qt::black);
  
  if (color == QColor(Qt::black)){

    for (int i=0; i < list_->count(); i++)
      list_->item( i )->setHidden(false);
    
  } else {
    
    for (int i=0; i < list_->count(); i++)
      if ( list_->item(i)->foreground().color() == color )
        list_->item( i )->setHidden(false);
      else
        list_->item( i )->setHidden(true);
  }
  
  list_->scrollToBottom();
}

//-------------------------------------------------------------------------------------
void LoggerWidget::showEvent ( QShowEvent * event ) {
  list_->scrollToBottom();
}

//-------------------------------------------------------------------------------------

/// handle keyPressEvents
void LoggerWidget::keyPressEvent (QKeyEvent * _event ) {
  // Return key event to parent if not one of the standard key combinations ( ... Core )
  if ( (_event->modifiers() & Qt::ControlModifier ) && ( _event->key() == Qt::Key_C ) )
    copySelected();
 
  else if ( (_event->modifiers() & Qt::ControlModifier ) && ( _event->key() == Qt::Key_A ) )
    list_->selectAll();
  
  else
    _event->ignore();
}

//-------------------------------------------------------------------------------------

/// show context menu
void LoggerWidget::contextMenuEvent ( QContextMenuEvent * event ){
 
  QPoint p = list_->mapToGlobal( list_->pos() );
  
  int x = event->globalX();
  int y = event->y() + p.y();
  
  context_->popup( event->globalPos() );
}

//-------------------------------------------------------------------------------------

///copy Selected rows to clipboard
void LoggerWidget::copySelected (){
  
  QString str = "";
  
  for (int i=0; i < list_->selectedItems().count(); i++)
    str += (list_->selectedItems()[i])->text() + "\n";
  
 QClipboard *clipboard = QApplication::clipboard();

 clipboard->setText(str);
}
