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
//   $Revision: 4482 $
//   $Author: moebius $
//   $Date: 2009-01-28 11:12:14 +0100 (Mi, 28. Jan 2009) $
//
//=============================================================================




//=============================================================================
//
//  Property storage Class for glWidgets
//
//=============================================================================

/**
 * \file ViewerProperties.hh
 * This file contains a class for managing all viewer settings
 */

//
#ifndef VIEWERPROPERTIES_HH
#define VIEWERPROPERTIES_HH

#include <QObject>

namespace Viewer {

  /// How to react on mouse events?
  enum ActionMode {
    // examine geometry, get transformations from mouse events
    ExamineMode,
    /** Picking mode. Actually no pick()ing is performed (!) but events are
        forwarded to the application via signalMouseEvent().
      */
    PickingMode,
    // same as picking, but emit signalMouseEventIdentify()
    QuestionMode,
    // Ligh rotation mode
    LightMode
  };

  class ViewerProperties : public QObject {

    Q_OBJECT

    public:
      ViewerProperties();
      ~ViewerProperties();

    //===========================================================================
    /** @name Action Mode States
    * @{ */
    //===========================================================================

    public slots:

      bool examineMode() { return(actionMode_ == Viewer::ExamineMode  ); };
      bool pickingMode() { return(actionMode_ == Viewer::PickingMode  ); };
      bool lightMode()   { return(actionMode_ == Viewer::LightMode    ); };
      bool questionMode(){ return(actionMode_ == Viewer::QuestionMode ); };

      /** Set action mode.
        The ActionMode determines the type of reaction on mouse events.
      */
      void setExamineMode() { actionMode(Viewer::ExamineMode  ); };
      void setPickingMode() { actionMode(Viewer::PickingMode  ); };
      void setLightMode()   { actionMode(Viewer::LightMode    ); };
      void setQuestionMode(){ actionMode(Viewer::QuestionMode ); };

    public:
      void               actionMode(Viewer::ActionMode _am);
      Viewer::ActionMode actionMode() { return actionMode_; };
      Viewer::ActionMode lastActionMode() { return lastActionMode_; };

    private :
      Viewer::ActionMode actionMode_, lastActionMode_;

    /** @} */


    //===========================================================================
    /** @name Snapshot settings
    * @{ */
    //===========================================================================

    public slots:
      /** Set the base file name and reset the counter for snapshots.

        The image file format is determined from the file name extension,
        the \a QImageIO formats are supported (e.g. ".ppm" [raw], ".png").

        The current snapshot counter will be added in front of the last "."
        in the filename.

        \a Note: Calling snapshotBaseFileName() will always reset the snapshot
        counter to 0.
      */
      void snapshotBaseFileName(const QString& _fname);

      /** Get the name for the current snapshot */
      QString snapshotName() { return snapshotName_; };

      /** Get the counter for the current snapshot and increases the counter */
      int snapshotCounter() { return snapshotCounter_++; };

    private:
      QString                      snapshotName_;
      int                          snapshotCounter_;

    /** @} */

    //===========================================================================
    /** @name Rendering Settings
    * @{ */
    //===========================================================================
    public slots:
      /** true if counter clockwise orientation should be used to define front facing orientation.
       */
      bool isCCWFront(){ return CCWFront_; };

      /// Set counter clockwise orientation as front
      void ccwFront() { CCWFront_ = true; emit updated(); };
      /// Set clockwise orientation as front
      void cwFront() { CCWFront_ = false; emit updated(); };

    private:
      bool CCWFront_;

    /** @} */

    signals:
      /** \brief This signal is emitted when the configuration has been changed
      */
      void updated();

      void actionModeChanged( Viewer::ActionMode );
  };

}


#endif //VIEWERPROPERTIES_HH