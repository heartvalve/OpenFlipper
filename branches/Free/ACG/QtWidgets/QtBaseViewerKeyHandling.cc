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
//  CLASS QtBaseViewer - IMPLEMENTATION
//
//=============================================================================


//== INCLUDES =================================================================

#include "QtBaseViewer.hh"
#include <QInputDialog>

//== NAMESPACES ===============================================================

namespace ACG {
namespace QtWidgets {


//== IMPLEMENTATION ==========================================================

void QtBaseViewer::disableKeyHandling(bool _state ) {
  disableKeyHandling_ = _state;
};

//-----------------------------------------------------------------------------

bool QtBaseViewer::keyHandlingState() {
  return disableKeyHandling_;
};

//-----------------------------------------------------------------------------

void QtBaseViewer::glKeyReleaseEvent(QKeyEvent* _event) {
  _event->ignore();
};

//-----------------------------------------------------------------------------

void QtBaseViewer::glKeyPressEvent(QKeyEvent* _event)
{
  // Pass key event to parent widget without any handling
  if ( disableKeyHandling_ ) {
    _event->ignore();
    return;
  }

  bool handled(false);


  // CTRL + ALT events
  if ((_event->modifiers() & Qt::ControlModifier) &&
      (_event->modifiers() & Qt::AltModifier))
  {

    switch (_event->key())
    {
      // Stereo steeing: eye distance
      case Qt::Key_E:
      {
        bool save(glareaGrabbed_);
        bool ok(false);

        // release grabbing to process input dialog
        if(save) releaseGLArea();

        double val = QInputDialog::getDouble( this, "Eye Dist", "Eye Dist:",
                                              eyeDist_,
                                              0.0, 100.0, 10,
                                              &ok);
        // restore old setting
        if(save) grabGLArea();

        if (ok) {
          eyeDist_ = val;
          handled = true;
          updateGL();
        }

        break;
      }


      // Stereo setting: focal length
      case Qt::Key_F:
      {
        bool save(glareaGrabbed_);
        bool ok(false);

        // release grabbing to process input dialog
        if(save) releaseGLArea();

        double val = QInputDialog::getDouble( this, "Focal Dist", "Focal Dist:",
                                              focalDist_,
                                              0.0, 100.0, 10,
                                              &ok);
        //restore old setting
        if(save) grabGLArea();

        if (ok) {
          focalDist_ = val;
          handled = true;
          updateGL();
        }
        break;
      }
    }
  } else { // Normal events

    switch (_event->key())
    {
      // Lock / unlock update
      case Qt::Key_ScrollLock:
      {
        if (!updateLocked_) {
          lockUpdate();
          handled = true;
          std::cerr << "Display update locked\n";

        } else {
          unlockAndUpdate();
          handled = true;
          std::cerr << "Display update un-locked\n";
        }
        break;
      }

      case Qt::Key_Escape:
      {
        actionMode(lastActionMode_);
        handled = true;
        break;
      }
    }
  }

  // If the event has not been handled by the baseviewer, check for Key events from subclasses
  if(!handled) {
    handled = viewKeyPressEvent(_event);
  }


  // give event to application
  if (!handled) {
    _event->ignore();
    emit(signalKeyPressEvent(_event));
  }

}




//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
