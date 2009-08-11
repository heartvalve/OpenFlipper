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
#include <QColor>
#include <OpenFlipper/common/GlobalDefines.hh>
#include <ACG/Math/VectorT.hh>
#include <ACG/Scenegraph/SceneGraph.hh>
#include <ACG/GL/GLState.hh>

// forward declaration
class ViewObjectMarker;

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

  class DLLEXPORT ViewerProperties : public QObject {

    Q_OBJECT

    public:
      ViewerProperties();
      ~ViewerProperties();
      
    //===========================================================================
    /** @name Draw Mode States
     * @{ */
    //===========================================================================      

    public:
      /// set draw mode (No test if this mode is available!)
      void drawMode(unsigned int _mode) { currentDrawMode_ = _mode; emit updated(); };
      
      /// get current draw mode
      unsigned int drawMode() { return currentDrawMode_; };
      
    private:
      
      unsigned int currentDrawMode_;

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
      void wheelZoomFactorShift(double _factor) { wZoomFactorShift_ = _factor; }; 

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

      /// Get current background color
      QRgb backgroundColorRgb(){ QColor c;
                                 c.setRedF(  backgroundColor_[0]);
                                 c.setGreenF(backgroundColor_[1]);
                                 c.setBlueF( backgroundColor_[2]);
                                 c.setAlphaF(backgroundColor_[3]);
                                 return c.rgba(); };

      /** Set background color.
      */
      void backgroundColor( ACG::Vec4f _color ) { backgroundColor_ = _color; emit updated(); };

      /** Set background color.
      */
      void backgroundColor( QRgb _color ) { QColor c(_color);
                                            backgroundColor_[0] = c.redF();
                                            backgroundColor_[1] = c.greenF();
                                            backgroundColor_[2] = c.blueF();
                                            backgroundColor_[3] = c.alphaF();
                                            emit updated(); };

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
      void twoSidedLighting(bool _state ) { twoSidedLighting_ = _state; emit updated(); }

      /// is 2-sided lighing enabled?
      bool twoSidedLighting() { return twoSidedLighting_; };

    private:
      bool twoSidedLighting_;
      
    //===========================================================================

    public slots:
      /// set multisampling on/off
      void multisampling(bool _state ) { multisampling_ = _state; emit updated(); }

      /// is multisampling enabled?
      bool multisampling() { return multisampling_; };

    private:
      bool multisampling_;      

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

    //===========================================================================

    public slots:
      /// set object marker for viewer
      void objectMarker (ViewObjectMarker* _marker) { objectMarker_ = _marker; emit updated(); }

      /// current object marker
      ViewObjectMarker* objectMarker() { return objectMarker_; };

    private:
      ViewObjectMarker *objectMarker_;

    //===========================================================================

    public:
      int currentViewingDirection();

      void currentViewingDirection(int _dir);

    private:
      int currentViewingDirection_;

    //===========================================================================

    /** @} */

    signals:
      /** \brief This signal is emitted when the configuration has been changed
      */
      void updated();

      void getPickMode(std::string& _mode );
      void setPickMode(const std::string  _mode );

      void getActionMode(Viewer::ActionMode& _am);
      void setActionMode(const Viewer::ActionMode  _am);

    public:

      /// get active pick mode
      std::string pickMode();

      /// set the pickMode
      void pickMode(const std::string _name);

      /// get the action mode
      Viewer::ActionMode actionMode();

      /// set active action mode
      void actionMode(const Viewer::ActionMode _am);

  };

}


#endif //VIEWERPROPERTIES_HH

