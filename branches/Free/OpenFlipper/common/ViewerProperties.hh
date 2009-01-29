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
#include <ACG/Math/VectorT.hh>
#include <ACG/Scenegraph/SceneGraph.hh>
#include <ACG/GL/GLState.hh>

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
    //===========================================================================
    /** @name Mouse Wheel Settings
    * @{ */
    //===========================================================================
    //===========================================================================


    public slots:
      /// Zoom factor when using mousewheel
      double wheelZoomFactor() { return wZoomFactor_; };

      /// Zoom factor when using mousewheel and pressing shift
      double wheelZoomFactorShift() { return wZoomFactorShift_; };

      /// Set zoom factor when using mousewheel
      void wheelZoomFactor(double _factor) { wZoomFactor_ = _factor; };

      /// Set zoom factor when using mousewheel and pressing shift
      void wheelZoomFactorShift(double _factor) { wZoomFactorShift_ = _factor; }; ;

    private:
      double wZoomFactor_;

      double wZoomFactorShift_;

    /** @} */

    //===========================================================================
    //===========================================================================
    /** @name View Control
    * @{ */
    //===========================================================================
    //===========================================================================

    /** @} */

    //===========================================================================
    //===========================================================================
    /** @name Rendering Settings
    * @{ */
    //===========================================================================
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


    //===========================================================================

    public slots:

      /// Get current background color
      ACG::Vec4f backgroundColor() { return backgroundColor_; }

      /** Set background color.
      */
      void backgroundColor( ACG::Vec4f _color ) { backgroundColor_ = _color; emit updated(); };

    private:
      ACG::Vec4f backgroundColor_;

    //===========================================================================

    public slots:

      /// Render picking or not
      bool renderPicking() { return renderPicking_; };

      /// Mode used for the picking renderer
      ACG::SceneGraph::PickTarget renderPickingMode() { return pickRendererMode_; };

      void renderPickin(bool _mode ) { renderPicking_ = _mode; emit updated(); };

      /** Use this function to switch the viewer to visualize picking.
      * When you set this function to true the renderer will render the picking
      * elements rather then the scene
      */
      void renderPicking(bool _renderPicking, ACG::SceneGraph::PickTarget _mode) {
        pickRendererMode_ = _mode ; renderPicking_ = _renderPicking; emit updated();
      }

    private:
      /** This variable controls if the scene is rendered in normal or in picking mode
      */
      bool renderPicking_;

      /** If rendering is in picking mode, this variable controls which picking targets
      * will be rendered. ( see renderPicking_ , renderPicking(bool _renderPicking, PickRendererMode _mode) ,
      * and ACG::SceneGraph::PickTarget
      */
      ACG::SceneGraph::PickTarget pickRendererMode_;


    //===========================================================================

    public slots:
      /** Lock update of display.
      If locked (isUpdateLocked()) then all calls to updateDisplayList()
      and updateGL() will have no effect! This is true until the display is
      unlockUpdate()'ed.
      */
      void lockUpdate()  { locked_++; };

      /// Unlock display locked by updateLock().
      void unLockUpdate(){
        locked_-- ;
        if ( locked_ <0 ) {
          std::cerr << "More unlocks then locks" << std::endl;
          locked_ = 0;
        }
      };

      /** Are updateDisplayList() and updateGL() locked?
      (c.f. lockUpdate()) */
      bool updateLocked() { return (locked_ != 0); };

    private:
      int locked_;


    //===========================================================================

    public slots:
      /// Get current state of backface culling
      bool backFaceCulling() { return backFaceCulling_; };

      /// Enable or disable backface culling
      void backFaceCulling(bool _state ) { backFaceCulling_ = _state; emit updated(); }

    private:
      bool backFaceCulling_;

    //===========================================================================

    public slots:
      /// set 2-sided lighting on/off
      void twoSidedLighting(bool _state ) { twoSidedLighting_ = _state; glState_->set_twosided_lighting( _state ); emit updated(); }

      /// is 2-sided lighing enabled?
      bool twoSidedLighting() { return twoSidedLighting_; };

    private:
      bool twoSidedLighting_;

    //===========================================================================

    public slots:
      /// set 2-sided lighting on/off
      void animation(bool _state ) { animation_ = _state; emit updated(); }

      /// is 2-sided lighing enabled?
      bool animation() { return animation_; };

    private:
      bool animation_;


    //===========================================================================

    public:
      /// Get the glState of the Viewer
      ACG::GLState& glState() { return (*glState_); };

      void setglState(ACG::GLState* _glState) { glState_ = _glState; };

    private:
      /// Pointer to the glState of the Viewer
      ACG::GLState* glState_;


    /** @} */

    signals:
      /** \brief This signal is emitted when the configuration has been changed
      */
      void updated();

      void actionModeChanged( Viewer::ActionMode );


  };

}


#endif //VIEWERPROPERTIES_HH