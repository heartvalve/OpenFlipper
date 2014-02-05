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
//  CLASS QtApplication
//
//=============================================================================

#ifndef QTAPPLICATION_HH
#define QTAPPLICATION_HH


//== INCLUDES =================================================================


// Qt
#include <QApplication>
#include <QObject>
#include <QEvent>
#include <QDateTime>

#include <QTimerEvent>

// stdc++
#include <vector>
#include <string>
#include <iostream>

#include "../Config/ACGDefines.hh"


//== FORWARD DECLARATIONS =====================================================


namespace ACG {
namespace QtWidgets {
  class QtMacroDialog;
}
}


//== NAMESPACES ===============================================================


namespace ACG {
namespace QtWidgets {


//== CLASS DEFINITION =========================================================


class ACGDLLEXPORT QtApplication : public QApplication
{
  Q_OBJECT

public:

  QtApplication(int _argc, char** _argv);
  virtual  ~QtApplication() { cleanUpEventBuffer(); }


public slots:

  // save recorded Events to file
  void saveFile(const char* _filename);

  // load recorded Events from file
  void loadFile(const char* _filename);

  // start playback
  void play();

  // stop playback or record
  void stop();

  // record
  void record();

  // change loop status for playback
  void loop(bool _b);


public:

  // filter events
  bool notify (QObject* _receiver, QEvent* _event);


private:

  // information for one event
  struct FootPrint
  {
    int          time;
    std::string  name;
    std::string  classname;
    std::string  parent;
    QPoint       cursorPos;
    QPoint       position;
    QSize        size;
    QEvent      *event;
  };

  typedef std::vector<FootPrint>  FootPrints;
  typedef FootPrints::iterator    FootPrintIter;


  // record and play events
  void recordEvent(QObject* _receiver, QEvent* _event);
  void playbackEvent(FootPrint & _fp);


  // store and restore sizes of all top-level widgets
  void storeTopLevelSizes();
  void restoreTopLevelSizes();

  // load and save sizes of top-level widgets
  void saveTopLevelSizes(std::ostream & _os);
  void loadTopLevelSizes(std::istream & _is);


  // find Widget for event _fp
  QWidget* findWidget(FootPrint & _fp);
  // timer callback
  void timerEvent(QTimerEvent* _e);
  // clear event buffer (pointers to QEvent)
  void cleanUpEventBuffer();


private:

  // state variables
  bool record_;
  bool playback_;
  bool play_loop_;

  // counter
  unsigned int eventnr_;

  // timer ID
  int timer_id_;

  // stop Time
  QTime time_;

  // vector to store events and top-levels sizes
  FootPrints events_, toplevels_;

  // macro dialog for interaction
  QtMacroDialog* dialog_;

  // Pointer to MainWidget
  QWidget* mainWidget_;

  // Main widget pos
  QPoint mainWidgetDiff_;

  // Old Main widget pos
  QPoint oldMainWidgetPos_;
};

//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
#endif // MACROAPPLICATION_HH defined
//=============================================================================
