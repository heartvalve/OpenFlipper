//=============================================================================
//
// CLASS PolyLinePlugin
//
// Author:  David Bommes <bommes@cs.rwth-aachen.de>
//
// Version: $Revision: 1$
// Date:    $Author$
//   $Date: 03-10-2007$
//
//
//=============================================================================

#ifndef POLYLINEPLUGIN_HH
#define POLYLINEPLUGIN_HH

#define CGAL_NO_AUTOLINK_MPFR
#define CGAL_NO_AUTOLINK_GMP

//== INCLUDES =================================================================

#include <QObject>
#include <QMenuBar>
#include <QTimer>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/common/Types.hh>
#include <ObjectTypes/PolyLine/PolyLine.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#include "PolyLineToolbarWidget.hh"

#include "QtPlaneSelect.hh"

#define CREATE_CUT_POLYLINE "Create Polyline"

//== CLASS DEFINITION =========================================================


/** Plugin for PolyLine Support
 */
class PolyLinePlugin : public BaseInterface, MouseInterface,
PickingInterface, ToolboxInterface, LoggingInterface, LoadSaveInterface, ToolbarInterface, ScriptInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
    Q_INTERFACES(MouseInterface)
    Q_INTERFACES(PickingInterface)
    Q_INTERFACES(ToolboxInterface)
    Q_INTERFACES(LoggingInterface)
    Q_INTERFACES(LoadSaveInterface)
    Q_INTERFACES(ToolbarInterface)
    Q_INTERFACES(ScriptInterface)

signals:
  
  // BaseInterface
  void updateView();
//   void updatedObject(int);
  void updatedObject(int _id, const UpdateType _type = UPDATE_ALL);

  // PickingInterface
  void addPickMode( const std::string& _mode );
  void addHiddenPickMode( const std::string& _mode );
  void setPickModeToolbar (const std::string& _mode, QToolBar * _toolbar);

  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

  // ToolbarInterface
  void addToolbar(QToolBar* _toolbar);
  void getToolBar( QString _name, QToolBar*& _toolbar);
  
  // ScriptInterface
  void scriptInfo( QString _functionName );
  
  // ToolboxInterface
  void addToolbox( QString _name  , QWidget* _widget );  
  
  void deleteObject( int _id );
  void addEmptyObject( DataType _type, int& _id);    


private slots :
  // BaseInterface
  void initializePlugin();

  void slotEditModeChanged();

  void slotMouseEvent( QMouseEvent* _event );

  void slotPickModeChanged( const std::string& _mode);

  void pluginsInitialized();

public slots :

  void slotEnablePickMode(QString _name);

public :

  /// Edit Mode of PolyLinePlugin
  enum EditMode { PL_NONE  =0,
                  PL_INSERT=1,
                  PL_DELETE,
                  PL_MOVE,
                  PL_SPLIT,
                  PL_MERGE,
                  PL_SMART_MOVE };

  /// default constructor
  PolyLinePlugin();

  /// default destructor
  ~PolyLinePlugin() {};

  /// Name of the Plugin
  QString name(){ return (QString("PolyLine")); };

  /// Description of the Plugin
  QString description() { return (QString("Handling of PolyLines (Insertion/Deletion/Modeling/...")); };

private slots:

  void slot_subdivide();
  void slot_subdivide_percent(bool _checked);
  void slot_decimate();
  void slot_decimate_percent(bool _checked);

#ifdef EXTENDED_POLY_LINE
  void slot_resample_on_edges();
#endif

  void slot_smooth();
  void slot_smooth( PolyLineObject*& _pol);
  void slot_project();
  void slot_project( PolyLineObject*& _pol);
  void slot_smooth_project();
  void slot_smooth_project( PolyLineObject*& _pol);

  void slot_smart_move_timer();

private :

  EditMode mode();

  // mouse events
  void me_insert    ( QMouseEvent* _event );
  void me_delete    ( QMouseEvent* _event );
  void me_move      ( QMouseEvent* _event );
  void me_split     ( QMouseEvent* _event );
  void me_merge     ( QMouseEvent* _event );
  void me_smart_move( QMouseEvent* _event );

  //===========================================================================
  /** @name ToolBox
  * @{ */
  //===========================================================================
  
  private :
    
    /// Widget for Toolbox
    PolyLineToolbarWidget* tool_;
  
  /** @} */  
  
  //===========================================================================
  /** @name ToolBar
  * @{ */
  //===========================================================================
  
  private :
    QAction*      polyLineAction_;
    QActionGroup* toolBarActions_;
    
    QToolBar*     toolbar_;
    
  private slots:
    
    /// Called by Toolbar to enable pick mode
    void slotSetPolyLineMode(QAction* _action);
    
  /** @} */  
  
  //===========================================================================
  /** @name PickToolBar
  * @{ */
  //===========================================================================
  
  private :
    QToolBar*     pickToolbar_;
    QActionGroup* pickToolBarActions_;
    
    QAction*      insertAction_;
    QAction*      deleteAction_;
    QAction*      moveAction_;
    QAction*      smartMoveAction_;
    QAction*      mergeAction_;
    QAction*      splitAction_;
    QAction*      cutAction_;
    
  private slots:
    
    /// Called by pick Toolbar
    void slotPickToolbarAction(QAction* _action);
    
  /** @} */
  
  //===========================================================================
  /** @name Template Functions
  * @{ */
  //===========================================================================
  
  private:
  
  /// get the points from the intersection between mesh and plane
  template< class MeshT > std::vector< ACG::Vec3d >
  getIntersectionPoints ( MeshT* _mesh, uint _fh, ACG::Vec3d _planeNormal ,
                          ACG::Vec3d _planePoint, bool& _closed );
                                                    
  /// get an edge of the mesh that is cut by the plane
  template< class MeshT >
  typename MeshT::EdgeHandle
  getCuttedEdge(MeshT& _mesh, ACG::Vec3d& _planeNormal, ACG::Vec3d& _planePoint);

  /** @} */
  
public slots:
  
  QString version() { return QString("1.02"); };
  
  //============================================
  
  /// Generates a polyLine of a plane intersection
  int generatePolyLineFromCut( int _objectId, Vector _planePoint, Vector _planeNormal, int _polyLineId = -1 );
  
private slots:
  
  /// Scissor Button was hit
  void slotScissorButton();
  
  /// Generate PolyLine after the cutPlane has been drawn
  void slotTriggerCutPlaneSelect();
  
private:
    int              cur_insert_id_;
    PolyLineObject  *cur_polyline_obj_;
    
    int               cur_move_id_;
    PolyLine::Point*  move_point_ref_;
    PolyLine::Point   move_point_orig_;
    
    int               cur_merge_id_;
    
    QTimer*           smart_move_timer_;
    
    PolyLineObject*   cur_smart_move_obj_;
    
    // Plane selection tool
    QtPlaneSelect*    planeSelect_;

};

#if defined(INCLUDE_TEMPLATES) && !defined(POLYLINEPLUGIN_CC)
#define SCISSORPLUGIN_TEMPLATES
#include "PolyLinePluginT.cc"
#endif

#endif //MOVEPLUGIN_HH
