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



#ifndef SELECTIONPLUGIN_HH
#define SELECTIONPLUGIN_HH

#include <QObject>
#include <QMenuBar>

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/BackupInterface.hh>
#include <OpenFlipper/BasePlugin/INIInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/ContextMenuInterface.hh>
#include <ACG/Scenegraph/GlutPrimitiveNode.hh>
#include <ACG/Scenegraph/LineNode.hh>
#include <OpenFlipper/common/Types.hh>
#include <OpenFlipper/BasePlugin/PluginFunctions.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

#ifdef ENABLE_POLYLINE_SUPPORT
#include <ObjectTypes/PolyLine/PolyLine.hh>
#endif

#ifdef ENABLE_BSPLINECURVE_SUPPORT
#include <ObjectTypes/BSplineCurve/BSplineCurve.hh>
#endif

#include "selectionToolbar.hh"

#include <OpenFlipper/INIFile/INIFile.hh>

#include <QGroupBox>

#define TOGGLE_SELECTION "Select (Toggle)"
#define PAINT_SPHERE_SELECTION "Select (Paint Sphere)"
#define CLOSEST_BOUNDARY_SELECTION "Select (Closest Boundary)"
#define LASSO_SELECTION "Select (Lasso)"
#define VOLUME_LASSO_SELECTION "Select (Volume Lasso)"
#define SURFACE_LASSO_SELECTION "Select (Surface Lasso)"
#define CONNECTED_COMPONENT_SELECTION "Select (Connected Component)"
#define FLOOD_FILL_SELECTION "Select (Flood Fill)"
#define CREATEMESH "Create Mesh from Selection"

enum SelectionPrimitive {
  VERTEX = 0x01,
  EDGE   = 0x02,
  FACE   = 0x04,
  OBJECT = 0x08
};

class SelectionPlugin : public QObject, BaseInterface , MouseInterface, KeyInterface, PickingInterface, ToolboxInterface, INIInterface, BackupInterface, LoggingInterface, ScriptInterface, ContextMenuInterface, ToolbarInterface, LoadSaveInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(INIInterface)
  Q_INTERFACES(BackupInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(ContextMenuInterface)
  Q_INTERFACES(ToolbarInterface)
  Q_INTERFACES(LoadSaveInterface)

  signals:
    //BaseInterface
    void updateView();
    void updatedObject(int);
    
    //PickingInterface
    void addHiddenPickMode( const std::string _mode );
    void addPickMode( const std::string _mode );
    void setPickModeMouseTracking( const std::string _mode, bool _mouseTracking);
    void setPickModeCursor( const std::string _mode, QCursor _cursor);
    
    //BackupInterface
    void createBackup( int _id , QString _name );
    
    //LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);
    
    //ScriptInterface
    void scriptInfo( QString _functionName );

    // ContextMenuInterface
    void addContextMenuItem(QAction* _action , ContextMenuType _type);
    void addContextMenuItem(QAction* _action , DataType _objectType , ContextMenuType _type );

    //KeyInterface
    void registerKey(int _key, Qt::KeyboardModifiers _modifiers, QString _description, bool _multiUse = false);

    // ToolboxInterface
    void addToolbox( QString _name  , QWidget* _widget ); 
    
    //ToolbarInterface
    void addToolbar(QToolBar* _toolbar);
    void getToolBar(QString _name, QToolBar*& _toolbar);

    //LoadSaveInterface
    void deleteObject( int _id );
    void addEmptyObject( DataType _type, int& _id);

  private slots:
    
    //MouseInterface
    void slotMouseWheelEvent(QWheelEvent * _event, const std::string & _mode);
    void slotMouseEvent( QMouseEvent* _event );
    
    //KeyInterface
    void slotKeyEvent( QKeyEvent* _event );
    void slotKeyReleaseEvent( QKeyEvent* _event );
    
    //PickingInterface
    void slotPickModeChanged( const std::string& _mode);
    
    //INIInterface
    void loadIniFile( INIFile& _ini , int _id);
    void saveIniFile( INIFile& _ini , int _id);
    
    //BaseInterface
    void initializePlugin();
    void pluginsInitialized();
    void slotObjectUpdated(int _id);
    void noguiSupported( ) {} ;

  public :

    SelectionPlugin();
    ~SelectionPlugin() {};

    //BaseInterface
    QString name() { return (QString(tr("Selections"))); };
    QString description( ) { return (QString(tr("Allows to select parts of Objects"))); };

    friend class SelectVolumeAction;

  private :
  //GUI objects
    /// Widget for Toolbox
    selectionToolbarWidget* tool_;

    /// Selection Toolbar
    QToolBar* toolBar_;
    QActionGroup* toolBarActions_;
    QActionGroup* toolBarTypes_;
    QAction* objectAction_;
    QAction* vertexAction_;
    QAction* edgeAction_;
    QAction* faceAction_;
    QAction* toggleAction_;
    QAction* paintSphereAction_;
    QAction* boundaryAction_;
    QAction* lassoAction_;
    QAction* volumeLassoAction_;
    QAction* surfaceLassoAction_;
    QAction* connectedAction_;
    QAction* floodFillAction_;
    QAction* createMeshFromSelAction_;

  private slots:
    /// Switch selection mode dependent which buttons are pressed in the toolbar
    void toolBarActionClicked(QAction* _action);

  private :
      
//===========================================================================
/** @name Template Functions
* @{ */
//===========================================================================
      
    /// Update face selection to correspond to the vertex selection
    template< typename MeshType >
    void update_regions(MeshType* _mesh);

    /// Use current selection to mark Vertices as Modifyable area vertices
    template< typename MeshType >
    void set_area(  MeshType* _mesh);

    /// Use current selection to mark Vertices as Handle vertices
    template< typename MeshType >
    void set_handle(MeshType* _mesh);

    /// convert current selection to features
    template< typename MeshType >
    void set_features(MeshType* _mesh);

    /// Clear Handle vertices
    template< typename MeshType >
    void clear_handle(MeshType* _mesh);

    /// Clear Area
    template< typename MeshType >
    void clear_area(MeshType* _mesh);

    /// Clear Features
    template< typename MeshType >
    void clear_features(MeshType* _mesh);

    /// Delete all selected elements of a mesh
    template< typename MeshT >
    bool deleteSelection(MeshT* _mesh);

    /// Colorize a selection
    template< typename MeshT >
    void colorizeSelection(MeshT* _mesh, SelectionPrimitive _type, int _red, int _green, int _blue);
    
    //=================================================================
    
    /// Create a new mesh from the selection
    template< class MeshT >
    void createMeshFromSelection( MeshT& _mesh, MeshT& _newMesh);
    
/** @} */

    /// when SHIFT key is pressed do source-selection instead of target selection in object-mode
    bool sourceSelection_;

    /// when CONTROL key is pressed: deselect instead of select
    bool deselection_;

    //===========================================================================
    /** @name Selection functions
    * @{ */
    //===========================================================================

    ///SelectionFlags (VERTEX, EDGE, FACE, OBJECT)
    unsigned char selectionType_;

    /// Toggle the selection state
    void toggleSelection(QMouseEvent* _event);

    /// Toggle the selection state in meshes
    template< class MeshT >
    void toggleMeshSelection(MeshT* _mesh, uint _fh, ACG::Vec3d& _hit_point);

    #ifdef ENABLE_POLYLINE_SUPPORT
      /// Toggle the selection state in PolyLines
      void togglePolyLineSelection(QMouseEvent* _event);
    #endif

    #ifdef ENABLE_BSPLINECURVE_SUPPORT
      /// Toggle the selection state in BSplineCurves
      void toggleBSplineCurveSelection(QMouseEvent* _event);
    #endif

    /// Handle Mouse move event for sphere painting selection
    void paintSphereSelection(QMouseEvent* _event);

    /// Use the event to paint selection with a sphere
    template< typename MeshT>
    void paintSphereSelection( MeshT* _mesh , int _target_idx, typename MeshT::Point _hitpoint );

    /// Select all primitves of connected component
    void componentSelection(QMouseEvent* _event);

    /// Select all primitves on a (approx) planar region
    void floodFillSelection(QMouseEvent* _event);

    /// Select all primitves of connected component
    template< class MeshT >
    void componentSelection(MeshT* _mesh, uint _fh);

    /// Select all primitves of a planar region surrounding the faceHandle
    template< class MeshT >
    void floodFillSelection(MeshT* _mesh, uint _fh);

    /// Handle Mouse move event for lasso selection
    void handleLassoSelection(QMouseEvent* _event, bool _volume);

    /// Select all primitves that are projected to the given region
    template< class MeshT >
    bool volumeSelection(MeshT* _mesh, ACG::GLState& _state, QRegion *_region);

    /** @} */
  private slots:

    /// Use current selection to set Area bits
    void slotSetArea();

    /// Use current selection to set Handle bits
    void slotSetHandle();

    /// convert current selection to feature
    void slotSetFeatures();

    /// Clear Handle Bits
    void slotClearHandle();

    /// Clear Area Bits
    void slotClearArea();

    /// Clear Features
    void slotClearFeatures();

    /// Delete all selected elements of the target meshes
    void slotDeleteSelection();

    /// Toggle the selection Restriction
    void slotToggleSelectionRestriction();

  private:

    /// Select mesh elements for each object in the list
    void forEachObject(QList<QPair<unsigned int, unsigned int> > &_list, SelectionPrimitive _type);

  //===========================================================================
  /** @name Button slots
  * @{ */
  //===========================================================================
  private slots:

    /// Select everything of the active object
    void slotSelectAll();

    /// Clear selection of current objects
    void slotClearSelection();

    /// Invert the current selection
    void slotInvertSelection();

    /// Select the boundary of the object
    void slotSelectBoundary();

    /// Shrink the current selection
    void slotShrinkSelection();

    /// Grow the current selection
    void slotGrowSelection();

    /// load a selection
    void slotLoadSelection();

    /// Save the current selection
    void slotSaveSelection();

    /// Convert selection types (modeling area, handle region or feature)
    void slotConvertSelectionType();

    /// Colorize given selection
    void slotColorizeSelection();

    //==========================================

    /// clear all vertex selections of every object
    void slotClearAllVertexSelections();

    /// clear all edge selections of every object
    void slotClearAllEdgeSelections();

    /// clear all face selections of every object
    void slotClearAllFaceSelections();


  /** @} */

  //===========================================================================
  /** @name Scriptable slots
  * @{ */
  //===========================================================================
  public slots:

    /// select given vertices
    void selectVertices( int objectId , IdList _vertexList );

    /// unselect given vertices
    void unselectVertices( int objectId , IdList _vertexList );

    /// Select all Vertices
    void selectAllVertices( int objectId );

    /// Unselect all vertices
    void clearVertexSelection( int objectId );

    /// Invert the current vertex selection
    void invertVertexSelection( int objectId );

    /// Select all boundary vertices of the given object
    void selectBoundaryVertices( int objectId );

    /// Select all vertices of the boundary close to the given vertex
    void selectClosestBoundaryVertices( int objectId, int VertexId );

    /// Shrink the current vertex selection
    void shrinkVertexSelection( int objectId );

    /// Grow the current vertex selection
    void growVertexSelection( int objectId );

    /// return a list of all selected vertices
    IdList getVertexSelection( int objectId );

    /// delete vertices and faces that are currently selected
    void deleteSelection( int objectId );

    /// colorize the vertex selection
    void colorizeVertexSelection(int objectId, int r, int g, int b );

    //==========================================

    /// Set vertices to be part of the handle area
    void selectHandleVertices( int objectId , IdList _vertexList );

    /// Remove vertices from handle area
    void unselectHandleVertices( int objectId , IdList _vertexList );

    /// Clear handle Area
    void clearHandleVertices( int objectId  );

    /// Set all vertices to be part of the handle area
    void setAllHandleVertices( int objectId );

    /// Get a list of all handle vertices
    IdList getHandleVertices( int objectId );

    /// Load a selection from an Flipper selection file for the given object
    void loadFlipperModelingSelection( int _objectId , QString _filename );

    /// Save a selection in Flipper Selection Format
    void saveFlipperModelingSelection( int _objectId , QString _filename );

    //==========================================

    /// Set vertices to be part of the modeling area
    void selectModelingVertices( int objectId , IdList _vertexList );

    /// Remove vertices from modeling area
    void unselectModelingVertices( int objectId , IdList _vertexList );

    /// Clear Modeling Area
    void clearModelingVertices( int objectId  );

    /// Set all vertices to be part of the modeling area
    void setAllModelingVertices( int objectId );

    /// Get a list of all modeling vertices
    IdList getModelingVertices( int objectId );

    //==========================================

    /// Select given Edges
    void selectEdges( int objectId , IdList _vertexList );

    /// Unselect given Edges
    void unselectEdges( int objectId , IdList _vertexList );

    /// Select all Edges
    void selectAllEdges( int objectId );

    /// Unselect all Edges
    void invertEdgeSelection( int objectId );

    /// Invert the current edge selection
    void clearEdgeSelection( int objectId );

    /// select boundary edges
    void selectBoundaryEdges( int objectId );

    /// return a list of all selected edges
    IdList getEdgeSelection( int objectId );
    
    /// colorize the edge selection
    void colorizeEdgeSelection(int objectId, int r, int g, int b );

    //==========================================

    /// Select given faces
    void selectFaces( int objectId , IdList _facesList );

    /// Unselect given faces
    void unselectFaces( int objectId , IdList _facesList );

    /// Select all faces
    void selectAllFaces( int objectId );

    /// Unselect all faces
    void clearFaceSelection( int objectId );

    /// Invert the current face selection
    void invertFaceSelection( int objectId );

    /// Select all boundary faces of the given object
    void selectBoundaryFaces( int objectId );

    /// Shrink the current face selection
    void shrinkFaceSelection( int objectId );

    /// Grow the current face selection
    void growFaceSelection( int objectId );

    /// return a list of all selected faces
    IdList getFaceSelection( int objectId );

    /// colorize the face selection
    void colorizeFaceSelection(int objectId, int r, int g, int b );

    /// save Selections into File
    void saveSelections();

    /// save Selection of one object
    void saveSelection( int _objectId  , QString _filename);

    /// load Selection of one object
    void loadSelection( int _objectId  , QString _filename);
    
    //=====================================================
    
    /// Create a mesh containing the selection of the given mesh
    int createMeshFromSelection( int _objectId );

    private slots:

    void saveSelection( TriMesh&  _mesh, QString _filename);
    void saveSelection( PolyMesh& _mesh, QString _filename);

    //======================================================

    /// Convert selection types (modeling area, handle region or feature)
    void convertSelectionType(StatusBits _from, StatusBits _to, bool _clearAfter);

    /// Convert selections (vertex, edge, face)
//    void convertSelection();

    /// Select for each selected vertex the incident edges and
    /// unselect vertices after (if _unselectAfter = true)
    void convertVtoESelection(bool _unselectAfter = true);

    /// Select for each selected vertex the adjacent faces and
    /// unselect vertices after (if _unselectAfter = true)
    void convertVtoFSelection(bool _unselectAfter = true);

    /// Select for each selected edge the adjacent vertices and
    /// unselect the edges after (if _unselectAfter = true)
    void convertEtoVSelection(bool _unselectAfter = true);

    /// Select for each selected edge the two adjacent faces and
    /// unselect the edges after (if _unselectAfter = true)
    void convertEtoFSelection(bool _unselectAfter = true);

    /// Select for each selected face the adjacent vertices and
    /// unselect the face after (if _unselectAfter = true)
    void convertFtoVSelection(bool _unselectAfter = true);

    /// Select for each selected face the adjacent edges and
    /// unselect the face after (if _unselectAfter = true)
    void convertFtoESelection(bool _unselectAfter = true);
    
    //==========================================================
    
    /// Create Mesh from Selection Button
    void slotCreateMeshFromSelection();
    
    /// Create mesh from selection of picked object
    void createMeshFromSelection(QMouseEvent* _event);

  /** @} */

  //===========================================================================
  /** @name Variables for sphere painting selection
  * @{ */
  //===========================================================================

  private:

    /// Sphere Nodes material
    ACG::SceneGraph::MaterialNode*      sphere_mat_node_;

    /// Sphere Node
    ACG::SceneGraph::GlutPrimitiveNode* sphere_node_;

    /// Line Node
    ACG::SceneGraph::LineNode* line_node_;

    /// Current radius of the sphere
    float sphere_radius_;

    /// true if currently selecting a sphere
    bool sphere_selection_;

    /// vector containing all object-id's whose selection has changed
    std::vector< int > selection_changes_;

    /// vector containing all projected lasso points
    std::vector< ACG::Vec3f > lasso_points_;

    /// vector containing all lasso points
    QPolygon lasso_2Dpoints_;

    /// true if currently selecting with lasso
    bool lasso_selection_;

  /** @} */


  //===========================================================================
  /** @name Variables for Surface-Lasso Selection
  * @{ */
  //===========================================================================

  private:

    /// flag is active while waiting for the creation of a polyLine
    bool waitingForPolyLineSelection_;

    /// stores the id of the polyLine which is used for selection
    int polyLineID_;

    #ifdef ENABLE_POLYLINE_SUPPORT
      /// function determines if inside/outside of polyline should be selected and selects the part
      void surfaceLassoSelection(QMouseEvent* _event);

      /// selects everything inside/outside of the given polyline
      template< class MeshT , class SpatialSearchT>
      void surfaceLassoSelection( MeshT*                     _mesh ,
                                  PolyLine*                  _line ,
                                  typename MeshT::FaceHandle _fh,
                                  SpatialSearchT*            _search);

    #endif

  /** @} */

  //===========================================================================
  /** @name Variables for Closest-Boundary Selection
  * @{ */
  //===========================================================================

  private:
    /// selects the closest boundary to a vertex with given handle
    template< class MeshT >
    void closestBoundarySelection(MeshT* _mesh, int _vh, unsigned char _selectionType);

    /// closest boundary selection
    void closestBoundarySelection(QMouseEvent* _event);

  /** @} */

  //===========================================================================
  /** @name Variables for FloodFill Selection
  * @{ */
  //===========================================================================

  private:
    double maxFloodFillAngle_;

  /** @} */

  //===========================================================================
  /** @name Context Menu
  * @{ */
  //===========================================================================

  private slots:

    /// called by the selection context Menu
    void selectionContextMenu(QAction* _action);

  private:
    ///Selection context menu
    QMenu* contextMenu_;

    /// Select all primitives of the given object
    void selectAll      ( int objectId );

    /// Clear the selection on the given object
    void clearSelection ( int objectId );

    /// invert the selection on the given object
    void invertSelection( int objectId );

    /// shrink the outer ring(s) of the selection
    void shrinkSelection( int objectId );

    /// grow the outer ring(s) of the selection
    void growSelection  ( int objectId );

    /// select the boundary of the giben object
    void selectBoundary ( int objectId );

    /// choose a color and colorize the selection
    void colorizeSelection(int objectId );

  /** @} */

  public slots:
    QString version() { return QString("1.0"); };

};

/// Traverse the scenegraph and call the selection function for mesh nodes
class SelectVolumeAction
{
public:

  SelectVolumeAction(QRegion *_region, SelectionPlugin *_plugin) :
    region_(_region), plugin_(_plugin) {}

  void enter (BaseNode* /*_node*/) {};
  
  void leave (BaseNode* /*_node*/) {};

  bool operator()(BaseNode* _node, ACG::GLState& _state);

private:

  QRegion         *region_;
  SelectionPlugin *plugin_;
};


//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(SELECTIONPLUGINT_C)
#define SELECTIONPLUGINT_TEMPLATES
#include "SelectionPluginT.cc"
#endif

#if defined(INCLUDE_TEMPLATES) && !defined(SELECTIONFUNCTIONST_C)
#define SELECTIONFUNCTIONST_TEMPLATES
#include "SelectionFunctionsT.cc"
#endif
//=============================================================================
#endif // SELECTIONPLUGIN_HH defined
//=============================================================================
