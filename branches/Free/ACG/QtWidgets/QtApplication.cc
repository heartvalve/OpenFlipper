/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2014 by Computer Graphics Group, RWTH Aachen      *
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
 *   $Revision$                                                       *
 *   $Author$                                                      *
 *   $Date$                   *
 *                                                                           *
\*===========================================================================*/



//=============================================================================
//
//  CLASS QtApplication - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================


// ACG
#include "QtApplication.hh"
//#include <ACG/QtWidgets/QtMacroDialog.hh>


// Qt
#include <QCursor>
#include <QWidget>
#include <QWheelEvent>
#include <QCloseEvent>
#include <QMoveEvent>
#include <QKeyEvent>
#include <QEvent>
#include <QTimerEvent>
#include <QContextMenuEvent>
#include <QMouseEvent>

// stdc++
#include <iostream>
#include <fstream>


//== NAMESPACES ===============================================================


namespace ACG {
namespace QtWidgets {

  
//== IMPLEMENTATION ========================================================== 


// input / output for QPoint
// static std::istream& operator>>(std::istream& _is, QPoint& _p) 
// { 
//   int x, y; 
//   _is >> x >> y; 
//   _p.setX(x); 
//   _p.setY(y); 
//   return _is; 
// }

// static std::ostream& operator<<(std::ostream& _os, const QPoint& _p) 
// {
//   _os << _p.x() << ' ' << _p.y(); 
//   return _os; 
// }


//-----------------------------------------------------------------------------


// input / output for QSize
// static std::istream& operator>>(std::istream& _is, QSize& _s) 
// { 
//   int w, h; 
//   _is >> w >> h; 
//   _s.setWidth(w); 
//   _s.setHeight(h); 
//   return _is; 
// }

// static std::ostream& operator<<(std::ostream& _os, const QSize& _s) 
// {
//   _os << _s.width() << ' ' << _s.height(); 
//   return _os; 
// }


//-----------------------------------------------------------------------------


// static std::string read_name(std::istream& _is)
// {
//   char cb[255];
//   _is.getline(cb, 255);
//   return std::string(cb);
// }


//-----------------------------------------------------------------------------


// copy qt event
template <class SomeEvent>
static QEvent* clone_event(QEvent* _event) 
{ return dynamic_cast<QEvent*>(new SomeEvent(*(dynamic_cast<SomeEvent*>(_event)) )); }



//-----------------------------------------------------------------------------


QtApplication::QtApplication(int _argc, char** _argv) 
  : QApplication(_argc, _argv),
    record_(false),
    playback_(false),
    play_loop_(false),
    eventnr_(0),
    timer_id_(0),
    dialog_(0),
    mainWidget_(0)
{
  // create new macro Dialog
//   dialog_ = new QtMacroDialog(0, "MacroDialog");


//   // connect functions to macro dialog
//   connect(dialog_,  SIGNAL(play()),
// 	  this,     SLOT(play()));
//   connect(dialog_,  SIGNAL(stop()),
// 	  this,     SLOT(stop()));    
//   connect(dialog_,  SIGNAL(record()),
// 	  this,     SLOT(record()));
//   connect(dialog_,  SIGNAL(loop(bool)),
// 	  this,     SLOT(loop(bool)));
//   connect(dialog_,  SIGNAL(saveFile(const char*)), 
// 	  this,     SLOT(saveFile(const char*)));
//   connect(dialog_,  SIGNAL(loadFile(const char*)), 
// 	  this,     SLOT(loadFile(const char*)));

}

//-----------------------------------------------------------------------------


void QtApplication::cleanUpEventBuffer()
{
  // delete memory of events
  for(unsigned int i=0; i<events_.size(); i++) 
    if (events_[i].event) 
      delete events_[i].event;

  events_.clear();
}


//-----------------------------------------------------------------------------


void QtApplication::loop(bool _b)
{
  play_loop_ = _b;
}


//-----------------------------------------------------------------------------


void QtApplication::play()
{
  // check for main widget
  mainWidget_ = activeWindow();
  //  mainWidget_ = mainWidget();
  if (!mainWidget_)
  {
    std::cerr << "No Main Widget defined!\n";
    return;
  }


  // stop recording first
  record_   = false;
  playback_ = true;
  eventnr_  = 0;

  restoreTopLevelSizes();

  mainWidgetDiff_ = mainWidget_->mapToGlobal(QPoint(0,0)) - oldMainWidgetPos_;

  time_.start();
  timer_id_ = startTimer(10);
}


//-----------------------------------------------------------------------------


void QtApplication::stop()
{
  record_ = false;
  playback_ = false;
  killTimer( timer_id_ );
}


//-----------------------------------------------------------------------------


void QtApplication::record()
{
  // check main widget
  mainWidget_ = activeWindow();
  //  mainWidget_ = mainWidget();
  if (!mainWidget_)
  {
    std::cerr << "No Main Widget defined!\n";
    return;
  }

  // stop playback first
  playback_ = false;
  killTimer( timer_id_ );

  record_ = true;

  cleanUpEventBuffer();
	
  storeTopLevelSizes();

  // save mainWidgetPos
  oldMainWidgetPos_ = mainWidget_->mapToGlobal(QPoint(0,0));

  // start recording Time
  time_.start();
}


//-----------------------------------------------------------------------------


bool QtApplication::notify (QObject* _receiver, QEvent* _event)
{
//   // record events
//   if (record_ && !playback_)
//     recordEvent(_receiver, _event);


//   //check for recording or playback Keypresses
//   if (_event->type() == QEvent::KeyPress)
//   {
//     switch(((QKeyEvent*) _event)->key())
//     {
//       case Qt::Key_F5:
// 	dialog_->show();
// 	break;

//       case Qt::Key_F6:
// 	play();
// 	break;

//       case Qt::Key_F7:
// 	stop();
// 	break;

//       case Qt::Key_F8:
// 	record();
// 	break;
//     }
//   }


  // handle Event like Qt do
  return QApplication::notify(_receiver, _event);
}


//-----------------------------------------------------------------------------


void QtApplication::recordEvent(QObject* /* _receiver */ , QEvent* /* _event */ )
{
//   // save only events for widgets
//   if (_receiver->inherits("QWidget"))
//   {
//     QWidget*receiver = (QWidget*)_receiver;


//     // skip events for macro dialog
// //     if (receiver->activeWindow()->windowTitle() == "MacroDialog")
// //       return;
// //     if (receiver->topLevelWidget()->name() == "MacroDialog")
// //       return;


//     // generate event footprint
//     FootPrint fp;
//     fp.event      = 0;
//     fp.time       = time_.elapsed();
//     fp.cursorPos  = mainWidget_->mapFromGlobal(QCursor::pos());
//     //    fp.name       = receiver->name();
//     //    fp.classname  = receiver->className();
//     fp.position   = receiver->pos();
//     fp.size       = receiver->size();
// //     fp.parent     = (receiver->parent() ? 
// // 		     receiver->parent()->name() : "noParent");



//     switch(_event->type())
//     {

//       // mouse event ----------------------------------------------------------
//       case QEvent::MouseButtonPress:
//       case QEvent::MouseButtonRelease:
//       case QEvent::MouseMove:
//       case QEvent::MouseButtonDblClick:
//       {
// 	fp.event = clone_event<QMouseEvent>(_event);
// 	break;
//       }


//       // wheel event ----------------------------------------------------------
//       case QEvent::Wheel:
//       {
// 	fp.event = clone_event<QWheelEvent>(_event);
// 	break;
//       }


//       // key event ------------------------------------------------------------
//       case QEvent::KeyPress:
//       case QEvent::KeyRelease:
//       case QEvent::Shortcut:
//       case QEvent::ShortcutOverride:
//       {
// 	QKeyEvent* e = (QKeyEvent*) _event;

// 	// skip F5, F6, F7, F8
// 	if (e->key() != Qt::Key_F5 &&  e->key() != Qt::Key_F6 &&
// 	    e->key() != Qt::Key_F7 &&  e->key() != Qt::Key_F8)
// 	  fp.event = clone_event<QKeyEvent>(_event);
// 	break;
//       }


//       // context menu event ---------------------------------------------------
//       case QEvent::ContextMenu:
//       {
// 	fp.event = clone_event<QContextMenuEvent>(_event);
// 	break;
//       }


//       // move event -----------------------------------------------------------
//       case QEvent::Move:
//       {
// 	if (_event->spontaneous())
// 	  fp.event = clone_event<QMoveEvent>(_event);
// 	break;
//       }


//       // close event ----------------------------------------------------------
//       case QEvent::Close:
//       {
// 	// can be non-spontaneous !
// 	fp.event = clone_event<QCloseEvent>(_event);
// 	break;
//       }


//       default: // avoid warning
// 	break;
//     }


//     // if event has been handled, store it
//     if (fp.event)  events_.push_back(fp);
//   }
}


//-----------------------------------------------------------------------------


void QtApplication::playbackEvent(FootPrint & /* _fp */ )
{
//   // determine widget for saved event at runtime
//   QWidget* eventWidget = findWidget(_fp);
//   if (!eventWidget) return;


//   // position and size
//   if (eventWidget->isTopLevel())
//   {
//     // restore widget size
//     if (eventWidget->size() != _fp.size)
//       eventWidget->resize(_fp.size);

//     // correct position wrt to main widget
//     if (eventWidget != mainWidget_)
//     {
//       QPoint newWidgetPos = _fp.position + mainWidgetDiff_;

//       if (newWidgetPos != eventWidget->pos())
// 	eventWidget->move(newWidgetPos);
//     }
//   }


//   // global cursor position
//   QPoint globalPos = mainWidget_->mapToGlobal(_fp.cursorPos);
//   QCursor::setPos(globalPos);


//   // correct global mouse position in events
//   switch(_fp.event->type())
//   {

//     // mouse event ----------------------------------------------------------
//     case QEvent::MouseButtonPress:
//     case QEvent::MouseButtonRelease:
//     case QEvent::MouseMove:
//     case QEvent::MouseButtonDblClick:
//     {
//       QMouseEvent* e = (QMouseEvent*) _fp.event;
//       QMouseEvent me = QMouseEvent(e->type(),
// 				   e->pos(),
// 				   globalPos,
// 				   e->button(),
// 				   e->state());
    
//       notify((QObject*) eventWidget, &me);
//       break;
//     }


//     // wheel event ------------------------------------------------------------
//     case QEvent::Wheel:
//     {
//       QWheelEvent* e = (QWheelEvent*) _fp.event;
//       QWheelEvent we = QWheelEvent(e->pos(),
// 				   globalPos,
// 				   e->delta(),
// 				   e->state(),
// 				   e->orientation());

//       notify((QObject*) eventWidget, &we);
//       break;
//     }


//     // context menu event -----------------------------------------------------
//     case QEvent::ContextMenu:
//     {
//       QContextMenuEvent* e = (QContextMenuEvent*) _fp.event;
//       QContextMenuEvent ce = QContextMenuEvent(e->reason(),
// 					       e->pos(),
// 					       globalPos,
// 					       e->state());

//       notify((QObject*) eventWidget, &ce);
//       break;
//     }


//     // move event -----------------------------------------------------------
//     case QEvent::Move:
//     {
//       // pos has been adjusted above, but its still needed here...
//       break;
//     }


//     // all other events... ----------------------------------------------------
//     default:
//     {
//       // send event unmodified to the widget
//       notify((QObject*) eventWidget, _fp.event);
//       break;
//     }
//   }
}


//-----------------------------------------------------------------------------


QWidget* QtApplication::findWidget(FootPrint & /* _fp */ )
{
//   QWidget                *w;
//   std::vector<QWidget*>  candidates;
//   QWidgetList            list = allWidgets();

//   for (int i = 0; i < list.size(); ++i)
//   {
//     w = list.at(i);

//     if (_fp.name == w->name() && _fp.classname == w->className())
//       if (_fp.parent == (w->parent() ? w->parent()->name() : "noParent"))
// 	candidates.push_back(w);
//   }



//   if (candidates.size() == 1)
//     return candidates[0];

//   else
//   {
//     std::cerr << "Error: found " << candidates.size() 
// 	      << " widgets with name " << "\"" << _fp.name << "\"\n";
//     return 0;
//   }

  return 0;
}


//-----------------------------------------------------------------------------


void QtApplication::timerEvent(QTimerEvent* /* _e */ )
{
  // all events processed ?
  if (eventnr_ < events_.size())
  {
    // get current event
    FootPrint fp = events_[eventnr_];

    // right time to send ?
    if (fp.time < time_.elapsed())
    {
      // increase counter
      ++eventnr_;
     
      // send event to widget
      playbackEvent(fp);
    }
  }

  // restart or stop playing
  else 
  {
    if (play_loop_)  // restart
    {
      eventnr_ = 0;  
      time_.start();
    }
    else   killTimer( timer_id_ );  // stop
  }
}


//-----------------------------------------------------------------------------


void QtApplication::storeTopLevelSizes()
{
//   // clear buffer
//   toplevels_.clear();


//   QWidgetList    list = topLevelWidgets();
//   QWidget        *w;
//   FootPrint      fp;


//   for (int i = 0; i < list.size(); ++i)
//   {
//     w = list.at(i);

//     fp.name      = w->name();
//     fp.classname = w->className();
//     fp.parent    = w->parent() ? w->parent()->name() : "noParent";
//     fp.size      = w->size();

//     toplevels_.push_back(fp);
//   }
}


//-----------------------------------------------------------------------------


void QtApplication::restoreTopLevelSizes()
{
//   QWidget*  w;

//   for(FootPrintIter it=toplevels_.begin(); it!=toplevels_.end(); ++it)
//   {
//     // find widget to restore size
//     if ((w = findWidget(*it)))
//     {
//       // size differs -> restore it
//       if (w->size() != it->size)
//       	w->resize(it->size);
//     }
//   }
}


//-----------------------------------------------------------------------------


void QtApplication::saveFile(const char* /* _filename */ )
{
//   std::ofstream ofs(_filename, std::ios::out);
//   if (!ofs) return;


//   // write File Signature
//   ofs << "QTMacro " << events_.size() << std::endl;
//   ofs << oldMainWidgetPos_ << std::endl;


//   // save toplevel sizes to file
//   saveTopLevelSizes(ofs);


//   // save all events
//   for(FootPrintIter it=events_.begin(); it!=events_.end(); ++it)
//   {
//     ofs << "###" << std::endl;
    
//     ofs << it->time                << std::endl;
//     ofs << it->name                << std::endl;
//     ofs << it->classname           << std::endl;
//     ofs << it->parent              << std::endl;
//     ofs << it->cursorPos           << std::endl;
//     ofs << it->position            << std::endl;
//     ofs << it->size                << std::endl;
//     ofs << (int)it->event->type()  << std::endl;

    
//     switch(it->event->type())
//     {
//       // mouse events ---------------------------------------------------------
//       case QEvent::MouseButtonPress:
//       case QEvent::MouseButtonRelease:
//       case QEvent::MouseMove:
//       case QEvent::MouseButtonDblClick:
//       {
// 	QMouseEvent* e = (QMouseEvent*) it->event;
// 	ofs << e->pos()          << std::endl;
// 	ofs << e->globalPos()    << std::endl;
// 	ofs << (int)e->button()  << std::endl;
// 	ofs << (int)e->state()   << std::endl;
// 	break;
//       }


//       // wheel events ---------------------------------------------------------
//       case QEvent::Wheel:
//       {
// 	QWheelEvent* e = (QWheelEvent*) it->event;
// 	ofs << e->pos()               << std::endl;
// 	ofs << e->globalPos()         << std::endl;
// 	ofs << e->delta()             << std::endl;
// 	ofs << (int)e->state()        << std::endl;
// 	ofs << (int)e->orientation()  << std::endl;
// 	break;
//       }


//       // key events -----------------------------------------------------------
//       case QEvent::KeyPress:
//       case QEvent::KeyRelease:
//       case QEvent::Shortcut:
//       case QEvent::ShortcutOverride:
//       {
// 	QKeyEvent* e = (QKeyEvent*) it->event;
// 	ofs << (int)e->key()           << std::endl;
// 	ofs << (int)e->ascii()         << std::endl;
// 	ofs << (int)e->state()         << std::endl;
// 	ofs << (int)e->isAutoRepeat()  << std::endl;
// 	ofs << (int)e->count()         << std::endl;
// 	break;
//       }


//       // context menu events --------------------------------------------------
//       case QEvent::ContextMenu:
//       {
// 	QContextMenuEvent* e = (QContextMenuEvent*) it->event;
// 	ofs << (int)e->reason() << std::endl;
// 	ofs << e->pos()         << std::endl;
// 	ofs << e->globalPos()   << std::endl;
// 	ofs << (int)e->state()  << std::endl;
// 	break;
//       }


//       // move events ----------------------------------------------------------
//       case QEvent::Move:
//       {
//        	QMoveEvent* e = (QMoveEvent*) it->event;
// 	ofs << e->pos()    << std::endl;
// 	ofs << e->oldPos() << std::endl;
// 	break;
//       }


//       // close events ---------------------------------------------------------
//       case QEvent::Close:
//       {
// 	// nothing has to be stored
// 	break;
//       }


//       default: // avoid warning
// 	break;
//     }
//   }

//   ofs.close();
}


//-----------------------------------------------------------------------------


void QtApplication::loadFile(const char* /* _filename */ )
{
//   std::ifstream ifs(_filename, std::ios::in);
//   if (!ifs) return;


//   // helper
//   std::string  s;
//   int          size = 0;
//   FootPrint    fp;



//   // check header
//   ifs >> s;
//   if (s != "QTMacro")
//   {
//     std::cerr << "wrong file format!!!" << std::endl;
//     return;
//   }
//   ifs >> size >> oldMainWidgetPos_;


//   // alloc
//   cleanUpEventBuffer();
//   events_.clear();


//   // load toplevel sizes
//   loadTopLevelSizes(ifs);


//   // load all events
//   for(int i=0; i<size; ++i)
//   {
//     fp.event = 0;


//     // check for errors
//     ifs >> s;
//     if (s!="###")
//     {
//       std::cerr << "Event parsing error: " << s << std::endl;
//       return;
//     }


//     // general event data
//     ifs >> fp.time;
//     ifs.ignore(1, '\n');
//     fp.name      = read_name(ifs);
//     fp.classname = read_name(ifs);
//     fp.parent    = read_name(ifs);
//     ifs >> fp.cursorPos >> fp.position >> fp.size;
//     ifs.ignore(1, '\n');


//     // determine event type
//     int type;
//     ifs >> type;
    

//     // type specific data
//     switch(type)
//     {

//       // mouse events ---------------------------------------------------------
//       case QEvent::MouseButtonPress:
//       case QEvent::MouseButtonRelease:
//       case QEvent::MouseMove:
//       case QEvent::MouseButtonDblClick:
//       {
// 	QPoint pos, globalPos;
// 	int button, state;
	
// 	ifs >> pos >> globalPos >> button >> state;

// 	fp.event = new QMouseEvent((QEvent::Type)type,
// 				   pos,
// 				   globalPos,
// 				   button,
// 				   state);
// 	break;
//       }


//       // mouse wheel events ---------------------------------------------------
//       case QEvent::Wheel:
//       {
// 	int delta, state, orientation;
// 	QPoint pos, globalPos;

// 	// read values from file
// 	ifs >> pos >> globalPos >> delta >> state >> orientation;

// 	fp.event = new QWheelEvent(pos,
// 				   globalPos,
// 				   delta,
// 				   state,
// 				   (Qt::Orientation) orientation);
// 	break;
//       }


//       // key events -----------------------------------------------------------
//       case QEvent::KeyPress:
//       case QEvent::KeyRelease:
//       case QEvent::Shortcut:
//       case QEvent::ShortcutOverride:
//       {
// 	int key, ascii, state, count, isAutoRepeat;
// 	char text[2];

// 	ifs >> key >> ascii >> state >> isAutoRepeat >> count;

// 	text[0] = (char)ascii; 
// 	text[1] = '\0';

// 	fp.event = new QKeyEvent((QEvent::Type)type,
// 				 key,
// 				 ascii,
// 				 state,
// 				 QString(text),
// 				 isAutoRepeat,
// 				 count);
// 	break;
//       }



//       // context menu events --------------------------------------------------
//       case QEvent::ContextMenu:
//       {
// 	int reason, state;
// 	QPoint pos, globalPos;

// 	ifs >> reason >> pos >> globalPos >> state;

// 	fp.event = new QContextMenuEvent((QContextMenuEvent::Reason)reason,
// 					 pos,
// 					 globalPos,
// 					 state);
// 	break;
//       }


//       // move events ----------------------------------------------------------
//       case QEvent::Move:
//       {
// 	QPoint pos, oldPos;

// 	ifs >> pos >> oldPos;

// 	fp.event = new QMoveEvent(pos, oldPos);
// 	break;
//       }


//       // close events ---------------------------------------------------------
//       case QEvent::Close:
//       {
// 	fp.event = new QCloseEvent();
// 	break;
//       }



//       default:
//       {
// 	std::cerr << "Error: unknown event type: " << type << std::endl;
// 	break;
//       }
//     }
    

//     // store event
//     if (fp.event) events_.push_back(fp);
//   }


//   ifs.close();
}


//-----------------------------------------------------------------------------


void QtApplication::saveTopLevelSizes(std::ostream & /* _os */ )
{
//   _os << "TLS " << toplevels_.size() << std::endl;
  
//   for(FootPrintIter it=toplevels_.begin(); it!=toplevels_.end(); ++it)
//   {
//     _os << it->name       << std::endl;
//     _os << it->classname  << std::endl;
//     _os << it->parent     << std::endl;
//     _os << it->size       << std::endl;
//   }

//   _os << "ENDTLS" << std::endl;

}


//-----------------------------------------------------------------------------


void QtApplication::loadTopLevelSizes(std::istream & /* _is */ )
{
//   std::string  s;
//   int          size;
//   FootPrint    fp;


//   // check header
//   _is >> s;
//   if (s != "TLS")
//   {
//     std::cerr << "error in toplevel block (start)\n";
//     return;
//   }
//   _is >> size;
//   _is.ignore(1, '\n');

  
//   // read top level sizes
//   toplevels_.clear();

//   for(int i=0; i<size; i++)
//   {
//     // read names
//     fp.name      = read_name(_is);
//     fp.classname = read_name(_is);
//     fp.parent    = read_name(_is);

//     // read size
//     _is >> fp.size;
//     _is.ignore(1, '\n');

//     // store footprint
//     toplevels_.push_back(fp);
//   }


//   _is >> s;
//   if (s != "ENDTLS")
//   {
//     std::cerr << "Error in toplevel block (end)\n";
//     return;
//   }
//   _is.ignore(1, '\n');
}


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
