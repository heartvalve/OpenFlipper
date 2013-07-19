/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *      Copyright (C) 2001-2011 by Computer Graphics Group, RWTH Aachen      *
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
//  CLASS QtBaseViewer
//
//=============================================================================

#ifndef ACG_QTBASEVIEWER_HH
#define ACG_QTBASEVIEWER_HH

//== INCLUDES =================================================================


#include "../GL/GLState.hh"
#include "../Scenegraph/SceneGraph.hh"
#include "../Scenegraph/DrawModes.hh"

#undef QT_NO_OPENGL
#include <QGLFormat>
#define QT_NO_OPENGL
#include <QBoxLayout>
#include <QtNetwork/QUdpSocket>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QDropEvent>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QMouseEvent>
#include <QAction>
#include <QSize>
#include <QMap>
#include <QString>
#include <QMenu>
#include <QToolBar>
#include <QTime>

#include <vector>
#include <string>


//== FORWARDDECLARATIONS ======================================================


class QPushButton;
class QToolButton;
class QStatusBar;
class QSplitter;
class QTimer;
class QImage;
class QSocketNotifier;
class QGraphicsWidget;
class QGraphicsGridLayout;


//== NAMESPACES ===============================================================


namespace ACG {
namespace QtWidgets {


//== FORWARDDECLARATIONS ======================================================


class QtWheel;
class QtGLGraphicsScene;
class QtGLGraphicsView;
class QtSceneGraphDialog;
class QtShiftPopupMenu;


//== CLASS DEFINITION =========================================================


/** \class QtBaseViewer QtBaseViewer.hh <ACG/QtWidgets/QtBaseViewer.hh>

    Base Viewer Widget: All viewers are derived from this one. Implement
    Mouse & Keyboard Events. Redefine Signals and Slots.
    See for example "QtExaminerViewer"
 **/

class ACGDLLEXPORT QtBaseViewer : public QWidget
{
Q_OBJECT

//-------------------------------------------------------------- public methods
public:

  /** Options for creating a QtBaseViewer.
    (cf. QtBaseViewer::QtBaseViewer(), applyOptions())
    */
  enum Options {
    /// all options disabled
    Nothing=0,

    /** Show \a private status bar? <br>
        If the widget is constructed (cf. QtBaseViewer::QtBaseViewer())
	with no status bar (\c _statusBar==0) then a \a private \c QStatusBar
	will be created. Use this option to show or hide it. It has \a no
	\a effect if the \c QStatusBar is provided externally.
     */
    ShowPrivateStatusBar=1,

    /** Show tool bar? <br>
        The tool bar consists of several buttons that can be used to control
	this view, etc. It will be placed vertically on the right side.

	\a Note: This toolbar inlcudes the wheels for translation along the
	z-axis (right, cf. ShowWheelZ)!

	The default setting is to show \a all buttons (see below).
     */
    ShowToolBar=2,

    /// Show pick button?  Effect only if ShowToolBar!
    ShowPickButton=4,
    /// Show lasso button? Effect only if ShowToolBar!
    ShowLassoButton=8,
    /// Show question button? Effect only if ShowToolBar!
    ShowQuestionButton=0x10,

    /// Show wheel for rotation around x-axis (left)?
    ShowWheelX=0x20,
    /// Show wheel for rotation around y-axis (bottom)?
    ShowWheelY=0x40,
    /** Show wheel for translation along z-axis (right)?
	Effect only if ShowToolBar!
     */
    ShowWheelZ=0x80,

    /// Show all three wheels (default)!
    ShowWheels=0xe0,

    /// default: all options (but not private statusbar) enabled
    DefaultOptions = 0xffff & (~ShowPrivateStatusBar)
  };




  //--------------------------------------------------- constructor / destructor

  /** Create a QtBaseViewer.
    @param _parent parent widget
    @param _name name (qt internal, qt debugging)
    @param _statusBar pointer to an existing status bar for this widget,
     if \c statusBar==0 then a \a private status bar will be created
    @param _format OpenGL context settings, will be passed to glarea()
    @param _share  Second base viewer for shared context
    @param _options QtBaseViewer::Options
  */
  QtBaseViewer( QWidget* _parent=0,
		const char* _name=0,
		QStatusBar *_statusBar=0,
		const QGLFormat* _format=0,
		const QtBaseViewer* _share=0,
		Options _options=DefaultOptions );

  /// Destructor.
  virtual ~QtBaseViewer();





  //------------------------------------------------------------- public methods

  virtual QSize sizeHint () const;

  /// get status bar, guaranteed to be not 0 (\c QStatusBar may be invisible)
  QStatusBar* statusBar() { return statusbar_; }
  /** Set status bar.
      Sets the \c QStatusBar widget that is used for informational
      output, e.g. the frame rate of the rotation animation.
      If \c sb==0 a \a private \c QStatusBar will be used,
      cf. Options. This private status bar may be invisible,
      but statusBar() will \a never return 0 even if setStatusBar(0)
      has been called!
  */
  void setStatusBar(QStatusBar* _sb);

  /// Apply ORed Options \c _options.
  void applyOptions(int _options);
  /// get current ORed Options
  int options() const { return options_; }


  /// Makes this widget the current widget for OpenGL operations.
  virtual void makeCurrent();
  /// Swaps the screen contents with the off-screen buffer.
  virtual void swapBuffers();


  /// get OpenGL state
  GLState& glState() { return *glstate_; }


  /** Set background color.
      Sets the OpenGL clear color to (_c.red(), _c.green(), _c.blue(), 0).
   */
  void backgroundColor(const Vec4f& _color) {
    glstate_->set_clear_color(_color); updateGL();
  }
  /// get background color
  Vec4f backgroundColor() { return glstate_->clear_color(); }


  /** Lock update of display.
      If locked (isUpdateLocked()) then all calls to updateDisplayList()
      and updateGL() will have no effect! This is true until the display is
      unlockUpdate()'ed.
  */
  virtual void lockUpdate();
  /// Unlock display locked by updateLock().
  virtual void unlockUpdate();
  /** Convenient unlock and update.
      Calls unlockUpdate() and then updateDisplayList() and updateGL()
      (Note that this has only an effect if !isUpdateLocked().)
  */
  virtual void unlockAndUpdate();
  /** Are updateDisplayList() and updateGL() locked?
      (c.f. lockUpdate()) */
  bool isUpdateLocked() const { return updateLocked_; }


  /// Lock update of projection matrix.
  void lockProjectionUpdate( void ) { projectionUpdateLocked_ = true; }

  /// Unlock update of projection matrix.
  void unlockProjectionUpdate( void ) { projectionUpdateLocked_ = false; }

  /// Is animation enabled?
  bool animation() const { return animation_; }
  /// Set animation enabled.
  void animation(bool _b);

  /// Enable/disable mouse tracking (move events with no button press)
  void trackMouse(bool _track);

  /// Enable/disable right button draw mode menu (default: enabled)
  void enablePopupMenu(bool _enable);






  /// Returns: root node of scene graph
  SceneGraph::BaseNode* sceneGraph() { return sceneGraphRoot_; }
  /// same but \c const
  const SceneGraph::BaseNode* sceneGraph() const { return sceneGraphRoot_; }
  /** Set scene graph.
      Sets the scene graph to the the graph rooted at \c _root.
      The available draw modes in the scene graph will be collected
      and the popup menu will be updated.<br>
      The \c sceneGraphToched() signal will be emitted (even if
      \c _root does not actually change).

      @param _root The root node of the scene graph
  */
  void sceneGraph(SceneGraph::BaseNode* _root);

  /// Navigation mode
  enum NavigationMode {
    NORMAL_NAVIGATION,     //!< Normal mode
    FIRSTPERSON_NAVIGATION  //!< First person mode
  };

   /// projection mode
  enum ProjectionMode {
    ORTHOGRAPHIC_PROJECTION, //!< orthographic
    PERSPECTIVE_PROJECTION   //!< perspective
  };

  /// Changes the projection mode and updates the projection matrix.
  void projectionMode(ProjectionMode _p);
  /// get current projection mode
  ProjectionMode projectionMode() const { return projectionMode_; }

  /// Changes the navigation mode
  void navigationMode(NavigationMode _n);
  /// get current navigation mode
  NavigationMode navigationMode() const { return navigationMode_; }

  /** Sets the center and dimension of the whole scene.  This point is
  used as fixpoint for rotations and to set the eye point far
  enough from the scene so that the whole scene is visible.
  */
  void setScenePos( const ACG::Vec3d& _center, double _radius, const bool _setCenter = true );

  /**
  * Set new center point of scene
  */
  void setSceneCenter( const ACG::Vec3d& _center );
  
  /** Get scene's center
  \see setScenePos()
  */
  const ACG::Vec3d& scene_center() const { return scene_center_; }
  /** Get scene's radius
  \see setScenePos()
  */
  double scene_radius() const { return scene_radius_; }
  
  void setSceneRadius(double radius) { scene_radius_ = radius; }
  
  /// set the viewing direction
  void viewingDirection( const ACG::Vec3d& _dir, const ACG::Vec3d& _up );
  
  /// set field of view y
  void setFovy( double _fovy );

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


  /** Set action mode.
      The ActionMode determines the type of reaction on mouse events.
      \a Note: You may also use the slots examineMode(), etc. to set this
      mode.
   */
  void actionMode(ActionMode);

  /// Return the last action mode
  ActionMode lastActionMode(){ return lastActionMode_; };

  /// get action mode
  ActionMode actionMode() const { return actionMode_; }


  /// orientation of the faces
 enum FaceOrientation {
    CCW_ORIENTATION, //!< counter clockwise (default)
    CW_ORIENTATION   //!< clockwise
  };

  /// set face orientation (used for back face culling, backFaceCulling())
  void faceOrientation(FaceOrientation);
  /// get face orientation
  FaceOrientation faceOrientation() const { return faceOrientation_; }

  /// set backface culling on/off
  void backFaceCulling( bool _b );
  /// is backface culling enabled?
  bool backFaceCulling() const { return backFaceCulling_; }

  /// set 2-sided lighting on/off
  void twoSidedLighting( bool _b );
  /// is 2-sided lighing enabled?
  bool twoSidedLighting() const { return twoSidedLighting_; }

  /// Automatically normalize normals?
  enum NormalsMode {
    /// use provided normals as is
    DONT_TOUCH_NORMALS,
    /** Automatically normalize normals.
        Input normals are not normalized \a or the transformation matrix
	does scaling or shearing.
     */
    NORMALIZE_NORMALS
  };

  /// set treatment of normals
  void normalsMode(NormalsMode _mode);
  /// get treatment of normals
  NormalsMode normalsMode() const { return normalsMode_; }


  /// get light rotation matrix
  GLMatrixd& light_matrix() { return light_matrix_; }
  /// rotete light sources
  void rotate_lights(Vec3d& _axis, double _angle);
  /// update light position
  void update_lights();


  /// copy current framebuffer to an QImage
  void copyToImage( QImage& _image, GLenum _buffer=GL_BACK) {
    copyToImage(_image, 0, 0, glWidth(), glHeight(), _buffer);
  }

  /// copy current framebuffer to an QImage
  void copyToImage( QImage& _image,
		    unsigned int _left,  unsigned int _top,
		    unsigned int _width, unsigned int _height,
		    GLenum _buffer );


  /// get width of QGLWidget
  unsigned int glWidth() const;
  /// get height of QGLWidget
  unsigned int glHeight() const;
  /// get size of QGLWIdget
  QSize glSize() const;
  /// map global to glarea coords
  QPoint glMapFromGlobal( const QPoint& _pos ) const;
  /// map glarea coords to global coords
  QPoint glMapToGlobal( const QPoint& _pos ) const;


  /// set draw mode (No test if this mode is available!)
  void drawMode(ACG::SceneGraph::DrawModes::DrawMode _mode)
  {
    curDrawMode_=_mode;
    updatePopupMenu();
  }

  /// get current draw mode
  ACG::SceneGraph::DrawModes::DrawMode drawMode() { return curDrawMode_; }

  /// convert current view to text representation
  void encodeView(QString& _view);
  /** Decode and apply text representation of view encoded by encodeView().
      If \c _view was successfully decoded it will immediately be applied
      and \c true is returned, \c false is returned else.
   */
  bool decodeView(const QString& _view);

  /// initialize modelview matrix to identity
  void initModelviewMatrix();

  // get all Mouse & Key Events for GlWidget
  void grabGLArea();

  // undo grabbing GLArea
  void releaseGLArea();

  /// One or two pass Drawing
  void drawBlendedObjects(bool _status) { blending_ = _status; }

  /// translate the scene and update modelview matrix
  void translate(const Vec3d& trans);

  /// rotate the scene (around its center) and update modelview matrix
  void rotate(const Vec3d& axis, double angle)
  { rotate(axis, angle, trackball_center_); }

  /// rotate the scene and update modelview matrix
  void rotate(const Vec3d& axis, double angle, const Vec3d& _center);

  /// Get the menu pointers (required to add them to the menubar as a temp workaround for a qt 4.3 bug
  QMenu * getPickMenu() { return pickMenu_; };
  QMenu * getFuncMenu() { return funcMenu_; };
  QMenu * getDrawMenu() { return drawMenu_; };

  /// Returns a pointer to the Toolbar
  QToolBar* getToolBar();

  /// Returns a pointer to the toolbar and removes it from the default position in the examiner widget
  QToolBar* removeToolBar();
  
  /// First person navigation: Move forward
  void moveForward();
  
  /// First person navigation: Move back
  void moveBack();
  
  /// First person navigation: Strafe left
  void strafeLeft();
  
  /// First person navigation: Strafe Right
  void strafeRight();

//---------------------------------------------------------------- public slots
public slots:

  /** Draws the scene immediately.
      This method does \a not trigger a paint event (like updateGL())
      but redraws the scene immediately. It does not swap buffers! This
      has to be done manually.
  */
  virtual void drawNow();
  /// Redraw scene. Triggers paint event for updating the view (cf. drawNow()).
  virtual void updateGL();

  /// set home position
  virtual void setHome();
  /// go to home pos
  virtual void home();
  /// view the whole scene
  virtual void viewAll();
  /// Fly to. Get closer if \c _move_back=\c false, get more distant else.
  virtual void flyTo(const QPoint& _pos, bool _move_back);
  /// Fly to, get closer.
  virtual void flyTo(const QPoint& _pos) { flyTo(_pos,false); }
  /// Fly to, get more distant.
  virtual void flyFrom(const QPoint& _pos) { flyTo(_pos,true); }

  /**  Fly to point and set new viewing direction (animated).
   * @param _position New viewer position ( the new eye point of the viewer )
   * @param _center   The new scene center ( the point we are looking at )
   * @param _time     Animation time in ms
   */
  virtual void flyTo(const Vec3d& _position,
		               const Vec3d& _center,
		               double       _time = 1000.0);

  /// calls actionMode() with \c ExamineMode (cf. ActionMode)
  virtual void examineMode() { actionMode(ExamineMode); }
  /// calls actionMode() with \c LightMode (cf. ActionMode)
  virtual void lightMode() { actionMode(LightMode); }
  /// calls actionMode() with \c QuestionMode (cf. ActionMode)
  virtual void questionMode() { actionMode(QuestionMode); }

  /// set perspective view (projectionMode(PERSPECTIVE_PROJECTION))
  virtual void perspectiveProjection();
  /// set orthographic view (projectionMode(ORTHOGRAPHIC_PROJECTION))
  virtual void orthographicProjection();
  /// toggle projection mode
  virtual void toggleProjectionMode();
  /// toggle navigation mode
  virtual void toggleNavigationMode();
  
  signals:
    
    void projectionModeChanged( bool _ortho );
    void navigationModeChanged( bool _normal );  

  public slots:
    
  /// show scenegraph widget
  virtual void showSceneGraphDialog();

  /// set view, used for synchronizing
  virtual void setView( const GLMatrixd& _modelview,
			const GLMatrixd& _inverse_modelview );

  void actionBackground();
  void actionCopyView();
  void actionPasteView();
  void actionPasteDropSize();
  void actionSynchronize();
  void actionAnimation();
  void actionBackfaceCulling();
  void actionTwoSidedLighting();
  void actionSynchronize(bool _enable);
  void actionAnimation(bool _enable);
  void actionBackfaceCulling(bool _enable);
  void actionTwoSidedLighting(bool _enable);

  void actionDrawMenu( QAction * _action );
  void actionPickMenu( QAction * _action );




//-------------------------------------------------------------- public signals
signals:

  /// emitted when OpenGL stuff can be initialized
  void signalInitializeGL();

  /** A signal for giving mouse events to the application.
      This is done if the Pick-Button (top-most button of the toolbar)
      is activated. Connect to this signal to implement application
      specific handling of mouse events.<br>
      \a Notes:
      \arg You have to test for the QMouseEvent::type() of the
      event yourself (press/release/move)!
      \arg signalMouseEventIdentify() is emitted on button press if
      actionMode()==Question.
  */
  void signalMouseEvent(QMouseEvent*, const std::string&);

  /// This signal does not send the pick mode and is kept for compatibility
  void signalMouseEvent(QMouseEvent*);

  /// Emitted in Pick mode. Uses pick mode.
  void signalWheelEvent(QWheelEvent*, const std::string&);

  /** Emitted instead o signalMouseEvent() if actionMode()==Question
      and \c _event is a mouse press event.
   */
  void signalMouseEventIdentify( QMouseEvent* );


  /// set view, used for synchronizing (cf. slotSetView())
  void signalSetView( const GLMatrixd& _modelview,
		      const GLMatrixd& _inverse_modelview );
  /// scene graph has changed
  void signalSceneGraphChanged(ACG::SceneGraph::BaseNode* _root);
  /// scene graph has changed
  void signalNodeChanged(ACG::SceneGraph::BaseNode* _node);
  /// action mode was changed
  void signalActionModeChanged(ACG::QtWidgets::QtBaseViewer::ActionMode _m);
  /// render callback
  void signalDrawScene(ACG::GLState* _state);


  signals:

  /** If popups are disabled context menu mode is set to custom Menu and the signal is passed
   * from the QGLWidget to this signal. You can use mapToGlobal for mapping this point to
   * global coords.
   */
  void signalCustomContextMenuRequested ( const QPoint & );


//----------------------------------------------------------- protected methods
protected:

  friend class QtGLGraphicsScene;
  friend class QtGLGraphicsView;

  /// initialize OpenGL states
  virtual void initializeGL();
  /// draw the scene. Triggered by updateGL().
  virtual void paintGL();
  /// handle resize events
  virtual void resizeGL(int _w, int _h);


  /// handle mouse press events
  virtual void glMousePressEvent(QMouseEvent* _event);
  /// handle mouse double click events
  virtual void glMouseDoubleClickEvent(QMouseEvent* _event);
  /// handle mouse move events
  virtual void glMouseMoveEvent(QMouseEvent* _event);
  /// handle mouse release events
  virtual void glMouseReleaseEvent(QMouseEvent* _event);
  /// handle mouse wheel events
  virtual void glMouseWheelEvent(QWheelEvent* _event);
  /// handle mouse press events
  virtual void glContextMenuEvent(QContextMenuEvent* _event);


  /// specialized viewer: hande mouse events
  virtual void viewMouseEvent( QMouseEvent* _event) = 0;
  /// specialized viewer: handle wheel events
  virtual void viewWheelEvent(QWheelEvent* _event) = 0;

  /// optional: hande mouse events to rotate light
  virtual void lightMouseEvent( QMouseEvent* /* _event */ ) {}


  protected:


  /// updates projection matrix
  void updateProjectionMatrix();
  /// update pick mode menu
  void updatePickMenu();



//------------------------------------------------------------- protected slots
protected slots:

  /// process graphics scene size changes
  virtual void sceneRectChanged(const QRectF & rect);

  /// process signals from wheelX_
  virtual void slotWheelX(double _dAngle);
  /// process signals from wheelX_
  virtual void slotWheelY(double _dAngle);
  /// process signals from wheelZ_
  virtual void slotWheelZ(double _dist);

  /// connected to scenegraph widget
  virtual void slotNodeChanged(ACG::SceneGraph::BaseNode* _node);

  /// correct ??? (same function as in qt src)
  virtual void cleanupEventFilter()
  { removeEventFilter( sender());}



//--------------------------------------------------------------- private slots
private slots:

  void hidePopupMenus();

//----------------------------------------------------------- private functions
private:

  // IDs for \c funcMenu_
  enum FuncMenuID {
    M_CopyView=0x100,
    M_PasteView,
    M_PasteDropSize,
    M_Animation,
    M_BackfaceCulling,
    M_TwoSidedLighting,
    M_Background,
    M_Snapshot,
    M_SnapshotName,
    M_SnapshotSavesView,
    M_Synchronize
  };

  /// Copy constructor. Never used!
  QtBaseViewer(const QtBaseViewer&);
  /// Assignment operator. Never used!
  QtBaseViewer& operator=(const QtBaseViewer&);

  // create widgets
  void createWidgets(const QGLFormat* _format,QStatusBar* _sb,
		     const QtBaseViewer* _share);

  /* Recursively draws each node in the scene graph.
      Called by paintGL(). */
  void drawScene();
  // helper called by drawScene().
  void drawScene_mono();
  // helper called by drawScene() when stereo viewing is active.
  void drawScene_stereo();


  // updates popup menu with the available draw modes
  void updatePopupMenu();

//-------------------------------------------------------------- protected data
protected:


  // scene center and radius
  Vec3d                        scene_center_, trackball_center_;
  double                       scene_radius_, trackball_radius_;


  // projection stuff
  GLdouble                     orthoWidth_;
  GLdouble                     near_, far_, fovy_;


  // scenegraph
  GLState                     *glstate_;


  // helper
  bool                         isRotating_;


  // current status bar, guaranteed to be !=0
  QStatusBar* statusbar_;



//---------------------------------------------------------------- private data
private:


  // data stored for home view
  Vec3d                        home_center_;
  double                       home_radius_;
  GLMatrixd                    home_modelview_,
                               home_inverse_modelview_;
  double                       homeOrthoWidth_;


  // matrix for rotating light position
  GLMatrixd                    light_matrix_;


  // modi
  NormalsMode                  normalsMode_;
  FaceOrientation              faceOrientation_;
  ProjectionMode               projectionMode_;
  NavigationMode               navigationMode_;
  
  ActionMode                   actionMode_, lastActionMode_;
  bool                         backFaceCulling_;
  bool                         twoSidedLighting_;
  bool                         animation_;

  // helper
  bool                         trackMouse_;
  bool                         popupEnabled_;
  bool                         glareaGrabbed_;
  double                       frame_time_;


  QMenu * pickMenu_;
  QMenu * funcMenu_;
  QMenu * drawMenu_;

  // scenegraph stuff
  SceneGraph::BaseNode*        sceneGraphRoot_;

  ACG::SceneGraph::DrawModes::DrawMode curDrawMode_,
                                       availDrawModes_;

  bool                         updateLocked_;
  bool                         projectionUpdateLocked_;
  bool                         blending_;


  // widget showing the scenegraph
  QtSceneGraphDialog*          sceneGraphDialog_;



  // options
  Options options_;
  // gl widget used as drawing area to paint the graphics scene
  QGLWidget* glWidget_;
  // graphics scene used to paint gl context and widgets
  QtGLGraphicsScene* glScene_;
  // graphics view that holds the gl scene
  QtGLGraphicsView* glView_;

  // Base graphics widget for wheels in the scene
  QGraphicsWidget* glBase_;
  // Base graphics widget layout
  QGraphicsGridLayout* glBaseLayout_;

  // private status bar, 0 if status bar is provided externally
  QStatusBar* privateStatusBar_;

  // Layout for the basic widget ( viewer + wheels )
  QGridLayout* glLayout_;

  // tool bar
  QToolBar    * buttonBar_;

  // set pick mode
  QToolButton* pickButton_;
  // set move mode
  QToolButton* moveButton_;
  // set light mode
  QToolButton* lightButton_;
  // set question mode
  QToolButton* questionButton_;
  // change projection mode (perspective/orthographic)
  QToolButton* projectionButton_;
  // go to home() position
  QToolButton* homeButton_;
  // setHome()
  QToolButton* setHomeButton_;
  // viewAll()
  QToolButton* viewAllButton_;
  // show sceneGraphDialog_
  QToolButton* sceneGraphButton_;
  // enable/disable stereo viewing
  QToolButton* stereoButton_;

  // rotate around x-axis
  QtWheel* wheelX_;
  // rotate around y-axis
  QtWheel* wheelY_;
  // translate along z-axis
  QtWheel* wheelZ_;

  // all actions
  QMap< QString, QAction * > action_;

  // vector of current draw mode actions
  std::vector< QAction * > drawMenuActions_;

  // Used to calculate the time passed between redraws
  QTime redrawTime_;
  
  //===========================================================================
  /** @name view handling
  * @{ */
  //===========================================================================
  signals:
    /// This signal is emitted when the scene is repainted due to any event.
    void viewUpdated();
    
    /// This signal is emitted whenever the view is changed by the user
    void viewChanged();  
    
  /** @} */    

  //===========================================================================
  /** @name Stereo
   * @{ */
  //===========================================================================

  public slots:

    /// toggle stereo mode
    virtual void toggleStereoMode();

    /// enable/disable stereo mode
    virtual void setStereoMode(bool _b);

  public:
    /** \brief Set eye distance for stereo
     *
     * Set the eye distance used for stereo projection. The function will enforce a redraw.
     */
    void setEyeDistance(double _distance);

    /// Get the current eye Distance
    double eyeDistance( );

    /** \brief Set focal distance for stereo
     *
     * Set the focal distance used for stereo projection. The function will enforce a redraw.
     */
    void setFocalDistance(double _distance);

    /// Get the current focal Distance
    double focalDistance( );

  private:
    // stereo stuff
    bool                         stereo_;
    double                       eyeDist_, focalDist_;


  /** @} */


  //===========================================================================
  /** @name Key handling
   * @{ */
  //===========================================================================

  signals:
    /** \brief Key Event received
     *
     * This signal is emitted if a Key press event is received which is not handled by
     * the widget itself. If internal key handling is disabled the events are passed to
     * the parent of this widget.
     */
    void signalKeyPressEvent(QKeyEvent*);

  public:
    /** \brief Enable or disable internal Key handling
     *
     * With this function you can enable or disable viewers internal key handling.
     * If disabled, all key events are passed to the widgets parent.
     */
    void disableKeyHandling(bool _state );

    /** \brief get the current key handling state.
     *
     */
    bool keyHandlingState();

  protected:

    /** \brief Get keyPress events from the glArea
     *
     * This function is called by the internal gl widget when receiving a key press event.
     */
    virtual void glKeyPressEvent(QKeyEvent*);

    /** \brief Get keyRelease events from the glArea
     *
     * This function is called by the internal gl widget when receiving a key release event.
     * Here these events are simply passed to the parent widget.
     *
     */
    virtual void glKeyReleaseEvent(QKeyEvent* _event);

    /** \brief Handle key events in view mode
     *
     * This funtion is called by the BaseViewer if a key press event occured in view mode.
     * This function has to be implemented by every viewer!
     *
     * @return If the derived class handled the event it has to return true otherwise false
     */
    virtual bool viewKeyPressEvent(QKeyEvent* _event) = 0;

  private:
    bool disableKeyHandling_;

  /** @} */

  //===========================================================================
  /** @name Drag and Drop
   * @{ */
  //===========================================================================

  protected:

    /// drag & drop for modelview copying
    virtual void startDrag();
    /// drag & drop for modelview copying
    virtual void glDragEnterEvent(QDragEnterEvent* _event);
    /// drag & drop for modelview copying
    virtual void glDropEvent(QDropEvent* _event);

  public:
    /// Enable or disable external dragging controls
    void setExternalDrag( bool _external ) { externalDrag_ = _external; };

  signals:
    /** Signal is emitted when Control modifier is pressed and mouse moded.
     * It will only be emitted if in externalDragMode ( see setExternalDrag )
     */
    void startDragEvent( QMouseEvent* _event );

    /** Signal is emitted only if in externalDragMode ( see setExternalDrag )
     * It will be send if a drag enters the gl widget.
     */
    void dragEnterEvent(QDragEnterEvent* _event);

    /** Signal is emitted only if in externalDragMode ( see setExternalDrag )
     * It will be send if a drop occurs in the gl widget.
     */
    void dropEvent( QDropEvent* _event );

  private:

    /** flag defining, if dragging will be handled internal by the viewer or passed via signals to
     * the application
    */
    bool externalDrag_;

  /** @} */


  //===========================================================================
  /** @name Synchronize different viewers
   * @{ */
  //===========================================================================

  public:
    /** Synchronize with other QtBaseViewer. A QtBaseViewer can
      be synchronized with other instances. The viewing position is
      copied from the other widget after each transformation.
    */
    void sync_connect(const QtBaseViewer*);

    /// unsync two sync_connect()ed QtBaseViewer's
    void sync_disconnect(const QtBaseViewer*);

    /// toggle global synchronization
    virtual void setSynchronization(bool _b);

    /// add host to synchronize with, given by its name
    bool add_sync_host(const QString& _name);
    /// add host to synchronize with, given by its address
    void add_sync_host(QHostAddress& _adr);

    bool synchronization();

  private slots:

    void sync_receive();
    void sync_send(const GLMatrixd& _modelview,
        const GLMatrixd& _inverse_modelview);

  private:
    /// synchronized with different viewer?
    bool                         synchronized_;

    /// Skips the next synch event
    bool                         skipNextSync_;

    /// socket used for synchronization
    QUdpSocket *                 socket_;

    /// List of hosts which should receive sync information
    std::vector<QHostAddress>    sync_hosts_;

  /** @} */

  //===========================================================================
  /** @name Snapshots
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
    virtual void snapshotBaseFileName(const QString& _fname);

    /** Trigger a snapshot and increase the snapshot counter.
        Save snapshot to file determined by snapshotBaseFileName() and
        the current snapshot counter. The \a back buffer will be saved.
    */
    virtual void snapshot();

    /** This action triggers a snapshot
     */
    void actionSnapshot();

    /** This action creates a widget for entering the snapshots name.
     */
    void actionSnapshotName();

    /** Doesn't do anything at the moment. Widget shows a checkbox for that.
     */
    void actionSnapshotSavesView();

  private:

    QString                      snapshotName_;
    int                          snapshotCounter_;
    QImage*                      snapshot_;

  /** @} */

  //===========================================================================
  /** @name Picking
   * @{ */
  //===========================================================================
  public:
    /** Apply pick action. <br>
     *   Information about the picked primitive is stored in the
     *   provided pointers. No information is stored if 0-pointers
     *   are given. Resulting values are defined only if \c true
     *   has been returned!<br>
     *
     * @param _pickTarget Select what should be picked: Faces/Vertices/...
     * @param _mousePos   The position to be used for picking
     * @param _nodeIdx    If picking is successfull this will contain the id of the scenegraph node picked.
     * @param _targetIdx  Special index defined by the picked Node. E.g. Facehandle/VertexHandle/.. for MeshNodes
     * @param _hitPointPtr Pointer to 3D point from picking
     * @return Successfull?
     */
    bool pick( SceneGraph::PickTarget _pickTarget,
               const QPoint& _mousePos,
               unsigned int& _nodeIdx,
               unsigned int& _targetIdx,
               Vec3d* _hitPointPtr=0 );

    /** get the coordinates of the picked point by z-buffer re-projection
     * @param _mousePos The position to pick
     * @param _hitPoint The point returned by the reprojection
     * @return Successfull?
     */
    bool fast_pick( const QPoint&  _mousePos,
                    Vec3d&         _hitPoint );

    /** \brief  add pick mode
     *
     *  @param _name           Name of the Pick Mode or "Separator" to insert a separator
     *  @param _mouse_tracking true: every mouse movement will emit mouse events not only when mousebutton is pressed
     *  @param _pos            position to insert the mode in the popup menu.
     *  @param _visible        Visible or hidden pick mode
     *  @param _cursor         Active cursor for that pick mode
     */
    void addPickMode(const std::string& _name,
                     bool               _mouse_tracking = false,
                     int                _pos            = -1,
                     bool               _visible        = true,
                     QCursor            _cursor         = Qt::ArrowCursor );

    /** clear all pick modes
     */
    void clearPickModes();

    /** return the currently active pick mode
     */
    const std::string& pickMode() const;

    /** Switch to given picking mode
     * @param _name Name of the picking mode
     */
    void pickMode(const std::string& _name);

    /** Switch to given picking mode
     * @param _id Id of the picking Mode
     */
    void pickMode( int _id );

    /** Use this function to switch the viewer to visualize picking.
     * When you set this function to true the renderer will render the picking
     * elements rather then the scene
     */
    void renderPicking(bool _renderPicking, ACG::SceneGraph::PickTarget _mode);

  public slots:

    /** calls actionMode() with \c PickingMode (cf. ActionMode)
     * Switches to pickingMode
     */
    virtual void pickingMode();

    /** \brief  set a new cursor for the pick mode
     *
     *  @param _name Name of the Pick Mode
     *  @param _cursor the new cursor
     */
    void setPickModeCursor(const std::string& _name, QCursor _cursor);

    /** \brief  set mouseTracking for the pick mode
     *
     *  @param _name Name of the Pick Mode
     *  @param _mouseTracking true: every mouse movement will emit mouse events not only when mousebutton is pressed
     */
    void setPickModeMouseTracking(const std::string& _name, bool _mouseTracking);

  signals:
    /** This signal is emitted when the pickMode is changed and contains the new PickMode
     */
    void signalPickModeChanged(const std::string&);

  private:

    /** Struct containing information about pickModes
     */
    struct PickMode
    {
      /// Constructor
      PickMode(const std::string& _n, bool _t, bool _v, QCursor _c) :
               name(_n), tracking(_t), visible(_v), cursor(_c) {}

      /** Name of the pickMode
       */
      std::string  name;

      /** MouseTracking enabled for this mode?
       */
      bool         tracking;

      /** Defines if the Mode will be visible in the popup Menu
       */
      bool         visible;

      /** Cursor used in  this pickMode
       */
      QCursor      cursor;
    };

    /** Vector of all Picking modes
     */
    std::vector<PickMode>  pick_modes_;

    /** Name of current pickMode
     */
    std::string            pick_mode_name_;

    /** Index of current pickMode
     */
    int                    pick_mode_idx_;

    /** This variable controls if the scene is rendered in normal or in picking mode
     */
    bool renderPicking_;

    /** If rendering is in picking mode, this variable controls which picking targets
     * will be rendered. ( see renderPicking_ , renderPicking(bool _renderPicking, PickRendererMode _mode) ,
     * and ACG::SceneGraph::PickTarget
     */
    ACG::SceneGraph::PickTarget pickRendererMode_;

  /** @} */
};


//=============================================================================
} // namespace QtWidgets
} // namespace ACG
//=============================================================================
#endif // ACG_QTBASEVIEWER_HH defined
//=============================================================================
