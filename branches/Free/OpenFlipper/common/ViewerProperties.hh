/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
*      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen       *
*                           www.openflipper.org                              *
*                                                                            *
*--------------------------------------------------------------------------- *
*  This file is part of OpenFlipper.                                         *
*                                                                            *
*  OpenFlipper is free software: you can redistribute it and/or modify       *
*  it under the terms of the GNU Lesser General Public License as            *
*  published by the Free Software Foundation, either version 3 of            *
*  the License, or (at your option) any later version with the               *
*  following exceptions:                                                     *
*                                                                            *
*  If other files instantiate templates or use macros                        *
*  or inline functions from this file, or you compile this file and          *
*  link it with other files to produce an executable, this file does         *
*  not by itself cause the resulting executable to be covered by the         *
*  GNU Lesser General Public License. This exception does not however        *
*  invalidate any other reasons why the executable file might be             *
*  covered by the GNU Lesser General Public License.                         *
*                                                                            *
*  OpenFlipper is distributed in the hope that it will be useful,            *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU LesserGeneral Public           *
*  License along with OpenFlipper. If not,                                   *
*  see <http://www.gnu.org/licenses/>.                                       *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
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
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/GL/GLState.hh>

// forward declaration
class ViewObjectMarker;

namespace Viewer {

  /** \brief Enum listing action modes of the viewers.
   *
   * These enums are used to control the reaction of the MouseInterface and the viewer on mouse events.
   */
  enum ActionMode {
    /** \brief Examine or Move mode
     *
     *  In this mode no events are passed to the MouseInterface.
     *  Only the viewer is active with mouse navigation.
     */
    ExamineMode,

    /** \brief Picking Mode
     *
     * In this mode the MouseInterface::slotMouseEvent() function gets called.
     *
     * The picking itself has to be performed by the plugins e.g. when implementing
     * \ref mouseInterfacePage
     *
     */
    PickingMode,

    /** \brief Question Mode
     *
     * In this mode the MouseInterface::slotMouseEventIdentify() function gets called
     *
     */
    QuestionMode,

    /** \brief Light mode
     *
     * In this mode the MouseInterface::slotMouseEventLight() function is called
     */
    LightMode
  };

  class DLLEXPORT ViewerProperties : public QObject {

    Q_OBJECT

    public:
      ViewerProperties(int _id);
      ~ViewerProperties();
      
    //===========================================================================
    /** @name Draw Mode States
     * @{ */
    //===========================================================================      

    public:
      /// set draw mode (No test if this mode is available!)
      void drawMode(ACG::SceneGraph::DrawModes::DrawMode _mode) { currentDrawMode_ = _mode;
                                                                  emit updated();
                                                                  emit drawModeChanged(viewerId_); };
      
      /// get current draw mode
      ACG::SceneGraph::DrawModes::DrawMode drawMode() { return currentDrawMode_; };
      
    private:
      
      ACG::SceneGraph::DrawModes::DrawMode currentDrawMode_;

    /** @} */
      
    //===========================================================================
    /** @name Snapshot settings
    * @{ */
    //===========================================================================

    public slots:
      /** Set the base file name and reset the counter for snapshots.

        The current snapshot counter will be added in front of the last "."
        in the filename.

        \a Note: Calling snapshotBaseFileName() will always reset the snapshot
        counter to 0.
      */
      void snapshotBaseFileName(const QString& _fname);

      /** Set the file type for snapshots. defaults to png" */
      void snapshotFileType(const QString& _type);
      
      /** Set the start index for the snapshot counter */
      void snapshotCounter(const int _counter){snapshotCounter_ = _counter;};
      
      /** Get the file type for the current snapshot */
      QString snapshotFileType() { return snapshotFileType_; };
      
      /** Get the name for the current snapshot */
      QString snapshotName() { return snapshotName_; };

      /** Get the counter for the current snapshot and increases the counter */
      int snapshotCounter() { return snapshotCounter_++; };

    private:
      QString                      snapshotName_;
      QString                      snapshotFileType_;
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

      /// Get current background color
      QColor backgroundQColor(){ QColor c;
                                 c.setRedF(  backgroundColor_[0]);
                                 c.setGreenF(backgroundColor_[1]);
                                 c.setBlueF( backgroundColor_[2]);
                                 c.setAlphaF(backgroundColor_[3]);
                                 return c; }

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

      /** Set background color.
      */
      void backgroundColor( QColor _color ) { backgroundColor_[0] = _color.redF();
                                              backgroundColor_[1] = _color.greenF();
                                              backgroundColor_[2] = _color.blueF();
                                              backgroundColor_[3] = _color.alphaF();
                                              emit updated(); };

    private:
      ACG::Vec4f backgroundColor_;


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
      /// set mipmapping on/off
      void mipmapping(bool _state ) { glState_->allow_mipmapping(_state); mipmapping_ = _state; emit updated(); }

      /// is mipmapping enabled?
      bool mipmapping() { return mipmapping_; };

    private:
      bool mipmapping_;

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

    public:
      bool rotationLocked();

      void rotationLocked(bool _lock);

    private:
      bool rotationLocked_;

    //===========================================================================

    /** @} */
    
    //===========================================================================
    //===========================================================================
    /** @name Projection controls and information
    * @{ */
    //===========================================================================
    //===========================================================================

    public:
      
      /// Get width of the gl scene in orthogonal projection mode (defaults to 2.0)
      double orthoWidth() { return orthoWidth_; };
      
      /// Set the width of the gl scene in orthogonal projection mode
      void orthoWidth(double _width){ orthoWidth_ = _width; emit updated(); };
      
      
      /// Return distance to near Plane
      double nearPlane(){ return nearPlane_;};
      
      /// set new near Plane distance
      void nearPlane(double _near){nearPlane_ = _near; emit updated(); };
      
      
      /// Return distance to far Plane
      double farPlane(){ return farPlane_;};
      
      /// set new near Plane distance
      void farPlane(double _far){farPlane_ = _far; emit updated(); };
      
      
      /// Get current scene center (rendering center)
      ACG::Vec3d sceneCenter(){ return sceneCenter_; };
      
      /// Set current scene center (rendering center)
      void sceneCenter(ACG::Vec3d _center){ sceneCenter_ = _center; emit updated(); };

      
      /// Get radius of the current scene
      double sceneRadius() {return sceneRadius_;};
      
      /// Set radius of the current scene
      void sceneRadius(double _radius ) { sceneRadius_ = _radius; emit updated();};
      
      
      /// Get virtual trackball center (rotation center when using mouse)
      ACG::Vec3d trackballCenter(){ return trackballCenter_; };
      
      /// Set virtual trackball center (rotation center when using mouse)
      void trackballCenter(ACG::Vec3d _center){ trackballCenter_ = _center; emit updated(); };
      
      
      /// Get trackball radius (rotation sphere when using mouse)
      double trackballRadius() {return trackballRadius_;};
      
      /// Set trackball radius   (rotation sphere when using mouse)
      void trackballRadius(double _radius ) { trackballRadius_ = _radius; emit updated();};
      
    private:
      
      /// Width of the gl scene in orthogonal mode ( defaults to 2.0 )
      double orthoWidth_;
      
      /// Distance to near Plane
      double nearPlane_;
      
      ///distance to far Plane
      double farPlane_;
      
      /// Current scene center (rendering center)
      ACG::Vec3d sceneCenter_;
      
      /// Radius of the current scene
      double sceneRadius_;
      
      /// Virtual trackball center (rotation center when using mouse)
      ACG::Vec3d trackballCenter_;
      
      /// trackball radius   (rotation sphere when using mouse)
      double trackballRadius_;
      
    /** @} */

    //===========================================================================
    //===========================================================================
    /** @name Stereo Settings
     * @{ */
    //===========================================================================
    //===========================================================================
    
    public:

      void stereo(bool _stereo) { stereo_ = _stereo; emit updated(); };
      bool stereo() {return stereo_; };

    private:
      /// Flag if stereo should be enabled for the current viewer
      bool stereo_;


    /** @} */

    signals:

      /** \brief This signal is emitted when the configuration has been changed
       *
      */
      void updated();

      /** \brief This signal is emitted when the drawmode changed
       *
       */
      void drawModeChanged(int _viewerId );

      void getPickMode(std::string& _mode );
      void setPickMode(const std::string&  _mode );

      void getActionMode(Viewer::ActionMode& _am);
      void setActionMode(const Viewer::ActionMode  _am);

    public:

      /// get active pick mode
      std::string pickMode();

      /// set the pickMode
      void pickMode(const std::string& _name);

      /// get the action mode
      Viewer::ActionMode actionMode();

      /// set active action mode
      void actionMode(const Viewer::ActionMode _am);

    //===========================================================================
    //===========================================================================
    /** @name Viewer id handling
     * @{ */
    //===========================================================================
    //===========================================================================

      /// Get the id of the viewer this viewerproperties belongs to
      int viewerId();

      /// Set the id of the viewer this viewerproperties belongs to
      void viewerId(int _id);

    private:

      /// Stores the id of the viewer this property belongs to
      int viewerId_;

      /// String for the Settings storage
      QString settingsSection_;

    /** @} */

  };

}


#endif //VIEWERPROPERTIES_HH

